#ifndef SAIL_SCHEDULE_INCLUDED
#define SAIL_SCHEDULE_INCLUDED

#include <string>


#include "sail_font.hpp"
#include "2D/sail_2d_quad.hpp"
#include "Sail/SailClient/UI/sail_ui_layer.hpp"
#include "Sail/SailClient/UI/sail_ui_system.hpp"
#include "Sail/SailClient/Gfx/sail_gfx.hpp"

#include "Carp/carp_dump.hpp"
#include "Carp/carp_log.hpp"
#include "Carp/carp_file.hpp"
#include "Carp/carp_time.hpp"

#include "sokol/sokol_gfx.h"
#include "sokol/sokol_glue.h"
#include "UI/sail_ui_quad.hpp"
#include "UI/sail_ui_image.hpp"
#include "UI/sail_ui_text.hpp"
#include "UI/sail_ui_textarea.hpp"

class Application;

class SailSchedule
{
public:
	void Setup(const std::vector<std::string>& argv)
	{
		// ��ʼ���ļ���
		std::string pre_name = "SailClient";
		if (!argv.empty()) pre_name = CarpFile::GetJustFileNameByPath(argv[0]);

		// ��ʼ��dump
#ifdef _WIN32
		s_carp_dump.Setup(pre_name, []() { s_carp_log.Shutdown(); });
#endif

		// ��ʼ����־
#ifndef __EMSCRIPTEN__
		CarpFile::CreateDeepFolder("Log");
		s_carp_log.Setup("Log/", pre_name, true);
#endif

		// ��ʼ����Ⱦ
		sg_desc desc;
		memset(&desc, 0, sizeof(desc));
		desc.context = sapp_sgcontext();
		sg_setup(&desc);

		// ��ӡ���
		const auto backend = sg_query_backend();
		if (backend == SG_BACKEND_GLCORE33) CARP_INFO("backend:SG_BACKEND_GLCORE33");
		else if (backend == SG_BACKEND_GLES2) CARP_INFO("backend:SG_BACKEND_GLES2");
		else if (backend == SG_BACKEND_GLES3) CARP_INFO("backend:SG_BACKEND_GLES3");
		else if (backend == SG_BACKEND_D3D11) CARP_INFO("backend:SG_BACKEND_D3D11");
		else if (backend == SG_BACKEND_METAL_IOS) CARP_INFO("backend:SG_BACKEND_METAL_IOS");
		else if (backend == SG_BACKEND_METAL_MACOS) CARP_INFO("backend:SG_BACKEND_METAL_MACOS");
		else if (backend == SG_BACKEND_METAL_SIMULATOR) CARP_INFO("backend:SG_BACKEND_METAL_SIMULATOR");
		else if (backend == SG_BACKEND_WGPU) CARP_INFO("backend:SG_BACKEND_WGPU");
		else if (backend == SG_BACKEND_WGPU) CARP_INFO("backend:SG_BACKEND_DUMMY");
		else CARP_INFO("backend:unknown");

		// ��ʼ��Ĭ��ͨ��
		memset(&m_pass_action, 0, sizeof(m_pass_action));
		m_pass_action.colors[0].action = SG_ACTION_CLEAR;
		m_pass_action.colors[0].value = { 0.0f, 0.0f, 0.0f, 1.0f };

		s_sail_ui_system.HandleViewResized(sapp_width(), sapp_height());

		// ��ʼ��ʱ��
		m_current_time = CarpTime::GetCurMSTime();
		m_last_time = m_current_time;
	}

