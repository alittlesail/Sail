#ifndef SAIL_UI_TEXTURE_INCLUDED
#define SAIL_UI_TEXTURE_INCLUDED

#include <memory>
#include "sokol/sokol_gfx.h"

class SailUITexture
{
public:
    SailUITexture(int width, int height, const sg_image& texture)
        : m_width(width), m_height(height), m_texture(texture)
    {
    }
    virtual ~SailUITexture()
    {
        if (m_texture.id != SG_INVALID_ID)
        {
            sg_destroy_image(m_texture);
            m_texture.id = SG_INVALID_ID;
        }
    }

public:
    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }
    const sg_image& GetTexture() const { return m_texture; }

private:
    int m_width = 0;
    int m_height = 0;
    sg_image m_texture{SG_INVALID_ID};
};

using SailUITexturePtr = std::shared_ptr<SailUITexture>;
using SailUITextureWeakPtr = std::weak_ptr<SailUITexture>;

#endif
