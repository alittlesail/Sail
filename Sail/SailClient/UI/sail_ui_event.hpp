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


// 失去焦点
class SailUIFocusOutEvent final : public SailUIEvent { public: static size_t Value() { return static_cast<size_t>(SailUIEventType::FOCUS_OUT); } };
// 获得焦点
class SailUIFocusInEvent final : public SailUIEvent { public: static size_t Value() { return static_cast<size_t>(SailUIEventType::FOCUS_IN); } };

// 移入
class SailUIMoveOutEvent final : public SailUIEvent { public: static size_t Value() { return static_cast<size_t>(SailUIEventType::MOVE_OUT); } };
// 移出
class SailUIMoveInEvent final : public SailUIEvent { public: static size_t Value() { return static_cast<size_t>(SailUIEventType::MOVE_IN); } };

// 隐藏
class SailUIHideEvent final : public SailUIEvent { public: static size_t Value() { return static_cast<size_t>(SailUIEventType::HIDE); } };
// 显示
class SailUIShowEvent final : public SailUIEvent { public: static size_t Value() { return static_cast<size_t>(SailUIEventType::SHOW); } };

// 变化事件
class SailUIChangedEvent final : public SailUIEvent { public: static size_t Value() { return static_cast<size_t>(SailUIEventType::CHANGED); } };
class SailUIChangedEndEvent final : public SailUIEvent { public: static size_t Value() { return static_cast<size_t>(SailUIEventType::CHANGED_END); } };
class SailUISelectChangedEvent final : public SailUIEvent { public: static size_t Value() { return static_cast<size_t>(SailUIEventType::SELECT_CHANGED); } };
class SailUIResizeEvent final : public SailUIEvent { public: static size_t Value() { return static_cast<size_t>(SailUIEventType::RESIZE); } };

// tab按键
class SailUITabKeyEvent final : public SailUIEvent { public: static size_t Value() { return static_cast<size_t>(SailUIEventType::TAB_KEY); } };
// enter按键
class SailUIEnterKeyEvent final : public SailUIEvent { public: static size_t Value() { return static_cast<size_t>(SailUIEventType::ENTER_KEY); } };
// @按键
class SailUIAtKeyEvent final : public SailUIEvent { public: static size_t Value() { return static_cast<size_t>(SailUIEventType::AT_KEY); } };
// ESC按键
class SailUIEscKeyEvent final : public SailUIEvent { public: static size_t Value() { return static_cast<size_t>(SailUIEventType::ESC_KEY); } };

// 向X拖拽
class SailUIDragUpEvent final : public SailUIEvent { public: static size_t Value() { return static_cast<size_t>(SailUIEventType::DRAG_UP); } };
class SailUIDragDownEvent final : public SailUIEvent { public: static size_t Value() { return static_cast<size_t>(SailUIEventType::DRAG_DOWN); } };
class SailUIDragLeftEvent final : public SailUIEvent { public: static size_t Value() { return static_cast<size_t>(SailUIEventType::DRAG_LEFT); } };
class SailUIDragRightEvent final : public SailUIEvent { public: static size_t Value() { return static_cast<size_t>(SailUIEventType::DRAG_RIGHT); } };

// 拖拽中
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

// 拖拽开始
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
// 拖拽结束
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

// 拖拽开始
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

// 拖拽结束
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

// 将一个控件拖拽到某个控件之上
class SailUIDropEvent final : public SailUIEvent
{
public: static size_t Value() { return static_cast<size_t>(SailUIEventType::DROP); }

    SailUIObjectPtr drop_target;
    double rel_x = 0.0;
    double rel_y = 0.0;
    double abs_x = 0.0;
    double abs_y = 0.0;
};

// 长按
class SailUILongButtonDownEvent final : public SailUIEvent
{
public: static size_t Value() { return static_cast<size_t>(SailUIEventType::LONG_BUTTON_DOWN); }

    double abs_x = 0.0;
    double abs_y = 0.0;
    double rel_x = 0.0;
    double rel_y = 0.0;
    int count = 0;
    bool is_drag = false;       // 是否有拖拽过
};
// 左键按下
class SailUILeftButtonDownEvent final : public SailUIEvent
{
public: static size_t Value() { return static_cast<size_t>(SailUIEventType::LEFT_BUTTON_DOWN); }

    double abs_x = 0.0;
    double abs_y = 0.0;
    double rel_x = 0.0;
    double rel_y = 0.0;
    int count = 0;
    bool is_drag = false;       // 是否有拖拽过
};
// 左键弹起
class SailUILeftButtonUpEvent final : public SailUIEvent
{
public: static size_t Value() { return static_cast<size_t>(SailUIEventType::LEFT_BUTTON_UP); }

