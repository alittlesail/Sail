#ifndef SAIL_UI_SYSTEM_INCLUDED
#define SAIL_UI_SYSTEM_INCLUDED

#include "sail_ui_object.hpp"
#include "sail_ui_layer.hpp"

struct SailTouchInfo
{
	uintptr_t touch_id = 0;
	int mouse_x = 0;
	int mouse_y = 0;
	int last_mouse_x = 0;
	int last_mouse_y = 0;
	bool dl = false;
	SailUIObjectPtr mfc;
	int mfc_rel_x = 0;
	int mfc_rel_y = 0;
	bool is_sfc = false;
};

class SailUISystem
{
public:
	SailUIObjectPtr GetFocus() const { return m_focus.lock(); }
	void SetFocus(const SailUIObjectPtr& control)
	{
		if (m_sfc == control) return;

		m_wfc = nullptr;
		if (m_sfc)
		{
		    if (m_dl)
		    {
				m_dl = false;
				m_dl_delta_x = 0;
				m_dl_delta_y = 0;
				if (m_left_button_down)
				{
					{
						SailUIButtonDragEndEvent event;
						event.target = m_sfc;
						event.rel_x = m_mfc_rel_x;
						event.rel_y = m_mfc_rel_y;

						if (m_mfc != m_sfc)
							m_sfc->GlobalToLocalMatrix2D(m_mouse_x, m_mouse_y, nullptr, event.rel_x, event.rel_y);

						event.delta_x = m_mouse_x - m_last_mouse_x;
						event.delta_y = m_mouse_y - m_last_mouse_y;
						event.abs_x = m_mouse_x;
						event.abs_y = m_mouse_y;
						m_sfc->DispatchEvent(event);
					}

					if (m_mfc)
					{
						SailUIDropEvent event;
						event.target = m_mfc;
						event.drop_target = m_sfc;
						event.rel_x = m_mfc_rel_x;
						event.rel_y = m_mfc_rel_y;
						event.abs_x = m_mouse_x;
						event.abs_y = m_mouse_y;
						m_mfc->DispatchEvent(event);
					}
				}
		    }

			auto sfc = m_sfc;

			m_sfc = nullptr;
			m_sl = false;
			SailUIFocusOutEvent event;
			sfc->DispatchEvent(event);
		}
	}

	int GetViewWidth() const { return m_view_width; }
	int GetViewHeight() const { return m_view_height; }

public:
	bool HandleMouseMove(int x, int y)
	{
		auto delta_x = x - m_mouse_x;
		auto delta_y = y - m_mouse_y;

		m_last_mouse_x = m_mouse_x;
		m_last_mouse_y = m_mouse_y;
		m_mouse_x = x;
		m_mouse_y = y;

		if (m_sl && m_left_button_down)
		{
		    if (m_dl)
		    {
				SailUIButtonDragEvent event;
				event.target = m_sfc;
				m_sfc->GlobalToLocalMatrix2D(x, y, nullptr, event.rel_x, event.rel_y);
				if (m_dl_delta_x > 0)
				{
					delta_x = m_dl_delta_x + delta_x;
					m_dl_delta_x = 0;
				}
				if (m_dl_delta_y > 0)
				{
					delta_y = m_dl_delta_y + delta_y;
					m_dl_delta_y = 0;
				}
				event.abs_x = x;
				event.abs_y = y;
				event.delta_x = delta_x;
				event.delta_y = delta_y;
				m_sfc->DispatchEvent(event);

				return true;
		    }

			if (!m_dl)
			{
				m_dl_delta_x += delta_x;
				m_dl_delta_y += delta_y;
				if (std::abs(m_dl_delta_x) >= m_max_dl_delta || std::abs(m_dl_delta_y) >= m_max_dl_delta)
				{
					m_dl = true;
					SailUIButtonDragBeginEvent event;
					event.target = m_sfc;
					const auto mouse_x = x - delta_x;
					const auto mouse_y = y - delta_y;
					m_sfc->GlobalToLocalMatrix2D(mouse_x, mouse_y, nullptr, event.rel_x, event.rel_y);
					event.abs_x = mouse_x;
					event.abs_y = mouse_y;
					event.delta_x = 0;
					event.delta_y = 0;
					m_sfc->DispatchEvent(event);
				}
			}

			return true;
		}

		UpdateMoveFocus(x, y);
		return m_mfc != nullptr;
	}

