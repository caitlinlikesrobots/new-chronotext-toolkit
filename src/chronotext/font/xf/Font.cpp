/*
 * THE NEW CHRONOTEXT TOOLKIT: https://github.com/arielm/new-chronotext-toolkit
 * COPYRIGHT (C) 2012-2015, ARIEL MALKA ALL RIGHTS RESERVED.
 *
 * THE FOLLOWING SOURCE-CODE IS DISTRIBUTED UNDER THE SIMPLIFIED BSD LICENSE:
 * https://github.com/arielm/new-chronotext-toolkit/blob/master/LICENSE.md
 */

#include "chronotext/font/xf/Font.h"
#include "chronotext/font/xf/FontManager.h"

using namespace std;
using namespace ci;

namespace chr
{
    namespace xf
    {
        Font::Font(FontManager &fontManager, FontData *data, FontTexture *texture, const Properties &properties)
        :
        texture(texture),
        properties(properties),
        indices(fontManager.getIndices(properties.slotCapacity))
        {
            assert(properties.slotCapacity > 0);

            // ---
            
            glyphCount = data->glyphCount;
            glyphs = data->glyphs;
            
            baseSize = data->baseSize;
            height = data->height;
            ascent = data->ascent;
            descent = data->descent;
            lineThickness = data->lineThickness;
            underlineOffset = data->underlineOffset;
            strikethroughOffset = data->strikethroughOffset;
            spaceAdvance = data->spaceAdvance;
            
            w = data->w;
            h = data->h;
            le = data->le;
            te = data->te;
            advance = data->advance;
            
            u1 = data->u1;
            v1 = data->v1;
            u2 = data->u2;
            v2 = data->v2;
            
            // ---
            
            if (gl::isExtensionAvailable("GL_EXT_texture_filter_anisotropic"))
            {
                if (properties.useAnisotropy)
                {
                    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &anisotropy);
                }
                else
                {
                    anisotropy = 1;
                }
            }
            else
            {
                anisotropy = 0;
            }
            
            // ---
            
            setSize(baseSize);
            setDirection(+1);
            setAxis(+1, +1);
            setColor(0, 0, 0, 1);
        }
        
        bool Font::isSpace(char16_t c) const
        {
            return (c == 0x20) || (c == 0xa0);
        }
        
        bool Font::isValid(char16_t c) const
        {
            return glyphs.count(c);
        }
        
        int Font::getGlyphIndex(char16_t c) const
        {
            if (isSpace(c))
            {
                return -2;
            }
            
            auto it = glyphs.find(c);
            
            if (it != glyphs.end())
            {
                return it->second;
            }
            
            return -1; // SHALL WE USE A "MISSING GLYPH" AND RETURN ITS INDEX?
        }
        
        u16string Font::getCharacters() const
        {
            u16string characters;
            characters.reserve(glyphs.size());
            
            for (auto &glyph : glyphs)
            {
                characters.push_back(glyph.first);
            }
            
            return characters;
        }
        
        void Font::setSize(float size)
        {
            Font::size = size;
            sizeRatio = size / baseSize;
        }
        
        void Font::setMiddleLineFactor(float factor)
        {
            middleLineFactor = factor;
        }
        
        void Font::setDirection(float direction)
        {
            Font::direction = direction;
        }
        
        void Font::setAxis(const Vec2f &axis)
        {
            Font::axis = axis;
        }
        
        void Font::setColor(const ColorA &color)
        {
            Font::color = color;
        }
        
        void Font::setColor(float r, float g, float b, float a)
        {
            color.r = r;
            color.g = g;
            color.b = b;
            color.a = a;
        }
        
        void Font::setClip(const Rectf &clipRect)
        {
            Font::clipRect = clipRect;
            hasClip = true;
        }
        
        void Font::setClip(float x1, float y1, float x2, float y2)
        {
            clipRect.x1 = x1;
            clipRect.y1 = y1;
            clipRect.x2 = x2;
            clipRect.y2 = y2;
            
            hasClip = true;
        }
        
        void Font::clearClip()
        {
            hasClip = false;
        }
        
        float Font::getSize() const
        {
            return size;
        }
        
        float Font::getDirection() const
        {
            return direction * axis.x;
        }
        
        const Vec2f& Font::getAxis() const
        {
            return axis;
        }
        
        float Font::getGlyphAdvance(int glyphIndex) const
        {
            if (glyphIndex == -2)
            {
                return spaceAdvance * sizeRatio;
            }
            
            if (glyphIndex == -1)
            {
                return 0;
            }
            
            return advance[glyphIndex] * sizeRatio;
        }
        
        float Font::getCharAdvance(char16_t c) const
        {
            return getGlyphAdvance(getGlyphIndex(c));
        }
        
        float Font::getStringAdvance(const u16string &s) const
        {
            return getSubStringAdvance(s, 0, s.size());
        }
        
        float Font::getSubStringAdvance(const u16string &s, int begin, int end) const
        {
            float advance = 0;
            
            for (int i = begin; i < end; i++)
            {
                advance += getCharAdvance(s.at(i));
            }
            
            return advance;
        }
        
        float Font::getHeight() const
        {
            return height * sizeRatio;
        }
        
        float Font::getAscent() const
        {
            return ascent * sizeRatio;
        }
        
        float Font::getDescent() const
        {
            return descent * sizeRatio;
        }

        float Font::getLineThickness() const
        {
            return lineThickness * sizeRatio;
        }

        float Font::getUnderlineOffset() const
        {
            return underlineOffset * sizeRatio;
        }
        
        float Font::getStrikethroughOffset() const
        {
            return strikethroughOffset * sizeRatio;
        }
       
