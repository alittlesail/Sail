#ifndef SAIL_2D_TEXT_INCLUDED
#define SAIL_2D_TEXT_INCLUDED

#include "sail_2d_image.hpp"
#include "Carp/carp_font.hpp"
#include "Sail/SailClient/sail_font.hpp"

struct Sail2DTextTextureInfo
{
	sg_image texture{SG_INVALID_ID};
	int width = 0;
	int height = 0;
	std::shared_ptr<CarpFont> font = nullptr;
	int ref_count = 0;
	std::string text;
};

extern Sail2DTextTextureInfo* CreateSail2DTextureInfo(const std::shared_ptr<CarpFont>& font, const std::string& text);
extern void ReleaseSail2DTextureInfo(Sail2DTextTextureInfo* info);
extern std::unordered_map<std::shared_ptr<CarpFont>, std::unordered_map<std::string, Sail2DTextTextureInfo*>> s_sail_2d_text_texture_map;

class Sail2DText : public Sail2DImage
{
public:
	virtual ~Sail2DText()
	{
		if (m_texture_info)
		{
			ReleaseSail2DTextureInfo(m_texture_info);
			m_texture_info = nullptr;
			m_texture.id = SG_INVALID_ID;
			m_real_width = m_real_height = 0;
		}
	}

public:
	int GetRealWidth()
	{
		if (m_calc_real_size == true) return m_real_width;

		if (m_text.empty()) return 0;
		if (m_font_path.empty()) return 0;
		if (m_font_size == 0) return 0;
		if (m_font == nullptr) m_font = s_sail_font.GetFont(m_font_path, m_font_size, m_font_style);
		if (m_font == nullptr) return 0;

		// calc size
		m_real_width = m_font->CutTextWidth(m_text.c_str(), false);
		m_real_height = m_font->GetFontHeight();

		m_calc_real_size = true;

		return m_real_width;
	}
	int GetRealHeight()
	{
		if (m_calc_real_size == true) return m_real_height;

		if (m_text.empty()) return 0;
		if (m_font_path.empty()) return 0;
		if (m_font_size == 0) return 0;

		if (m_font == nullptr)
			m_font = s_sail_font.GetFont(m_font_path, m_font_size, m_font_style);
		if (m_font == nullptr) return 0;

		// calc size
		m_real_width = m_font->CutTextWidth(m_text.c_str(), false);
		m_real_height = m_font->GetFontHeight();

		m_calc_real_size = true;
		return m_real_height;
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

		m_font = nullptr;
		m_need_redraw = true;
		m_calc_real_size = false;
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
		m_calc_real_size = false;
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

		m_font = nullptr;
		m_need_redraw = true;
		m_calc_real_size = false;
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
		m_calc_real_size = false;
	}
	bool GetItalic() const { return (m_font_style & CARP_FONT_STYLE_ITALIC) != 0; }

public:
	void SetFontPath(const std::string& font_path)
	{
		if (m_font_path == font_path) return;
		m_font_path = font_path;
		m_font = nullptr;

		m_need_redraw = true;
		m_calc_real_size = false;
	}
	const std::string& GetFontPath() const { return m_font_path; }
	void SetFontSize(int font_size)
	{
		if (m_font_size == font_size) return;
		m_font_size = font_size;
		m_font = nullptr;

		m_need_redraw = true;
		m_calc_real_size = false;
	}
	int GetFontSize() const { return m_font_size; }
	void SetText(const std::string& text)
	{
		m_text = text;

		m_need_redraw = true;
		m_calc_real_size = false;
	}
	const std::string& GetText() const { return m_text; }
	int GetFontHeight()
	{
		if (m_font == nullptr) m_font = s_sail_font.GetFont(m_font_path, m_font_size, m_font_style);
		if (m_font == nullptr) return 0;

		return m_font->GetFontHeight();
	}
	int CutTextByWidth(float width, const char* content, int max_width)
	{
		if (m_font == nullptr) m_font = s_sail_font.GetFont(m_font_path, m_font_size, m_font_style);
		if (m_font == nullptr) return 0;

		m_calc_width_list.clear();
		return m_font->CutTextByWidth(content, static_cast<int>(width), max_width, false, &m_calc_width_list);
	}
	int GetCutWidthListCount() const { return static_cast<int>(m_calc_width_list.size()); }
	int GetCutWidthByIndex(int index) const { return m_calc_width_list[index]; }
	void ClearCutWidthCache() { m_calc_width_list.clear(); }
	int CalcTextWidth(const char* content)
	{
		if (m_font == nullptr) m_font = s_sail_font.GetFont(m_font_path, m_font_size, m_font_style);
		if (m_font == nullptr) return 0;

		return m_font->CutTextWidth(content, false);
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
			Draw();
			m_need_redraw = false;
			m_calc_real_size = true;
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
	void Draw()
	{
		// release current texture
		if (m_texture_info)
		{
			ReleaseSail2DTextureInfo(m_texture_info);
			m_texture_info = nullptr;
			m_texture.id = SG_INVALID_ID;
			m_real_width = m_real_height = 0;
		}

		if (m_text.empty()) return;
		if (m_font == nullptr) m_font = s_sail_font.GetFont(m_font_path, m_font_size, m_font_style);
		if (m_font == nullptr) return;

		m_texture_info = CreateSail2DTextureInfo(m_font, m_text);
		if (m_texture_info == nullptr) return;
		m_texture = m_texture_info->texture;
		m_real_width = m_texture_info->width;
		m_real_height = m_texture_info->height;
	}

private:
	bool m_need_redraw = false;
	Sail2DTextTextureInfo* m_texture_info = nullptr;

private:
	bool	m_calc_real_size = false;
	int m_real_width = 0, m_real_height = 0;

private:
	std::string m_text;
	unsigned int m_font_style = CARP_FONT_STYLE_NORMAL;
	std::string m_font_path;
	int m_font_size = 0;
	std::shared_ptr<CarpFont> m_font;

private:
	std::vector<int> m_calc_width_list;
};

#endif

#ifdef SAIL_2D_TEXT_IMPL
#ifndef SAIL_2D_TEXT_IMPL_INCLUDE
#define SAIL_2D_TEXT_IMPL_INCLUDE

std::unordered_map<std::shared_ptr<CarpFont>, std::unordered_map<std::string, Sail2DTextTextureInfo*>> s_sail_2d_text_texture_map;

Sail2DTextTextureInfo* CreateSail2DTextureInfo(const std::shared_ptr<CarpFont>& font, const std::string& text)
{
	auto& text_map = s_sail_2d_text_texture_map[font];
	auto it = text_map.find(text);
	if (it != text_map.end())
	{
		++it->second->ref_count;
		return it->second;
	}

	// create surface
	int width = 0, height = 0;
	auto image = s_sail_font.CreateTexture(font, text.c_str(), width, height);
	if (image.id == SG_INVALID_ID)
	{
		CARP_ERROR("Font Helper create surface failed!");
		return nullptr;
	}

	auto* info = new Sail2DTextTextureInfo();
	info->font = font;
	info->ref_count = 1;
	info->text = text;
	info->width = width;
	info->height = height;
	info->texture = image;
	text_map[text] = info;

	return info;
}

void ReleaseSail2DTextureInfo(Sail2DTextTextureInfo* info)
{
	--info->ref_count;
	if (info->ref_count > 0) return;

	auto text_it = s_sail_2d_text_texture_map.find(info->font);
	if (text_it == s_sail_2d_text_texture_map.end()) return;
	auto font_it = text_it->second.find(info->text);
	if (font_it == text_it->second.end()) return;

	if (font_it->second->texture.id != SG_INVALID_ID)
	{
		sg_destroy_image(font_it->second->texture);
		font_it->second->texture.id = SG_INVALID_ID;
	}
	delete font_it->second;
	text_it->second.erase(font_it);
	if (text_it->second.empty()) s_sail_2d_text_texture_map.erase(text_it);
}

#endif
#endif