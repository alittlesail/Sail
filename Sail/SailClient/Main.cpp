
#include <asio.hpp>

#ifdef _WIN32
#include <crtdbg.h>
#endif // _WIN32

#include "sokol/sokol_app.h"
#include "sail_schedule.hpp"

#include <vector>
#include <string>

static std::vector<std::string> s_argv;

static void SokolInit()
{
    s_sail_schedule.Setup(s_argv);
}

static void SokolFrame()
{
    s_sail_schedule.Frame(); 
}

static void SokolCleanUp()
{
    s_sail_schedule.Shutdown();
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
    return desc;
}

#define SOKOL_LOG(msg) CARP_INFO(msg)

#define SOKOL_IMPL
#define SOKOL_D3D11
#include "sokol/sokol_app.h"
#define SOKOL_GFX_IMPL
#include "sokol/sokol_gfx.h"
#define SOKOL_GLUE_IMPL
#include "sokol/sokol_glue.h"

#define CARP_DUMP_IMPL
#include "Carp/carp_dump.hpp"
#undef CARP_DUMP_IMPL

#define CARP_LOG_IMPL
#include "Carp/carp_log.hpp"
#undef CARP_LOG_IMPL

#define SAIL_SCHEDULE_IMPL
#include "sail_schedule.hpp"
#undef SAIL_SCHEDULE_IMPL

#define SAIL_UI_LAYER_IMPL
#include "Sail/SailClient/UI/sail_ui_layer.hpp"
#undef SAIL_UI_LAYER_IMPL

#define SAIL_UI_SYSTEM_IMPL
#include "Sail/SailClient/UI/sail_ui_system.hpp"
#undef SAIL_UI_SYSTEM_IMPL