	bool HandleLeftButtonDown(int x, int y, int count)
	{
		m_left_button_down = true;
		m_left_button_count = count;
		m_left_is_touch = false;

		return HandleButtonDown<SailUILeftButtonDownEvent>(x, y, count);
	}

	bool HandleLeftButtonUp(int x, int y)
	{
		m_left_button_down = false;
		return HandleButtonUp<SailUILeftButtonUpEvent>(x, y);
	}

	bool HandleMiddleButtonDown(int x, int y, int count)
	{
		return HandleButtonDown<SailUIMiddleButtonDownEvent>(x, y, count);
	}

	bool HandleMiddleButtonUp(int x, int y)
	{
		return HandleButtonUp<SailUIMiddleButtonUpEvent>(x, y);
	}

	template <typename T>
	bool HandleButtonDown(int x, int y, int count)
	{
		m_wfc = nullptr;
		s_sail_ui_layer.HandleLRButtonDown(x, y);

		UpdateMoveFocus(x, y);

		if (m_mfc)
		{
			m_sl = true;
			if (m_mfd) m_mfd->MoveToTop();

			T event;
			event.target = m_mfc;
			event.abs_x = x;
			event.abs_y = y;
			event.rel_x = m_mfc_rel_x;
			event.rel_y = m_mfc_rel_y;
			event.count = count;
			event.is_drag = false;
			m_mfc->DispatchEvent(event);

			if (std::is_same<T, SailUILeftButtonDownEvent>::value)
			{
			    
			}

			if (!m_sfc)
			{
				m_sfc = m_mfc;
				SailUIFocusInEvent event;
				m_sfc->DispatchEvent(event);
			}
			else if (m_mfc != m_sfc)
			{
				SailUIFocusOutEvent out_event;
				m_sfc->DispatchEvent(out_event);
				m_sfc = m_mfc;
				SailUIFocusInEvent in_event;
				m_sfc->DispatchEvent(in_event);
			}

			return true;
		}

		if (m_sfc)
		{
			SailUIFocusOutEvent event;
			m_sfc->DispatchEvent(event);
			m_sfc = nullptr;
			m_sl = false;
		}

		return false;
	}

	template <typename T>
	bool HandleButtonUp(int x, int y)
	{
		if (m_sl == false || !m_sfc) return false;

		m_sl = false;
		int rel_x = 0, rel_y = 0;
		m_sfc->GlobalToLocalMatrix2D(x, y, nullptr, rel_x, rel_y);

		auto save_dl = m_dl;
		if (m_dl)
		{
			m_dl = false;
			m_dl_delta_x = 0;
			m_dl_delta_y = 0;

			SailUIButtonDragEndEvent event;
			event.target = m_sfc;
			event.abs_x = x;
			event.abs_y = y;
			event.delta_x = m_mouse_x - m_last_mouse_x;
			event.delta_y = m_mouse_y - m_last_mouse_y;
			event.rel_x = rel_x;
			event.rel_y = rel_y;
			m_sfc->DispatchEvent(event);
		}

		if (m_sfc)
		{
			T event;
			event.target = m_sfc;
			event.abs_x = x;
			event.abs_y = y;
			event.rel_x = rel_x;
			event.rel_y = rel_y;
			event.is_drag = save_dl;
			m_sfc->DispatchEvent(event);
		}

		UpdateMoveFocus(x, y);

		if (save_dl && m_mfc)
		{
			SailUIDropEvent event;
			event.target = m_mfc;
			event.drop_target = m_sfc;
			event.rel_x = m_mfc_rel_x;
			event.rel_y = m_mfc_rel_y;
			event.abs_x = m_mouse_x;
			event.abs_y = m_mouse_y;
			m_mfc->DispatchEvent(event);
		}

		return m_mfc != nullptr;
	}

	bool HandleRightButtonDown(int x, int y, int count)
	{
		s_sail_ui_layer.HandleLRButtonDown(x, y);

		if (m_sl) return false;
		// 相对于移动焦点控件的相对坐标
		// 更新移动焦点控件
		UpdateMoveFocus(x, y);
		// 如果有移动焦点控件
		if (m_mfc)
		{
			SailUIRightButtonDownEvent event;
			event.target = m_mfc;
			event.abs_x = x;
			event.abs_y = y;
			event.rel_x = m_mfc_rel_x;
			event.rel_y = m_mfc_rel_y;
			event.count = count;
			event.is_drag = false;
			// 则向控件发送鼠标右键点击事件（这个消息一般用于触发显示右键菜单）
			m_mfc->DispatchEvent(event);
			return true;
		}
		return false;
	}

