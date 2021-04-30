#ifndef SAIL_UI_QUAD_INCLUDED
#define SAIL_UI_QUAD_INCLUDED

#include <memory>

#include "sail_ui_event.hpp"
#include "sail_ui_object.hpp"
#include "Sail/SailClient/2D/sail_2d_quad.hpp"

class SailUIQuad : public SailUIObject
{
public:
    SailUIQuad() { m_show = std::make_shared<Sail2DQuad>(); }
    virtual ~SailUIQuad() {}

    void Init() override
    {
        const auto self = std::dynamic_pointer_cast<SailUIQuad>(shared_from_this());
        AddEventListener<SailUILeftButtonUpEvent>(self);
        AddEventListener<SailUIMiddleButtonUpEvent>(self);
        AddEventListener<SailUITouchUpEvent>(self);
    }

public:
    void OnEvent(const SailUILeftButtonUpEvent& event)
    {
        if (event.rel_x >= 0 && event.rel_y >= 0 && event.rel_x < event.target->GetWidth() && event.rel_y < event.target->GetHeight())
        {
            SailUILeftClickEvent c_event;
            c_event.is_drag = event.is_drag;
            c_event.count = event.count;
            DispatchEvent(c_event);
        }
    }

    void OnEvent(const SailUIMiddleButtonUpEvent& event)
    {
        if (event.rel_x >= 0 && event.rel_y >= 0 && event.rel_x < event.target->GetWidth() && event.rel_y < event.target->GetHeight())
        {
            SailUIMiddleClickEvent c_event;
            c_event.is_drag = event.is_drag;
            c_event.count = event.count;
            DispatchEvent(c_event);
        }
    }

    void OnEvent(const SailUITouchUpEvent& event)
    {
        if (event.rel_x >= 0 && event.rel_y >= 0 && event.rel_x < event.target->GetWidth() && event.rel_y < event.target->GetHeight())
        {
            SailUIMiddleClickEvent c_event;
            c_event.is_drag = event.is_drag;
            c_event.count = event.count;
            DispatchEvent(c_event);
        }
    }
};

#endif
