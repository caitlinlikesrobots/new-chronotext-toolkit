/*
 * THE NEW CHRONOTEXT TOOLKIT: https://github.com/arielm/new-chronotext-toolkit
 * COPYRIGHT (C) 2012-2014, ARIEL MALKA ALL RIGHTS RESERVED.
 *
 * THE FOLLOWING SOURCE-CODE IS DISTRIBUTED UNDER THE SIMPLIFIED BSD LICENSE:
 * https://github.com/arielm/new-chronotext-toolkit/blob/master/LICENSE.md
 */

#pragma once

#include "chronotext/system/SystemInfoBase.h"

namespace chr
{
    namespace system
    {
        class Info : public InfoBase
        {
        public:
            std::string model;
            std::string machine;
            
            bool isPodTouch;
            bool isIPhone;
            bool isPad;
            bool isPadMini;
            bool isSimulator;
        };
    }
}