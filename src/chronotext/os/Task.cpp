/*
 * THE NEW CHRONOTEXT TOOLKIT: https://github.com/arielm/new-chronotext-toolkit
 * COPYRIGHT (C) 2012-2014, ARIEL MALKA ALL RIGHTS RESERVED.
 *
 * THE FOLLOWING SOURCE-CODE IS DISTRIBUTED UNDER THE SIMPLIFIED BSD LICENSE:
 * https://github.com/arielm/new-chronotext-toolkit/blob/master/LICENSE.md
 */

#include "chronotext/os/Task.h"
#include "chronotext/os/TaskManager.h"
#include "chronotext/Context.h"

#include <chrono>

using namespace std;
using namespace ci;

using namespace context;

namespace chr
{
    void Task::sleep(double seconds)
    {
#if defined(CINDER_ANDROID)
        boost::this_thread::sleep_for(boost::chrono::duration<double, boost::chrono::seconds::period>(seconds)); // XXX: A CURRENT LIMITATION OF SAFETY-DANK'S ANDROID PORT
#else
        this_thread::sleep_for(chrono::duration<double, chrono::seconds::period>(seconds));
#endif
    }
    
    Task::~Task()
    {
        LOGI_IF(TaskManager::LOG_VERBOSE) << __PRETTY_FUNCTION__ << " | " << taskId << " | " << this << endl;
        
        detach(); // OTHERWISE: THE APPLICATION MAY CRASH WHEN SHUT-DOWN ABRUPTLY
    }
    
    int Task::getId()
    {
        lock_guard<mutex> lock(_mutex);
        return taskId;
    }
    
    bool Task::hasStarted()
    {
        lock_guard<mutex> lock(_mutex);
        return started;
    }
    
    bool Task::hasEnded()
    {
        lock_guard<mutex> lock(_mutex);
        return ended;
    }
    
    bool Task::isCancelRequired()
    {
        lock_guard<mutex> lock(_mutex);
        return cancelRequired;
    }
    
    // ---
    
    /*
     * - ONLY ACCESSIBLE FROM TASKS
     * - INTENDED TO BE INVOKED ON THE TASK-THREAD
     */
    
    bool Task::post(function<void()> &&fn)
    {
        if (started && !ended)
        {
            return os::post(forward<function<void()>>(fn), synchronous);
        }
        
        return false;
    }
    
    // ---
    
    /*
     * INVOKED ON THE SKETCH-THREAD, FROM TaskManager::addTask()
     */
    
    void Task::start(bool forceSync)
    {
        if (initialized && !started)
        {
            lock_guard<mutex> lock(_mutex);
            
            LOGI_IF(TaskManager::LOG_VERBOSE) << __PRETTY_FUNCTION__ << " | " << taskId << " | " << this << endl;
            
            synchronous = forceSync;
            
            if (forceSync)
            {
                started = true;
                run();
                ended = true;
            }
            else
            {
                started = true;
                _thread = thread(&Task::performRun, this);
            }
        }
        else
        {
            assert(false);
        }
    }
    
    /*
     * INVOKED ON THE SKETCH-THREAD, FROM TaskManager::cancelTask()
     */
    
    void Task::cancel()
    {
        if (!synchronous && started)
        {
            lock_guard<mutex> lock(_mutex);

            LOGI_IF(TaskManager::LOG_VERBOSE) << __PRETTY_FUNCTION__ << " | " << taskId << " | " << this << endl;
            
            cancelRequired = true;
        }
        else
        {
            assert(false);
        }
    }
    
    /*
     * ASSERTIONS:
     *
     * - SUPPOSED TO WORK NO MATTER ON WHICH THREAD INVOKED
     * - NO-HARM IF _thread WAS NEVER ACTUALLY USED
     */
    
    void Task::detach()
    {
        if (_thread.joinable())
        {
            _thread.detach();
        }
    }
    
    /*
     * INVOKED ON THE SKETCH-THREAD, FROM TaskManager::registerTask()
     */
    
    bool Task::performInit(shared_ptr<TaskManager> manager, int taskId)
    {
        if (!initialized)
        {
            lock_guard<mutex> lock(_mutex);

            if (init())
            {
                LOGI_IF(TaskManager::LOG_VERBOSE) << __PRETTY_FUNCTION__ << " | " << taskId << " | " << this << endl;
                
                Task::manager = manager;
                Task::taskId = taskId;
                
                initialized = true;
                return true;
            }
            
            return false;
        }
        else
        {
            assert(false);
        }
    }
    
    /*
     * INTENDED TO BE INVOKED FROM Task::start(), ON THE TASK-THREAD
     */
    
    void Task::performRun()
    {
        if (started && !ended)
        {
            LOGI_IF(TaskManager::LOG_VERBOSE) << __PRETTY_FUNCTION__ << " [BEGIN] | " << taskId << " | " << this << endl;
            
            if (!isCancelRequired()) // TODO: DOUBLE-CHECK IF TEST MAKES SENSE
            {
                /*
                 * ThreadSetup IS MANDATORY ON OSX AND iOS (DUMMY ON ANDROID AND WINDOWS)
                 *
                 * TODO: A SIMILAR SYSTEM IS NECESSARY ON ANDROID FOR ATTACHING/DETACHING THE THREAD TO/FROM JAVA
                 */
                ThreadSetup forCocoa;
                
                run();
            }
            
            lock_guard<mutex> lock(_mutex); // TODO: FOLLOW-UP
            
            ended = true;

            LOGI_IF(TaskManager::LOG_VERBOSE) << __PRETTY_FUNCTION__ << " [END] | " << taskId << " | " << this << endl;

            /*
             * IT IS NECESSARY TO WAIT FOR THE FUNCTIONS WHICH MAY HAVE BEEN POSTED DURING Task::run()
             *
             * TODO: CONSIDER USING A LAMBDA INSTEAD OF bind
             */
            os::post(bind(&TaskManager::endTask, manager, taskId), false);
            
            /*
             * TODO:
             *
             * INVESTIGATE IN WHICH "STATE" IS THE THREAD UNTIL Task::performShutdown() IS INVOKED
             *
             * SHOULD WE BE IN A "WAIT STATE" UNTIL THEN?
             */
        }
        else
        {
            assert(false);
        }
    }
    
    /*
     * INVOKED ON THE SKETCH-THREAD
     *
     * OPTION 1: FROM TaskManager::cancelTask()
     * OPTION 2: FROM TaskManager::endTask()
     */
    
    void Task::performShutdown()
    {
        if (!started || ended)
        {
            LOGI_IF(TaskManager::LOG_VERBOSE) << __PRETTY_FUNCTION__ << " | " << taskId << " | " << this << endl;
            
            shutdown();
            
            /*
             * TODO: TRY "JOINING" INSTEAD OF "DETACHING"
             */
            detach();
        }
        else
        {
            assert(false);
        }
    }
}
