/*
 * THE NEW CHRONOTEXT TOOLKIT: https://github.com/arielm/new-chronotext-toolkit
 * COPYRIGHT (C) 2012-2015, ARIEL MALKA ALL RIGHTS RESERVED.
 *
 * THE FOLLOWING SOURCE-CODE IS DISTRIBUTED UNDER THE SIMPLIFIED BSD LICENSE:
 * https://github.com/arielm/new-chronotext-toolkit/blob/master/LICENSE.md
 */

#include "Sketch.h"

#include "chronotext/Context.h"
#include "chronotext/font/xf/TextHelper.h"
#include "chronotext/utils/GLUtils.h"

using namespace std;
using namespace ci;
using namespace ci::app;
using namespace chr;
using namespace chr::xf;

const float DOT_RADIUS_DP = 22;
const float DOT_RADIUS_PIXELS = 56; // SPECIFIC TO "dot_112.png"

const float FONT_SIZE = 22; // DP
const float PADDING = 22; // DP

const float FRICTION = 0.01f;
const float DT = 1.0f;

void Sketch::setup()
{
    TextureManager::LOG_VERBOSE = true;
    FontManager::LOG_VERBOSE = true;

    dot = textureManager.getTexture(InputSource::getResource("dot_112.png"), true, Texture::Request::FLAGS_TRANSLUCENT);
    font = fontManager.getFont(InputSource::getResource("Roboto_Regular_64.fnt"), XFont::Properties2d());
    
    scale = getDisplayInfo().density / DisplayInfo::REFERENCE_DENSITY;
    particle = Particle(getWindowCenter(), scale * DOT_RADIUS_DP);
    
    // ---
    
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);
}

void Sketch::start(StartReason reason)
{
    acceleration = Vec2f::zero();
    delegate().enableAccelerometer(15);
}

void Sketch::stop(StopReason reason)
{
    delegate().disableAccelerometer();
}

void Sketch::update()
{
    accumulateForces();
    verlet();
    satifsfyConstraints();
}

void Sketch::draw()
{
    gl::clear(Color::gray(1.0f), false);
    gl::setMatricesWindow(getWindowSize(), true);
    
    gl::color(Color::gray(0.5f));
    utils::gl::drawGrid(getWindowBounds(), scale * DOT_RADIUS_DP * 2, Vec2f(0, clock()->getTime() * 60));
    
    // ---
    
    drawDot(particle.position, particle.radius, ColorA(1, 0, 0, 1));
    
    string text = utils::format::time(clock()->getTime());
    drawText(text, Vec2f(0, getWindowHeight()) + Vec2f(PADDING, -PADDING) * scale, XFont::ALIGN_LEFT, XFont::ALIGN_BOTTOM, scale * FONT_SIZE, ColorA(0, 0, 0, 1));
}

void Sketch::event(int eventId)
{
    switch (eventId)
    {
        case EVENT_CONTEXT_LOST:
        {
            /*
             * DISCARDING: FOR RELEASING GL NAMES
             * AT THIS STAGE: GL MEMORY HAS ALREADY BEEN INVALIDATED
             */
            
            textureManager.discardTextures();
            fontManager.discardTextures();
            
            break;
        }
            
        case EVENT_CONTEXT_RENEWED:
        {
            textureManager.reloadTextures(); // NOT MANDATORY (TEXTURES ARE LAZILY RELOADED)
            fontManager.reloadTextures(); // NOT MANDATORY (GLYPH TEXTURES ARE LAZILY RELOADED)
            
            /*
             * DEFAULT GL STATES MUST BE RESTORED AS WELL
             */
            
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glEnable(GL_BLEND);
            
            glDisable(GL_DEPTH_TEST);
            glDepthMask(GL_FALSE);
            
            break;
        }
            
        case EVENT_MEMORY_WARNING:
        {
            /*
             * IN ORDER TO AVOID "INTERFERENCES" WITH GL NAMES:
             * RELOADING MUST TAKE PLACE ONLY AFTER EVERYTHING HAVE BEEN DISCARDED
             */
            
            textureManager.discardTextures();
            fontManager.unload();
            
            break;
        }
    }
}

bool Sketch::keyDown(const KeyEvent &keyEvent)
{
    switch (CinderDelegate::getCode(keyEvent))
    {
        case KeyEvent::KEY_w:
            event(EVENT_MEMORY_WARNING);
            return true;
    }
    
    return false;
}


void Sketch::accelerated(AccelEvent event)
{
    acceleration = Vec2f(+event.getRawData().x, -event.getRawData().y);
}

// ---

void Sketch::drawDot(const Vec2f &position, float radius, const ColorA &color)
{
    gl::color(color);
    
    glPushMatrix();
    gl::translate(position);
    gl::scale(radius / DOT_RADIUS_PIXELS);
    
    dot->begin();
    dot->drawFromCenter();
    dot->end();
    
    glPopMatrix();
}

void Sketch::drawText(const string &text, const Vec2f &position, XFont::Alignment alignX, XFont::Alignment alignY, float fontSize, const ColorA &color)
{
    font->setColor(color);
    font->setSize(fontSize);
    
    TextHelper::drawAlignedText(*font, utils::to<u16string>(text), position, alignX, alignY);
}

// ---

void Sketch::accumulateForces()
{
    particle.acceleration = particle.mass * acceleration;
}

void Sketch::verlet()
{
    auto tmp = particle.position;
    particle.position = (2 - FRICTION) * particle.position - (1 - FRICTION) * particle.previousPosition + particle.acceleration * DT * DT;
    particle.previousPosition = tmp;
}

void Sketch::satifsfyConstraints()
{
    Rectf bounds(particle.radius, particle.radius, getWindowWidth() - particle.radius, getWindowHeight() - particle.radius);
    auto velocity = particle.position - particle.previousPosition;
    
    if (particle.position.x < bounds.x1)
    {
        particle.position.x = bounds.x1 - velocity.x * 0.5f;
        particle.previousPosition.x = bounds.x1;
    }
    else if (particle.position.x > bounds.x2)
    {
        particle.position.x = bounds.x2 -  velocity.x * 0.5f;
        particle.previousPosition.x = bounds.x2;
    }
    
    if (particle.position.y < bounds.y1)
    {
        particle.position.y = bounds.y1 - velocity.y * 0.5f;
        particle.previousPosition.y = bounds.y1;
    }
    else if (particle.position.y > bounds.y2)
    {
        particle.position.y = bounds.y2 - velocity.y * 0.5f;
        particle.previousPosition.y = bounds.y2;
    }
}
