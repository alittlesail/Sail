#ifndef SAIL_2D_IMAGE_INCLUDED
#define SAIL_2D_IMAGE_INCLUDED

#include "sokol/sokol_gfx.h"
#include "sail_2d_object.hpp"
#include "Sail/SailClient/Gfx/sail_gfx.hpp"

class Sail2DImage : public Sail2DObject
{
public:
	virtual ~Sail2DImage() {}

public:
	virtual void SetTexture(const sg_image& texture)
	{
		if (texture.id == SG_INVALID_ID) SetSelfMatrixDirty();
		m_texture = texture;
	}
	virtual void ClearTexture()
	{
		m_texture.id = SG_INVALID_ID;
		SetSelfMatrixDirty();
	}

public:
	virtual void SetTextureCoordinate(float top, float bottom, float left, float right)
	{
		m_tex_top = top;
		m_tex_bottom = bottom;
		m_tex_left = left;
		m_tex_right = right;

		m_texture_dirty = true;
	}

protected:
	void UpdateTextureCoordinate() override
	{
		if (!m_texture_dirty) return;
		m_texture_dirty = false;

		m_vertices[SailGfx2DBatchRender::U_0] = m_tex_left; m_vertices[SailGfx2DBatchRender::V_0] = m_tex_top;
		m_vertices[SailGfx2DBatchRender::U_1] = m_tex_right; m_vertices[SailGfx2DBatchRender::V_1] = m_tex_top;
		m_vertices[SailGfx2DBatchRender::U_2] = m_tex_right; m_vertices[SailGfx2DBatchRender::V_2] = m_tex_bottom;

		m_vertices[SailGfx2DBatchRender::VERTEX_ALL_FLOAT_COUNT + SailGfx2DBatchRender::U_0] = m_tex_left;
		m_vertices[SailGfx2DBatchRender::VERTEX_ALL_FLOAT_COUNT + SailGfx2DBatchRender::V_0] = m_tex_top;
		m_vertices[SailGfx2DBatchRender::VERTEX_ALL_FLOAT_COUNT + SailGfx2DBatchRender::U_1] = m_tex_right;
		m_vertices[SailGfx2DBatchRender::VERTEX_ALL_FLOAT_COUNT + SailGfx2DBatchRender::V_1] = m_tex_bottom;
		m_vertices[SailGfx2DBatchRender::VERTEX_ALL_FLOAT_COUNT + SailGfx2DBatchRender::U_2] = m_tex_left;
		m_vertices[SailGfx2DBatchRender::VERTEX_ALL_FLOAT_COUNT + SailGfx2DBatchRender::V_2] = m_tex_bottom;
	}

	void UpdateVertexCoordinate() override
	{
		if (!m_vertex_dirty) return;
		m_vertex_dirty = false;

		CarpVector2D coordinate[4];
		coordinate[0].x = 0.0f; coordinate[0].y = 0.0f;
		coordinate[1].x = static_cast<float>(m_width); coordinate[1].y = 0.0f;
		coordinate[2].x = static_cast<float>(m_width); coordinate[2].y = static_cast<float>(m_height);
		coordinate[3].x = 0.0f; coordinate[3].y = static_cast<float>(m_height);

		for (auto& i : coordinate)
            i.Multiply(m_global_matrix);

		m_vertices[SailGfx2DBatchRender::X_0] = coordinate[0].x; m_vertices[SailGfx2DBatchRender::Y_0] = coordinate[0].y;
		m_vertices[SailGfx2DBatchRender::X_1] = coordinate[1].x; m_vertices[SailGfx2DBatchRender::Y_1] = coordinate[1].y;
		m_vertices[SailGfx2DBatchRender::X_2] = coordinate[2].x; m_vertices[SailGfx2DBatchRender::Y_2] = coordinate[2].y;

		m_vertices[SailGfx2DBatchRender::VERTEX_ALL_FLOAT_COUNT + SailGfx2DBatchRender::X_0] = coordinate[0].x;
		m_vertices[SailGfx2DBatchRender::VERTEX_ALL_FLOAT_COUNT + SailGfx2DBatchRender::Y_0] = coordinate[0].y;
		m_vertices[SailGfx2DBatchRender::VERTEX_ALL_FLOAT_COUNT + SailGfx2DBatchRender::X_1] = coordinate[2].x;
		m_vertices[SailGfx2DBatchRender::VERTEX_ALL_FLOAT_COUNT + SailGfx2DBatchRender::Y_1] = coordinate[2].y;
		m_vertices[SailGfx2DBatchRender::VERTEX_ALL_FLOAT_COUNT + SailGfx2DBatchRender::X_2] = coordinate[3].x;
		m_vertices[SailGfx2DBatchRender::VERTEX_ALL_FLOAT_COUNT + SailGfx2DBatchRender::Y_2] = coordinate[3].y;
	}