	bool HandleRightButtonUp(int x, int y) { return false; }

	bool HandleMouseWheel(int x, int y)
	{
		if (m_mfc == nullptr) return false;

		if (m_wfc == nullptr)
		{
			auto control = m_mfc;
			while (control)
			{
			    if (control->GetCanScroll())
			    {
					m_wfc = control;
					break;
			    }
				control = control->GetShowParent();
			}
		}

		if (m_wfc)
		{
			SailUIMiddleButtonWheelEvent event;
			event.target = m_wfc;
			event.delta_x = x;
			event.delta_y = y;
			m_wfc->DispatchEvent(event);
		}

		return true;
	}

	void UpdateMoveFocus(int x, int y)
	{
		// 移动焦点控件
		SailUIObjectPtr mfc;
		// 移动焦点对话框
		SailUIObjectPtr mfd;
		s_sail_ui_layer.PickUp(x, y, mfc, mfd, m_mfc_rel_x, m_mfc_rel_y);

		// 保存移动焦点对话框
		m_mfd = mfd;
		// 如果计算得到的移动焦点控件就是当前的移动焦点控件
		if (m_mfc == mfc)
		{
			// 发送鼠标移动事件
			if (mfc)
			{
				SailUIMouseMoveEvent event;
				event.target = mfc;
				event.abs_x = x;
				event.abs_y = y;
				event.rel_x = m_mfc_rel_x;
				event.rel_y = m_mfc_rel_y;
				mfc->DispatchEvent(event);
			}
			return;
		}

		// 发送鼠标移出事件
		if (m_mfc)
		{
			SailUIMoveOutEvent event;
			m_mfc->DispatchEvent(event);
		}

		// 发送鼠标移入事件
		if (mfc)
		{
			SailUIMoveInEvent event;
			mfc->DispatchEvent(event);
		}	

		// 保存新的移动焦点控件和移动焦点对话框
		m_mfc = mfc;
		m_wfc = nullptr;
	}

	void HandleViewResized(int width, int height)
	{
		if (m_view_width == width && m_view_height == height) return;

		m_view_width = width;
		m_view_height = height;
		s_sail_ui_layer.HandleViewResized(width, height);
	}

	void HandleMouseEnterWindow()
	{
	    
	}

	void HandleMouseLeaveWindow()
	{
		if (m_left_button_down)
			HandleLeftButtonUp(m_mouse_x, m_mouse_y);
	}

	bool HandleTouchDown(int x, int y, uintptr_t touch_id)
	{
	    if (m_left_button_down == false)
	    {
			m_left_button_down = true;
			m_left_button_count = 1;
			m_left_is_touch = true;
			m_left_touch_id = touch_id;
			m_mouse_x = x;
			m_mouse_y = y;
			return HandleButtonDown<SailUILeftButtonDownEvent>(x, y, 1);
	    }

		SailTouchInfo info;
		info.touch_id = touch_id;
		info.mouse_x = x;
		info.mouse_y = y;

		SailUIObjectPtr mfd;

		s_sail_ui_layer.PickUp(x, y, info.mfc, mfd, info.mfc_rel_x, info.mfc_rel_y);
		if (!info.mfc) return false;

		info.is_sfc = m_sl && (info.mfc == m_mfc || info.mfc == m_sfc);

		SailUITouchDownEvent event;
		event.target = info.mfc;
		event.abs_x = x;
		event.abs_y = y;
		event.rel_x = info.mfc_rel_x;
		event.rel_y = info.mfc_rel_y;
		event.touch_id = info.touch_id;
		event.is_sfc = info.is_sfc;
		event.is_drag = false;
		info.mfc->DispatchEvent(event);

		m_touch_map[info.touch_id] = info;

		return true;
	}

