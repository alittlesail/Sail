#ifndef SAIL_UI_EVENT_INCLUDED
#define SAIL_UI_EVENT_INCLUDED

#include <memory>
#include <string>

class SailUIObject;
using SailUIObjectPtr = std::shared_ptr<SailUIObject>;
using SailUIObjectWeakPtr = std::weak_ptr<SailUIObject>;

class SailUIEventListener : public std::enable_shared_from_this<SailUIEventListener>
{
public:
    virtual ~SailUIEventListener() {}
};

using SailUIEventListenerPtr = std::shared_ptr<SailUIEventListener>;
using SailUIEventListenerWeakPtr = std::weak_ptr<SailUIEventListener>;

class SailUIEvent
{
public:
    SailUIObjectPtr target;
};

template <typename E>
class SailUIEventTypeValue
{
public:
    static size_t Value() { return E::Value(); }
};

enum class SailUIEventType
{
    FOCUS_OUT = 1,
    FOCUS_IN,

    MOVE_OUT,
    MOVE_IN,

    HIDE,
    SHOW,

    CHANGED,
    CHANGED_END,
    SELECT_CHANGED,

    RESIZE,

    TAB_KEY,
    ENTER_KEY,
    AT_KEY,
    ESC_KEY,

    DRAG_UP,
    DRAG_DOWN,
    DRAG_LEFT,
    DRAG_RIGHT,

    BUTTON_DRAG,
    BUTTON_DRAG_BEGIN,
    BUTTON_DRAG_END,

    FINGER_DRAG,
    FINGER_DRAG_BEGIN,
    FINGER_DRAG_END,

    DROP,

    LONG_BUTTON_DOWN,
    LEFT_BUTTON_DOWN,
    LEFT_BUTTON_UP,
    MIDDLE_BUTTON_DOWN,
    MIDDLE_BUTTON_UP,
    MIDDLE_BUTTON_WHEEL,
    RIGHT_BUTTON_DOWN,
    RIGHT_BUTTON_UP,

    FINGER_DOWN,
    FINGER_UP,

    LEFT_CLICK,
    MIDDLE_CLICK,
    RIGHT_CLICK,
    FINGER_CLICK,

    TEXT_INPUT,
    KEY_DOWN,
    KEY_UP,

    MOUSE_MOVE,

    SYSTEM_SELECT_FILE,
    SYSTEM_SELECT_DIRECTORY,
    SYSTEM_SAVE_FILE,
};


// ʧȥ����
class SailUIFocusOutEvent final : public SailUIEvent { public: static size_t Value() { return static_cast<size_t>(SailUIEventType::FOCUS_OUT); } };
// ��ý���
class SailUIFocusInEvent final : public SailUIEvent { public: static size_t Value() { return static_cast<size_t>(SailUIEventType::FOCUS_IN); } };

// ����
class SailUIMoveOutEvent final : public SailUIEvent { public: static size_t Value() { return static_cast<size_t>(SailUIEventType::MOVE_OUT); } };
// �Ƴ�
class SailUIMoveInEvent final : public SailUIEvent { public: static size_t Value() { return static_cast<size_t>(SailUIEventType::MOVE_IN); } };

// ����
class SailUIHideEvent final : public SailUIEvent { public: static size_t Value() { return static_cast<size_t>(SailUIEventType::HIDE); } };
// ��ʾ
class SailUIShowEvent final : public SailUIEvent { public: static size_t Value() { return static_cast<size_t>(SailUIEventType::SHOW); } };

// �仯�¼�
class SailUIChangedEvent final : public SailUIEvent { public: static size_t Value() { return static_cast<size_t>(SailUIEventType::CHANGED); } };
class SailUIChangedEndEvent final : public SailUIEvent { public: static size_t Value() { return static_cast<size_t>(SailUIEventType::CHANGED_END); } };
class SailUISelectChangedEvent final : public SailUIEvent { public: static size_t Value() { return static_cast<size_t>(SailUIEventType::SELECT_CHANGED); } };
class SailUIResizeEvent final : public SailUIEvent { public: static size_t Value() { return static_cast<size_t>(SailUIEventType::RESIZE); } };

