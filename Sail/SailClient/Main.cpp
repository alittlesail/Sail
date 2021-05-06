
#include <asio.hpp>

#ifdef _WIN32
#include <crtdbg.h>
#endif // _WIN32

#include "sokol/sokol_app.h"
#include "sail_schedule.hpp"
#include "sail_resource.hpp"
#include "sail_font.hpp"

#include <vector>
#include <string>

static std::vector<std::string> s_argv;

void Test2()
{
	auto layer = SailUIObject::CreateUI<SailUIObjects>();
	auto dialog = SailUIObject::CreateUI<SailUIObjects>();
	dialog->SetWidth(200);
	dialog->SetHeight(200);


	layer->AddChild(dialog);
	auto quad = SailUIObject::CreateUI<SailUIQuad>();
	quad->SetWidth(100);
	quad->SetHeight(100);
	dialog->AddChild(quad);

	for (int i = 0; i < 100; ++i)
	{

		auto image = SailUIObject::CreateUI<SailUIImage>();
		image->SetX(100);
		image->SetY(i);
		image->SetWidth(100);
		image->SetHeight(100);
		image->SetTexturePath("baboon.png");
		dialog->AddChild(image);
	}


	{
		auto text = SailUIObject::CreateUI<SailUIText>();
		text->SetX(300);
		text->SetFontPath("YaHei-Consolas.ttf");
		text->SetFontSize(30);
		text->SetText("Hello Sail Text");
		text->AdjustSize();
		dialog->AddChild(text);
	}

	{
		auto text = SailUIObject::CreateUI<SailUITextArea>();
		text->SetX(300);
		text->SetY(35);
		text->SetWidth(200);
		text->SetHeight(200);
		text->SetFontPath("YaHei-Consolas.ttf");
		text->SetFontSize(30);
		text->SetText("Hello Sail TextArea");
		dialog->AddChild(text);
	}

	s_sail_ui_layer.AddLayer(layer);
}

void Test()
{
	std::set<std::string> path_set;
	path_set.insert("YaHei-Consolas.ttf");
	s_sail_resource.Progress(path_set, [](bool finished, std::unordered_map<std::string, SailResource::ProgressInfo>& info_map)
		{
			if (!finished) return;
			auto& info = info_map["YaHei-Consolas.ttf"];
			if (info.finished && !info.failed)
				s_sail_font.AddFont("YaHei-Consolas.ttf", info.memory);
			Test2();
		});
}

static void SokolInit()
{
    s_sail_schedule.Setup(s_argv);
    s_sail_resource.Setup();

    Test();
}

static void SokolFrame()
{
    s_sail_schedule.Frame();
    s_sail_resource.Frame();
}

static void SokolCleanUp()
{
    s_sail_resource.Shutdown();
    s_sail_schedule.Shutdown();
    s_sail_font.Shutdown();
}

static void SokolEvent(const sapp_event* event)
{
    if (event == nullptr) return;
    s_sail_schedule.HandleEvent(*event);
}

sapp_desc sokol_main(int argc, char* argv[])
{
#ifdef _WIN32
#ifdef _DEBUG
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
#endif // _WIN32

    s_argv.clear();
    for (int i = 0; i < argc; ++i) s_argv.emplace_back(argv[i]);

    sapp_desc desc = {nullptr};
    desc.width = 640;
    desc.height = 480;
    desc.init_cb = SokolInit;
    desc.frame_cb = SokolFrame;
    desc.cleanup_cb = SokolCleanUp;
    desc.event_cb = SokolEvent;
#if _DEBUG
    desc.win32_console_create = true;
#endif
    return desc;
}

#define CARP_DUMP_IMPL
#include "Carp/carp_dump.hpp"
#define CARP_LOG_IMPL
#include "Carp/carp_log.hpp"
#define SAIL_SCHEDULE_IMPL
#include "sail_schedule.hpp"
#define SAIL_RESOURCE_IMPL
#include "Sail/SailClient/sail_resource.hpp"
#define SAIL_FONT_IMPL
#include "Sail/SailClient/sail_font.hpp"
#define SAIL_2D_TEXT_IMPL
#include "Sail/SailClient/2D/sail_2d_text.hpp"
#define SAIL_UI_LAYER_IMPL
#include "Sail/SailClient/UI/sail_ui_layer.hpp"
#define SAIL_UI_SYSTEM_IMPL
#include "Sail/SailClient/UI/sail_ui_system.hpp"
#define SAIL_UI_TEXTURE_SYSTEM_IMPL
#include "Sail/SailClient/UI/sail_ui_texture_system.hpp"
#define SAIL_GFX_IMPL
#include "Sail/SailClient/Gfx/sail_gfx.hpp"

#define SOKOL_LOG(msg) CARP_INFO(msg)

#define SOKOL_IMPL
#include "sokol/sokol_app.h"
#define SOKOL_GFX_IMPL
#include "sokol/sokol_gfx.h"
#define SOKOL_GLUE_IMPL
#include "sokol/sokol_glue.h"
#define SOKOL_FETCH_IMPL
#include "sokol/sokol_fetch.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"