	bool HandleTouchUp(int x, int y, uintptr_t touch_id)
	{
	    if (m_left_button_down && m_left_is_touch && m_left_touch_id == touch_id)
	    {
			m_left_button_down = false;
			return HandleButtonUp<SailUILeftButtonUpEvent>(x, y);
	    }

		auto it = m_touch_map.find(touch_id);
		if (it == m_touch_map.end()) return false;

		SailTouchInfo info = it->second;
		m_touch_map.erase(it);

		info.last_mouse_x = info.mouse_x;
		info.last_mouse_y = info.mouse_y;
		info.mouse_x = x;
		info.mouse_y = y;

		auto save_dl = info.dl;
		if (info.dl)
		{
			info.dl = false;
			SailUITouchDragEndEvent event;
			event.target = info.mfc;
			event.abs_x = x;
			event.abs_y = y;
			event.rel_x = info.mfc_rel_x;
			event.rel_y = info.mfc_rel_y;
			event.delta_x = info.mouse_x - info.last_mouse_x;
			event.delta_y = info.mouse_y - info.last_mouse_y;
			event.touch_id = touch_id;
			event.is_sfc = info.is_sfc;
			info.mfc->DispatchEvent(event);
		}

		SailUITouchUpEvent event;
		event.target = info.mfc;
		event.abs_x = x;
		event.abs_y = y;
		event.rel_x = info.mfc_rel_x;
		event.rel_y = info.mfc_rel_y;
		event.touch_id = touch_id;
		event.is_sfc = info.is_sfc;
		event.is_drag = save_dl;
		info.mfc->DispatchEvent(event);

		return true;
	}

	bool HandleTouchMove(int x, int y, uintptr_t touch_id)
	{
		if (m_left_button_down && m_left_is_touch && m_left_touch_id == touch_id)
			return HandleMouseMove(x, y);

		auto it = m_touch_map.find(touch_id);
		if (it == m_touch_map.end()) return false;
		auto& info = it->second;

		auto delta_x = x - info.mouse_x;
		auto delta_y = y - info.mouse_y;

		info.last_mouse_x = info.mouse_x;
		info.last_mouse_y = info.mouse_y;
		info.mouse_x = x;
		info.mouse_y = y;

		if (info.mfc)
			info.mfc->GlobalToLocalMatrix2D(x, y, nullptr, info.mfc_rel_x, info.mfc_rel_y);

		if (info.dl)
		{
			SailUITouchDragEvent event;
			event.target = info.mfc;
			event.delta_x = delta_x;
			event.delta_y = delta_y;
			event.abs_x = x;
			event.abs_y = y;
			event.rel_x = info.mfc_rel_x;
			event.rel_y = info.mfc_rel_y;
			event.touch_id = touch_id;
			event.is_sfc = info.is_sfc;
			info.mfc->DispatchEvent(event);
			return true;
		}

		if (info.dl == false)
		{
			info.dl = true;
			SailUITouchDragBeginEvent event;
			event.target = info.mfc;
			event.delta_x = delta_x;
			event.delta_y = delta_y;
			event.abs_x = x;
			event.abs_y = y;
			event.rel_x = info.mfc_rel_x;
			event.rel_y = info.mfc_rel_y;
			event.touch_id = touch_id;
			event.is_sfc = info.is_sfc;
			info.mfc->DispatchEvent(event);
			return true;
		}

		return false;
	}

private:
	SailUIObjectWeakPtr m_focus;

public:
	int GetMouseX() const { return m_mouse_x; }
	int GetMouseY() const { return m_mouse_y; }

private:
	int m_view_width = 0, m_view_height = 0;

	int m_mouse_x = 0, m_mouse_y = 0;
	int m_last_mouse_x = 0, m_last_mouse_y = 0;

	bool m_left_button_down = false;
	int m_left_button_count = 0;
	bool m_left_is_touch = false;
	uintptr_t m_left_touch_id = 0;

	SailUIObjectPtr m_mfc;
	int m_mfc_rel_x = 0;
	int m_mfc_rel_y = 0;

	SailUIObjectPtr m_mfd;
	SailUIObjectPtr m_sfc;
	SailUIObjectPtr m_wfc;

	bool m_sl = false;
	bool m_dl = false;
	int m_dl_delta_x = 0;
	int m_dl_delta_y = 0;
	int m_max_dl_delta = 0;
	int m_max_lp_delta = 0;

	std::unordered_map<uintptr_t, SailTouchInfo> m_touch_map;
};

extern SailUISystem s_sail_ui_system;

#endif

#ifdef SAIL_UI_SYSTEM_IMPL
SailUISystem s_sail_ui_system;
#endif

