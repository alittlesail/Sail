#ifndef SAIL_UI_OBJECTS_INCLUDED
#define SAIL_UI_OBJECTS_INCLUDED

#include <memory>

#include "sail_ui_object.hpp"
#include "Sail/SailClient/2D/sail_2d_objects.hpp"

class SailUIObjects : public SailUIObject
{
public:
    friend class SailUISystem;

public:
    SailUIObjects()
    {
        m_2d_objects = std::make_shared<Sail2DObjects>();
        m_show = m_2d_objects;

        m_pickup_child = true;                       // 默认拾取子控件
        m_pickup_this = false;                       // 是否拾取自己
    }
    virtual ~SailUIObjects() {}

public:
    int GetChildIndex(const SailUIObjectPtr& child) const override
    {
        const auto self = std::enable_shared_from_this<SailUIObject>::shared_from_this();
        if (child == nullptr || child == self) return -1;

        const auto show_parent = child->m_show_parent.lock();
        const auto logic_parent = child->m_logic_parent.lock();
        if (show_parent != self && logic_parent != self) return -1;

        for (size_t i = 0; i < m_children.size(); ++i)
        {
            if (m_children[i] == child) return static_cast<int>(i);
        }

        return -1;
    }

    bool SetChildIndex(const SailUIObjectPtr& child, int index) override
    {
        const auto cur_index = GetChildIndex(child);
        if (cur_index < 0) return false;
        if (cur_index == index) return true;

        if (index >= static_cast<int>(m_children.size()) - 1) index = static_cast<int>(m_children.size()) - 1;
        if (index < 0) index = 0;

        m_children.erase(m_children.begin() + cur_index);
        m_2d_objects->RemoveChild(child->m_show);
        if (index < m_children.size())
            m_2d_objects->AddChildBefore(m_children[index]->m_show, child->m_show);
        else
            m_2d_objects->AddChild(child->m_show);
        m_children.insert(m_children.begin() + index, child);

        return true;
    }

    SailUIObjectPtr GetChildByIndex(int index) const override
    {
        if (index < 0 || index >= static_cast<int>(m_children.size())) return nullptr;
        return m_children[index];
    }

    const std::vector<SailUIObjectPtr>& GetChildren() const override { return m_children; }
    int GetChildCount() const override { return static_cast<int>(m_children.size()); }

    bool AddChild(const SailUIObjectPtr& child, int index = -1) override
    {
        const auto self = std::enable_shared_from_this<SailUIObject>::shared_from_this();
        if (child == nullptr || child == self) return false;

        auto show_parent = child->m_show_parent.lock();
        auto logic_parent = child->m_logic_parent.lock();
        if (show_parent == self || logic_parent == self) return true;

        if (logic_parent) logic_parent->RemoveChild(child);
        else if (show_parent) show_parent->RemoveChild(child);

        child->m_show_parent = self;
        child->m_logic_parent.reset();

        if (index < 0 || index >= static_cast<int>(m_children.size()) || m_children.empty())
        {
            m_children.emplace_back(child);
            m_2d_objects->AddChild(child->m_show);
        }
        else
        {
            if (index < 0) index = 0;
            const auto& back_child = m_children[index];
            m_2d_objects->AddChildBefore(back_child->m_show, child->m_show);
            m_children.insert(m_children.begin() + index, child);
        }

        child->UpdateVisible();
        child->UpdateAlpha();
        child->UpdateDisabled();
        child->UpdateLayout();

        return true;
    }

    bool RemoveChild(const SailUIObjectPtr& child) override
    {
        if (child == nullptr) return false;

        const auto self = std::enable_shared_from_this<SailUIObject>::shared_from_this();
        const auto show_parent = child->m_show_parent.lock();
        const auto logic_parent = child->m_logic_parent.lock();
        if (show_parent != self && logic_parent != self) return false;

        for (size_t index = 0; index < m_children.size(); ++index)
        {
            if (m_children[index] == child)
            {
                m_children.erase(m_children.begin() + index);
                m_2d_objects->RemoveChild(child->m_show);
                child->m_show_parent.reset();
                child->m_logic_parent.reset();
                return true;
            }
        }

        return false;
    }

    int SpliceChild(int index, int count = -1) override
    {
        const int remain_count = static_cast<int>(m_children.size()) - index;
        if (count < 0) count = remain_count;
        if (count > remain_count) count = remain_count;
        if (count <= 0) return 0;

        const int old_count = static_cast<int>(m_children.size());

        const auto end_index = std::min(index + count, old_count);
        for (int i = index; i < end_index; ++i)
        {
            const auto& child = m_children[i];

            m_2d_objects->RemoveChild(child->m_show);
            child->m_show_parent.reset();
            child->m_logic_parent.reset();
        }

        m_children.erase(m_children.begin() + index, m_children.begin() + end_index);
        return old_count - static_cast<int>(m_children.size());
    }

