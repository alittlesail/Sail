#ifndef SAIL_UI_OBJECT_INCLUDED
#define SAIL_UI_OBJECT_INCLUDED

#include <memory>

#include "Carp/carp_event_dispatcher.hpp"
#include "sail_ui_event.hpp"
#include "Sail/SailClient/2D/sail_2d_object.hpp"

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
    SIZE_MARGIN = 4, // ����ڸ��ؼ���С��ȥһ��ֵ
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

public:
    virtual ~SailUIObject() {}

public:
    // ���úͻ�ȡ������ת��
    SailUIObjectPtr GetEventTransferTarget() const { return m_event_transfer_target.lock(); }
    void SetEventTransferTarget(const SailUIObjectWeakPtr& target) { m_event_transfer_target = target; }

    // ���úͻ�ȡ��קת��
    SailUIObjectPtr GetDragTransferTarget() const { return m_drag_transfer_target.lock(); }
    void SetDragTransferTarget(const SailUIObjectWeakPtr& target) { m_drag_transfer_target = target; }

public:
    // �ַ��¼�
    template <typename E>
    void DispatchEvent(E& event)
    {
        static_assert(std::is_base_of<SailUIEvent, E>::value, "any class must extends SailUIEvent");

        // ������������ˣ���ôֱ�ӷ���
        if (m_abs_disabled) return;

        // ���targetû�����ã�������Ϊ�Լ�
        if (!event.target) event.target = std::enable_shared_from_this<SailUIObject>::shared_from_this();

        // �����������ת�ƣ���ô�ͽ��д���
        auto event_target = m_event_transfer_target.lock();
        if (event_target)
        {
            if (event_target->m_abs_disabled) return;
            event_target->DispatchEventImpl(event);
            return;
        }

        // �������קת�ƣ���ô���ݲ�ͬ����ק���д���
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

        // �ַ��¼�
        DispatchEventImpl(event);
    }

private:
    // �Ƿ���ã����_abs_disableΪtrue����ô������DispatchEventʱʲô������
    bool m_abs_disabled = false;
    // �¼�ת�ƶ���
    SailUIObjectWeakPtr m_event_transfer_target; // ������ת��
    SailUIObjectWeakPtr m_drag_transfer_target;  // ��קת��

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

