#ifndef SAIL_FONT_INCLUDED
#define SAIL_FONT_INCLUDED

#include <map>

#include "Carp/carp_file.hpp"
#include "Carp/carp_font.hpp"
#include "Carp/carp_log.hpp"
#include "Carp/carp_surface.hpp"

class SailFont
{
public:
	~SailFont() { }

public:
	std::shared_ptr<CarpFont> GetFont(const char* font_path, unsigned int font_size, unsigned int font_style)
	{
		// check font path
		if (!font_path) return nullptr;
		const std::string font_full_path = font_path;

		// find font
		auto& style_map = m_font_map[font_full_path];
		auto& size_map = style_map[font_style];

		// find font size
		const auto size_it = size_map.find(font_size);
		if (size_it != size_map.end())
			return size_it->second;

		const auto it = m_font_file_map.find(font_path);
		if (it == m_font_file_map.end()) return nullptr;
		auto font = std::make_shared<CarpFont>(it->second.data(), it->second.size(), font_size, font_style);

		// save font
		size_map[font_size] = font;

		// return font
		return font;
	}

	CarpSurface* CreateSurface(CarpFont* font, const char* content)
	{
		if (content == 0 || font == 0) return nullptr;

		auto* carp_bitmap = font->CreateBitmapFromUTF8(content, false);
		if (carp_bitmap == nullptr) return nullptr;

		if (carp_bitmap->width <= 0 || carp_bitmap->height <= 0)
		{
			delete carp_bitmap;
			return nullptr;
		}

		auto* surface = new CarpSurface(carp_bitmap->width, carp_bitmap->height);
		if (surface == nullptr)
		{
			delete carp_bitmap;
			return nullptr;
		}

		for (int row = 0; row < carp_bitmap->height; ++row)
		{
			const int offset = row * carp_bitmap->width;
			for (int col = 0; col < carp_bitmap->width; ++col)
			{
				const unsigned int value = carp_bitmap->bitmap[offset + col];
				if (value != 0)
				{
					const unsigned int color = value << 24 | 0x00FFFFFF;
					surface->SetPixel(col, row, color);
				}
			}
		}

		delete carp_bitmap;
		return surface;
	}

	void Shutdown()
	{
		m_font_map.clear();
		m_font_file_map.clear();
	}

	void AddFont(const std::string& font_path, std::vector<char>& memory)
	{
		auto it = m_font_file_map.find(font_path);
		if (it != m_font_file_map.end())
		{
			CARP_ERROR("already font path:" << font_path);
			return;
		}
		m_font_file_map[font_path] = std::move(memory);
	}

private:
	// font size map font object
	typedef std::map<unsigned int, std::shared_ptr<CarpFont>> FontSizeMap;
	// font style map font object
	typedef std::map<unsigned int, FontSizeMap> FontStyleMap;
	// font path map TTF_FontMap
	typedef std::map<std::string, FontStyleMap> FontMap;
	FontMap m_font_map;
	std::map<std::string, std::vector<char>> m_font_file_map;

};

extern SailFont s_sail_font;

#endif


#ifdef SAIL_FONT_IMPL
#ifndef SAIL_FONT_IMPL_INCLUDE
#define SAIL_FONT_IMPL_INCLUDE
#define CARP_FONT_IMPL
#include "Carp/carp_font.hpp"
SailFont s_sail_font;
#endif
#endif

