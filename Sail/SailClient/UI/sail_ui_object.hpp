#ifndef SAIL_UI_OBJECT_INCLUDED
#define SAIL_UI_OBJECT_INCLUDED

#include <memory>

#include "Carp/carp_event_dispatcher.hpp"
#include "sail_ui_event.hpp"
#include "Sail/SailClient/2D/sail_2d_object.hpp"

#ifdef GetYValue
#undef GetYValue
#endif

enum class SailUIPosType
{
    POS_ABS = 1,

    POS_ALIGN_STARTING = 2,
    POS_ALIGN_CENTER = 3,
    POS_ALIGN_ENDING = 4,

    POS_PERCENT_STARTING = 7,
    POS_PERCENT_CENTER = 8,
    POS_PERCENT_ENDING = 9,
};

enum class SailUISizeType
{
    SIZE_ABS = 1,
    SIZE_PERCENT = 2,
    SIZE_MARGIN = 4, // 相对于父控件大小减去一定值

    SIZE_PERCENT_MAX_VALUE = std::numeric_limits<int>::max(),
};

class SailUIObjectUserData
{
    
};
using SailUIObjectUserDataPtr = std::shared_ptr<SailUIObjectUserData>;
using SailUIObjectUserDataWeakPtr = std::weak_ptr<SailUIObjectUserData>;

