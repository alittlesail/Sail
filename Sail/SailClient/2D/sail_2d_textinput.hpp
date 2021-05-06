#ifndef SAIL_2D_TEXTINPUT_INCLUDED
#define SAIL_2D_TEXTINPUT_INCLUDED

#include "sail_2d_image.hpp"
#include "sail_2d_quad.hpp"
#include "Carp/carp_font.hpp"
#include "Carp/carp_string.hpp"
#include "Sail/SailClient/sail_font.hpp"

class Sail2DTextInput : public Sail2DImage
{
public:
	Sail2DTextInput()
	{
		m_cursor.SetVisible(false);
		ClearText();
	}
	~Sail2DTextInput()
	{
		if (m_texture.id != SG_INVALID_ID)
		{
			sg_destroy_image(m_texture);
			m_texture.id = SG_INVALID_ID;
		}
	}

public:
	void SetWidth(int width) override
	{
		if (m_width == width) return;

		if (width > m_real_width || width < m_real_width)
			m_need_redraw = true;

		m_width = width;
		m_self_matrix_dirty = true;
	}
	void SetHeight(int height) override
	{
		if (m_height == height) return;

		m_height = height;
		m_cursor.SetHeight(height);
		m_self_matrix_dirty = true;
	}

public:
	void SetCursorRed(float red) { m_cursor.SetRed(red); }
	void SetCursorGreen(float green) { m_cursor.SetGreen(green); }
	void SetCursorBlue(float blue) { m_cursor.SetBlue(blue); }
	void SetCursorAlpha(float alpha) { m_cursor.SetAlpha(alpha); }

	void SetAlpha(float alpha) override { m_text_alpha = alpha; }

public:
	void SetBold(bool bold)
	{
		const bool is_bold = (m_font_style & CARP_FONT_STYLE_BOLD) != 0;
		if (is_bold == bold) return;

		if (bold) m_font_style |= CARP_FONT_STYLE_BOLD;
		else m_font_style &= ~CARP_FONT_STYLE_BOLD;

		if (!m_font_path.empty() && m_font_size > 0)
			m_font = s_sail_font.GetFont(m_font_path, m_font_size, m_font_style);

		const std::string text = GetText();
		ClearText();
		InsertTextImpl(text);
		ResetCursor();
	}
	bool GetBold() const { return (m_font_style & CARP_FONT_STYLE_BOLD) != 0; }
	void SetUnderline(bool underline)
	{
		const bool is_underline = (m_font_style & CARP_FONT_STYLE_UNDERLINE) != 0;
		if (is_underline == underline) return;

		if (underline) m_font_style |= CARP_FONT_STYLE_UNDERLINE;
		else m_font_style &= ~CARP_FONT_STYLE_UNDERLINE;

		if (!m_font_path.empty() && m_font_size > 0)
			m_font = s_sail_font.GetFont(m_font_path, m_font_size, m_font_style);
		m_need_redraw = true;
	}
	bool GetUnderline() const { return (m_font_style & CARP_FONT_STYLE_UNDERLINE) != 0; }
	void SetDeleteline(bool deleteline)
	{
		const bool is_deleteline = (m_font_style & CARP_FONT_STYLE_DELETELINE) != 0;
		if (is_deleteline == deleteline) return;

		if (deleteline) m_font_style |= CARP_FONT_STYLE_DELETELINE;
		else m_font_style &= ~CARP_FONT_STYLE_DELETELINE;

		if (!m_font_path.empty() && m_font_size > 0)
			m_font = s_sail_font.GetFont(m_font_path, m_font_size, m_font_style);
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

		if (!m_font_path.empty() && m_font_size > 0)
			m_font = s_sail_font.GetFont(m_font_path, m_font_size, m_font_style);
		m_need_redraw = true;
	}
	bool GetItalic() const { return (m_font_style & CARP_FONT_STYLE_ITALIC) != 0; }

public:
	void SetPasswordMode(bool mode)
	{
		if (m_password_mode == mode) return;
		m_password_mode = mode;

		const std::string text = GetText();
		ClearText();
		InsertTextImpl(text);
		ResetCursor();
	}
	bool GetPasswordMode() const { return m_password_mode; }

public:
	void ShowCursor(bool show)
	{
		m_cursor.SetVisible(show);
	}

