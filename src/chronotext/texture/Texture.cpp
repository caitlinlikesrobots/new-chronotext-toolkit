/*
 * THE NEW CHRONOTEXT TOOLKIT: https://github.com/arielm/new-chronotext-toolkit
 * COPYRIGHT (C) 2012-2014, ARIEL MALKA ALL RIGHTS RESERVED.
 *
 * THE FOLLOWING SOURCE-CODE IS DISTRIBUTED UNDER THE SIMPLIFIED BSD LICENSE:
 * https://github.com/arielm/new-chronotext-toolkit/blob/master/LICENSE.md
 */

#include "Texture.h"

#include "chronotext/texture/TextureHelper.h"
#include "chronotext/Context.h"

using namespace std;
using namespace ci;
using namespace context;

namespace chr
{
    bool Texture::VERBOSE = false;
    
    Texture::Texture(InputSource::Ref inputSource, bool useMipmap, Request::Flags flags)
    :
    Texture(Request(inputSource, useMipmap, flags))
    {}
    
    Texture::Texture(const Request &request)
    :
    Texture(TextureHelper::fetchTextureData(request))
    {}
    
    Texture::Texture(const Data &data)
    :
    request(data.request),
    glId(0)
    {
        setTarget(TextureHelper::uploadTextureData(data));
        memoryUsage = TextureHelper::getTextureMemoryUsage(data);
    }
    
    Texture::~Texture()
    {
        resetTarget();
    }
    
    void Texture::discard()
    {
        resetTarget();
    }
    
    bool Texture::reload()
    {
        if (!target)
        {
            setTarget(TextureHelper::loadTexture(request));
        }
        
        return bool(target);
    }
    
    int Texture::getWidth() const
    {
        return width;
    }
    
    int Texture::getHeight() const
    {
        return height;
    }
    
    Vec2i Texture::getSize() const
    {
        return Vec2i(width, height);
    }
    
    int Texture::getCleanWidth() const
    {
        return width * maxU;
    }
    
    int Texture::getCleanHeight() const
    {
        return height * maxV;
    }
    
    Vec2i Texture::getCleanSize() const
    {
        return Vec2i(width * maxU, height * maxV);
    }
    
    float Texture::getMaxU() const
    {
        return maxU;
    }
    
    float Texture::getMaxV() const
    {
        return maxV;
    }
    
    Vec2f Texture::getMaxUV() const
    {
        return Vec2f(maxU, maxV);
    }
    
    int64_t Texture::getMemoryUsage() const
    {
        return memoryUsage;
    }
    
    void Texture::setTarget(ci::gl::TextureRef target)
    {
        if (!Texture::target)
        {
            Texture::target = target;
            glId = target->getId();
            
            // ---
            
            string memoryStats;
            
            if (VERBOSE && TextureHelper::PROBE_MEMORY)
            {
                auto memoryInfo = getMemoryInfo();
                const auto &memoryProbe = TextureHelper::probes[target.get()];
                
                auto delta1 = memoryManager().compare(memoryProbe.memoryInfo[0], memoryProbe.memoryInfo[1]);
                auto delta2 = memoryManager().compare(memoryProbe.memoryInfo[1], memoryInfo);
                
                memoryStats = " | " +
                MemoryInfo::write(memoryProbe.memoryUsage) + ", " +
                MemoryInfo::write(delta1) + ", " +
                MemoryInfo::write(delta2);
            }
            
            // ---
            
            width = target->getWidth();
            height = target->getHeight();
            maxU = target->getMaxU();
            maxV = target->getMaxV();
            
            // ---
            
            LOGI_IF(VERBOSE) <<
            "TEXTURE UPLOADED: " <<
            request.inputSource->getFilePathHint() << " | " <<
            glId << " | " <<
            width << "x" << height <<
            memoryStats <<
            endl;
        }
    }
    
    void Texture::resetTarget()
    {
        if (target)
        {
            auto previousTarget = target.get();
            auto previousGLId = glId;
            
            target.reset();
            glId = 0;
            
            // ---
            
            string memoryStats;
            
            if (VERBOSE && TextureHelper::PROBE_MEMORY)
            {
                auto memoryInfo = getMemoryInfo();
                const auto &memoryProbe = TextureHelper::probes[previousTarget];
                
                auto delta = -memoryManager().compare(memoryProbe.memoryInfo[2], memoryInfo);
                
                memoryStats = " | " +
                MemoryInfo::write(memoryProbe.memoryUsage) + ", " +
                MemoryInfo::write(delta);
            }
            
            LOGI_IF(VERBOSE) <<
            "TEXTURE DISCARDED: " <<
            previousGLId <<
            memoryStats <<
            endl;
        }
    }
    
    void Texture::bind()
    {
        reload();
        glBindTexture(GL_TEXTURE_2D, glId);
    }
    
    void Texture::begin()
    {
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glEnable(GL_TEXTURE_2D);
        
        bind();
    }
    
    void Texture::end()
    {
        glDisable(GL_TEXTURE_2D);
        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    }
    
    void Texture::drawFromCenter()
    {
        draw(width * maxU * 0.5f, height * maxV * 0.5f);
    }
    
    void Texture::draw(float rx, float ry)
    {
        float x1 = -rx;
        float y1 = -ry;
        
        float x2 = x1 + width * maxU;
        float y2 = y1 + height * maxV;
        
        const float vertices[] =
        {
            x1, y1,
            x2, y1,
            x2, y2,
            x1, y2
        };
        
        const float coords[] =
        {
            0, 0,
            maxU, 0,
            maxU, maxV,
            0, maxV
        };
        
        glTexCoordPointer(2, GL_FLOAT, 0, coords);
        glVertexPointer(2, GL_FLOAT, 0, vertices);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    }
    
    void Texture::drawInRect(const Rectf &rect, float ox, float oy)
    {
        const float vertices[] =
        {
            rect.x1, rect.y1,
            rect.x2, rect.y1,
            rect.x2, rect.y2,
            rect.x1, rect.y2
        };
        
        float u1 = (rect.x1 - ox) / width;
        float v1 = (rect.y1 - oy) / height;
        float u2 = (rect.x2 - ox) / width;
        float v2 = (rect.y2 - oy) / height;
        
        const float coords[] =
        {
            u1, v1,
            u2, v1,
            u2, v2,
            u1, v2
        };
        
        glTexCoordPointer(2, GL_FLOAT, 0, coords);
        glVertexPointer(2, GL_FLOAT, 0, vertices);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    }
}
