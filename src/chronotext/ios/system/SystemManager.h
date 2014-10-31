/*
 * THE NEW CHRONOTEXT TOOLKIT: https://github.com/arielm/new-chronotext-toolkit
 * COPYRIGHT (C) 2012-2014, ARIEL MALKA ALL RIGHTS RESERVED.
 *
 * THE FOLLOWING SOURCE-CODE IS DISTRIBUTED UNDER THE MODIFIED BSD LICENSE:
 * https://github.com/arielm/new-chronotext-toolkit/blob/master/LICENSE.md
 */

#pragma once

#include "chronotext/system/SystemManagerBase.h"

namespace chronotext
{
    class SystemManager : public SystemManagerBase
    {
    public:
        static SystemManager& instance();
        static SystemInfo getSystemInfo();
        
    protected:
        SystemManager();
        void updateSystemInfo();
        
        std::string getDeviceString();
        
        std::string getModel();
        std::string getMachine();
        
        bool isPodTouch();
        bool isIPhone();
        bool isPad();
        bool isPadMini();
        bool isSimulator();
    };
}