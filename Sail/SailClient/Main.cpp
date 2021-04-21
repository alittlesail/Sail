
#include <asio.hpp>

#ifdef _WIN32
#include <stdlib.h>
#include <crtdbg.h>
#endif // _WIN32

#include "sokol/sokol_app.h"

static void SokolInit()
{
}

static void SokolFrame()
{
    
}

static void SokolCleanUp()
{
    
}

static void SokolEvent(const sapp_event*)
{
}

sapp_desc sokol_main(int argc, char* argv[])
{
#ifdef _WIN32
#ifdef _DEBUG
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
#endif // _WIN32

    sapp_desc desc = {0};
    desc.width = 640;
    desc.height = 480;
    desc.init_cb = SokolInit;
    desc.frame_cb = SokolFrame;
    desc.cleanup_cb = SokolCleanUp;
    desc.event_cb = SokolEvent;
    return desc;
}

#define SOKOL_IMPL
#define SOKOL_D3D11
#include "sokol/sokol_app.h"

#define CARP_CONSOLE_IMPL
#include "Carp/carp_console.hpp"
#define CARP_DUMP_IMPL
#include "Carp/carp_dump.hpp"
#define CARP_LOG_IMPL
#include "Carp/carp_log.hpp"