    void RemoveAllChild() override
    {
        for (auto& child : m_children)
        {
            child->m_show_parent.reset();
            child->m_logic_parent.reset();
        }

        m_2d_objects->RemoveAllChild();
        m_children.resize(0);
    }

    void UpdateAlpha() override
    {
        SailUIObject::UpdateAlpha();

        for (auto& child : m_children)
            child->UpdateAlpha();
    }

    void UpdateVisible() override
    {
        SailUIObject::UpdateVisible();

        m_2d_objects->SetVisible(m_abs_visible);

        for (auto& child : m_children)
            child->UpdateVisible();
    }

    void UpdateDisabled() override
    {
        SailUIObject::UpdateDisabled();

        for (auto& child : m_children)
            child->UpdateDisabled();
    }

    void PickUp(int x, int y, SailUIObjectPtr& out_pick, int& out_x, int& out_y) override
    {
        // 检查是否被禁用，是否可见
        if (m_ignore || m_abs_disabled || m_abs_visible == false)
        {
            out_pick = nullptr;
            out_x = 0;
            out_y = 0;
            return;
        }

        // 计算出相对于控件的点
        auto xx = static_cast<float>(x - m_x);
        auto yy = static_cast<float>(y - m_y);

        // 处理旋转
        if (m_rotate != 0)
        {
            // 逆旋转
            const auto rad = -m_rotate;
            const auto cos = std::cosf(rad);
            const auto sin = std::sinf(rad);
            const auto xxx = xx * cos + yy * -sin;
            const auto yyy = xx * sin + yy * cos;

            xx = xxx;
            yy = yyy;
        }

        // 处理缩放
        if (m_scale_x != 0) xx /= m_scale_x;
        if (m_scale_y != 0) yy /= m_scale_y;

        // 移动到原来的位置
        const auto rel_x = static_cast<int>(xx) + m_center_x;
        const auto rel_y = static_cast<int>(yy) + m_center_y;

        if (m_scale_x <= 0 || m_scale_y <= 0)
        {
            if (m_modal)
            {
                out_pick = std::enable_shared_from_this<SailUIObject>::shared_from_this();
                out_x = rel_x;
                out_y = rel_y;
                return;
            }

            out_pick = nullptr;
            out_x = rel_x;
            out_y = rel_y;
            return;
        }

        // 检查位置是否在控件范围内
        if (rel_x >= 0 && rel_y >= 0 && rel_x < m_width && rel_y < m_height)
        {
            // 如果是模态则直接返回自己
            if (m_modal)
            {
                out_pick = std::enable_shared_from_this<SailUIObject>::shared_from_this();
                out_x = rel_x;
                out_y = rel_y;
                return;
            }

            out_pick = std::enable_shared_from_this<SailUIObject>::shared_from_this();
            out_x = rel_x;
            out_y = rel_y;
            return;
        }

        if (m_pickup_child)
        {
            for (auto it = m_children.rbegin(); it != m_children.rend(); ++it)
            {
                SailUIObjectPtr v_pick; int v_x = 0; int v_y = 0;
                (*it)->PickUp(rel_x, rel_y, v_pick, v_x, v_y);
                if (v_pick != nullptr)
                {
                    out_pick = v_pick;
                    out_x = v_x;
                    out_y = v_y;
                    return;
                }
            }
        }

        if (m_modal || m_pickup_child == false || m_pickup_this)
        {
            out_pick = std::enable_shared_from_this<SailUIObject>::shared_from_this();
            out_x = rel_x;
            out_y = rel_y;
            return;
        }

        out_pick = nullptr;
        out_x = rel_x;
        out_y = rel_y;
    }

public:
    void SetWidth(int value) override
    {
        if (m_width == value) return;

        m_width = value;
        if (m_width_type == SailUISizeType::SIZE_ABS)
            m_width_value = value;

        for (auto& child : m_children)
        {
            UpdateWidthLayout(child);
            UpdateXLayout(child);
        }

        m_2d_objects->SetWidth(value);
    }

    void SetHeight(int value) override
    {
        if (m_height == value) return;

        m_height = value;
        if (m_height_type == SailUISizeType::SIZE_ABS)
            m_height_value = value;

        for (auto& child : m_children)
        {
            UpdateHeightLayout(child);
            UpdateYLayout(child);
        }

        m_2d_objects->SetHeight(value);
    }

