#ifndef SAIL_2D_TEXTAREA_INCLUDED
#define SAIL_2D_TEXTAREA_INCLUDED

#include "sail_2d_image.hpp"
#include "Carp/carp_font.hpp"
#include "Carp/carp_string.hpp"
#include "Sail/SailClient/sail_font.hpp"

class Sail2DTextArea : public Sail2DImage
{
public:
	virtual ~Sail2DTextArea()
	{
		if (m_texture.id != SG_INVALID_ID)
		{
			sg_destroy_image(m_texture);
			m_texture.id = SG_INVALID_ID;
		}
	}

public:
	void SetBold(bool bold)
	{
		const bool is_bold = (m_font_style & CARP_FONT_STYLE_BOLD) != 0;
		if (is_bold == bold) return;

		if (bold)
			m_font_style |= CARP_FONT_STYLE_BOLD;
		else
			m_font_style &= ~CARP_FONT_STYLE_BOLD;

		m_font = 0;
		m_need_redraw = true;
	}
	bool GetBold() const { return (m_font_style & CARP_FONT_STYLE_BOLD) != 0; }
	void SetUnderline(bool underline)
	{
		const bool is_underline = (m_font_style & CARP_FONT_STYLE_UNDERLINE) != 0;
		if (is_underline == underline) return;

		if (underline)
			m_font_style |= CARP_FONT_STYLE_UNDERLINE;
		else
			m_font_style &= ~CARP_FONT_STYLE_UNDERLINE;

		m_font = 0;
		m_need_redraw = true;
	}
	bool GetUnderline() const { return (m_font_style & CARP_FONT_STYLE_UNDERLINE) != 0; }
	void SetDeleteline(bool deleteline)
	{
		const bool is_deleteline = (m_font_style & CARP_FONT_STYLE_DELETELINE) != 0;
		if (is_deleteline == deleteline) return;

		if (deleteline)
			m_font_style |= CARP_FONT_STYLE_DELETELINE;
		else
			m_font_style &= ~CARP_FONT_STYLE_DELETELINE;

		m_font = 0;
		m_need_redraw = true;
	}
	bool GetDeleteline() const { return (m_font_style & CARP_FONT_STYLE_DELETELINE) != 0; }
	void SetItalic(bool italic)
	{
		const bool is_italic = (m_font_style & CARP_FONT_STYLE_ITALIC) != 0;
		if (is_italic == italic) return;

		if (italic)
			m_font_style |= CARP_FONT_STYLE_ITALIC;
		else
			m_font_style &= ~CARP_FONT_STYLE_ITALIC;

		m_font = nullptr;
		m_need_redraw = true;
	}
	bool GetItalic() const { return (m_font_style & CARP_FONT_STYLE_ITALIC) != 0; }

public:
	void SetFontPath(const std::string& font_path)
	{
		if (m_font_path == font_path) return;
		m_font_path = font_path;
		m_font = nullptr;
		m_need_redraw = true;
	}
	const std::string& GetFontPath() const { return m_font_path; }
	void SetFontSize(int font_size)
	{
		if (m_font_size == font_size) return;
		m_font_size = font_size;
		m_font = nullptr;
		m_need_redraw = true;
	}
	int GetFontSize() const { return m_font_size; }
	void SetText(const std::string& text)
	{
		m_text = text;
		m_need_redraw = true;
	}
	const std::string& GetText() const { return m_text; }

public:
	enum HAlign
	{
		HALIGN_LEFT,
		HALIGN_CENTER,
		HALIGN_RIGHT
	};

	enum VAlign
	{
		VALIGN_TOP,
		VALIGN_CENTER,
		VALIGN_BOTTOM
	};