	//=====================================================================================
private:
	struct CharInfo
	{
		std::string str;		// str content
		int text_index = 0;			// text index

		int pre_width = 0;			// total width text(not include this)
		int acc_width = 0;			// total width text(include this)
	};
	typedef std::list<CharInfo> CharInfoList;
	/**
	 * clear text
	 */
	void ClearText()
	{
		// clear char list
		m_char_list.clear();

		// clear text
		m_text = "";

		// add a end char
		m_char_list.push_back(CharInfo());
		CharInfo& info = m_char_list.back();
		info.acc_width = 0;
		info.pre_width = 0;
		info.text_index = 0;
		info.str = "";

		// reset all iterator
		m_start_it = m_char_list.begin();
		m_cursor_it = m_char_list.begin();
		m_select_it = m_char_list.end();

		m_need_redraw = true;
	}
	/**
	 * reset cursor
	 */
	void ResetCursor()
	{
		if (!m_font) return;

		// If not selected text
		if (m_select_it == m_char_list.end() || m_select_it == m_cursor_it)
		{
			// Set the cursor width
			m_cursor.SetWidth(m_cursor_width);
			// Set cursor position
			m_cursor.SetX(m_cursor_it->pre_width - m_start_it->pre_width);
			return;
		}

		// Set start and end position
		auto delete_begin = m_select_it;
		auto delete_end = m_cursor_it;
		if (m_select_it->pre_width > m_cursor_it->pre_width)
		{
			delete_begin = m_cursor_it;
			delete_end = m_select_it;
		}

		// Set cursor position
		int cursor_x = 0;
		if (delete_begin->pre_width > m_start_it->pre_width)
			cursor_x = (delete_begin->pre_width - m_start_it->pre_width);
		m_cursor.SetX(cursor_x);

		// Set the cursor width
		int cursor_width = (delete_end->pre_width - delete_begin->pre_width);
		if (cursor_width + cursor_x > m_width)
			cursor_width = m_width - cursor_x;
		m_cursor.SetWidth(cursor_width);
	}
	/**
	 * Closer to the starting position and distance
	 * @param distance
	 */
	void AdjustStartItCloseToCursor(int distance)
	{
		while (m_start_it != m_cursor_it)
		{
			if (m_cursor_it->pre_width - m_start_it->pre_width < distance)
				break;

			++m_start_it;

			m_need_redraw = true;
		}
	}
	/**
	 * away from Starting position and distance
	 * @param distance
	 */
	void AdjustStartItAwayFromCursor(int distance)
	{
		// If the starting position is equal to the cursor position
		if (m_start_it == m_cursor_it && m_start_it != m_char_list.begin())
		{
			// To calculate the starting position
			auto begin_it = m_cursor_it;
			while (m_cursor_it->pre_width - begin_it->pre_width < distance
				&& begin_it != m_char_list.begin())
				--begin_it;
			// Copies the result to the starting position
			m_start_it = begin_it;

			m_need_redraw = true;
		}
	}
	/**
	 * Delete the specified range node
	 * @param begin_it
	 * @param end_it
	 */
	void DeleteTextImpl(const CharInfoList::iterator& begin_it, const CharInfoList::iterator& end_it)
	{
		if (!m_font) return;
		if (begin_it == end_it) return;

		// Set start and end position
		auto delete_begin = begin_it;
		auto delete_end = end_it;
		if (begin_it->pre_width > end_it->pre_width)
		{
			delete_begin = end_it;
			delete_end = begin_it;
		}

		// If the starting position among them adjust the starting position for the final position
		if (m_start_it->pre_width >= delete_begin->pre_width)
			m_start_it = delete_end;

		// Calculate the char length
		int select_text_len = 0;
		for (auto it = delete_begin; it != delete_end; ++it)
			select_text_len += static_cast<int>(it->str.size());

		// Calculated width character
		std::string calc_text;
		int acc_width = 0;
		auto calc_it = delete_begin;
		if (calc_it != m_char_list.begin())
		{
			--calc_it;
			calc_text.append(calc_it->str);
			acc_width = calc_it->acc_width;
		}
		// Bridging the cursor to the end of the characters
		int width_1;
		if (m_password_mode)
			width_1 = m_font->CutTextWidth("*a", false);
		else
			width_1 = m_font->CutTextWidth((calc_it->str + "a").c_str(), false);
		int width_2;
		if (m_password_mode)
		{
			if (!delete_end->str.empty())
				width_2 = m_font->CutTextWidth("**a", false);
			else
				width_2 = m_font->CutTextWidth("*a", false);
		}
		else
			width_2 = m_font->CutTextWidth((calc_it->str + delete_end->str + "a").c_str(), false);
		const int offset_width = (width_2 - width_1 + acc_width) - delete_end->acc_width;
		// Recalculate, from the current cursor back until the end, the width of the characters
		for (auto it = delete_end; it != m_char_list.end(); ++it)
		{
			it->acc_width += offset_width;
			it->pre_width += offset_width;
			it->text_index -= select_text_len;
		}
		// Delete the selected node
		for (auto it = delete_begin; it != delete_end;)
			it = m_char_list.erase(it);

		m_need_redraw = true;
	}
	/**
	 * Insert text at the location of the cursor
	 * @param text
	 */
	void InsertTextImpl(const std::string& text)
	{
		if (!m_font) return;
		// The computed text length
		const int text_len = static_cast<int>(text.size());
		if (text_len <= 0) return;

		// Whether the current cursor position to the starting position
		bool adjust_start_it = (m_start_it == m_cursor_it);
		// Initialize
		const char* str = text.c_str();
		// The starting point for each character, each character width
		while (*str)
		{
			// Forward migration
			const int byte_count = CarpString::UTF8GetByteCountOfOneWord(*str);
			// Save the information
			m_cursor_it = m_char_list.insert(m_cursor_it, CharInfo());
			if (adjust_start_it) { --m_start_it; adjust_start_it = false; }
			// copy string
			m_cursor_it->str.assign(str, byte_count);
			// Set the current character index
			auto calc_it = m_cursor_it;
			if (calc_it == m_char_list.begin())
			{
				m_cursor_it->text_index = 0;
				m_cursor_it->pre_width = 0;

				const int width_1 = m_font->CutTextWidth("a", false);
				int width_2;
				if (m_password_mode)
					width_2 = m_font->CutTextWidth("*a", false);
				else
					width_2 = m_font->CutTextWidth((calc_it->str + "a").c_str(), false);
				m_cursor_it->acc_width = width_2 - width_1;
			}
			else
			{
				--calc_it;
				m_cursor_it->text_index = calc_it->text_index + byte_count;
				m_cursor_it->pre_width = calc_it->acc_width;

				int width_1;
				if (m_password_mode)
					width_1 = m_font->CutTextWidth("*a", false);
				else
					width_1 = m_font->CutTextWidth((calc_it->str + "a").c_str(), false);
				int width_2;
				if (m_password_mode)
					width_2 = m_font->CutTextWidth("**a", false);
				else
					width_2 = m_font->CutTextWidth((calc_it->str + m_cursor_it->str + "a").c_str(), false);
				m_cursor_it->acc_width = width_2 - width_1 + calc_it->acc_width;
			}
			// The cursor backward step
			++m_cursor_it;
			// offset
			str += byte_count;
		}
		// Bridging the cursor pointing to the current character
		auto calc_it = m_cursor_it;
		--calc_it;
		int width_1;
		if (m_password_mode)
			width_1 = m_font->CutTextWidth("*a", false);
		else
			width_1 = m_font->CutTextWidth((calc_it->str + "a").c_str(), false);
		int width_2;
		if (m_password_mode)
		{
			if (!m_cursor_it->str.empty())
				width_2 = m_font->CutTextWidth("**a", false);
			else
				width_2 = m_font->CutTextWidth("*a", false);
		}
		else
			width_2 = m_font->CutTextWidth((calc_it->str + m_cursor_it->str + "a").c_str(), false);
		// Recalculate, from the current cursor back until the end, the width of the characters
		const int offset_width = width_2 - width_1 + calc_it->acc_width - m_cursor_it->acc_width;
		for (auto it = m_cursor_it; it != m_char_list.end(); ++it)
		{
			it->acc_width += offset_width;
			it->pre_width += offset_width;
			it->text_index += text_len;
		}

		m_need_redraw = true;
	}

