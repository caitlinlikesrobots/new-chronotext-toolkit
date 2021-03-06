/*
 * THE NEW CHRONOTEXT TOOLKIT: https://github.com/arielm/new-chronotext-toolkit
 * COPYRIGHT (C) 2012-2015, ARIEL MALKA ALL RIGHTS RESERVED.
 *
 * THE FOLLOWING SOURCE-CODE IS DISTRIBUTED UNDER THE SIMPLIFIED BSD LICENSE:
 * https://github.com/arielm/new-chronotext-toolkit/blob/master/LICENSE.md
 */

#include "chronotext/android/system/MemoryManager.h"
#include "chronotext/Context.h"

/*
 * MEMORY MEASUREMENT SEEMS RELIABLE:
 *
 *
 * MEASUREMENT AND LOW-MEMORY-DETECTION ARE BOTH HANDLED USING JAVA AND THE ActivityManager.MemoryInfo STRUCTURE:
 * http://developer.android.com/reference/android/app/ActivityManager.MemoryInfo.html
 *
 *
 * FINDINGS:
 *
 * 1) FREE MEMORY:
 *    - WORKS AS INTENDED
 *
 * 2) TOTAL MEMORY:
 *    - NOT ACCESSIBLE VIA JAVA UNTIL API 16
 *      - SEE "PREVIOUS TESTABLE MILESTONE" BELOW ON HOW TO QUERY IT VIA C++
 *    - BUT IN ANY CASE:
 *      - THIS VALUE IS USELESS
 *
 *
 * TODO:
 *
 * 1) TEST ALTERNATIVES FOR LOW-MEMORY-DETECTION:
 *    - USING THE ComponentCallbacks2.onTrimMemory() CALLBACK:
 *      http://developer.android.com/reference/android/content/ComponentCallbacks2.html
 */

/*
 * ADDITIONAL REFERENCES:
 *
 * - http://stackoverflow.com/a/18894037/50335
 * - http://grepcode.com/file/repository.grepcode.com/java/ext/com.google.android/android/4.1.1_r1/com/android/server/am/ActivityManagerService.java#5404
 * - http://grepcode.com/file/repository.grepcode.com/java/ext/com.google.android/android/4.1.1_r1/com/android/server/am/ProcessList.java#ProcessList.getMemLevel%28int%29
 * - http://grepcode.com/file/repository.grepcode.com/java/ext/com.google.android/android/4.1.1_r1/com/android/internal/util/MemInfoReader.java
 *
 *
 * TO QUERY VIA TERMINAL WHEN ANDROID DEVICE IS CONNECTED:
 *
 * - adb shell cat /proc/meminfo
 * - adb shell cat /sys/module/lowmemorykiller/parameters/minfree
 */

/*
 * PREVIOUS TESTABLE MILESTONE: USING C++ FOR QUERYING FREE-MEMORY INSTEAD OF JAVA
 *
 * - https://github.com/arielm/new-chronotext-toolkit/blob/ffa9f658e609f13bf969a17fc76beba3dbe22735/src/chronotext/android/system/MemoryManager.cpp
 * - https://github.com/arielm/chronotext-playground/blob/486d4c4ac02a5e471ed5a1b1cc1cee16bc1044fe/Sketches/ContextRework/src/TestingMemory.h
 */

using namespace std;
using namespace ci;

namespace chr
{
    namespace memory
    {
        void Manager::setup()
        {
            initial = getInfo();
            
            LOGI_IF(LOG_VERBOSE) << "MEMORY INFO: " << initial << endl;
        }
        
        void Manager::shutdown()
        {
            LOGI_IF(LOG_VERBOSE) << "MEMORY INFO: " << getInfo() << endl;
        }
        
        // ---
        
        /*
         * INTENDED TO BE CALLED REGULARELY (E.G. AT EACH FRAME), ON THE RENDERER'S THREAD
         */
        
        void Manager::update()
        {
            try
            {
                const JsonTree &query = delegate().jsonQuery("getMemoryInfo");
                auto lowMemory = query["lowMemory"].getValue<bool>();
                
                if (lowMemory)
                {
                    LOGI_IF(LOG_WARNING) << "ANDROID: LOW-MEMORY WARNING" << endl;
                    
                    delegate().handleEvent(CinderSketch::EVENT_MEMORY_WARNING);
                }
            }
            catch (exception &e)
            {}
        }
        
        // ---
        
        Info Manager::getInfo()
        {
            int64_t freeMemory = -1;
            int64_t usedMemory = -1;
            
            try
            {
                const JsonTree &query = delegate().jsonQuery("getMemoryInfo");
                
                auto availMem = query["availMem"].getValue<int64_t>();
                auto threshold = query["threshold"].getValue<int64_t>();
                
                // ---
                
                freeMemory = availMem - threshold;
                usedMemory = compare(initial, Info(freeMemory));
            }
            catch (exception &e)
            {}
            
            return Info(freeMemory, usedMemory);
        }
        
        int64_t Manager::compare(const Info &before, const Info &after)
        {
            if ((before.free != -1) && (after.free != -1))
            {
                return before.free - after.free;
            }
            
            return 0;
        }
    }
}
