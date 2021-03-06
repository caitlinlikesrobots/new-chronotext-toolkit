/*
 * THE NEW CHRONOTEXT TOOLKIT: https://github.com/arielm/new-chronotext-toolkit
 * COPYRIGHT (C) 2012-2014, ARIEL MALKA ALL RIGHTS RESERVED.
 *
 * THE FOLLOWING SOURCE-CODE IS DISTRIBUTED UNDER THE SIMPLIFIED BSD LICENSE:
 * https://github.com/arielm/new-chronotext-toolkit/blob/master/LICENSE.md
 */

/*
 * ADDITIONAL CONTRIBUTORS: MICHAEL BOCCARA
 */

/*
 * TODO:
 *
 * 1) TEST AND DEVELOP FURTHER:
 *    - SEE "INNER" TODOS IN TaskManager AND Task
 *
 * 2) HANDLE PROPERLY THE SHUTING-DOWN OF "UNDERGOING" TASKS:
 *    - SEE RELATED TODOS IN Context AND CinderDelegate
 *
 * 3) HANDLE PROPERLY THE TOPIC OF "MAXIMUM CONCURRENT THREADS":
 *    - TASKS COULD PROVIDE HINTS REGARDING WHICH "RESOURCES" THEY MAY "CONSUME":
 *      - E.G. DISK OR NETWORK
 *    - TAKE THE "THREAD POOL PATTERN" IN COUNT:
 *      - http://en.wikipedia.org/wiki/Thread_pool_pattern
 *    - CONSIDER USING "GRAND CENTRAL DISPATCH" ON COCOA:
 *      - http://en.wikipedia.org/wiki/Grand_Central_Dispatch
 *
 * 4) CREATE TESTS AND SAMPLES:
 *    - EXTREME CASES:
 *      - Task::run() RETURNS IMMEDIATLY
 *      - ETC.
 *    - CANCELLATION
 *    - "POSTING" FROM TASK
 *    - ETC.
 *
 * 5) CONSIDER USING C++11 FEATURES LIKE:
 *    - std::future
 *    - std::async
 *    - std::thread_local
 *    - std::atomic
 *    - ETC.
 */

#pragma once

#include "chronotext/os/Task.h"

#include <map>
#include <set>
#include <queue>
#include <atomic>

namespace chr
{
    class TaskManager : public std::enable_shared_from_this<TaskManager>
    {
    public:
        static std::atomic<bool> LOG_VERBOSE;
        static std::atomic<int> MAX_CONCURRENT_THREADS;

        static std::shared_ptr<TaskManager> create()
        {
            return std::shared_ptr<TaskManager>(new TaskManager());
        }
        
        /*
         * RETURNS NULL IF:
         *
         * - NOT INVOKED ON THE SKETCH-THREAD
         * - NO TASK IS REGISTERED WITH THIS ID
         * - THE TASK HAS ALREADY BEEN EXECUTED (TASKS ARE NOT REUSABLE)
         *
         * NOTE: THE RETURNED POINTER IS NOT INTENDED FOR STORAGE
         */
        Task* getTask(int taskId);
        
        /*
         * RETURNS 0 IF THE TASK CAN'T BE REGISTERED
         *
         * CAUSES:
         *
         * - NOT INVOKED ON THE SKETCH-THREAD
         * - THE TASK IS ALREADY REGISTERED (TASKS ARE NOT REUSABLE)
         * - Task::init() RETURNED FALSE
         */
        int registerTask(std::shared_ptr<Task> task);
        
        /*
         * RETURNS FALSE IF THE TASK CAN'T BE ADDED
         *
         * CAUSES:
         *
         * - NOT INVOKED ON THE SKETCH-THREAD
         * - NO TASK IS REGISTERED WITH THIS ID
         * - THE TASK HAS ALREADY BEEN ADDED
         * - IO-SERVICE IS NOT DEFINED
         */
        bool addTask(int taskId, bool forceSync = false);
        
        inline bool addTask(std::shared_ptr<Task> &&task, bool forceSync = false)
        {
            int taskId = registerTask(std::forward<std::shared_ptr<Task>>(task));
            return taskId ? addTask(taskId, forceSync) : false;
        }
        
        /*
         * RETURNS FALSE IF THE TASK CAN'T BE CANCELLED (OR CAN'T BE REMOVED IF NOT-YET STARTED)
         *
         * CAUSES:
         *
         * - NOT INVOKED ON THE SKETCH-THREAD
         * - NO TASK IS REGISTERED WITH THIS ID
         * - THE TASK HAS ALREADY BEEN EXECUTED
         *
         * ASYNCHRONOUS TASKS ONLY:
         * - THE TASK IS ALREADY AWAITING CANCELLATION
         */
        bool cancelTask(int taskId);
        
    protected:
        int taskCount = 0;
        
        std::map<int, std::shared_ptr<Task>> tasks;
        std::set<int> startedTasks;
        std::set<int> postponedTasks;
        std::queue<int> taskQueue;
        
    private:
        friend class Task;
        
        TaskManager() = default;
        TaskManager(const TaskManager &other) = delete;
        
        void endTask(int taskId);
        bool nextTask();
    };
}