// tab����
class SailUITabKeyEvent final : public SailUIEvent { public: static size_t Value() { return static_cast<size_t>(SailUIEventType::TAB_KEY); } };
// enter����
class SailUIEnterKeyEvent final : public SailUIEvent { public: static size_t Value() { return static_cast<size_t>(SailUIEventType::ENTER_KEY); } };
// @����
class SailUIAtKeyEvent final : public SailUIEvent { public: static size_t Value() { return static_cast<size_t>(SailUIEventType::AT_KEY); } };
// ESC����
class SailUIEscKeyEvent final : public SailUIEvent { public: static size_t Value() { return static_cast<size_t>(SailUIEventType::ESC_KEY); } };

// ��X��ק
class SailUIDragUpEvent final : public SailUIEvent { public: static size_t Value() { return static_cast<size_t>(SailUIEventType::DRAG_UP); } };
class SailUIDragDownEvent final : public SailUIEvent { public: static size_t Value() { return static_cast<size_t>(SailUIEventType::DRAG_DOWN); } };
class SailUIDragLeftEvent final : public SailUIEvent { public: static size_t Value() { return static_cast<size_t>(SailUIEventType::DRAG_LEFT); } };
class SailUIDragRightEvent final : public SailUIEvent { public: static size_t Value() { return static_cast<size_t>(SailUIEventType::DRAG_RIGHT); } };

// ��ק��
class SailUIButtonDragEvent final : public SailUIEvent
{
public: static size_t Value() { return static_cast<size_t>(SailUIEventType::BUTTON_DRAG); }

    double rel_x = 0.0;
    double rel_y = 0.0;
    double delta_x = 0.0;
    double delta_y = 0.0;
    double abs_x = 0.0;
    double abs_y = 0.0;
};

// ��ק��ʼ
class SailUIButtonDragBeginEvent final : public SailUIEvent
{
public: static size_t Value() { return static_cast<size_t>(SailUIEventType::BUTTON_DRAG_BEGIN); }

    double rel_x = 0.0;
    double rel_y = 0.0;
    double delta_x = 0.0;
    double delta_y = 0.0;
    double abs_x = 0.0;
    double abs_y = 0.0;
};
// ��ק����
class SailUIButtonDragEndEvent final : public SailUIEvent
{
public: static size_t Value() { return static_cast<size_t>(SailUIEventType::BUTTON_DRAG_END); }

    double rel_x = 0.0;
    double rel_y = 0.0;
    double delta_x = 0.0;
    double delta_y = 0.0;
    double abs_x = 0.0;
    double abs_y = 0.0;
};

class SailUIFingerDragEvent final : public SailUIEvent
{
public: static size_t Value() { return static_cast<size_t>(SailUIEventType::FINGER_DRAG); }

    double rel_x = 0.0;
    double rel_y = 0.0;
    double delta_x = 0.0;
    double delta_y = 0.0;
    double abs_x = 0.0;
    double abs_y = 0.0;
    std::string finger_key;
    bool is_sfc = false;
};

// ��ק��ʼ
class SailUIFingerDragBeginEvent final : public SailUIEvent
{
public: static size_t Value() { return static_cast<size_t>(SailUIEventType::FINGER_DRAG_BEGIN); }

    double rel_x = 0.0;
    double rel_y = 0.0;
    double delta_x = 0.0;
    double delta_y = 0.0;
    double abs_x = 0.0;
    double abs_y = 0.0;
    std::string finger_key;
    bool is_sfc = false;
};

// ��ק����
class SailUIFingerDragEndEvent : public SailUIEvent
{
public: static size_t Value() { return static_cast<size_t>(SailUIEventType::FINGER_DRAG_END); }