class SailUIObject : public std::enable_shared_from_this<SailUIObject>
                    , public SailUIEventListener
                    , public CarpEventDispatcher<SailUIEventListener, SailUIEvent, SailUIEventTypeValue>
{
public:
    friend class SailUISystem;
    friend class SailUIObjects;
    friend class SailUIObject;

public:
    virtual ~SailUIObject() {}
    virtual void Init() {}

public:
    template <typename T>
    static std::shared_ptr<T> CreateUI()
    {
        auto ui = std::make_shared<T>();
        ui->Init();
        return ui;
    }

public:
    // 设置和获取无条件转移
    SailUIObjectPtr GetEventTransferTarget() const { return m_event_transfer_target.lock(); }
    void SetEventTransferTarget(const SailUIObjectWeakPtr& target) { m_event_transfer_target = target; }

    // 设置和获取拖拽转移
    SailUIObjectPtr GetDragTransferTarget() const { return m_drag_transfer_target.lock(); }
    void SetDragTransferTarget(const SailUIObjectWeakPtr& target) { m_drag_transfer_target = target; }

public:
    // 分发事件
    template <typename E>
    void DispatchEvent(E& event)
    {
        static_assert(std::is_base_of<SailUIEvent, E>::value, "any class must extends SailUIEvent");

        // 如果本身禁用了，那么直接返回
        if (m_abs_disabled) return;

        // 如果target没有设置，就设置为自己
        if (!event.target) event.target = shared_from_this();

        // 如果有无条件转移，那么就进行处理
        auto event_target = m_event_transfer_target.lock();
        if (event_target)
        {
            if (event_target->m_abs_disabled) return;
            event_target->DispatchEventImpl(event);
            return;
        }

        // 如果有拖拽转移，那么根据不同的拖拽进行处理
        auto drag_target = m_drag_transfer_target.lock();
        if (drag_target)
        {
            if (std::is_same<E, SailUIButtonDragEvent>::value || std::is_same<E, SailUIButtonDragBeginEvent>::value)
            {
                if (drag_target->m_abs_disabled) return;
                drag_target->DispatchEventImpl(event);
                return;
            }

            if (std::is_same<E, SailUIButtonDragEndEvent>::value)
            {
                if (drag_target->m_abs_disabled) return;
                drag_target->DispatchEventImpl(event);
                return;
            }
        }

        // 分发事件
        DispatchEventImpl(event);
    }

private:
    // 是否禁用，如果_abs_disable为true，那么它调用DispatchEvent时什么都不做
    bool m_abs_disabled = false;
    // 事件转移对象
    SailUIObjectWeakPtr m_event_transfer_target; // 无条件转移
    SailUIObjectWeakPtr m_drag_transfer_target;  // 拖拽转移

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

public:
    void SetHandCursor(bool value)
    {
        if (m_hand_cursor == value) return;
        m_hand_cursor = value;

        if (value)
        {
            AddEventListener<SailUIMoveInEvent, SailUIObject>(shared_from_this());
            AddEventListener<SailUIMoveOutEvent, SailUIObject>(shared_from_this());
        }
        else
        {
            RemoveEventListener<SailUIMoveInEvent>(shared_from_this());
            RemoveEventListener<SailUIMoveOutEvent>(shared_from_this());
        }
    }
    bool GetHandCursor() const { return m_hand_cursor; }

public:
    void OnEvent(const SailUIMoveInEvent& event)
    {
        if (!m_hand_cursor) return;
        // System_SetHandCursor();
    }

    void OnEvent(const SailUIMoveOutEvent& event)
    {
        if (!m_hand_cursor) return;
        // System_SetNormalCursor();
    }

protected:
    bool m_hand_cursor = false; // 鼠标移入时，是否变成手势

public:
    Sail2DObjectPtr GetNativeShow() const { return m_show; }

    // 文本控件相关接口
public:
    virtual const std::string& GetText() { static std::string empty; return empty; }
    virtual void SetText(const std::string& value) {}

    // 选择控件相关接口
public:
    virtual bool GetSelected() { return false; }
    virtual void SetSelected(bool value) { }

    // 复核控件相关接口
public:
    virtual const std::vector<SailUIObjectPtr>& GetChildren() const { static std::vector<SailUIObjectPtr> empty;  return empty; }
    virtual int GetChildCount() const { return 0; }
    virtual int GetChildIndex(const SailUIObjectPtr& child) const { return -1; }
    virtual SailUIObjectPtr GetChildByIndex(int index) const { return nullptr; }
    virtual bool SetChildIndex(const SailUIObjectPtr& object, int index) { return false; }
    virtual bool AddChild(const SailUIObjectPtr& object, int index = -1) { return false; }
    virtual bool RemoveChild(const SailUIObjectPtr& object) { return false; }
    virtual int SpliceChild(int index, int count = -1) { return 0; }
    virtual void RemoveAllChild() {}

    // 编辑控件相关接口
public:
    virtual bool GetEditable() const { return false; }
    virtual int GetFontSize() const { return 0; }
    virtual int GetCursorX() const { return 0; }
    virtual int GetCursorY() const { return 0; }
    virtual bool IsInput() const { return false; }

public:
    bool IsFocus() const;
    void SetFocus(bool value);

public:
    void SetCanScroll(bool value) { m_can_scroll = value; }
    bool GetCanScroll() const { return m_can_scroll; }
    void SetModal(bool value) { m_modal = value; }
    bool GetModal() const { return m_modal; }

public:
    void SetClip(bool value) { m_clip = value; m_show->SetClip(value); }
    bool GetClip() const { return m_clip; }
    void SetIgnore(bool value) { m_ignore = value; }
    bool GetIgnore() const { return m_ignore; }

public:
    // 检查鼠标是否在当前控件范围内
    bool IsMouseIn();

public:
    // 当前控件，相对于窗口的坐标，不考虑旋转
    void LocalToGlobal(const SailUIObjectPtr& target, int& out_x, int& out_y)
    {
        float x = 0.0f;
        float y = 0.0f;
        SailUIObjectPtr parent = shared_from_this();
        while (parent)
        {
            float scale_x = 1.0;
            float scale_y = 1.0;
            auto show_parent = parent->m_show_parent.lock();
            if (show_parent)
            {
                scale_x = show_parent->GetScaleX();
                scale_y = show_parent->GetScaleY();
            }
            x += scale_x * parent->GetX();
            y += scale_y * parent->GetY();
            if (target == show_parent) break;
            parent = show_parent;
        }

        out_x = static_cast<int>(floor(x));
        out_y = static_cast<int>(floor(y));
    }

    // 当前控件，相对于窗口坐标
    CarpMatrix2D LocalToGlobalMatrix2D(const SailUIObjectPtr& target)
    {
        CarpMatrix2D result;

        std::vector<SailUIObjectPtr> list;
        SailUIObjectPtr parent = shared_from_this();
        while (parent)
        {
            if (target == parent) break;
            list.emplace_back(parent);
            parent = parent->m_show_parent.lock();
        }

        for (size_t index = 0; index < list.size(); ++index)
        {
            const size_t cur_index = list.size() - 1 - index;
            const auto& object = list[cur_index];
            CarpMatrix2D m;
            if (cur_index == 0) m.Scale(static_cast<float>(object->GetWidth()), static_cast<float>(object->GetHeight()));
            m.Translation(static_cast<float>(-object->GetCenterX()), static_cast<float>(-object->GetCenterY()));
            m.Rotate(object->GetRotate());
            m.Translation(static_cast<float>(object->GetCenterX()), static_cast<float>(object->GetCenterY()));
            m.Scale(object->GetScaleX(), object->GetScaleY());
            m.Translation(static_cast<float>(object->GetX()), static_cast<float>(object->GetY()));

            m.Multiply(result);
            result = m;
        }

        return result;
    }

    // 窗口的坐标，相对于当前控件的坐标
    void GlobalToLocalMatrix2D(int x, int y, const SailUIObjectPtr& target, int& out_x, int& out_y)
    {
        std::vector<SailUIObjectPtr> list;
        SailUIObjectPtr parent = shared_from_this();
        while (parent)
        {
            if (target == parent) break;
            list.emplace_back(parent);
            parent = parent->m_show_parent.lock();
        }

        SailUIObjectPtr pick;
        for (size_t index = 0; index < list.size(); ++index)
        {
            const size_t cur_index = list.size() - 1 - index;
            const auto& object = list[cur_index];

            object->PickUpSelf(x, y, pick, out_x, out_y);
        }
    }

    // 从父控件移除
    void RemoveFromParent()
    {
        auto parent = GetParent();
        if (!parent) return;
        parent->RemoveChild(shared_from_this());
    }

    // 移动到父控件的顶层
    void MoveToTop()
    {
        auto parent = m_show_parent.lock();
        if (!parent) return;

        parent->SetChildIndex(shared_from_this(), parent->GetChildCount() - 1);
    }
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

protected:
	bool m_clip = false;		// 是否被裁减	如果为true，不管怎么样都看不到
	bool m_ignore = false;		// 是否忽略		如果为true，不管怎么样都操作不到
	Sail2DObjectPtr m_show;		// 显示控件

public:
    SailUIObjectPtr GetParent() const
    {
        auto logic_parent = m_logic_parent.lock();
        if (logic_parent) return logic_parent;
        return m_show_parent.lock();
    }
    SailUIObjectPtr GetShowParent() const { return m_show_parent.lock(); }
    SailUIObjectPtr GetLogicParent() const { return m_logic_parent.lock(); }

protected:
	SailUIObjectWeakPtr m_show_parent;	// 显示级父控件，最直接的父控件
	SailUIObjectWeakPtr m_logic_parent;	// 逻辑级父控件，用于跳级管理

public:
    int GetX() const { return m_x; }
    void SetX(int value)
    {
        if (m_x == value) return;
        m_x = value;
        if (m_x_type == SailUIPosType::POS_ABS) m_x_value = value;
        m_show->SetX(m_x);
    }
    SailUIPosType GetXType() const { return m_x_type; }
    void SetXType(SailUIPosType value)
    {
        if (m_x_type == value) return;
        m_x_type = value;
        auto parent = m_show_parent.lock();
        if (parent) parent->UpdateXLayout(shared_from_this());
    }
    int GetXValue() const { return m_x_value; }
    void SetXValue(int value)
    {
        if (m_x_value == value) return;
        m_x_value = value;
        auto parent = m_show_parent.lock();
        if (parent) parent->UpdateXLayout(shared_from_this());
    }

    int GetY() const { return m_y; }
    void SetY(int value)
    {
        if (m_y == value) return;
        m_y = value;
        if (m_y_type == SailUIPosType::POS_ABS) m_y_value = value;
        m_show->SetY(static_cast<int>(floor(m_y)));
    }
    SailUIPosType GetYType() const { return m_y_type; }
    void SetYType(SailUIPosType value)
    {
        if (m_y_type == value) return;
        m_y_type = value;
        auto parent = m_show_parent.lock();
        if (parent) parent->UpdateYLayout(shared_from_this());
    }
    int GetYValue() const { return m_y_value; }
    void SetYValue(int value)
    {
        if (m_y_value == value) return;
        m_y_value = value;
        auto parent = m_show_parent.lock();
        if (parent) parent->UpdateYLayout(shared_from_this());
    }

protected:
    int m_x = 0;		// x坐标
    int m_y = 0;		// y坐标
    SailUIPosType m_x_type = SailUIPosType::POS_ABS;    // 绝对坐标
    int m_x_value = 0; // x坐标类型值
    SailUIPosType m_y_type = SailUIPosType::POS_ABS;    // 绝对坐标
    int m_y_value = 0; // y坐标类型值

public:
    virtual int GetWidth() const { return m_width; }
    virtual void SetWidth(int value)
    {
        if (m_width == value) return;
        m_width = value;
        if (m_width_type == SailUISizeType::SIZE_ABS)
            m_width_value = value;
        m_show->SetWidth(static_cast<int>(floor(m_width)));
    }
    SailUISizeType GetWidthType() const { return m_width_type; }
    void SetWidthType(SailUISizeType value)
    {
        if (m_width_type == value) return;
        m_width_type = value;

        auto parent = m_show_parent.lock();
        if (parent)
        {
            parent->UpdateWidthLayout(shared_from_this());
            if (m_x_type != SailUIPosType::POS_ABS && m_x_type != SailUIPosType::POS_ALIGN_STARTING && m_x_type != SailUIPosType::POS_PERCENT_STARTING)
                parent->UpdateXLayout(shared_from_this());
        }
        else if (m_width_type == SailUISizeType::SIZE_ABS)
        {
            SetWidth(m_width_value);
        }
    }
    int GetWidthValue() const { return m_width_value; }
    void SetWidthValue(int value)
    {
        if (m_width_value == value) return;
        m_width_value = value;

        auto parent = m_show_parent.lock();
        if (parent)
        {
            parent->UpdateWidthLayout(shared_from_this());
            if (m_x_type != SailUIPosType::POS_ABS && m_x_type != SailUIPosType::POS_ALIGN_STARTING && m_x_type != SailUIPosType::POS_PERCENT_STARTING)
                parent->UpdateXLayout(shared_from_this());
        }
        else if (m_width_type == SailUISizeType::SIZE_ABS)
        {
            SetWidth(m_width_value);
        }
    }

    virtual int GetHeight() const { return m_height; }
    virtual void SetHeight(int value)
    {
        m_height = value;
        if (m_height_type == SailUISizeType::SIZE_ABS)
            m_height_value = value;
        m_show->SetHeight(static_cast<int>(floor(m_height)));
    }
    SailUISizeType GetHeightType() const { return m_height_type; }
    void SetHeightType(SailUISizeType value)
    {
        if (m_height_type == value) return;
        m_height_type = value;

        auto parent = m_show_parent.lock();
        if (parent)
        {
            parent->UpdateHeightLayout(shared_from_this());
            if (m_y_type != SailUIPosType::POS_ABS && m_y_type != SailUIPosType::POS_ALIGN_STARTING && m_y_type != SailUIPosType::POS_PERCENT_STARTING)
                parent->UpdateYLayout(shared_from_this());
        }
        else if (m_height_type == SailUISizeType::SIZE_ABS)
        {
            SetHeight(m_height_value);
        }
    }
    int GetHeightValue() const { return m_height_value; }
    void SetHeightValue(int value)
    {
        if (m_height_value == value) return;
        m_height_value = value;

        auto parent = m_show_parent.lock();
        if (parent)
        {
            parent->UpdateHeightLayout(shared_from_this());
            if (m_y_type != SailUIPosType::POS_ABS && m_y_type != SailUIPosType::POS_ALIGN_STARTING && m_y_type != SailUIPosType::POS_PERCENT_STARTING)
                parent->UpdateYLayout(shared_from_this());
        }
        else if (m_height_type == SailUISizeType::SIZE_ABS)
        {
            SetHeight(m_height_value);
        }
    }

public:
    void UpdateLayout()
    {
        auto parent = m_show_parent.lock();
        if (!parent) return;

        parent->UpdateWidthLayout(shared_from_this());
        parent->UpdateHeightLayout(shared_from_this());
        parent->UpdateXLayout(shared_from_this());
        parent->UpdateYLayout(shared_from_this());
    }

    virtual void PickUp(int x, int y, SailUIObjectPtr& out_pick, int& out_x, int& out_y)
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
        float xx = static_cast<float>(x - m_x);
        float yy = static_cast<float>(y - m_y);

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
                out_pick = shared_from_this();
                out_x = rel_x;
                out_y = rel_y;
                return;
            }

            out_pick = nullptr;
            out_x = rel_x;
            out_y = rel_y;
            return;
        }

        // 如果是模态则直接返回自己
        if (m_modal)
        {
            out_pick = shared_from_this();
            out_x = rel_x;
            out_y = rel_y;
            return;
        }
            
        // 检查位置是否在控件范围内
        if (rel_x >= 0 && rel_y >= 0 && rel_x < m_width && rel_y < m_height)
        {
            out_pick = shared_from_this();
            out_x = rel_x;
            out_y = rel_y;
            return;
        }

        out_pick = nullptr;
        out_x = rel_x;
        out_y = rel_y;
    }

    void PickUpSelf(int x, int y, SailUIObjectPtr& out_pick, int& out_x, int& out_y)
    {
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
            out_pick = nullptr;
            out_x = rel_x;
            out_y = rel_y;
            return;
        }

        // 检查位置是否在控件范围内
        if (m_abs_visible && rel_x >= 0 && rel_y >= 0 && rel_x < GetWidth() && rel_y < GetHeight())
        {
            out_pick = shared_from_this();
            out_x = rel_x;
            out_y = rel_y;
            return;
        }

        out_pick = nullptr;
        out_x = rel_x;
        out_y = rel_y;
    }

