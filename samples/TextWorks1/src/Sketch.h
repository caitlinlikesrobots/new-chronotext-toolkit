/*
 * THE NEW CHRONOTEXT TOOLKIT: https://github.com/arielm/new-chronotext-toolkit
 * COPYRIGHT (C) 2012-2014, ARIEL MALKA ALL RIGHTS RESERVED.
 *
 * THE FOLLOWING SOURCE-CODE IS DISTRIBUTED UNDER THE SIMPLIFIED BSD LICENSE:
 * https://github.com/arielm/new-chronotext-toolkit/blob/master/LICENSE.md
 */

/*
 * FEATURES:
 *
 * 1) DEMONSTRATES HOW TO DRAW CRISP TEXT
 *
 * 2) DEMONSTRATES HOW TO KEEP THE SAME PHYSICAL FONT-SIZE ACROSS DEVICES
 */

/*
 * IN SYNC WITH ResRework BRANCH / COMMIT 2ae1614
 */

#pragma once

#include "chronotext/cinder/CinderSketch.h"
#include "chronotext/font/zf/FontManager.h"

class Sketch : public chr::CinderSketch
{
public:
    Sketch()
    :
    CinderSketch()
    {}
    
    void setup() final;
    void draw() final;
    
    void drawAlignedText(chr::ZFont &font, const std::string &text, const ci::Vec2f &position, chr::ZFont::Alignment alignX, chr::ZFont::Alignment alignY);

protected:
    chr::zf::FontManager fontManager;
    std::shared_ptr<chr::ZFont> font;
};
