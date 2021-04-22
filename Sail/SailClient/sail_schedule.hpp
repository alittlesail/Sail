#ifndef SAIL_SCHEDULE_INCLUDED
#define SAIL_SCHEDULE_INCLUDED

#include <string>

#include "sail_ui_layer.hpp"

#include "Carp/carp_dump.hpp"
#include "Carp/carp_log.hpp"
#include "Carp/carp_file.hpp"
#include "Carp/carp_time.hpp"

#include "sokol/sokol_gfx.h"
#include "sokol/sokol_glue.h"

#ifdef __EMSCRIPTEN__
#include "emscripten.h"
#endif

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

		// ��ʼ��Ĭ��ͨ��
		memset(&m_pass_action, 0, sizeof(m_pass_action));
		m_pass_action.colors[0].action = SG_ACTION_CLEAR;
		m_pass_action.colors[0].value = { 0.0f, 0.0f, 0.0f, 1.0f };

		// ��ʼ��ʱ��
		m_current_time = CarpTime::GetCurMSTime();
		m_last_time = m_current_time;
	}

	void Shutdown()
	{
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
	}

public:
	// ִ��֡
	void Frame()
	{
		// ��ȡ��ǰʱ��
		m_current_time = CarpTime::GetCurMSTime();
		// ������ʱ��
		auto interval = m_current_time - m_last_time;
		// ����ʱ��
		m_last_time = m_current_time;

		// ��ʼ��ͨ����Ⱦ
		sg_begin_default_pass(&m_pass_action, sapp_width(), sapp_height());

		s_sail_ui_layer.Render();

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