    void UpdateXLayout(const SailUIObjectPtr& child) override
    {
        if (child == nullptr || child->m_show_parent.lock() != std::enable_shared_from_this<SailUIObject>::shared_from_this())
            return;

        const auto x_type = child->m_x_type;
        if (x_type == SailUIPosType::POS_ABS || x_type == SailUIPosType::POS_ALIGN_STARTING)
            // 以控件左边为锚点，向右偏移x_value
            child->SetX(child->m_x_value);
        else if(x_type == SailUIPosType::POS_ALIGN_CENTER)
            // 以控件中心为锚点，向右偏移x_value
            child->SetX((m_width - child->GetWidth()) / 2 + child->m_x_value);
        else if(x_type == SailUIPosType::POS_ALIGN_ENDING)
            // 以控件右边为锚点，向左偏移x_value
            child->SetX(m_width - child->GetWidth() - child->m_x_value);
        else if(x_type == SailUIPosType::POS_PERCENT_STARTING)
            // 以控件左边为锚点，计算百分比位置
            child->SetX(m_width * child->m_x_value);
        else if(x_type == SailUIPosType::POS_PERCENT_CENTER)
            // 以控件中心为锚点，计算百分比位置
            child->SetX(static_cast<int>((m_width - child->GetWidth()) / 2.0f + m_width * (child->m_x_value / static_cast<float>(SailUISizeType::SIZE_PERCENT_MAX_VALUE))));
        else if(x_type == SailUIPosType::POS_PERCENT_ENDING)
            // 以控件右边为锚点，计算百分比位置
            child->SetX(static_cast<int>((m_width - child->GetWidth()) * (1.0f - child->m_x_value / static_cast<float>(SailUISizeType::SIZE_PERCENT_MAX_VALUE))));
    }

    void UpdateYLayout(const SailUIObjectPtr& child) override
    {
        if (child == nullptr || child->m_show_parent.lock() != std::enable_shared_from_this<SailUIObject>::shared_from_this())
            return;

        const auto y_type = child->m_y_type;
        if (y_type == SailUIPosType::POS_ABS || y_type == SailUIPosType::POS_ALIGN_STARTING)
            // 以控件左边为锚点，向右偏移y_value
            child->SetY(child->m_y_value);
        else if (y_type == SailUIPosType::POS_ALIGN_CENTER)
            // 以控件中心为锚点，向右偏移y_value
            child->SetY((m_height - child->GetHeight()) / 2 + child->m_y_value);
        else if (y_type == SailUIPosType::POS_ALIGN_ENDING)
            // 以控件右边为锚点，向左偏移y_value
            child->SetY(m_height - child->GetHeight() - child->m_y_value);
        else if (y_type == SailUIPosType::POS_PERCENT_STARTING)
            // 以控件左边为锚点，计算百分比位置
            child->SetY(m_height * child->m_y_value);
        else if (y_type == SailUIPosType::POS_PERCENT_CENTER)
            // 以控件中心为锚点，计算百分比位置
            child->SetY(static_cast<int>((m_height - child->GetHeight()) / 2.0f + m_width * (child->m_y_value / static_cast<float>(SailUISizeType::SIZE_PERCENT_MAX_VALUE))));
        else if (y_type == SailUIPosType::POS_PERCENT_ENDING)
            // 以控件右边为锚点，计算百分比位置
            child->SetY(static_cast<int>((m_height - child->GetHeight()) * (1.0f - child->m_y_value / static_cast<float>(SailUISizeType::SIZE_PERCENT_MAX_VALUE))));
    }

    void UpdateWidthLayout(const SailUIObjectPtr& child) override
    {
        if (child == nullptr || child->m_show_parent.lock() != std::enable_shared_from_this<SailUIObject>::shared_from_this())
            return;

        const auto width_type = child->m_width_type;
        if (width_type == SailUISizeType::SIZE_PERCENT)
        {
            auto real_width = m_width * child->m_width_value;
            if (real_width < 0) real_width = 0;
            child->SetWidth(real_width);
        }
        else if(width_type == SailUISizeType::SIZE_MARGIN)
        {
            auto real_width = m_width - child->m_width_value;
            if (real_width < 0) real_width = 0;
            child->SetWidth(real_width);
        }
        else if(width_type == SailUISizeType::SIZE_ABS)
        {
            auto real_width = child->m_width_value;
            if (real_width < 0) real_width = 0;
            child->SetWidth(real_width);
        }
    }

    void UpdateHeightLayout(const SailUIObjectPtr& child) override
    {
        if (child == nullptr || child->m_show_parent.lock() != std::enable_shared_from_this<SailUIObject>::shared_from_this())
            return;

        const auto height_type = child->m_height_type;
        if (height_type == SailUISizeType::SIZE_PERCENT)
        {
            auto real_height = m_height * child->m_height_value;
            if (real_height < 0) real_height = 0;
            child->SetHeight(real_height);
        }
        else if (height_type == SailUISizeType::SIZE_MARGIN)
        {
            auto real_height = m_height - child->m_height_value;
            if (real_height < 0) real_height = 0;
            child->SetHeight(real_height);
        }
        else if (height_type == SailUISizeType::SIZE_ABS)
        {
            auto real_height = child->m_height_value;
            if (real_height < 0) real_height = 0;
            child->SetHeight(real_height);
        }
    }

private:
    std::vector<SailUIObjectPtr> m_children;
    Sail2DObjectsPtr m_2d_objects;
};

using SailUIObjectsPtr = std::shared_ptr<SailUIObjects>;
using SailUIObjectsWeakPtr = std::weak_ptr<SailUIObjects>;

#endif