    double abs_x = 0.0;
    double abs_y = 0.0;
    double rel_x = 0.0;
    double rel_y = 0.0;
    int count = 0;
    bool is_drag = false;       // 是否有拖拽过
};
// 中键按下
class SailUIMiddleButtonDownEvent final : public SailUIEvent
{
public: static size_t Value() { return static_cast<size_t>(SailUIEventType::MIDDLE_BUTTON_DOWN); }

    double abs_x = 0.0;
    double abs_y = 0.0;
    double rel_x = 0.0;
    double rel_y = 0.0;
    int count = 0;
    bool is_drag = false;       // 是否有拖拽过
};
// 中键弹起
class SailUIMiddleButtonUpEvent final : public SailUIEvent
{
public: static size_t Value() { return static_cast<size_t>(SailUIEventType::MIDDLE_BUTTON_UP); }

    double abs_x = 0.0;
    double abs_y = 0.0;
    double rel_x = 0.0;
    double rel_y = 0.0;
    int count = 0;
    bool is_drag = false;       // 是否有拖拽过
};
// 中键滚动
class SailUIMiddleButtonWheelEvent final : public SailUIEvent
{
public: static size_t Value() { return static_cast<size_t>(SailUIEventType::MIDDLE_BUTTON_WHEEL); }

      double delta_x = 0.0;
      double delta_y = 0.0;
};
// 右键按下
class SailUIRightButtonDownEvent final : public SailUIEvent
{
public: static size_t Value() { return static_cast<size_t>(SailUIEventType::RIGHT_BUTTON_DOWN); }

    double abs_x = 0.0;
    double abs_y = 0.0;
    double rel_x = 0.0;
    double rel_y = 0.0;
    int count = 0;
    bool is_drag = false;       // 是否有拖拽过
};
// 右键弹起
class SailUIRightButtonUpEvent final : public SailUIEvent
{
public: static size_t Value() { return static_cast<size_t>(SailUIEventType::RIGHT_BUTTON_UP); }

    double abs_x = 0.0;
    double abs_y = 0.0;
    double rel_x = 0.0;
    double rel_y = 0.0;
    int count = 0;
    bool is_drag = false;       // 是否有拖拽过
};

// 触控按下
class SailUIFingerDownEvent final : public SailUIEvent
{
public: static size_t Value() { return static_cast<size_t>(SailUIEventType::FINGER_DOWN); }

    double abs_x = 0.0;
    double abs_y = 0.0;
    double rel_x = 0.0;
    double rel_y = 0.0;
    std::string finger_key;
    bool is_sfc = false;
    bool is_drag = false;       // 是否有拖拽过
};
// 触控弹起
class SailUIFingerUpEvent final : public SailUIEvent
{
public: static size_t Value() { return static_cast<size_t>(SailUIEventType::FINGER_UP); }

    double abs_x = 0.0;
    double abs_y = 0.0;
    double rel_x = 0.0;
    double rel_y = 0.0;
    std::string finger_key;
    bool is_sfc = false;
    bool is_drag = false;       // 是否有拖拽过
};

// 点击事件
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
    bool handled = false; // 是否处理过了，用于拦截快捷键
};
class SailUIKeyUpEvent final : public SailUIEvent
{
public: static size_t Value() { return static_cast<size_t>(SailUIEventType::KEY_UP); }

    int mod = 0;
    int sym = 0;
    int scan_code = 0;
    bool custom = false;
    bool handled = false; // 是否处理过了，用于拦截快捷键
};

class SailUIMouseMoveEvent final : public SailUIEvent
{
public: static size_t Value() { return static_cast<size_t>(SailUIEventType::MOUSE_MOVE); }

    double abs_x = 0.0;
    double abs_y = 0.0;
    double rel_x = 0.0;
    double rel_y = 0.0;
};

// 系统选择文件完毕
class SailUISystemSelectFileEvent final : public SailUIEvent
{
public: static size_t Value() { return static_cast<size_t>(SailUIEventType::SYSTEM_SELECT_FILE); }

    std::string path;
};

// 系统选择文件夹
class SailUISystemSelectDirectoryEvent final : public SailUIEvent
{
public: static size_t Value() { return static_cast<size_t>(SailUIEventType::SYSTEM_SELECT_DIRECTORY); }

    std::string path;
};

// 系统保存文件，而选择的文件完毕
class SailUISystemSaveFileEvent final : public SailUIEvent
{
public: static size_t Value() { return static_cast<size_t>(SailUIEventType::SYSTEM_SAVE_FILE); }

    std::string path;
};

#endif