	void Shutdown()
	{
		s_sail_gfx_2d_batch_render.Clear();
		s_sail_ui_texture_system.Shutdown();
		s_sail_ui_layer.Shutdown();
		s_sail_ui_system.Shutdown();

		// �ͷ���Ⱦ
		sg_shutdown();

		// �ͷ���־
		s_carp_log.Shutdown();

		// �ͷ�dump
#ifdef _WIN32
		s_carp_dump.Shutdown();
#endif
	}
	
public:
	// �����¼�
	void HandleEvent(const sapp_event& event)
	{
		if (event.type == SAPP_EVENTTYPE_MOUSE_DOWN)
		{
			if (event.mouse_button == SAPP_MOUSEBUTTON_LEFT)
				s_sail_ui_system.HandleLeftButtonDown(static_cast<int>(event.mouse_x), static_cast<int>(event.mouse_y), 1);
			else if (event.mouse_button == SAPP_MOUSEBUTTON_MIDDLE)
				s_sail_ui_system.HandleMiddleButtonDown(static_cast<int>(event.mouse_x), static_cast<int>(event.mouse_y), 1);
			else if (event.mouse_button == SAPP_MOUSEBUTTON_RIGHT)
				s_sail_ui_system.HandleRightButtonDown(static_cast<int>(event.mouse_x), static_cast<int>(event.mouse_y), 1);
		}
		else if (event.type == SAPP_EVENTTYPE_MOUSE_UP)
		{
			if (event.mouse_button == SAPP_MOUSEBUTTON_LEFT)
				s_sail_ui_system.HandleLeftButtonUp(static_cast<int>(event.mouse_x), static_cast<int>(event.mouse_y));
			else if (event.mouse_button == SAPP_MOUSEBUTTON_MIDDLE)
				s_sail_ui_system.HandleMiddleButtonUp(static_cast<int>(event.mouse_x), static_cast<int>(event.mouse_y));
			else if (event.mouse_button == SAPP_MOUSEBUTTON_RIGHT)
				s_sail_ui_system.HandleRightButtonUp(static_cast<int>(event.mouse_x), static_cast<int>(event.mouse_y));
		}
		else if (event.type == SAPP_EVENTTYPE_MOUSE_MOVE)
		{
			s_sail_ui_system.HandleMouseMove(static_cast<int>(event.mouse_x), static_cast<int>(event.mouse_y));
		}
		else if (event.type == SAPP_EVENTTYPE_MOUSE_SCROLL)
		{
			s_sail_ui_system.HandleMouseWheel(static_cast<int>(event.scroll_x), static_cast<int>(event.scroll_y));
		}
		else if (event.type == SAPP_EVENTTYPE_MOUSE_ENTER)
		{
			s_sail_ui_system.HandleMouseEnterWindow();
		}
		else if (event.type == SAPP_EVENTTYPE_MOUSE_LEAVE)
		{
			s_sail_ui_system.HandleMouseLeaveWindow();
		}
		else if (event.type == SAPP_EVENTTYPE_TOUCHES_BEGAN)
		{
			for (int i = 0; i < event.num_touches; ++i)
				s_sail_ui_system.HandleTouchDown(static_cast<int>(event.touches[i].pos_x), static_cast<int>(event.touches[i].pos_y), event.touches[i].identifier);
		}
		else if (event.type == SAPP_EVENTTYPE_TOUCHES_MOVED)
		{
			for (int i = 0; i < event.num_touches; ++i)
				s_sail_ui_system.HandleTouchMove(static_cast<int>(event.touches[i].pos_x), static_cast<int>(event.touches[i].pos_y), event.touches[i].identifier);
		}
		else if (event.type == SAPP_EVENTTYPE_TOUCHES_ENDED || event.type == SAPP_EVENTTYPE_TOUCHES_CANCELLED)
		{
			for (int i = 0; i < event.num_touches; ++i)
				s_sail_ui_system.HandleTouchUp(static_cast<int>(event.touches[i].pos_x), static_cast<int>(event.touches[i].pos_y), event.touches[i].identifier);
		}
	}

public:
	// ִ��֡
	void Frame()
	{
		// ��ȡ��ǰʱ��
		m_current_time = CarpTime::GetCurMSTime();
		// ������ʱ��
		const auto interval = m_current_time - m_last_time;
		// ����ʱ��
		m_last_time = m_current_time;

		// ��ʼ��ͨ����Ⱦ
		sg_begin_default_pass(&m_pass_action, sapp_width(), sapp_height());

		s_sail_ui_layer.Render();
		s_sail_gfx_2d_batch_render.Flush();

		// �ύ���
		sg_end_pass();
		sg_commit();
	}

	// �˳�����
	void Exit() const
	{
		sapp_quit();
	}

	// ǿ���˳�
	void ForceExit() const
	{
		exit(0);
	}

private:
	// ������һ��ʱ��
	time_t m_last_time = 0;
	// ���浱ǰʱ��
	time_t m_current_time = 0;

private:
	// Ĭ��ͨ��
	sg_pass_action m_pass_action = {};
};

extern SailSchedule s_sail_schedule;

#endif

#ifdef SAIL_SCHEDULE_IMPL
#ifndef SAIL_SCHEDULE_IMPL_INCLUDE
#define SAIL_SCHEDULE_IMPL_INCLUDE
SailSchedule s_sail_schedule;
#endif
#endif