	void UpdateVertexColor() override
	{
		if (!m_color_dirty) return;
		m_color_dirty = false;

		m_vertices[SailGfx2DBatchRender::R_0] = m_red;
		m_vertices[SailGfx2DBatchRender::R_1] = m_red;
		m_vertices[SailGfx2DBatchRender::R_2] = m_red;

		m_vertices[SailGfx2DBatchRender::VERTEX_ALL_FLOAT_COUNT + SailGfx2DBatchRender::R_0] = m_red;
		m_vertices[SailGfx2DBatchRender::VERTEX_ALL_FLOAT_COUNT + SailGfx2DBatchRender::R_1] = m_red;
		m_vertices[SailGfx2DBatchRender::VERTEX_ALL_FLOAT_COUNT + SailGfx2DBatchRender::R_2] = m_red;

		m_vertices[SailGfx2DBatchRender::G_0] = m_green;
		m_vertices[SailGfx2DBatchRender::G_1] = m_green;
		m_vertices[SailGfx2DBatchRender::G_2] = m_green;

		m_vertices[SailGfx2DBatchRender::VERTEX_ALL_FLOAT_COUNT + SailGfx2DBatchRender::G_0] = m_green;
		m_vertices[SailGfx2DBatchRender::VERTEX_ALL_FLOAT_COUNT + SailGfx2DBatchRender::G_1] = m_green;
		m_vertices[SailGfx2DBatchRender::VERTEX_ALL_FLOAT_COUNT + SailGfx2DBatchRender::G_2] = m_green;

		m_vertices[SailGfx2DBatchRender::B_0] = m_blue;
		m_vertices[SailGfx2DBatchRender::B_1] = m_blue;
		m_vertices[SailGfx2DBatchRender::B_2] = m_blue;

		m_vertices[SailGfx2DBatchRender::VERTEX_ALL_FLOAT_COUNT + SailGfx2DBatchRender::B_0] = m_blue;
		m_vertices[SailGfx2DBatchRender::VERTEX_ALL_FLOAT_COUNT + SailGfx2DBatchRender::B_1] = m_blue;
		m_vertices[SailGfx2DBatchRender::VERTEX_ALL_FLOAT_COUNT + SailGfx2DBatchRender::B_2] = m_blue;

		m_vertices[SailGfx2DBatchRender::A_0] = m_alpha;
		m_vertices[SailGfx2DBatchRender::A_1] = m_alpha;
		m_vertices[SailGfx2DBatchRender::A_2] = m_alpha;

		m_vertices[SailGfx2DBatchRender::VERTEX_ALL_FLOAT_COUNT + SailGfx2DBatchRender::A_0] = m_alpha;
		m_vertices[SailGfx2DBatchRender::VERTEX_ALL_FLOAT_COUNT + SailGfx2DBatchRender::A_1] = m_alpha;
		m_vertices[SailGfx2DBatchRender::VERTEX_ALL_FLOAT_COUNT + SailGfx2DBatchRender::A_2] = m_alpha;
	}

public:
	void Render(const CarpMatrix2D& parent, bool parent_changed) override
	{
		if (m_clip) return;
		if (!m_visible) return;
		if (m_alpha <= 0) return;
		if (m_texture.id == SG_INVALID_ID) return; 

		UpdateGlobalMatrix2D(parent, parent_changed);
		UpdateVertexCoordinate();
		UpdateTextureCoordinate();
		UpdateVertexColor();

		s_sail_gfx_2d_batch_render.Push(m_texture, m_vertices);
		s_sail_gfx_2d_batch_render.Push(m_texture, &m_vertices[SailGfx2DBatchRender::VERTEX_ALL_FLOAT_COUNT]);
	}

protected:
	sg_image m_texture{SG_INVALID_ID};

protected:
	float m_tex_top = 0.0f, m_tex_bottom = 1.0f, m_tex_left = 0.0f, m_tex_right = 1.0f;

protected:
	float m_vertices[SailGfx2DBatchRender::VERTEX_ALL_FLOAT_COUNT * 2]{};
};
#endif


