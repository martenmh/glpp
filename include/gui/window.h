#ifndef LIBRARY_WINDOW_H
#define LIBRARY_WINDOW_H

#include "common.h"
#include "graphics/debug.h"
#include "gui/event.h"
#include "math-lib2/include/ml/graphics/color.h"
#include "ml/vector.h"
#include "utils/include/sig.h"

namespace glpp {
	/**
     * Window class for an OpenGL context
     * Abstracts glfw for context creation, user input and opening windows
     * @see RenderWindow
    */
	class Window {
		friend struct EventSystem;
		double prevX, prevY;
		rgba   backgroundColor;
		// GLFW Input Modes:
		CursorMode cursorMode;
		bool       stickyKeys;
		bool       stickyMouseButtons;
		bool       lockKeyMods;
		bool       rawMouseMotion;

		size_t m_height, m_width;
		//size_t topx, topy;
		std::string                            label;
		DrawPosition                           draw_position;
		inline static std::vector<GLFWwindow*> m_windows;
		GLFWwindow*                            glfwWindow; // Window handle

		/* Callbacks: are triggered by GLFW, the callback functions dispatch their respective signal */
		/**
         *
         * @param window
         * @param key
         * @param scancode
         * @param action
         * @param mods
         */
		static void onKeyPressCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

		/**
         * Callback function that's called on window resize
         * @param window to be called from when resizing
         * @param width the new m_width
         * @param height the new m_height
         */
		inline static void onResizeCallback(GLFWwindow* window, int width, int height);

		/**
         * Callback function that's called on mouse movement
         * Dispatch onMouseMove signal
         * @param window
         * @param xpos of the mouse
         * @param ypos of the mouse
         */
		static void onMouseMoveCallback(GLFWwindow* window, double xpos, double ypos);

		/**
         * Callback function that is called on mouse button activity
         * Triggers onMouseMove signal
         * @param window
         * @param button The button pressed
         * @param state Whether the button was perssed or released
         * @param mods bit field describing the pressed modifier keys
         */
		static void onMouseClickCallback(GLFWwindow* window, int button, int state, int mods);

		/**
         * Callback function that's called on mouse scroll
         * @param window
         * @param xoffset the X offset of the mouse (Vertical scroll)
         * @param yoffset The Y offset of the mouse (Horizontal scroll)
         */
		static void onMouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

		/**
         *
         * @param window
         * @param code
         * @param description
         */
		static void onErrorCallback(int code, const char* description);
		/**
         *
         * @param window
         * @param state
         */
		static void onCursorEnterCallback(GLFWwindow* window, int state);

	  public:
		GLFWwindow* getGLFWwindow(){
			return glfwWindow;
		}
		/**
         * Construct a window
         * @param height of the window
         * @param width of the window
         * @param label of the window
         * @param drawPosition set the drawing positions to either Relative or Absolute compared to the window
         */
		Window(size_t height, size_t width, std::string label, DrawPosition drawPosition = DrawPosition::Relative);
		~Window();

		void setCursorMode(CursorMode cursorMode) {
			glfwSetInputMode(glfwWindow, GLFW_CURSOR, static_cast<int>(cursorMode));
		}
		inline void setAliasing() {}

		/**
         * Display all items that must be displayed
         */
		void display();

		/**
         * @param key the key to check
         * @return if the key has been pressed in the previous window cycle
         */
		inline bool keyIsPressed(int key) { return glfwGetKey(glfwWindow, key) == GLFW_PRESS; }
		inline bool keyIsPressed(Key key) { return glfwGetKey(glfwWindow, static_cast<int>(key)) == GLFW_PRESS; }

		/**
         * Poll window events
         */
		inline void pollEvents() { glfwPollEvents(); }

		/**
         * @return if the window is open
         */
		inline bool isOpen() { return glfwWindowShouldClose(glfwWindow) == 0; }
		inline bool doMagic(rgba backgroundColor) {
			pollEvents();
			clear(backgroundColor);
			display();
			return isOpen();
		}
		/**
         * Set the background color
         */
		void setBackgroundColor(float red, float green, float blue, float alpha = 1.0f);
		void setBackgroundColor(rgb rgb);
		void setBackgroundColor(rgba rgba);

		/**
         * Clear the screen with the set background
         * Background color defaults to white
         */
		void clear();

		/**
         * Clears the window without setting a color,
         * either use setBackgroundColor() before or use clear()
         */
		void clearWindow();

		/**
         * Clear the window with a specific background color
         */
		void clear(float r, float g, float b, float a = 1.0f);
		void clear(rgb rgb);
		void clear(rgba rgba);

		void setAntialiasing(unsigned int val) {
		}

		/**
         * @return the aspect ratio
         */
		float  aspect() { return m_width / m_height; }
		size_t height() { return m_height; }
		size_t width() { return m_width; }

		void close();
	};

