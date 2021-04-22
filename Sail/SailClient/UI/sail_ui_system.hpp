#ifndef SAIL_UI_SYSTEM_INCLUDED
#define SAIL_UI_SYSTEM_INCLUDED

class SailUISystem
{
public:
	SailUISystem()
	{
	}
};

extern SailUISystem s_sail_ui_system;

#endif

#ifdef SAIL_UI_SYSTEM_IMPL
SailUISystem s_sail_ui_system;
#endif

