#ifndef SAIL_UI_IMAGE_INCLUDED
#define SAIL_UI_IMAGE_INCLUDED

#include <memory>

#include "sail_ui_event.hpp"
#include "sail_ui_object.hpp"
#include "sail_ui_texture_system.hpp"
#include "Sail/SailClient/2D/sail_2d_quad.hpp"

class SailUIImage : public SailUIObject
{
public:
    SailUIImage() { m_sail_2d_image = std::make_shared<Sail2DImage>(); m_show = m_sail_2d_image; }
    virtual ~SailUIImage() {}

    void Init() override
    {
        const auto self = std::dynamic_pointer_cast<SailUIImage>(shared_from_this());
        AddEventListener<SailUILeftButtonUpEvent>(self);
        AddEventListener<SailUIMiddleButtonUpEvent>(self);
        AddEventListener<SailUITouchUpEvent>(self);
    }

public:
    void SetTexturePath(const std::string& value)
    {
        if (m_texture_path == value) return;

        if (!m_texture_path.empty())
        {
            m_sail_2d_image->ClearTexture();
            m_texture = nullptr;
        }

        m_texture_path = value;
        if (!m_texture_path.empty())
        {
            s_sail_ui_texture_system.SetTexture(shared_from_this(), value);
        }
    }

    const std::string& GetTexturePath() const { return m_texture_path; }

    int GetTextureWidth() const
    {
        if (m_texture == nullptr) return 0;
        return m_texture->GetWidth();
    }

    int GetTextureHeight() const
    {
        if (m_texture == nullptr) return 0;
        return m_texture->GetHeight();
    }

private:
    void SetTextureCoordinate(float top, float bottom, float left, float right) override
    {
        m_sail_2d_image->SetTextureCoordinate(top, bottom, left, right);
    }

    void SetTexture(const SailUITexturePtr& texture) override
    {
        m_texture = texture;
        if (m_texture == nullptr) m_sail_2d_image->ClearTexture();
        else m_sail_2d_image->SetTexture(m_texture->GetTexture());
    }

    std::string m_texture_path;
    SailUITexturePtr m_texture;
    std::shared_ptr<Sail2DImage> m_sail_2d_image;

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