    double rel_x = 0.0;
    double rel_y = 0.0;
    double delta_x = 0.0;
    double delta_y = 0.0;
    double abs_x = 0.0;
    double abs_y = 0.0;
    std::string finger_key;
    bool is_sfc = false;
};

// ��һ���ؼ���ק��ĳ���ؼ�֮��
class SailUIDropEvent final : public SailUIEvent
{
public: static size_t Value() { return static_cast<size_t>(SailUIEventType::DROP); }

    SailUIObjectPtr drop_target;
    double rel_x = 0.0;
    double rel_y = 0.0;
    double abs_x = 0.0;
    double abs_y = 0.0;
};

// ����
class SailUILongButtonDownEvent final : public SailUIEvent
{
public: static size_t Value() { return static_cast<size_t>(SailUIEventType::LONG_BUTTON_DOWN); }

    double abs_x = 0.0;
    double abs_y = 0.0;
    double rel_x = 0.0;
    double rel_y = 0.0;
    int count = 0;
    bool is_drag = false;       // �Ƿ�����ק��
};
// �������
class SailUILeftButtonDownEvent final : public SailUIEvent
{
public: static size_t Value() { return static_cast<size_t>(SailUIEventType::LEFT_BUTTON_DOWN); }

    double abs_x = 0.0;
    double abs_y = 0.0;
    double rel_x = 0.0;
    double rel_y = 0.0;
    int count = 0;
    bool is_drag = false;       // �Ƿ�����ק��
};
// �������
class SailUILeftButtonUpEvent final : public SailUIEvent
{
public: static size_t Value() { return static_cast<size_t>(SailUIEventType::LEFT_BUTTON_UP); }

    double abs_x = 0.0;
    double abs_y = 0.0;
    double rel_x = 0.0;
    double rel_y = 0.0;
    int count = 0;
    bool is_drag = false;       // �Ƿ�����ק��
};
// �м�����
class SailUIMiddleButtonDownEvent final : public SailUIEvent
{
public: static size_t Value() { return static_cast<size_t>(SailUIEventType::MIDDLE_BUTTON_DOWN); }

    double abs_x = 0.0;
    double abs_y = 0.0;
    double rel_x = 0.0;
    double rel_y = 0.0;
    int count = 0;
    bool is_drag = false;       // �Ƿ�����ק��
};
// �м�����
class SailUIMiddleButtonUpEvent final : public SailUIEvent
{
public: static size_t Value() { return static_cast<size_t>(SailUIEventType::MIDDLE_BUTTON_UP); }

    double abs_x = 0.0;
    double abs_y = 0.0;
    double rel_x = 0.0;
    double rel_y = 0.0;
    int count = 0;
    bool is_drag = false;       // �Ƿ�����ק��
};
// �м�����
class SailUIMiddleButtonWheelEvent final : public SailUIEvent
{
public: static size_t Value() { return static_cast<size_t>(SailUIEventType::MIDDLE_BUTTON_WHEEL); }

      double delta_x = 0.0;
      double delta_y = 0.0;
};
// �Ҽ�����
class SailUIRightButtonDownEvent final : public SailUIEvent
{
public: static size_t Value() { return static_cast<size_t>(SailUIEventType::RIGHT_BUTTON_DOWN); }

    double abs_x = 0.0;
    double abs_y = 0.0;
    double rel_x = 0.0;
    double rel_y = 0.0;
    int count = 0;
    bool is_drag = false;       // �Ƿ�����ק��
};
// �Ҽ�����
class SailUIRightButtonUpEvent final : public SailUIEvent
{
public: static size_t Value() { return static_cast<size_t>(SailUIEventType::RIGHT_BUTTON_UP); }

    double abs_x = 0.0;
    double abs_y = 0.0;
    double rel_x = 0.0;
    double rel_y = 0.0;
    int count = 0;
    bool is_drag = false;       // �Ƿ�����ק��
};

