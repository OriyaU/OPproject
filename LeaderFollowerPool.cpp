#include "LeaderFollowerPool.hpp"
#include <chrono>
#include <iostream>
#include "MSTTree.hpp"
#include "MST.hpp"
using namespace std;


// Update constructor to accept shared answer stream and mutex
LeaderFollowerPool::LeaderFollowerPool(int numThreads, std::mutex& ansMutex)
    : numThreads_(numThreads), stopFlag_(false), ansMutex(ansMutex), leader_exists(false) {
    for (int i = 0; i < numThreads_; ++i) {
       std::thread t(&LeaderFollowerPool::leaderRole, this);  // יצירת thread חדש שמפעיל את leaderRole
    workers_.push_back(std::move(t));     
    }
}
void LeaderFollowerPool::start() {
    {
        lock_guard<mutex> lock(mutexstop);  // Lock the mutex
        stopFlag_ = false;
    }
}

LeaderFollowerPool::~LeaderFollowerPool() {
    stop();
}

void LeaderFollowerPool::addEventHandler(void* task) {
    std::lock_guard<std::mutex> lock(mutexqueue);
    eventQueue_.push(task);
    cv_.notify_one();  // Notify the leader thread to handle the event
}

void LeaderFollowerPool::mainFunction(void* task) {
    std::lock_guard<std::mutex> lock(ansMutex);  // Lock the shared answer stream
    MSTTree mst_stats;

    auto* taskTuple = static_cast<tuple<MST* , string* , int>*>(task);
   
    MST* clientMST = std::get<0>(*taskTuple);
  //  string* clientAns = std::get<1>(*taskTuple);
    int fdclient = std::get<2>(*taskTuple);

    std::ostringstream localAns;  // Local answer stream
    localAns << "Thread " << std::this_thread::get_id() << "\n";
    localAns << " Longest path: " << mst_stats.getLongestDistance(*clientMST) << "\n";
    localAns << " Shortest path: " << mst_stats.getShortestDistance(*clientMST) << "\n";
    localAns << " Average path: " << mst_stats.getAverageDistance(*clientMST) << "\n";
    localAns << " Total weight: " << mst_stats.getTotalWeight(*clientMST) << "\n";
    delete taskTuple;  // Clean up dynamically allocated memory

    // send the response back to the client
    if (send(fdclient, localAns.str().c_str(), localAns.str().length(), 0) == -1) {
        perror("send");
    }
}
// here we role the leader from thread to thread that was sleep until yet
void LeaderFollowerPool::leaderRole() 
{
    while (true) {
        void* task = nullptr;
        {
            std::unique_lock<std::mutex> lock(mutexqueue);
            cv_.wait(lock, [this] {
                std::lock_guard<std::mutex> stopLock(mutexstop);  // Lock mutexstop for stopFlag_ check and we don't want any other thread change the stopFlag when we check the flag.
                return (!eventQueue_.empty() && !leader_exists) || stopFlag_;
            });
            
           leader_exists = true;

            if (stopFlag_ && eventQueue_.empty()) {
                return;  // Shutdown check
            }

            if (!eventQueue_.empty()) {
                // Get the next task
                task = std::move(eventQueue_.front());
                eventQueue_.pop();
                leader_exists = false;
                // Notify other threads that a new leader can be elected
                followerRole();
             }
        }

        // Execute the task outside the lock
        if (task) {
            mainFunction(task);
        }
    }
}

void LeaderFollowerPool::followerRole() {
    cv_.notify_one();  // Wake up next thread to be leader
}

// Method to gracefully stop the thread pool
void LeaderFollowerPool::stop() {
    {
        std::lock_guard<std::mutex> lock(mutexqueue);  // Lock the mutex for the stop
        stopFlag_ = true;
        cv_.notify_all();  // Notify all waiting threads with the lock held
    }

    for (auto& worker : workers_) {
        if (worker.joinable()) {
            worker.join();  // Wait for all worker threads to finish
        }
    }
    workers_.clear();  // Clear the worker threads vector

    // cout << "LeaderFollowerPool stopped all worker threads." << endl;
}