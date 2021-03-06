/*
 * THE NEW CHRONOTEXT TOOLKIT: https://github.com/arielm/new-chronotext-toolkit
 * COPYRIGHT (C) 2012-2014, ARIEL MALKA ALL RIGHTS RESERVED.
 *
 * THE FOLLOWING SOURCE-CODE IS DISTRIBUTED UNDER THE SIMPLIFIED BSD LICENSE:
 * https://github.com/arielm/new-chronotext-toolkit/blob/master/LICENSE.md
 */

/*
 * TODO:
 *
 * 1) PROBE (CPU) TIME AS WELL
 *
 * 2) FAIL EARLIER WHEN "MAX-SIZE" IS EXCEEDED:
 *    - CURRENTLY NECESSARY IN:
 *      - fetchTextureData() WHEN CALLING PVRHelper::decompressGZ() OR PVRHelper::decompressCCZ
 *        - TRIVIAL: THE INFO LIES IN THE "PVR HEADER"
 *      - fetchTranslucentTextureData() AND fetchPowerOfTwoTextureData()
 *        - NON TRIVIAL:
 *          - CINDER'S IMAGE-IO PROTOCOL DOES NOT SEEM TO BE ADAPTED:
 *            - I.E. THERE SHOULD BE SOME USER-DEFINABLE CALLBACK, INVOKED AS-SOON-AS THE IMAGE-HEADER IS PARSED
 *              - OBVIOUSLY: MOST OF THE IMAGE-LOADING SOLUTIONS (E.G. stb_image) ALLOW TO "GUESS" IMAGE-SIZE BEFORE FULLY LOADING IT
 */

#pragma once

#include "chronotext/texture/Texture.h"
#include "chronotext/system/MemoryInfo.h"

namespace chr
{
    class TextureHelper
    {
    public:
        struct Record
        {
            int64_t memoryUsage;
            MemoryInfo memoryInfo[2];
        };
        
        static MemoryInfo memoryInfo[2];
        static std::map<ci::gl::Texture*, Record> records;

        // ---
        
        static Texture::Ref loadTexture(const Texture::Request &textureRequest);

        static ci::gl::Texture* loadTarget(const Texture::Request &textureRequest);
        static Texture::Data fetchTextureData(const Texture::Request &textureRequest);
        static ci::gl::Texture* uploadTextureData(const Texture::Data &textureData);
        
        static ci::Vec2i getTextureSize(const Texture::Data &textureData);
        static int64_t getTextureMemoryUsage(const Texture::Data &textureData);
        
    protected:
        static bool isOverSized(const Texture::Request &textureRequest, const ci::Vec2i &size);

        static Texture::Data fetchTranslucentTextureData(const Texture::Request &textureRequest);
        static Texture::Data fetchPowerOfTwoTextureData(const Texture::Request &textureRequest);
    };
}
