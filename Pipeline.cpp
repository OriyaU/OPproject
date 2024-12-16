#include "Pipeline.hpp"
using namespace std;

// ActiveObject class that processes a task

ActiveObject::ActiveObject(std::function<void(void*, std::mutex&)> func, int id) 
    :  workerThread(nullptr),stopFlag(false), function(func), nextQueueActiveObject(nullptr), id(id) {}

// Copy constructor
ActiveObject::ActiveObject(const ActiveObject& other)
    : acMutex(), cv(), stopFlag(other.stopFlag), function(other.function), nextQueueActiveObject(other.nextQueueActiveObject), id(other.id) {}


void ActiveObject::addTask(void* task) {
    std::lock_guard<std::mutex> lock(acMutex);  // Lock the mutex to safely access the queue not having the situatinin of addeng and reading from the queue at the same time)
    tasks.push(task);
    cv.notify_one();
    //cout << "Task added to ActiveObject. Queue size: " << tasks.size() << endl;
}

void ActiveObject::runTasks(vector<ActiveObject>& activeObjects, mutex& send_mutex) {
    while (true) {
        void* task;
        {
            std::unique_lock<std::mutex> lock(acMutex);
            cv.wait(lock, [this] { return !tasks.empty() || stopFlag; });

            if (stopFlag && tasks.empty()) {
                return;
            }

            if (!tasks.empty()) {
                task = tasks.front();
                tasks.pop();
            }



            if (task) {
                function(task, send_mutex);
                //cout << "Task executed. Thread ID: " << std::this_thread::get_id() << endl;

                if (nextQueueActiveObject) {
                    std::lock_guard<std::mutex> nextLock(activeObjects[id + 1].acMutex);  // Lock before modifying next queue
                    nextQueueActiveObject->push(task);
                    activeObjects[id + 1].cv.notify_one();
                  //  cout << "Task passed to next ActiveObject. Thread ID: " << std::this_thread::get_id() << endl;
                }
            }
        }
    }
}

class Pipeline; // Forward declaration not strictly needed here since we included Pipeline.hpp already

Pipeline::Pipeline() {
    // Create ActiveObjects (workers) for each task
    activeObjects.push_back(ActiveObject(Pipeline::taskLongestDistance, 0));   // Task 1
    activeObjects.push_back(ActiveObject(Pipeline::taskShortestDistance, 1));  // Task 2
    activeObjects.push_back(ActiveObject(Pipeline::taskAverageDistance, 2));   // Task 3
    activeObjects.push_back(ActiveObject(Pipeline::taskTotalWeight, 3));       // Task 4

    for (int i = 0; i < (int)activeObjects.size() - 1; ++i) {
        activeObjects[i].nextQueueActiveObject = &activeObjects[i + 1].tasks;
    }
}

Pipeline::~Pipeline() {
    stop();  // Ensure all threads are stopped and joined

    for (auto& activeObject : activeObjects) {
        delete activeObject.workerThread;
    }
    // stop();
}

void Pipeline::addRequest(void* task) {
    //cout << "Task added to Pipeline (first worker)." << endl;
    //std::lock_guard<std::mutex> lock(activeObjects[0].acMutex);
    activeObjects[0].addTask(task);
    //activeObjects[0].cv.notify_one();
    //cout << "Task added to Pipeline (first worker). Initial ActiveObject notified." << endl;
}

void Pipeline::start(mutex& send_mutex) {
   // cout << "[DEBUG] Pipeline starting all worker threads." << endl;
    for (auto& activeObject : activeObjects) {
        activeObject.workerThread = new std::thread(&ActiveObject::runTasks, &activeObject, std::ref(activeObjects), std::ref(send_mutex));
        //cout << "[DEBUG] Worker thread started for ActiveObject ID: " << activeObject.id << endl;

        // cout << "[DEBUG] Worker thread started for ActiveObject." << endl;
    }
}

void Pipeline::stop() {
    for (auto& activeObject : activeObjects) {
        std::lock_guard<std::mutex> lock(activeObject.acMutex);
        activeObject.stopFlag = true;
        activeObject.cv.notify_one();
    }

    for (auto& activeObject : activeObjects) {
        if (activeObject.workerThread->joinable()) {
            activeObject.workerThread->join();
        }
    }
}

//     // Task 1: Longest Distance
void Pipeline::taskLongestDistance(void* task, mutex& send_mutex) {
//std::cout << "Thread ID: " << std::this_thread::get_id() << " Calculated LongestDistance" << std::endl;
    auto* taskTuple = static_cast<std::tuple<MST*, string*, int>*>(task);
    if (taskTuple != nullptr) {
        MSTTree stats;
        MST* mst_graph = std::get<0>(*taskTuple);
        string* clientAns = std::get<1>(*taskTuple);
        if (mst_graph != nullptr && clientAns != nullptr) {
            {
                lock_guard<mutex> lock(send_mutex);
                *clientAns += "Longest Distance: " + std::to_string(stats.getLongestDistance(*mst_graph)) + "\n";
            }
        }
    } else {
     //   cout << "Error: Invalid task tuple" << endl;
    }
}
    
// Task 2: Shortest Distance
void Pipeline::taskShortestDistance(void* task, mutex& send_mutex) {
    // std::cout << "Thread ID: " << std::this_thread::get_id() << " Calculated shortest distance" << std::endl;

    auto* taskTuple = static_cast<tuple<MST*, string*, int>*>(task);
    MST* mst_graph = std::get<0>(*taskTuple);
    string* clientAns = std::get<1>(*taskTuple);

    MSTTree stats;
    {
        std::lock_guard<std::mutex> lock(send_mutex);
        *clientAns += "Shortest Distance: " + std::to_string(stats.getShortestDistance(*mst_graph)) + "\n";
    }
}

// Task 3: Average Distance
void Pipeline::taskAverageDistance(void* task, mutex& send_mutex) {

     //std::cout << "Thread ID: " << std::this_thread::get_id() << " Calculated average distance" << std::endl;

    auto* taskTuple = static_cast<tuple<MST*, string*, int>*>(task);
    MST* mst_graph = std::get<0>(*taskTuple);
    string* clientAns = std::get<1>(*taskTuple);

    MSTTree stats;
    {
        std::lock_guard<std::mutex> lock(send_mutex);
        *clientAns += "Average Distance: " + std::to_string(stats.getAverageDistance(*mst_graph)) + "\n";
    }
}

// Task 4: Total Weight
void Pipeline::taskTotalWeight(void* task, mutex& send_mutex) {

    //std::cout << "Thread ID: " << std::this_thread::get_id() << " Calculated total weight" << std::endl;

    auto* taskTuple = static_cast<tuple<MST*, string*, int>*>(task);
    MST* mst_graph = std::get<0>(*taskTuple);
    string* clientAns = std::get<1>(*taskTuple);
    int fdclient = std::get<2>(*taskTuple);

    MSTTree stats;
    {
        std::lock_guard<std::mutex> lock(send_mutex);
        *clientAns += "Total Weight: " + std::to_string(stats.getTotalWeight(*mst_graph)) + "\n";

        delete taskTuple;

        // send the response back to the client
        if (send(fdclient, clientAns->c_str(), clientAns->size(), 0) == -1) {
            perror("send");
        }
    }
};