	//=====================================================================================
public:
	/**
	 * set font
	 * @param font_path
	 */
	void SetFontPath(const std::string& font_path)
	{
		if (m_font_path == font_path) return;
		m_font_path = font_path;

		if (m_font_size <= 0) return;
		m_font = s_sail_font.GetFont(m_font_path, m_font_size, m_font_style);
		m_cursor_width = GetCursorWidth(m_font_size);

		const std::string text = GetText();
		ClearText();
		InsertTextImpl(text);
		ResetCursor();
	}
	const std::string& GetFontPath() const { return m_font_path; }
	void SetFontSize(int font_size)
	{
		if (m_font_size == font_size) return;
		m_font_size = font_size;
		
		if (m_font_path.empty()) return;
		m_font = s_sail_font.GetFont(m_font_path, m_font_size, m_font_style);
		m_cursor_width = GetCursorWidth(m_font_size);

		const std::string text = GetText();
		ClearText();
		InsertTextImpl(text);
		ResetCursor();
	}
	int GetFontSize() const { return m_font_size; }
	/**
	 * set text
	 * @param text
	 */
	void SetText(const std::string& text)
	{
		ClearText();
		InsertTextImpl(text);
		ResetCursor();
	}
	const std::string& GetText() const { return m_text; }
	/**
	 * set default text, it shows when text input is empty
	 * @param mode: default or not
	 * @param text: default text
	 */
	void SetDefaultText(bool mode, const std::string& text)
	{
		m_default_text_mode = mode;
		SetText(text);
	}
	/*
	 * set default alpha
	 * @param alpha
	 */
	void SetDefaultTextAlpha(float alpha)
	{
		m_default_text_alpha = alpha;
	}
	float GetDefaultTextAlpha() const { return m_default_text_alpha; }
	/**
	 * get default mode or not
	 * @return default or not
	 */
	bool IsDefaultText() const
	{
		return m_default_text_mode;
	}
	/**
	 * get text
	 * @return text
	 */
	const std::string& GetText()
	{
		m_text = "";
		for (auto it = m_char_list.begin(); it != m_char_list.end(); ++it)
			m_text.append(it->str);
		return m_text;
	}
	/**
	 * get selected text
	 * @return text
	 */
	const std::string& GetSelectText()
	{
		m_select_text = "";

		if (m_select_it != m_char_list.end())
		{
			// init start and end position
			auto delete_begin = m_select_it;
			auto delete_end = m_cursor_it;
			if (m_select_it->pre_width > m_cursor_it->pre_width)
			{
				delete_begin = m_cursor_it;
				delete_end = m_select_it;
			}

			// append char to a string
			for (auto it = delete_begin; it != delete_end; ++it)
				m_select_text.append(it->str);
		}

		return m_select_text;
	}
	/**
	 * delete selected text
	 * @return delete or not
	 */
	bool DeleteSelectText()
	{
		if (!m_font) return false;
		// selected text or not
		if (m_select_it == m_char_list.end()) return false;
		// adjust cursor position
		const auto it = m_cursor_it;
		if (m_select_it->pre_width > m_cursor_it->pre_width)
			m_cursor_it = m_select_it;
		// delete text
		DeleteTextImpl(m_select_it, it);
		// adjust start iterator
		AdjustStartItAwayFromCursor(m_width / 2);
		// set not selected
		m_select_it = m_char_list.end();
		// reset cursor
		ResetCursor();

		return true;
	}
	/**
	 * insert text
	 * @param text
	 * @return insert or not
	 */
	bool InsertText(const std::string& text)
	{
		if (!m_font) return false;
		if (!text.empty()) m_default_text_mode = false;

		DeleteSelectText();
		InsertTextImpl(text);
		AdjustStartItCloseToCursor(m_width);
		ResetCursor();

		return true;
	}
	/**
	 * delete one char
	 * @param left true:delete left, false:delete right
	 * @return delete or not
	 */
	bool DeleteText(bool left)
	{
		if (!m_font) return false;
		// If the current selected text can be removed, then remove the returned directly
		if (DeleteSelectText())	return true;

		auto it = m_cursor_it;
		if (left)
		{
			if (m_cursor_it == m_char_list.begin()) return false;
			--it;
			DeleteTextImpl(it, m_cursor_it);
			// Adjust the start cursor
			AdjustStartItAwayFromCursor(m_width / 2);
		}
		else
		{
			if (m_cursor_it->str.empty()) return false;
			++m_cursor_it;
			DeleteTextImpl(it, m_cursor_it);
		}
		// Set cursor position
		ResetCursor();

		return true;
	}