// ���ذ���
class SailUIFingerDownEvent final : public SailUIEvent
{
public: static size_t Value() { return static_cast<size_t>(SailUIEventType::FINGER_DOWN); }

    double abs_x = 0.0;
    double abs_y = 0.0;
    double rel_x = 0.0;
    double rel_y = 0.0;
    std::string finger_key;
    bool is_sfc = false;
    bool is_drag = false;       // �Ƿ�����ק��
};
// ���ص���
class SailUIFingerUpEvent final : public SailUIEvent
{
public: static size_t Value() { return static_cast<size_t>(SailUIEventType::FINGER_UP); }

    double abs_x = 0.0;
    double abs_y = 0.0;
    double rel_x = 0.0;
    double rel_y = 0.0;
    std::string finger_key;
    bool is_sfc = false;
    bool is_drag = false;       // �Ƿ�����ק��
};

// ����¼�
class SailUILeftClickEvent final : public SailUIEvent
{
public: static size_t Value() { return static_cast<size_t>(SailUIEventType::LEFT_CLICK); }

    bool is_drag = false;
    int count = 0;
};

class SailUIMiddleClickEvent final : public SailUIEvent
{
public: static size_t Value() { return static_cast<size_t>(SailUIEventType::MIDDLE_CLICK); }

    bool is_drag = false;
    int count = 0;
};

class SailUIRightClickEvent final : public SailUIEvent
{
public: static size_t Value() { return static_cast<size_t>(SailUIEventType::RIGHT_CLICK); }

      bool is_drag = false;
      int count = 0;
};

class SailUIFingerClickEvent final : public SailUIEvent
{
public: static size_t Value() { return static_cast<size_t>(SailUIEventType::FINGER_CLICK); }

    bool is_drag = false;
    int count = 0;
};

class SailUITextInputEvent final : public SailUIEvent
{
public: static size_t Value() { return static_cast<size_t>(SailUIEventType::TEXT_INPUT); }

    std::string text;
    bool custom = false;
};

class SailUIKeyDownEvent final : public SailUIEvent
{
public: static size_t Value() { return static_cast<size_t>(SailUIEventType::KEY_DOWN); }

    int mod = 0;
    int sym = 0;
    int scan_code = 0;
    bool custom = false;
    bool handled = false; // �Ƿ�����ˣ��������ؿ�ݼ�
};
class SailUIKeyUpEvent final : public SailUIEvent
{
public: static size_t Value() { return static_cast<size_t>(SailUIEventType::KEY_UP); }

    int mod = 0;
    int sym = 0;
    int scan_code = 0;
    bool custom = false;
    bool handled = false; // �Ƿ�����ˣ��������ؿ�ݼ�
};

class SailUIMouseMoveEvent final : public SailUIEvent
{
public: static size_t Value() { return static_cast<size_t>(SailUIEventType::MOUSE_MOVE); }

    double abs_x = 0.0;
    double abs_y = 0.0;
    double rel_x = 0.0;
    double rel_y = 0.0;
};

// ϵͳѡ���ļ����
class SailUISystemSelectFileEvent final : public SailUIEvent
{
public: static size_t Value() { return static_cast<size_t>(SailUIEventType::SYSTEM_SELECT_FILE); }

    std::string path;
};

// ϵͳѡ���ļ���
class SailUISystemSelectDirectoryEvent final : public SailUIEvent
{
public: static size_t Value() { return static_cast<size_t>(SailUIEventType::SYSTEM_SELECT_DIRECTORY); }

    std::string path;
};

// ϵͳ�����ļ�����ѡ����ļ����
class SailUISystemSaveFileEvent final : public SailUIEvent
{
public: static size_t Value() { return static_cast<size_t>(SailUIEventType::SYSTEM_SAVE_FILE); }

    std::string path;
};

#endif
