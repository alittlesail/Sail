#ifndef SAIL_UI_TEXT_INCLUDED
#define SAIL_UI_TEXT_INCLUDED

#include <memory>

#include "sail_ui_event.hpp"
#include "sail_ui_object.hpp"
#include "Sail/SailClient/2D/sail_2d_text.hpp"

class SailUIText : public SailUIObject
{
public:
    SailUIText() { m_sail_2d_text = std::make_shared<Sail2DText>(); m_show = m_sail_2d_text; }
    virtual ~SailUIText() {}

    void Init() override
    {
        const auto self = std::dynamic_pointer_cast<SailUIText>(shared_from_this());
        AddEventListener<SailUILeftButtonUpEvent>(self);
        AddEventListener<SailUIMiddleButtonUpEvent>(self);
        AddEventListener<SailUITouchUpEvent>(self);
    }

    void SetText(const std::string& text) override { m_sail_2d_text->SetText(text); }
    const std::string& GetText() const override { return m_sail_2d_text->GetText(); }

    void SetFontPath(const std::string& font_path) { m_sail_2d_text->SetFontPath(font_path); }
    const std::string& GetFontPath() const { return m_sail_2d_text->GetFontPath(); }

    void SetFontSize(int font_size) { m_sail_2d_text->SetFontSize(font_size); }
    int GetFontSize() const override { return m_sail_2d_text->GetFontSize(); }

    void AdjustSize()
    {
        SetWidth(m_sail_2d_text->GetRealWidth());
        SetHeight(m_sail_2d_text->GetRealHeight());
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

private:
    std::shared_ptr<Sail2DText> m_sail_2d_text;
};

#endif