	void SetHAlign(int align)
	{
		if (m_halign == align) return;

		m_halign = align;
		m_need_redraw = true;
	}
	void SetVAlign(int align)
	{
		if (m_valign == align) return;

		m_valign = align;
		m_need_redraw = true;
	}

public:
	int GetRealHeight()
	{
		if (m_need_redraw) Draw(false);
		return m_real_height;
	}

public:
	void NeedDraw() { m_need_redraw = true; }
	void Render(const CarpMatrix2D& parent, bool parent_changed) override
	{
		if (m_clip) return;
		if (!m_visible) return;
		if (m_alpha <= 0) return;

		if (m_need_redraw)
		{
			Draw(true);
			m_need_redraw = false;
			SetSelfMatrixDirty();
		}

		if (m_texture.id == SG_INVALID_ID) return;

		UpdateGlobalMatrix2D(parent, parent_changed);
		UpdateVertexCoordinate();
		UpdateTextureCoordinate();
		UpdateVertexColor();

		s_sail_gfx_2d_batch_render.Push(m_texture, m_vertices);
		s_sail_gfx_2d_batch_render.Push(m_texture, &m_vertices[SailGfx2DBatchRender::VERTEX_ALL_FLOAT_COUNT]);
	}

private:
	void Draw(bool draw)
	{
		// if texture exist, release first
		if (m_texture.id != SG_INVALID_ID)
		{
			sg_destroy_image(m_texture);
			m_texture.id = SG_INVALID_ID;
		}

		m_real_height = 0;

		if (m_text.empty()) return;
		if (m_font_path.empty()) return;
		if (m_font_size == 0) return;
		if (m_font == nullptr) m_font = s_sail_font.GetFont(m_font_path.c_str(), m_font_size, m_font_style);
		if (m_font == nullptr) return;

		int width = m_width;
		int height = m_height;
		// set total size
		int total_width = width;
		int total_height = height;

		// get font height
		int font_height = m_font->GetFontHeight();
		// start layout
		const char* str = m_text.c_str();
		// remain size
		int width_offset = 0;
		int height_offset = 0;
		int gap = m_font->GetLineGap();
		std::vector<int> height_offset_array, width_offset_array;
		std::vector<std::string> string_array;
		// calc text
		std::string calc_text;
		while (true)
		{
			int byte_count = CarpString::UTF8GetByteCountOfOneWord(*str);

			// check char
			if (byte_count == 1)
			{
				if (*str == '\0')
				{
					width_offset_array.push_back(width_offset);
					height_offset_array.push_back(height_offset);
					string_array.push_back(calc_text);

					break;
				}
				else if (*str == '\r')
				{
					str += byte_count;
					continue;
				}
				else if (*str == '\n')
				{
					str += byte_count;

					height_offset_array.push_back(height_offset);
					height_offset = gap + height_offset + font_height;

					width_offset_array.push_back(width_offset);
					string_array.push_back(calc_text);

					width_offset = 0;
					calc_text = "";

					continue;
				}
			}

			std::string next_calc_text = calc_text;
			if (*str == '\t' && byte_count == 1)
				next_calc_text.append("    ");
			else
				next_calc_text.append(str, byte_count);

			int text_width = m_font->CutTextWidth(next_calc_text.c_str(), false);
			int next_width_offset = text_width;

			if (next_width_offset > total_width && !calc_text.empty())
			{
				height_offset_array.push_back(height_offset);
				height_offset = gap + height_offset + font_height;

				width_offset_array.push_back(width_offset);
				string_array.push_back(calc_text);

				width_offset = 0;
				calc_text = "";

				continue;
			}

			str += byte_count;
			width_offset = next_width_offset;
			calc_text = next_calc_text;
		}
		// total line
		int line_count = static_cast<int>(height_offset_array.size());
		// total height
		m_real_height = height_offset_array[line_count - 1] + font_height;

		// if not need to draw then return
		if (draw == false) return;

		// at least 1*1
		if (total_width <= 0 || total_height <= 0) return;

		// create surface
		auto* total_surface = new CarpSurface(total_width, total_height);

		// offset at height
		height_offset = 0;
		if (m_valign == VALIGN_CENTER)
			height_offset = static_cast<int>((total_height - m_real_height) * 0.5f);
		else if (m_valign == VALIGN_BOTTOM)
			height_offset = total_height - m_real_height;
		// read line
		for (int i = 0; i < line_count; ++i)
		{
			const int current_height_offset = height_offset_array[i] + height_offset;
			// check visible or not
			if (current_height_offset > total_height || current_height_offset + font_height <= 0)
				continue;
			// create text surface
			auto* surface = s_sail_font.CreateSurface(m_font.get(), string_array[i].c_str());
			if (!surface) continue;

			// offset at width
			width_offset = 0;
			if (m_halign == HALIGN_CENTER)
				width_offset = static_cast<int>((total_width - surface->GetWidth()) * 0.5f);
			else if (m_halign == HALIGN_RIGHT)
				width_offset = total_width - surface->GetWidth();
			// copy surface
			CarpSurfaceRect src_rect;
			src_rect.x = width_offset < 0 ? -width_offset : 0;
			src_rect.w = width_offset < 0 ? total_width : surface->GetWidth();
			src_rect.y = current_height_offset < 0 ? -current_height_offset : 0;
			src_rect.h = surface->GetHeight() - src_rect.y;

			CarpSurfaceRect dst_rect;
			dst_rect.x = width_offset < 0 ? 0 : width_offset;
			dst_rect.y = current_height_offset < 0 ? 0 : current_height_offset;
			dst_rect.w = src_rect.w;
			dst_rect.h = src_rect.h;

			total_surface->CopyFrom(surface, &src_rect, dst_rect.x, dst_rect.y);
			delete surface;
		}

		auto image = sg_alloc_image();
		if (image.id == SG_INVALID_ID)
		{
			delete total_surface;
			return;
		}

		sg_image_desc desc{};
		desc.width = total_surface->GetWidth();
		desc.height = total_surface->GetHeight();
		desc.pixel_format = SG_PIXELFORMAT_RGBA8;
		desc.min_filter = SG_FILTER_LINEAR;
		desc.mag_filter = SG_FILTER_LINEAR;
		desc.data.subimage[0][0].ptr = total_surface->GetPixels();
		desc.data.subimage[0][0].size = total_surface->GetWidth() * total_surface->GetHeight() * 4;
		sg_init_image(image, desc);
		delete total_surface;

		m_texture = image;
	}

private:
	bool m_need_redraw = false;

private:
	int m_halign = HALIGN_LEFT;
	int m_valign = VALIGN_TOP;
	int m_real_height = 0;

private:
	std::string m_text;
	unsigned int m_font_style = CARP_FONT_STYLE_NORMAL;
	std::string m_font_path;
	int m_font_size = 0;
	std::shared_ptr<CarpFont> m_font;
};

#endif