public:
    // 裁剪，优化渲染速度
    virtual void ClipRect(double x, double y, double width, double height, double h_move, double v_move)
    {
    }

    virtual void DeepLayout()
    {
    }

public:
    virtual void UpdateWidthLayout(const SailUIObjectPtr& object) {}
    virtual void UpdateHeightLayout(const SailUIObjectPtr& object) {}
    virtual void UpdateXLayout(const SailUIObjectPtr& object) {}
    virtual void UpdateYLayout(const SailUIObjectPtr& object) {}

protected:
    int m_width = 0;		// 宽度
    int m_height = 0;		// 高度
    SailUISizeType m_width_type = SailUISizeType::SIZE_ABS;    // 绝对宽度
    int m_width_value = 0; // 宽度类型值
    SailUISizeType m_height_type = SailUISizeType::SIZE_ABS;    // 绝对高度
    int m_height_value = 0; // 高度类型值

public:
    float GetScaleX() const { return m_scale_x; }
    void SetScaleX(float value) { m_scale_x = value; m_show->SetScaleX(m_scale_x); }
    float GetScaleY() const { return m_scale_y; }
    void SetScaleY(float value) { m_scale_y = value; m_show->SetScaleY(m_scale_y); }
    float GetRotate() const { return m_rotate; }
    void SetRotate(float value) { m_rotate = value; m_show->SetRotate(m_rotate); }
    int GetCenterX() const { return m_center_x; }
    void SetCenterX(int value) { if (m_center_x == value) return; m_center_x = value; m_show->SetCenterX(m_center_x); }
    int GetCenterY() const { return m_center_y; }
    void SetCenterY(int value) { if (m_center_y == value) return; m_center_y = value; m_show->SetCenterY(m_center_y); }

