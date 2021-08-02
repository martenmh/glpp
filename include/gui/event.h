#ifndef EVENT_H
#define EVENT_H

#include "common.h"
#include "utils/include/sig.h"

#include <stdexcept>
#include <string>

namespace glpp {
	enum class CursorMode : int {
		Normal   = GLFW_CURSOR_NORMAL,
		Hidden   = GLFW_CURSOR_HIDDEN,
		Disabled = GLFW_CURSOR_DISABLED
	};

	// TODO add cursor mode support
	/**
GLFW_CURSOR,
    * GLFW_CURSOR_NORMAL,
    * GLFW_CURSOR_HIDDEN,   // hides cursor when over window
    * GLFW_CURSOR_DISABLED  // grabs the cursor for use in program
GLFW_STICKY_KEYS,
    * GLFW_TRUE / GLFW_FALSE
GLFW_STICKY_MOUSE_BUTTONS,
    * GLFW_TRUE / GLFW_FALSE
GLFW_LOCK_KEY_MODS,         // same as glfw_sticky keys but with GLFW_MOD_CAPS_LOCK && GLFW_MOD_NUM_LOCK
    * GLFW_TRUE / GLFW_FALSE
GLFW_RAW_MOUSE_MOTION
    * GLFW_TRUE / GLFW_FALSE
**/

	//class Window {
	//    size_t      height, width;
	//    size_t      topx, topy;
	//    std::string label;
	//    GLFWwindow* glfwWindow;
	//};

	enum class DrawPosition {
		Absolute,
		Relative
	};

	class Window;

	struct Event {
		GLFWwindow* parent;
	};

	// Bitfield access for Modifier keys
	enum class ModKey : int {
		Shift    = 0b000001,
		Ctrl     = 0b000010,
		Alt      = 0b000100,
		Super    = 0b001000,
		CapsLock = 0b010000,
		NumLock  = 0b100000,
	};
	enum class Key : int {
		/* The unknown key */
		Unknown = GLFW_KEY_UNKNOWN,
		/* Printable keys */
		Space             = GLFW_KEY_SPACE,
		Apostrophe        = GLFW_KEY_APOSTROPHE,
		Comma             = GLFW_KEY_COMMA,
		Minus             = GLFW_KEY_MINUS,
		Period            = GLFW_KEY_PERIOD,
		Slash             = GLFW_KEY_SLASH,
		Key0              = GLFW_KEY_0,
		Key1              = GLFW_KEY_1,
		Key2              = GLFW_KEY_2,
		Key3              = GLFW_KEY_3,
		Key4              = GLFW_KEY_4,
		Key5              = GLFW_KEY_5,
		Key6              = GLFW_KEY_6,
		Key7              = GLFW_KEY_7,
		Key8              = GLFW_KEY_8,
		Key9              = GLFW_KEY_9,
		Semicolon         = GLFW_KEY_SEMICOLON,
		Equal             = GLFW_KEY_EQUAL,
		A                 = GLFW_KEY_A,
		B                 = GLFW_KEY_B,
		C                 = GLFW_KEY_C,
		D                 = GLFW_KEY_D,
		E                 = GLFW_KEY_E,
		F                 = GLFW_KEY_F,
		G                 = GLFW_KEY_G,
		H                 = GLFW_KEY_H,
		I                 = GLFW_KEY_I,
		J                 = GLFW_KEY_J,
		K                 = GLFW_KEY_K,
		L                 = GLFW_KEY_L,
		M                 = GLFW_KEY_M,
		N                 = GLFW_KEY_N,
		O                 = GLFW_KEY_O,
		P                 = GLFW_KEY_P,
		Q                 = GLFW_KEY_Q,
		R                 = GLFW_KEY_R,
		S                 = GLFW_KEY_S,
		T                 = GLFW_KEY_T,
		U                 = GLFW_KEY_U,
		V                 = GLFW_KEY_V,
		W                 = GLFW_KEY_W,
		X                 = GLFW_KEY_X,
		Y                 = GLFW_KEY_Y,
		Z                 = GLFW_KEY_Z,
		LeftBracket       = GLFW_KEY_LEFT_BRACKET,
		KEY_BACKSLASH     = GLFW_KEY_BACKSLASH,
		KEY_RIGHT_BRACKET = GLFW_KEY_RIGHT_BRACKET,
		KEY_GRAVE_ACCENT  = GLFW_KEY_GRAVE_ACCENT,
		KEY_WORLD_1       = GLFW_KEY_WORLD_1,
		KEY_WORLD_2       = GLFW_KEY_WORLD_2,
		/* Function keys */
		ESCAPE       = GLFW_KEY_ESCAPE,
		ENTER        = GLFW_KEY_ENTER,
		TAB          = GLFW_KEY_TAB,
		BACKSPACE    = GLFW_KEY_BACKSPACE,
		INSERT       = GLFW_KEY_INSERT,
		DELETE       = GLFW_KEY_DELETE,
		RIGHT        = GLFW_KEY_RIGHT,
		LEFT         = GLFW_KEY_LEFT,
		DOWN         = GLFW_KEY_DOWN,
		UP           = GLFW_KEY_UP,
		PAGE_UP      = GLFW_KEY_PAGE_UP,
		PAGE_DOWN    = GLFW_KEY_PAGE_DOWN,
		HOME         = GLFW_KEY_HOME,
		END          = GLFW_KEY_END,
		CAPS_LOCK    = GLFW_KEY_CAPS_LOCK,
		SCROLL_LOCK  = GLFW_KEY_SCROLL_LOCK,
		NUM_LOCK     = GLFW_KEY_NUM_LOCK,
		PRINT_SCREEN = GLFW_KEY_PRINT_SCREEN,
		PAUSE        = GLFW_KEY_PAUSE,
		F1           = GLFW_KEY_F1,
		F2           = GLFW_KEY_F2,
		F3           = GLFW_KEY_F3,
		F4           = GLFW_KEY_F4,
		F5           = GLFW_KEY_F5,
		F6           = GLFW_KEY_F6,
		F7           = GLFW_KEY_F7,
		F8           = GLFW_KEY_F8,
		F9           = GLFW_KEY_F9,
		F10          = GLFW_KEY_F10,
		F11          = GLFW_KEY_F11,
		F12          = GLFW_KEY_F12,
		F13          = GLFW_KEY_F13,
		F14          = GLFW_KEY_F14,
		F15          = GLFW_KEY_F15,
		F16          = GLFW_KEY_F16,
		F17          = GLFW_KEY_F17,
		F18          = GLFW_KEY_F18,
		F19          = GLFW_KEY_F19,
		F20          = GLFW_KEY_F20,
		F21          = GLFW_KEY_F21,
		F22          = GLFW_KEY_F22,
		F23          = GLFW_KEY_F23,
		F24          = GLFW_KEY_F24,
		F25          = GLFW_KEY_F25,
		KP_0         = GLFW_KEY_KP_0,
		KP_1         = GLFW_KEY_KP_1,
		KP_2         = GLFW_KEY_KP_2,
		KP_3         = GLFW_KEY_KP_3,
		KP_4         = GLFW_KEY_KP_4,
		KP_5         = GLFW_KEY_KP_5,
		KP_6         = GLFW_KEY_KP_6,
		KP_7         = GLFW_KEY_KP_7,
		KP_8         = GLFW_KEY_KP_8,
		KP_9         = GLFW_KEY_KP_9,
		KP_DECIMAL   = GLFW_KEY_KP_DECIMAL,
		KP_DIVIDE    = GLFW_KEY_KP_DIVIDE,
		KP_MULTIPLY  = GLFW_KEY_KP_MULTIPLY,
		KP_SUBTRACT  = GLFW_KEY_KP_SUBTRACT,
		KP_ADD       = GLFW_KEY_KP_ADD,
		KP_ENTER     = GLFW_KEY_KP_ENTER,
		KP_EQUAL     = GLFW_KEY_KP_EQUAL,
		LeftShift    = GLFW_KEY_LEFT_SHIFT,
		LeftCtrl     = GLFW_KEY_LEFT_CONTROL,
		LEFT_ALT     = GLFW_KEY_LEFT_ALT,
		LEFT_SUPER   = GLFW_KEY_LEFT_SUPER,
		RIGHT_SHIFT  = GLFW_KEY_RIGHT_SHIFT,
		RightCtrl    = GLFW_KEY_RIGHT_CONTROL,
		RIGHT_ALT    = GLFW_KEY_RIGHT_ALT,
		RIGHT_SUPER  = GLFW_KEY_RIGHT_SUPER,
		MENU         = GLFW_KEY_MENU,
		LAST         = GLFW_KEY_LAST
	};
	enum class MouseButton {
		Button1 = GLFW_MOUSE_BUTTON_1,
		Button2 = GLFW_MOUSE_BUTTON_2,
		Button3 = GLFW_MOUSE_BUTTON_3,
		Button4 = GLFW_MOUSE_BUTTON_4,
		Button5 = GLFW_MOUSE_BUTTON_5,
		Button6 = GLFW_MOUSE_BUTTON_6,
		Button7 = GLFW_MOUSE_BUTTON_7,
		Button8 = GLFW_MOUSE_BUTTON_8,
		Last    = GLFW_MOUSE_BUTTON_LAST,
		Left    = GLFW_MOUSE_BUTTON_LEFT,
		Right   = GLFW_MOUSE_BUTTON_RIGHT,
		Middle  = GLFW_MOUSE_BUTTON_MIDDLE
	};

