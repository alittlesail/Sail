#ifndef SAIL_2D_QUAD_INCLUDED
#define SAIL_2D_QUAD_INCLUDED

#include "sail_2d_image.hpp"

class Sail2DQuad : public Sail2DImage
{
public:
	Sail2DQuad()
	{
		m_texture = s_sail_gfx_2d_batch_render.GetQuadTexture();
		SetTextureCoordinate(0.1f, 0.9f, 0.1f, 0.9f);
	}
	virtual ~Sail2DQuad()
	{
	}
};

#endif
