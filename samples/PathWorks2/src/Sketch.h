/*
 * THE NEW CHRONOTEXT TOOLKIT: https://github.com/arielm/new-chronotext-toolkit
 * COPYRIGHT (C) 2012-2014, ARIEL MALKA ALL RIGHTS RESERVED.
 *
 * THE FOLLOWING SOURCE-CODE IS DISTRIBUTED UNDER THE MODIFIED BSD LICENSE:
 * https://github.com/arielm/new-chronotext-toolkit/blob/master/LICENSE.md
 */

#pragma once

#include "chronotext/cinder/CinderSketch.h"
#include "chronotext/texture/TextureManager.h"
#include "chronotext/path/SplinePath.h"
#include "chronotext/path/StrokeHelper.h"

class Sketch : public chr::CinderSketch
{
    chr::TextureManager textureManager;

    chr::TextureRef lineTexture;
    chr::TextureRef dashedLineTexture;
    chr::TextureRef roadTexture;
    chr::TextureRef dotTexture;
    
    chr::FollowablePath path1;
    chr::TexturedTriangleStrip strip1;
    
    chr::SplinePath spline2;
    chr::FollowablePath path2;
    
    std::vector<chr::FollowablePath> paths;
    std::vector<chr::TexturedTriangleStrip> strips;
    ci::Vec2f offset;
    
    float scale;
    float position;
    
public:
    Sketch(void *context, void *delegate = NULL);
    
    void setup(bool renewContext);
    void resize();
    void update();
    void draw();
    
    void drawDotOnPath(const chr::FollowablePath &path);
};
