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
		// 初始化文件名
		std::string pre_name = "SailClient";
		if (!argv.empty()) pre_name = CarpFile::GetJustFileNameByPath(argv[0]);

		// 初始化dump
#ifdef _WIN32
		s_carp_dump.Setup(pre_name, []() { s_carp_log.Shutdown(); });
#endif

		// 初始化日志
#ifndef __EMSCRIPTEN__
		CarpFile::CreateDeepFolder("Log");
		s_carp_log.Setup("Log/", pre_name, true);
#endif

		// 初始化渲染
		sg_desc desc;
		memset(&desc, 0, sizeof(desc));
		desc.context = sapp_sgcontext();
		sg_setup(&desc);

		// 初始化默认通道
		memset(&m_pass_action, 0, sizeof(m_pass_action));
		m_pass_action.colors[0].action = SG_ACTION_CLEAR;
		m_pass_action.colors[0].value = { 0.0f, 0.0f, 0.0f, 1.0f };

		// 初始化时间
		m_current_time = CarpTime::GetCurMSTime();
		m_last_time = m_current_time;
	}

	void Shutdown()
	{
		// 释放渲染
		sg_shutdown();

		// 释放日志
		s_carp_log.Shutdown();

		// 释放dump
#ifdef _WIN32
		s_carp_dump.Shutdown();
#endif
	}
	
public:
	// 处理事件
	void HandleEvent(const sapp_event& event)
	{
	}

public:
	// 执行帧
	void Frame()
	{
		// 获取当前时间
		m_current_time = CarpTime::GetCurMSTime();
		// 计算间隔时间
		auto interval = m_current_time - m_last_time;
		// 保存时间
		m_last_time = m_current_time;

		// 初始化通道渲染
		sg_begin_default_pass(&m_pass_action, sapp_width(), sapp_height());

		s_sail_ui_layer.Render();

		// 提交变更
		sg_end_pass();
		sg_commit();

	}

	// 退出进程
	void Exit() const
	{
		sapp_quit();
	}

	// 强制退出
	void ForceExit() const
	{
		exit(0);
	}

private:
	// 保存上一次时间
	time_t m_last_time = 0;
	// 保存当前时间
	time_t m_current_time = 0;

private:
	// 默认通道
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