public:
    void SetHandCursor(bool value)
    {
        if (m_hand_cursor == value) return;
        m_hand_cursor = value;

        if (value)
        {
            AddEventListener<SailUIMoveInEvent, SailUIObject>(std::enable_shared_from_this<SailUIObject>::shared_from_this());
            AddEventListener<SailUIMoveOutEvent, SailUIObject>(std::enable_shared_from_this<SailUIObject>::shared_from_this());
        }
        else
        {
            RemoveEventListener<SailUIMoveInEvent>(std::enable_shared_from_this<SailUIObject>::shared_from_this());
            RemoveEventListener<SailUIMoveOutEvent>(std::enable_shared_from_this<SailUIObject>::shared_from_this());
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
    bool m_hand_cursor = false; // �������ʱ���Ƿ�������

public:
    Sail2DObjectPtr GetNativeShow() const { return m_show; }

    virtual const std::string& GetText() { static std::string empty; return empty; }
    virtual void SetText(const std::string& value) {}

    virtual bool GetSelected() { return false; }
    virtual void SetSelected(bool value) { }

public:
    virtual const std::vector<SailUIObjectPtr>& GetChildren() { static std::vector<SailUIObjectPtr> empty;  return empty; }
    virtual int GetChildCount() { return 0; }
    virtual bool SetChildIndex(const SailUIObjectPtr& object, int index) { return false; }
    virtual bool AddChild(const SailUIObjectPtr& object, int index = -1) { return false; }
    virtual bool RemoveChild(const SailUIObjectPtr& object) { return false; }
    virtual bool SpliceChild(int index, int count = -1) { return false; }
    virtual void RemoveAllChild() {}

public:
    bool IsFocus() const { return m_focus; }
    virtual bool IsInput() const { return false; }

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
    // ��ǰ�ؼ�������ڴ��ڵ����꣬��������ת
    void LocalToGlobal(const SailUIObjectPtr& target, int& out_x, int& out_y)
    {
        float x = 0.0f;
        float y = 0.0f;
        SailUIObjectPtr parent = std::enable_shared_from_this<SailUIObject>::shared_from_this();
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

    // ��ǰ�ؼ�������ڴ�������
    CarpMatrix2D LocalToGlobalMatrix2D(const SailUIObjectPtr& target)
    {
        CarpMatrix2D result;

        std::vector<SailUIObjectPtr> list;
        SailUIObjectPtr parent = std::enable_shared_from_this<SailUIObject>::shared_from_this();
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
            if (cur_index == 0) m.Scale(object->GetWidth(), object->GetHeight());
            m.Translation(-object->GetCenterX(), -object->GetCenterY());
            m.Rotate(object->GetAngle());
            m.Translation(object->GetCenterX(), object->GetCenterY());
            m.Scale(object->GetScaleX(), object->GetScaleY());
            m.Translation(object->GetX(), object->GetY());

            m.Multiply(result);
            result = m;
        }

        return result;
    }

    // ���ڵ����꣬����ڵ�ǰ�ؼ�������
    void GlobalToLocalMatrix2D(int x, int y, const SailUIObjectPtr& target, int& out_x, int& out_y)
    {
        std::vector<SailUIObjectPtr> list;
        SailUIObjectPtr parent = std::enable_shared_from_this<SailUIObject>::shared_from_this();
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

    // �Ӹ��ؼ��Ƴ�
    void RemoveFromParent()
    {
        auto parent = GetParent();
        if (!parent) return;
        parent->RemoveChild(std::enable_shared_from_this<SailUIObject>::shared_from_this());
    }

    // �ƶ������ؼ��Ķ���
    void MoveToTop()
    {
        auto parent = m_show_parent.lock();
        if (!parent) return;

        parent->SetChildIndex(std::enable_shared_from_this<SailUIObject>::shared_from_this(), parent->GetChildCount() - 1);
    }
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

protected:
	bool m_clip = false;		// �Ƿ񱻲ü�	���Ϊtrue��������ô����������
	bool m_ignore = false;		// �Ƿ����		���Ϊtrue��������ô������������
	Sail2DObjectPtr m_show;		// ��ʾ�ؼ�

public:
    SailUIObjectPtr GetParent() const
    {
        auto logic_parent = m_logic_parent.lock();
        if (logic_parent) return logic_parent;
        return m_show_parent.lock();
    }

protected:
	SailUIObjectWeakPtr m_show_parent;	// ��ʾ�����ؼ�����ֱ�ӵĸ��ؼ�
	SailUIObjectWeakPtr m_logic_parent;	// �߼������ؼ���������������

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
        if (parent) parent->UpdateXLayout(std::enable_shared_from_this<SailUIObject>::shared_from_this());
    }
    int GetXValue() const { return m_x_value; }
    void SetXValue(int value)
    {
        m_x_value = value;
        auto parent = m_show_parent.lock();
        if (parent) parent->UpdateXLayout(std::enable_shared_from_this<SailUIObject>::shared_from_this());
    }

    int GetY() const { return m_y; }
    void SetY(int value)
    {
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
        if (parent) parent->UpdateYLayout(std::enable_shared_from_this<SailUIObject>::shared_from_this());
    }
    int GetYValue() const { return m_y_value; }
    void SetYValue(int value)
    {
        m_y_value = value;
        auto parent = m_show_parent.lock();
        if (parent) parent->UpdateYLayout(std::enable_shared_from_this<SailUIObject>::shared_from_this());
    }

protected:
    int m_x = 0;		// x����
    int m_y = 0;		// y����
    SailUIPosType m_x_type = SailUIPosType::POS_ABS;    // ��������
    int m_x_value = 0; // x��������ֵ
    SailUIPosType m_y_type = SailUIPosType::POS_ABS;    // ��������
    int m_y_value = 0; // y��������ֵ

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
            parent->UpdateWidthLayout(std::enable_shared_from_this<SailUIObject>::shared_from_this());
            if (m_x_type != SailUIPosType::POS_ABS && m_x_type != SailUIPosType::POS_ALIGN_STARTING && m_x_type != SailUIPosType::POS_PERCENT_STARTING)
                parent->UpdateXLayout(std::enable_shared_from_this<SailUIObject>::shared_from_this());
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
            parent->UpdateWidthLayout(std::enable_shared_from_this<SailUIObject>::shared_from_this());
            if (m_x_type != SailUIPosType::POS_ABS && m_x_type != SailUIPosType::POS_ALIGN_STARTING && m_x_type != SailUIPosType::POS_PERCENT_STARTING)
                parent->UpdateXLayout(std::enable_shared_from_this<SailUIObject>::shared_from_this());
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
            parent->UpdateHeightLayout(std::enable_shared_from_this<SailUIObject>::shared_from_this());
            if (m_y_type != SailUIPosType::POS_ABS && m_y_type != SailUIPosType::POS_ALIGN_STARTING && m_y_type != SailUIPosType::POS_PERCENT_STARTING)
                parent->UpdateYLayout(std::enable_shared_from_this<SailUIObject>::shared_from_this());
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
            parent->UpdateHeightLayout(std::enable_shared_from_this<SailUIObject>::shared_from_this());
            if (m_y_type != SailUIPosType::POS_ABS && m_y_type != SailUIPosType::POS_ALIGN_STARTING && m_y_type != SailUIPosType::POS_PERCENT_STARTING)
                parent->UpdateYLayout(std::enable_shared_from_this<SailUIObject>::shared_from_this());
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

        parent->UpdateWidthLayout(std::enable_shared_from_this<SailUIObject>::shared_from_this());
        parent->UpdateHeightLayout(std::enable_shared_from_this<SailUIObject>::shared_from_this());
        parent->UpdateXLayout(std::enable_shared_from_this<SailUIObject>::shared_from_this());
        parent->UpdateYLayout(std::enable_shared_from_this<SailUIObject>::shared_from_this());
    }

    void PickUp(int x, int y, SailUIObjectPtr& out_pick, int& out_x, int& out_y)
    {
        // ����Ƿ񱻽��ã��Ƿ�ɼ�
        if (m_ignore || m_abs_disabled || m_abs_visible == false)
        {
            out_pick = nullptr;
            out_x = 0;
            out_y = 0;
            return;
        }

        // ���������ڿؼ��ĵ�
        auto xx = x - m_x;
        auto yy = y - m_y;

        // ������ת
        if (m_angle != 0)
        {
            // ����ת
            auto rad = -m_angle;
            auto cos = std::cosf(rad);
            auto sin = std::sinf(rad);
            auto xxx = xx * cos + yy * -sin;
            auto yyy = xx * sin + yy * cos;

            xx = xxx;
            yy = yyy;
        }

        // ��������
        if (m_scale_x > 0) xx /= m_scale_x;
        if (m_scale_y > 0) yy /= m_scale_y;

        // �ƶ���ԭ����λ��
        const auto rel_x = xx + m_center_x;
        const auto rel_y = yy + m_center_y;

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

        // �����ģ̬��ֱ�ӷ����Լ�
        if (m_modal)
        {
            out_pick = std::enable_shared_from_this<SailUIObject>::shared_from_this();
            out_x = rel_x;
            out_y = rel_y;
            return;
        }
            
        // ���λ���Ƿ��ڿؼ���Χ��
        if (m_pickup_rect && rel_x >= 0 && rel_y >= 0 && rel_x < m_width && rel_y < m_height)
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

    void PickUpSelf(int x, int y, SailUIObjectPtr& out_pick, int& out_x, int& out_y)
    {
        // ���������ڿؼ��ĵ�
        auto xx = x - m_x;
        auto yy = y - m_y;

        // ������ת
        if (m_angle != 0)
        {
            // ����ת
            auto rad = -m_angle;
            auto cos = std::cosf(rad);
            auto sin = std::sinf(rad);
            auto xxx = xx * cos + yy * -sin;
            auto yyy = xx * sin + yy * cos;

            xx = xxx;
            yy = yyy;
        }

        // ��������
        if (m_scale_x > 0) xx /= m_scale_x;
        if (m_scale_y > 0) yy /= m_scale_y;

        // �ƶ���ԭ����λ��
        auto rel_x = xx + m_center_x;
        auto rel_y = yy + m_center_y;

        if (m_scale_x <= 0 || m_scale_y <= 0)
        {
            out_pick = nullptr;
            out_x = rel_x;
            out_y = rel_y;
            return;
        }

        // ���λ���Ƿ��ڿؼ���Χ��
        if (m_abs_visible && rel_x >= 0 && rel_y >= 0 && rel_x < GetWidth() && rel_y < GetHeight())
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
    // �ü����Ż���Ⱦ�ٶ�
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
    int m_width = 0.0;		// ����
    int m_height = 0.0;		// �߶�
    SailUISizeType m_width_type = SailUISizeType::SIZE_ABS;    // ���Կ���
    int m_width_value = 0.0; // ��������ֵ
    SailUISizeType m_height_type = SailUISizeType::SIZE_ABS;    // ���Ը߶�
    int m_height_value = 0.0; // �߶�����ֵ

public:
    float GetScaleX() const { return m_scale_x; }
    void SetScaleX(float value) { m_scale_x = value; m_show->SetScaleX(m_scale_x); }
    float GetScaleY() const { return m_scale_y; }
    void SetScaleY(float value) { m_scale_y = value; m_show->SetScaleY(m_scale_y); }
    float GetAngle() const { return m_angle; }
    void SetAngle(float value) { m_angle = value; m_show->SetAngle(m_angle); }
    int GetCenterX() const { return m_center_x; }
    void SetCenterX(int value) { m_center_x = value; m_show->SetCenterX(m_center_x); }
    int GetCenterY() const { return m_center_y; }
    void SetCenterY(int value) { m_center_y = value; m_show->SetCenterY(m_center_y); }

protected:
    float m_scale_x = 1.0;              // ��������
    float m_scale_y = 1.0;              // �߶�����
    float m_angle = 0.0;                // ��ת�Ƕ�
    int m_center_x = 0;             // ��ת����
    int m_center_y = 0;             // ��ת����

public:
    float GetRed() const { return m_red; }
    void SetRed(float value) { m_red = value; m_show->SetRed(m_red); }
    float GetGreen() const { return m_green; }
    void SetGreen(float value) { m_green = value; m_show->SetGreen(m_green); }
    float GetBlue() const { return m_blue; }
    void SetBlue(float value) { m_blue = value; m_show->SetBlue(m_blue); }

protected:
    float m_red = 1.0;                  // �ؼ��ĺ�ɫ����
    float m_green = 1.0;                // �ؼ�����ɫ����
    float m_blue = 1.0;                 // �ؼ�����ɫ����

public:
    float GetAlpha() const { return m_alpha; }
    float GetAbsAlpha() const { return m_abs_alpha; }
    void SetAlpha(float value)
    {
        m_alpha = value;
        const auto parent = m_show_parent.lock();
        if (parent)
            m_abs_alpha = parent->m_abs_alpha * value;
        else
            m_abs_alpha = value;
        m_show->SetAlpha(m_abs_alpha);
    }

protected:
    float m_alpha = 1.0;       // ��͸����
    float m_abs_alpha = 1.0;   // ���Բ�͸����

public:
    bool GetVisible() const { return m_visible; }
    bool GetAbsVisible() const { return m_abs_visible; }
    void SetVisible(bool value)
    {
        m_visible = value;
        const auto parent = m_show_parent.lock();
        if (parent)
            m_abs_visible = parent->m_abs_visible && value;
        else
            m_abs_visible = value;
        m_show->SetVisible(m_abs_visible);
    }

protected:
    bool m_visible = true;      // ��ʾ
    bool m_abs_visible = true;  // ������ʾ

public:
    bool GetDisabled() const { return m_disabled; }
    bool GetAbsDisabled() const { return m_abs_disabled; }
    void SetDisabled(bool value)
    {
        m_disabled = value;
        const auto parent = m_show_parent.lock();
        if (parent)
            m_abs_disabled = parent->m_abs_disabled || value;
        else
            m_abs_disabled = value;
    }

protected:
    bool m_disabled = false;    // �ؼ��Ƿ����
    bool m_focus = false;       // ��ǰ�Ƿ��ǽ���

protected:
    bool m_modal = false;           // �Ƿ���ģ̬�ؼ�
    bool m_pickup_rect = true;      // ʰȡ��ʱ���Ƿ��жϾ�������
    bool m_pickup_child = false;    // �Ƿ�ʰȡ�ӿؼ�
    bool m_pickup_this = false;     // �Ƿ�ʰȡ�Լ�
    bool m_can_scroll = false;      // �Ƿ��ǹ����ؼ�

public:
    SailUIObjectUserDataWeakPtr user_data;
};

#endif