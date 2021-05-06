#ifndef SAIL_UI_TEXTINPUT_INCLUDED
#define SAIL_UI_TEXTINPUT_INCLUDED

#include <memory>

#include "sail_ui_event.hpp"
#include "sail_ui_object.hpp"
#include "Sail/SailClient/2D/sail_2d_textinput.hpp"

class SailUITextInput : public SailUIObject
{
public:
    SailUITextInput() { m_sail_2d_textinput = std::make_shared<Sail2DTextInput>(); m_show = m_sail_2d_textinput; }
    virtual ~SailUITextInput() {}

    void Init() override
    {
        const auto self = std::dynamic_pointer_cast<SailUITextInput>(shared_from_this());

        AddEventListener<SailUIFocusInEvent>(self);
        AddEventListener<SailUIFocusOutEvent>(self);

        AddEventListener<SailUIMoveInEvent>(self);
        AddEventListener<SailUIMoveOutEvent>(self);

        AddEventListener<SailUILeftButtonDownEvent>(self);

        AddEventListener<SailUITextInputEvent>(self);
        AddEventListener<SailUIKeyDownEvent>(self);

        AddEventListener<SailUIButtonDragBeginEvent>(self);
        AddEventListener<SailUIButtonDragEvent>(self);
    }

    bool IsInput() const override { return true; }

    void SetEditable(bool value) { m_editable = value; }
    bool GetEditable() const override { return m_editable; }

    void SetText(const std::string& text) override
    {
        m_is_selecting = false;
        m_sail_2d_textinput->SetDefaultText(text.empty(), m_default_text);
        m_sail_2d_textinput->SetText(text);
    }
    const std::string& GetText() const override
    {
        static std::string empty;
        if (m_sail_2d_textinput->IsDefaultText()) return empty;
        return m_sail_2d_textinput->GetText();
    }

    void SetDefaultText(const std::string& text)
    {
        m_default_text = text;
        if (m_sail_2d_textinput->IsDefaultText())
            m_sail_2d_textinput->SetDefaultText(true, m_default_text);
    }
    const std::string& GetDefaultText() const { return m_default_text; }

    void SetDefaultTextAlpha(float alpha) { m_sail_2d_textinput->SetDefaultTextAlpha(alpha); }
    float GetDefaultTextAlpha() const { return m_sail_2d_textinput->GetDefaultTextAlpha(); }

    void SetPasswordMode(bool value)
    {
        m_is_selecting = false;
        m_sail_2d_textinput->SetPasswordMode(value);
    }
    bool GetPasswordMode() const { return m_sail_2d_textinput->GetPasswordMode(); }

    void SetFontPath(const std::string& font_path) { m_sail_2d_textinput->SetFontPath(font_path); }
    const std::string& GetFontPath() const { return m_sail_2d_textinput->GetFontPath(); }

    void SetFontSize(int font_size) { m_sail_2d_textinput->SetFontSize(font_size); }
    int GetFontSize() const override { return m_sail_2d_textinput->GetFontSize(); }

    void SetBold(bool bold) { m_sail_2d_textinput->SetBold(bold); }
    bool GetBold() const { return m_sail_2d_textinput->GetBold(); }

    void SetUnderline(bool underline) { m_sail_2d_textinput->SetUnderline(underline); }
    bool GetUnderline() const { return m_sail_2d_textinput->GetUnderline(); }

    void SetDeleteline(bool deleteline) { m_sail_2d_textinput->SetDeleteline(deleteline); }
    bool GetDeleteline() const { return m_sail_2d_textinput->GetDeleteline(); }

    void SetItalic(bool italic) { m_sail_2d_textinput->SetItalic(italic); }
    bool GetItalic() const { return m_sail_2d_textinput->GetItalic(); }

public:
    void OnEvent(const SailUIFocusInEvent& event)
    {
        m_sail_2d_textinput->ShowCursor(true);

    }

    void OnEvent(const SailUIFocusOutEvent& event)
    {
    }

    void OnEvent(const SailUIMoveInEvent& event)
    {
    }

    void OnEvent(const SailUIMoveOutEvent& event)
    {
    }

    void OnEvent(const SailUILeftButtonDownEvent& event)
    {
        
    }

    void OnEvent(const SailUITextInputEvent& event)
    {
        
    }

    void OnEvent(const SailUIButtonDragBeginEvent& event)
    {
        
    }

    void OnEvent(const SailUIButtonDragEvent& event)
    {
        
    }

private:
    std::shared_ptr<Sail2DTextInput> m_sail_2d_textinput;
    std::string m_default_text;
    bool m_editable = true;
    bool m_is_selecting = false;
};

#endif