	//=====================================================================================
private:
	/**
	 * offset cursor left or right
	 * @param left true: offset left, false: offset right
	 */
	void CursorOffsetImpl(bool left)
	{
		if (left)
		{
			if (m_cursor_it == m_char_list.begin()) return;

			if (m_cursor_it != m_start_it)
			{
				--m_cursor_it;

				ResetCursor();
				return;
			}

			--m_cursor_it;
			--m_start_it;

			m_need_redraw = true;

			AdjustStartItAwayFromCursor(m_width / 2);

			ResetCursor();
			return;
		}

		if (m_cursor_it->str.empty()) return;

		++m_cursor_it;
		AdjustStartItCloseToCursor(m_width);

		ResetCursor();
	}

	/**
	 * get iterator by offset x
	 * @param offset
	 * @return char info iterator
	 */
	CharInfoList::iterator GetIteratorByOffset(int offset) const
	{
		// Starting from the text
		const int real_offset = offset + m_start_it->pre_width;

		auto cursor_it = m_start_it;
		while (!cursor_it->str.empty())
		{
			if (real_offset > cursor_it->acc_width)
			{
				++cursor_it;
				continue;
			}

			const int half_char_width = (cursor_it->acc_width - cursor_it->pre_width) / 2;
			if (real_offset - cursor_it->pre_width > half_char_width)
				++cursor_it;

			break;
		}

		return cursor_it;
	}

