
#include <asio.hpp>

#ifdef _WIN32
#include <crtdbg.h>
#endif // _WIN32

#include "sokol/sokol_app.h"
#include "sail_schedule.hpp"
#include "sail_resource.hpp"
#include "sail_font.hpp"
#include "Sail/SailClient/gfx/sail_gfx.hpp"

#include <vector>
#include <string>

static std::vector<std::string> s_argv;

static void SokolInit()
{
    s_sail_schedule.Setup(s_argv);
    s_sail_resource.Setup();

    s_sail_schedule.Test();
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