protected:
    float m_scale_x = 1.0;          // 宽度缩放
    float m_scale_y = 1.0;          // 高度缩放
    float m_rotate = 0.0;           // 旋转弧度
    int m_center_x = 0;             // 旋转中心
    int m_center_y = 0;             // 旋转中心

public:
    float GetRed() const { return m_red; }
    void SetRed(float value) { m_red = value; m_show->SetRed(m_red); }
    float GetGreen() const { return m_green; }
    void SetGreen(float value) { m_green = value; m_show->SetGreen(m_green); }
    float GetBlue() const { return m_blue; }
    void SetBlue(float value) { m_blue = value; m_show->SetBlue(m_blue); }

protected:
    float m_red = 1.0;                  // 控件的红色分量
    float m_green = 1.0;                // 控件的绿色分量
    float m_blue = 1.0;                 // 控件的蓝色分量

public:
    float GetAlpha() const { return m_alpha; }
    void SetAlpha(float value)
    {
        m_alpha = value;
        UpdateAlpha();
    }
    virtual void UpdateAlpha()
    {
        const auto parent = m_show_parent.lock();
        if (parent)
            m_abs_alpha = parent->m_abs_alpha * m_alpha;
        else
            m_abs_alpha = m_alpha;
        m_show->SetAlpha(m_abs_alpha);
    }

protected:
    float m_alpha = 1.0;       // 不透明度
    float m_abs_alpha = 1.0;   // 绝对不透明度

public:
    bool GetVisible() const { return m_visible; }
    void SetVisible(bool value);
    virtual void UpdateVisible();

protected:
    bool m_visible = true;      // 显示
    bool m_abs_visible = true;  // 绝对显示

public:
    bool GetDisabled() const { return m_disabled; }
    void SetDisabled(bool value);
    virtual void UpdateDisabled();

protected:
    bool m_disabled = false;    // 控件是否禁用

protected:
    bool m_modal = false;           // 是否是模态控件
    bool m_pickup_child = false;    // 是否拾取子控件
    bool m_pickup_this = false;     // 是否拾取自己
    bool m_can_scroll = false;      // 是否是滚动控件

public:
    SailUIObjectUserDataWeakPtr user_data;
};

#endif