	/*
	*  resize cursor width by font_size
	* @param font_size
	*/
	static int GetCursorWidth(int font_size)
	{
		if (font_size < 15) return 1;
		if (font_size > 100) return 10;

		return font_size / 10;
	}
public:
	/**
	 * offset cursor left or right
	 * @param left true: offset left, false: offset right
	 */
	void CursorOffset(bool left)
	{
		// If the current status for the selected text
		if (m_select_it != m_char_list.end())
		{
			m_select_it = m_char_list.end();
			// To reset the cursor position
			ResetCursor();
			return;
		}

		CursorOffsetImpl(left);
	}

	/**
	 * offset select cursor left or right
	 * @param left true: offset left, false: offset right
	 */
	void SelectCursorOffset(bool left)
	{
		// Set select starting point
		if (m_select_it == m_char_list.end())
			m_select_it = m_cursor_it;

		// Offset the cursor around
		CursorOffsetImpl(left);
	}
	/**
	 * offset cursor to home
	 */
	void SetCursorToHome()
	{
		if (m_start_it->pre_width > 0)
			m_need_redraw = true;

		m_select_it = m_char_list.end();
		m_start_it = m_char_list.begin();
		m_cursor_it = m_char_list.begin();

		ResetCursor();
	}
	/**
	 * offset cursor to end
	 */
	void SetCursorToEnd()
	{
		m_select_it = m_char_list.end();
		m_cursor_it = m_char_list.end();
		--m_cursor_it;

		// Adjust the starting position
		if (m_cursor_it->pre_width - m_start_it->pre_width > m_width)
			AdjustStartItCloseToCursor(m_width);

		ResetCursor();
	}
	/**
	 * select all text
	 */
	void SelectAll()
	{
		m_select_it = m_char_list.begin();
		m_cursor_it = m_char_list.end();
		--m_cursor_it;

		// Adjust the starting position
		if (m_cursor_it->pre_width - m_start_it->pre_width > m_width)
			AdjustStartItCloseToCursor(m_width);

		ResetCursor();
	}
	/**
	 * set cursor offset by x
	 * @param offset
	 */
	void ClickCursor(int offset)
	{
		m_select_it = m_char_list.end();

		// Set cursor position
		m_cursor_it = GetIteratorByOffset(offset);

		ResetCursor();
	}
	/**
	 * set cursor offset by x
	 * @param offset
	 */
	bool ClickWordCursor(int offset)
	{
		// Set cursor position
		m_cursor_it = GetIteratorByOffset(offset);

		// find char to left
		m_select_it = m_cursor_it;
		auto cursor_tmp = m_cursor_it;
		while (cursor_tmp != m_char_list.begin())
		{
			--cursor_tmp;
			if (cursor_tmp->str.size() == 1
				&& cursor_tmp->str[0] != '_'
				&& !CarpString::IsNumber(cursor_tmp->str[0])
				&& !CarpString::IsLetter(cursor_tmp->str[0]))
				break;
			--m_cursor_it;
		}
		// find char to right
		while (m_select_it != m_char_list.end())
		{
			if (m_select_it->str.size() == 1
				&& m_select_it->str[0] != '_'
				&& !CarpString::IsNumber(m_select_it->str[0])
				&& !CarpString::IsLetter(m_select_it->str[0]))
				break;
			++m_select_it;
		}
		if (m_select_it == m_char_list.end())
			--m_select_it;

		if (m_select_it == m_cursor_it)
			m_select_it = m_char_list.end();
		else if (m_select_it != m_char_list.end())
			AdjustStartItCloseToCursor(m_width);

		ResetCursor();

		return m_select_it != m_char_list.end();
	}
	/**
	 * cursor drag begin
	 */
	void DragCursorBegin()
	{
		m_select_it = m_cursor_it;
	}
	/**
	 * cursor drag ing
	 * @param offset_x
	 * @param offset_y
	 */
	void DragCursor(int offset_x, int offset_y)
	{
		m_cursor_it = GetIteratorByOffset(offset_x);

		if (m_start_it->pre_width >= m_cursor_it->pre_width)
		{
			while (m_start_it != m_char_list.begin() && m_start_it->pre_width >= m_cursor_it->pre_width)
			{
				--m_start_it;
				m_need_redraw = true;
			}
			ResetCursor();
			return;
		}

		if (m_cursor_it->pre_width - m_start_it->pre_width > m_width)
		{
			AdjustStartItCloseToCursor(m_width);
			ResetCursor();
			return;
		}

		ResetCursor();
	}
	/**
	 * get cursor x
	 * @return x
	 */
	int GetCursorX() const { return m_cursor.GetX(); }
	/**
	 * get cursor y
	 * @return y
	 */
	int GetCursorY() const { return m_cursor.GetY() - m_pos_y; }
	/**
	 * get cursor height
	 * @return height
	 */
	int GetCursorHeight() { return m_cursor.GetHeight(); }

public:
	void NeedDraw() { m_need_redraw = true; }
	void Render(const CarpMatrix2D& parent, bool parent_changed) override
	{
		if (m_clip) return;
		if (!m_visible) return;
		if (m_text_alpha <= 0) return;

		if (m_need_redraw)
		{
			Draw();
			m_need_redraw = false;
			SetSelfMatrixDirty();
		}

		const bool global_changed = UpdateGlobalMatrix2D(parent, parent_changed);
		UpdateVertexCoordinate();
		UpdateTextureCoordinate();

		if (m_texture.id != SG_INVALID_ID)
		{
			if (m_default_text_mode)
			{
				Sail2DImage::SetAlpha(m_default_text_alpha);
				UpdateVertexColor();
			}
			else
			{
				Sail2DImage::SetAlpha(m_text_alpha);
				UpdateVertexColor();
			}

			s_sail_gfx_2d_batch_render.Push(m_texture, m_vertices);
			s_sail_gfx_2d_batch_render.Push(m_texture, &m_vertices[SailGfx2DBatchRender::VERTEX_ALL_FLOAT_COUNT]);
		}

		m_cursor.Render(m_global_matrix, global_changed);
	}
	void UpdateVertexCoordinate() override
	{
		if (!m_vertex_dirty) return;
		m_vertex_dirty = false;

		CarpVector2D coordinate[4];
		coordinate[0].x = 0.0f; coordinate[0].y = 0.0f;
		coordinate[1].x = static_cast<float>(m_real_width); coordinate[1].y = 0.0f;
		coordinate[2].x = static_cast<float>(m_real_width); coordinate[2].y = static_cast<float>(m_real_height);
		coordinate[3].x = 0.0f; coordinate[3].y = static_cast<float>(m_real_height);

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

private:
	void Draw()
	{
		// release current texture
		if (m_texture.id != SG_INVALID_ID)
		{
			sg_destroy_image(m_texture);
			m_texture.id = SG_INVALID_ID;
		}

		// check param
		if (!m_font || m_char_list.empty() || m_width <= 0 || m_height <= 0) return;

		// According to the width of the current, capture the real rendering portion of the string
		std::string content;
		content.reserve(m_text.size());
		auto char_it = m_start_it;
		while (char_it != m_char_list.end())
		{
			if (m_width < char_it->acc_width - m_start_it->pre_width) break;
			if (!m_default_text_mode && m_password_mode && !char_it->str.empty())
				content.append("*");
			else
				content.append(char_it->str);
			++char_it;
		}

		// render text to surface
		if (content.empty()) return;
		const auto image = s_sail_font.CreateTexture(m_font, content.c_str(), m_real_width, m_real_height);
		if (image.id == SG_INVALID_ID)
		{
			CARP_ERROR("CreateSurface failed!");
			return;
		}

		m_texture = image;
		m_vertex_dirty = true;
	}

private:
	bool m_need_redraw = false;

private:
	int m_real_width = 0, m_real_height = 0;

private:
	int m_cursor_width = 1;
	//////////////////////////////////////////////////////////////////////////////
		// all text
	std::string m_text;
	// select text
	std::string m_select_text;
	// all char info
	CharInfoList m_char_list;
	// font object
	std::shared_ptr<CarpFont> m_font;
	std::string m_font_path;
	int m_font_size = 0;
	// font style
	unsigned int m_font_style = CARP_FONT_STYLE_NORMAL;
	// password mode or not
	bool m_password_mode = false;
	// default mode or not
	bool m_default_text_mode = true;
	// text alpha
	float m_text_alpha = 1.0f;
	// default text alpha
	float m_default_text_alpha = 1.0f;
	//////////////////////////////////////////////////////////////////////////////
private:
	// start iterator
	CharInfoList::iterator m_start_it;
	// cursor iterator
	CharInfoList::iterator m_cursor_it;
	// select iterator
	CharInfoList::iterator m_select_it;

	/////////////////////////////////////////////////////////////////////////////
private:
	Sail2DQuad m_cursor;
};

#endif
