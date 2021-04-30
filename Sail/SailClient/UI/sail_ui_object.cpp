
#include "sail_ui_object.hpp"
#include "sail_ui_system.hpp"

bool SailUIObject::IsFocus() const
{
    return s_sail_ui_system.GetFocus() == shared_from_this();
}

void SailUIObject::SetFocus(bool value)
{
    if (value)
    {
        s_sail_ui_system.SetFocus(shared_from_this());
        return;
    }

    if (s_sail_ui_system.GetFocus() == shared_from_this())
        s_sail_ui_system.SetFocus(nullptr);
}

bool SailUIObject::IsMouseIn()
{
    int abs_x = 0, abs_y = 0;
    LocalToGlobal(nullptr, abs_x, abs_y);
    const int mouse_x = s_sail_ui_system.GetMouseX();
    const int mouse_y = s_sail_ui_system.GetMouseY();
    if (mouse_x < abs_x) return false;
    if (mouse_y < abs_y) return false;
    if (mouse_x > abs_x + GetWidth() * GetScaleX()) return false;
    if (mouse_y > abs_y + GetHeight() * GetScaleY()) return false;
    return true;
}

void SailUIObject::SetVisible(bool value)
{
    if (m_visible == value) return;
    m_visible = value;
    UpdateVisible();
}

void SailUIObject::UpdateVisible()
{
    // 如果是隐藏起来，则不会是焦点
    if (m_visible == false && s_sail_ui_system.GetFocus() == shared_from_this())
        s_sail_ui_system.SetFocus(nullptr);

    const auto parent = m_show_parent.lock();
    if (parent)
        m_abs_visible = parent->m_abs_visible && m_visible;
    else
        m_abs_visible = m_visible;
    m_show->SetVisible(m_abs_visible);
}

void SailUIObject::SetDisabled(bool value)
{
    if (m_disabled == value) return;
    m_disabled = value;
    UpdateDisabled();
}

void SailUIObject::UpdateDisabled()
{
    // 如果是隐藏起来，则不会是焦点
    if (m_disabled == false && s_sail_ui_system.GetFocus() == shared_from_this())
        s_sail_ui_system.SetFocus(nullptr);

    const auto parent = m_show_parent.lock();
    if (parent)
        m_abs_disabled = parent->m_abs_disabled || m_disabled;
    else
        m_abs_disabled = m_disabled;
}
