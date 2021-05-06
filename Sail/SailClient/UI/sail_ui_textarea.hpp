#ifndef SAIL_UI_TEXTAREA_INCLUDED
#define SAIL_UI_TEXTAREA_INCLUDED

#include <memory>

#include "sail_ui_event.hpp"
#include "sail_ui_object.hpp"
#include "Sail/SailClient/2D/sail_2d_textarea.hpp"

class SailUITextArea : public SailUIObject
{
public:
    SailUITextArea() { m_sail_2d_textarea = std::make_shared<Sail2DTextArea>(); m_show = m_sail_2d_textarea; }
    virtual ~SailUITextArea() {}

    void Init() override
    {
        const auto self = std::dynamic_pointer_cast<SailUITextArea>(shared_from_this());
        AddEventListener<SailUILeftButtonUpEvent>(self);
        AddEventListener<SailUIMiddleButtonUpEvent>(self);
        AddEventListener<SailUITouchUpEvent>(self);
    }

    void SetText(const std::string& text) override { m_sail_2d_textarea->SetText(text); }
    const std::string& GetText() const override { return m_sail_2d_textarea->GetText(); }

    void SetFontPath(const std::string& font_path) { m_sail_2d_textarea->SetFontPath(font_path); }
    const std::string& GetFontPath() const { return m_sail_2d_textarea->GetFontPath(); }

    void SetFontSize(int font_size) { m_sail_2d_textarea->SetFontSize(font_size); }
    int GetFontSize() const override { return m_sail_2d_textarea->GetFontSize(); }

    void SetBold(bool bold) { m_sail_2d_textarea->SetBold(bold); }
    bool GetBold() const { return m_sail_2d_textarea->GetBold(); }

    void SetUnderline(bool underline) { m_sail_2d_textarea->SetUnderline(underline); }
    bool GetUnderline() const { return m_sail_2d_textarea->GetUnderline(); }

    void SetDeleteline(bool deleteline) { m_sail_2d_textarea->SetDeleteline(deleteline); }
    bool GetDeleteline() const { return m_sail_2d_textarea->GetDeleteline(); }

    void SetItalic(bool italic) { m_sail_2d_textarea->SetItalic(italic); }
    bool GetItalic() const { return m_sail_2d_textarea->GetItalic(); }

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
    std::shared_ptr<Sail2DTextArea> m_sail_2d_textarea;
};

#endif