        float Font::getOffsetX(const u16string &text, Alignment align) const
        {
            switch (align)
            {
                case ALIGN_MIDDLE:
                    return -0.5f * getStringAdvance(text) * direction;

                case ALIGN_LEFT:
                    return (direction > 0) ? 0 : getStringAdvance(text);

                case ALIGN_RIGHT:
                    return (direction > 0) ? -getStringAdvance(text) : 0;
                    
                default:
                    return 0;
            }
        }
        
        float Font::getOffsetY(Alignment align) const
        {
            switch (align)
            {
                case ALIGN_MIDDLE:
                    return (middleLineFactor != 0) ? (middleLineFactor * (ascent - descent) * sizeRatio) : getStrikethroughOffset();
                    
                case ALIGN_TOP:
                    return +getAscent();
                    
                case ALIGN_BOTTOM:
                    return -getDescent();
                    
                default:
                    return 0;
            }
        }
        
        void Font::begin(bool useColor)
        {
            glEnable(GL_TEXTURE_2D);
            
            if (useColor)
            {
                glEnableClientState(GL_COLOR_ARRAY);
            }
            else
            {
                gl::color(color);
            }
            
            glEnableClientState(GL_VERTEX_ARRAY);
            glEnableClientState(GL_TEXTURE_COORD_ARRAY);
            
            // ---
            
            texture->bind(); // RELOADS TEXTURE, IF NECESSARY
            
            if (anisotropy)
            {
                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, anisotropy);
            }
        }
        
        void Font::end(bool useColor)
        {
            glDisable(GL_TEXTURE_2D);
            
            if (useColor)
            {
                glDisableClientState(GL_COLOR_ARRAY);
            }
            
            glDisableClientState(GL_VERTEX_ARRAY);
            glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        }
        
        void Font::beginSequence(FontSequence *sequence, bool useColor)
        {
            if (began == 0)
            {
                sequenceUseColor = useColor;
                
                if (!batch)
                {
                    batch = unique_ptr<QuadBatch>(new QuadBatch);
                }
                else
                {
                    batch->clear();
                }
                
                if (sequence)
                {
                    Font::sequence = sequence;
                    sequence->begin(useColor);
                }
                
                clearClip();
            }
            
            began++;
        }
        
        void Font::endSequence()
        {
            began--;
            
            if (began == 0)
            {
                if (sequence)
                {
                    batch->pack();
                    sequence->addBatch(move(batch));
                    
                    sequence->end();
                    sequence = nullptr;
                }
                else
                {
                    begin(sequenceUseColor);
                    batch->flush(getIndices(), sequenceUseColor);
                    end(sequenceUseColor);
                }
            }
        }
        
        void Font::replaySequence(FontSequence *sequence)
        {
            begin(sequence->useColor);
            sequence->replay(getIndices());
            end(sequence->useColor);
        }
        
        void Font::incrementSequence()
        {
            if (sequenceUseColor)
            {
                batch->addColor(color);
            }
            
            if (batch->size() == properties.slotCapacity)
            {
                if (sequence)
                {
                    batch->pack();
                    sequence->addBatch(move(batch));
                }
                else
                {
                    begin(sequenceUseColor);
                    batch->flush(getIndices(), sequenceUseColor);
                    end(sequenceUseColor);
                }
                
                if (!batch)
                {
                    batch = unique_ptr<QuadBatch>(new QuadBatch);
                }
                else
                {
                    batch->clear();
                }
            }
        }
        
        bool Font::fillQuad(Quad &quad, int glyphIndex, float x, float y) const
        {
            if (glyphIndex < 0)
            {
                return false;
            }
            
            if (direction * axis.x > 0)
            {
                quad.x1 = x + le[glyphIndex] * sizeRatio;
                quad.x2 = quad.x1 + w[glyphIndex] * sizeRatio;
            }
            else
            {
                quad.x2 = x - le[glyphIndex] * sizeRatio;
                quad.x1 = quad.x2 - w[glyphIndex] * sizeRatio;
            }
            
            if (axis.x > 0)
            {
                quad.u1 = u1[glyphIndex];
                quad.u2 = u2[glyphIndex];
            }
            else
            {
                quad.u1 = u2[glyphIndex];
                quad.u2 = u1[glyphIndex];
            }
            
            if (axis.y > 0)
            {
                quad.y1 = y - te[glyphIndex] * sizeRatio;
                quad.y2 = quad.y1 + h[glyphIndex] * sizeRatio;
                
                quad.v1 = v1[glyphIndex];
                quad.v2 = v2[glyphIndex];
            }
            else
            {
                quad.y2 = y + te[glyphIndex] * sizeRatio;
                quad.y1 = quad.y2 - h[glyphIndex] * sizeRatio;
                
                quad.v1 = v2[glyphIndex];
                quad.v2 = v1[glyphIndex];
            }
            
            return true;
        }
        
        bool Font::clipQuad(Quad &quad) const
        {
            return quad.clip(clipRect, texture->getSize() * sizeRatio / axis);
        }

        void Font::addGlyph(int glyphIndex, float x, float y, float z)
        {
            Quad quad;
            
            if (fillQuad(quad, glyphIndex, x, y))
            {
                if (!hasClip || clipQuad(quad))
                {
                    batch->addQuad(quad, z);
                    incrementSequence();
                }
            }
        }
        
        void Font::addTransformedGlyph(int glyphIndex, float x, float y)
        {
            Quad quad;
            
            if (fillQuad(quad, glyphIndex, x, y))
            {
                if (!hasClip || clipQuad(quad))
                {
                    matrix.addTransformedQuad(quad, batch->vertices);
                    incrementSequence();
                }
            }
        }
    }
}
