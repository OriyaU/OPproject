#ifndef PIPELINE_HPP
#define PIPELINE_HPP

#include <sys/socket.h>  // Include this header for the send function
#include <sys/types.h>
#include <condition_variable>
#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <queue>
#include <sstream>
#include <thread>
#include <vector>
#include "graph.hpp"
#include "MST.hpp"
#include "MSTTree.hpp"
#include "MST_strategy.hpp"
//ה-acMutex נועד לשמור על תקינות הנתונים בעת גישה מקבילית לתור המשימות של ה-ActiveObject. מאחר שלמספר שרשורים עשויה להיות גישה לאותו תור בו-זמנית (שרשור אחד עשוי להוסיף משימה, ואחר לקרוא משימה), עלינו להשתמש ב-mutex כדי לוודא שרק שרשור אחד יוכל לבצע שינוי בתור המשימות בכל רגע נתון.

//ללא mutex, ייתכן מצב שבו שני שרשורים ניגשים לאותו מבנה נתונים (התור) במקביל, וכתוצאה מכך נוצרים שיבושים כמו קריאה וכתיבה בו-זמנית שאינם מתואמים, ולבסוף התנהגות לא מוגדרת שעלולה לגרום לבאגים או קריסות. ה-mutex מבטיח שהגישה למשאב המשותף (התור) מסודרת בצורה טורית (thread-safe).

class ActiveObject {
public:
    std::thread* workerThread;//Each ActiveObject runs tasks in its own thread.
    std::queue<void*> tasks;//Stores the tasks assigned to this ActiveObject.
    std::mutex acMutex;//Protects access to the tasks queue to ensure thread-safety.
    std::condition_variable cv;//Notifies the thread when a new task is available or when it should stop.
    bool stopFlag;
    std::function<void(void*, std::mutex&)> function;  // The function that the worker will execute
    std::queue<void*>* nextQueueActiveObject;          // Queue for the next ActiveObject
    int id;

    ActiveObject(std::function<void(void*, std::mutex&)> func, int id);
    // Copy constructor
    ActiveObject(const ActiveObject& other);

    void addTask(void* task);
    void runTasks(std::vector<ActiveObject>& activeObjects, std::mutex& send_mutex);
};

class Pipeline {
public:
    Pipeline();
    ~Pipeline();

    void addRequest(void* task);
    void start(std::mutex& send_mutex);
    void stop();

private:
    // Task 1: Longest Distance
    static void taskLongestDistance(void* task, std::mutex& send_mutex);

    // Task 2: Shortest Distance
    static void taskShortestDistance(void* task, std::mutex& send_mutex);

    // Task 3: Average Distance
    static void taskAverageDistance(void* task, std::mutex& send_mutex);

    // Task 4: Total Weight
    static void taskTotalWeight(void* task, std::mutex& send_mutex);

    std::vector<ActiveObject> activeObjects;
};

#endif // PIPELINE_HPP