	enum class KeyAction : int {
		Pressed = GLFW_RELEASE,
		Release = GLFW_PRESS,
		Repeat  = GLFW_REPEAT
	};

	struct KeyEvent: public Event {
		Key       key;
		int       scancode;
		KeyAction action;
		int       modkeys;
	};

	struct CursorEvent: public Event {
		bool state;
	};

	struct ScrollEvent: public Event {
		double yoffset;
		double xoffset;
	};

	struct MouseClickEvent: public Event {
		bool        state;
		MouseButton button;
		int         mods;
	};

	struct MouseMoveEvent: public Event {
		double xpos;
		double ypos;
	};
	struct ResizeEvent: public Event {
		int height;
		int width;
	};

	struct EventSystem {
		static bool                   keyPressed(Key key);
		static inline KeyEvent        keyEvent;
		static inline CursorEvent     cursorEvent;
		static inline ScrollEvent     scrollEvent;
		static inline MouseClickEvent mouseClickEvent;
		static inline MouseMoveEvent  mouseMoveEvent;
		/** Signals to connect to **/
		// TODO: dispatch events as references
		inline static signal<void(KeyEvent)>        onKeyPress;
		inline static signal<void(CursorEvent)>     onCursorEnter;
		inline static signal<void(ScrollEvent)>     onMouseScroll;
		inline static signal<void(MouseClickEvent)> onMouseClick;
		inline static signal<void(MouseMoveEvent)>  onMouseMove;
		inline static signal<void(ResizeEvent)>     onResize;
	};

	class Window;

	bool EventSystem::keyPressed(Key key) {
		//    if(Window::m_windows.empty()){
		//        throw std::runtime_error("Expected at least 1 created window.");
		//    }
		//    return glfwGetKey(Window::m_windows[0], static_cast<int>(key));
	}
}


#endif //EVENT_H