	Window::Window(size_t width, size_t height, std::string label, DrawPosition drawPosition):
		m_height(height), m_width(width), label(label), draw_position(drawPosition), backgroundColor(0.0f, 0.0f, 0.0f, 0.0f) {
		if(!glfwInit()) {
			throw std::runtime_error("Failed to initialize GLEW\n");
		}
		//glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_FALSE);
		glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL
#ifdef __APPLE
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
#endif

		glfwWindow = glfwCreateWindow(width, height, label.c_str(), NULL, NULL);
		if(glfwWindow == NULL) {
			throw std::runtime_error("Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible.");
			glfwTerminate();
		}
		glfwMakeContextCurrent(glfwWindow); // Initialize GLEW
        //glfwSwapInterval(1); // Enable vsync

		if(drawPosition == DrawPosition::Relative) {
			glfwSetFramebufferSizeCallback(glfwWindow, onResizeCallback);
		}

		if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
			std::cout << "Failed to initialize GLAD" << std::endl;
		}

		// Set callbacks
		glfwSetErrorCallback(onErrorCallback);
		glfwSetScrollCallback(glfwWindow, onMouseScrollCallback);
		glfwSetCursorPosCallback(glfwWindow, onMouseMoveCallback);
		glfwSetMouseButtonCallback(glfwWindow, onMouseClickCallback);
		glfwSetKeyCallback(glfwWindow, onKeyPressCallback);
		glfwSetCursorEnterCallback(glfwWindow, onCursorEnterCallback);

		glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		m_windows.push_back(glfwWindow);
	}

	Window::~Window() {
		glfwDestroyWindow(glfwWindow);
		glfwTerminate();
	}

	void Window::display() {
		glfwSwapBuffers(glfwWindow);
	}

	void Window::setBackgroundColor(float red, float green, float blue, float alpha) {
		backgroundColor = rgba(red, green, blue, alpha);
		glClearColor(red, green, blue, alpha);
	}
	void Window::setBackgroundColor(rgb rgb) {
		backgroundColor = rgba(rgb);
		glClearColor(floatFromColor(rgb.r()), floatFromColor(rgb.g()), floatFromColor(rgb.b()), 1.0f);
	}
	void Window::setBackgroundColor(rgba rgba) {
		backgroundColor = rgba;
		glClearColor(backgroundColor[0], backgroundColor[1], backgroundColor[2], backgroundColor[3]);
	}

	void Window::clearWindow() {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void Window::clear() {
		glClearColor(backgroundColor[0], backgroundColor[1], backgroundColor[2], backgroundColor[3]);
		clearWindow();
	}
	void Window::clear(float r, float g, float b, float a) {
		glClearColor(r, g, b, a);
		clearWindow();
	}
	void Window::clear(rgb rgb) {
		glClearColor(rgb.r(), rgb.g(), rgb.b(), 255);
		clearWindow();
	}
	void Window::clear(rgba rgba) {
		glClearColor(rgba.r(), rgba.g(), rgba.b(), rgba.a());
		clearWindow();
	}

	void Window::close() {
		glfwSetWindowShouldClose(glfwWindow, true);
	}

	void Window::onMouseMoveCallback(GLFWwindow* window, double xpos, double ypos) {
		EventSystem::mouseMoveEvent = MouseMoveEvent{{window}, xpos, ypos};
		EventSystem::onMouseMove(EventSystem::mouseMoveEvent);
	}
	void Window::onKeyPressCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
		//Window::m_keyEvent = KeyEvent{{window}, static_cast<Key>(key), scancode, static_cast<KeyAction>(action), mods};
		EventSystem::keyEvent = KeyEvent{{window}, static_cast<Key>(key), scancode, static_cast<KeyAction>(action), mods};
		EventSystem::onKeyPress(EventSystem::keyEvent);
	}
	void Window::onMouseClickCallback(GLFWwindow* window, int button, int state, int mods) {
		EventSystem::mouseClickEvent = MouseClickEvent{{window}, static_cast<bool>(state), MouseButton(button), mods};
		EventSystem::onMouseClick(EventSystem::mouseClickEvent);
	}
	void Window::onMouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
		EventSystem::scrollEvent = ScrollEvent{{window}, yoffset, xoffset};
		EventSystem::onMouseScroll(EventSystem::scrollEvent);
	}
	void Window::onErrorCallback(int code, const char* description) {
		logging::error(format("An error has occurred in Window({}): {}", code, description));
	}
	void Window::onCursorEnterCallback(GLFWwindow* window, int state) {
		EventSystem::cursorEvent = CursorEvent{{window}, static_cast<bool>(state)};
		EventSystem::onCursorEnter(EventSystem::cursorEvent);
	}
	void Window::onResizeCallback(GLFWwindow* window, int width, int height) {
		glViewport(0, 0, width, height);
		EventSystem::onResize(ResizeEvent{{window}, height, width});
	}
} // namespace glpp

#endif //LIBRARY_WINDOW_H
