#ifndef CAMERA_H
#define CAMERA_H

#include "gui/window.h"
#include "ml/transform.h"

namespace glpp {

	class Camera /*: public Transformable */ {
	  protected:
		ml::vec3 m_pos   = ml::vec3(0.0f, 0.0f, 0.0f);
		ml::vec3 m_up    = ml::vec3(0.0f, 1.0f, 0.0f);
		ml::vec3 m_front = ml::vec3(0.0f, 0.0f, 1.0f);
		ml::mat4 m_projection;

		enum CameraMode {
			Perspective,
			Orthographic
		} m_cameraMode;

	  public:
		float fov    = 45.0f;
		float width  = 0;
		float height = 0;
		float near   = 0.1f;
		float far    = 100.0f;

		/** Initialize a camera with an Orthographic  projection **/
		explicit inline Camera(int width, int height, float near = 0.1f, float far = 100.0f, CameraMode mode = CameraMode::Orthographic):
			width(width), height(height), near(near), far(far), m_cameraMode(mode) {
		}

		/** Initialize a camera with a Perspective projection using the m_width and m_height(this makes it switching **/
		explicit inline Camera(float fov, int width, int height, float near = 0.1f, float far = 100.0f, CameraMode mode = CameraMode::Perspective):
			fov(fov), width(width), height(height), near(near), far(far), m_cameraMode(mode) {
		}

		inline float aspect() {
			return width / height;
		}

		template<typename T>
		inline ml::mat4T<T> lookAt(const ml::vec3T<T>& pos, const ml::vec3T<T>& target, const ml::vec3T<T>& up) {
			return ml::lookAt(pos, target, up);
		}

		inline ml::mat4 projection() {
			if(m_cameraMode == CameraMode::Orthographic) {
				return ml::ortho(-(width / 2) / fov, width / 2 / fov, -(height / 2) / fov, height / 2 / fov, near, far);
			} else {
				return ml::perspective(fov, aspect(), near, far);
			}
		}

		virtual inline ml::mat4 view() {
			return ml::lookAt(m_pos, ml::vec3(m_pos + m_front), m_up);
		}

		inline ml::vec3&      position() { return m_pos; }
		inline void           setMode(CameraMode mode) { m_cameraMode = mode; }
		inline void           toggleMode() { m_cameraMode = m_cameraMode == CameraMode::Orthographic ? CameraMode::Perspective : CameraMode::Orthographic; }
		inline constexpr bool isOrtho() const { return m_cameraMode == Orthographic; }
		inline constexpr bool isPerspective() const { return m_cameraMode == Perspective; }

		inline void up(float amount = 1.0f) { m_pos += (m_up * amount); }
		inline void down(float amount = 1.0f) { m_pos -= (m_up * amount); }
		inline void front(float amount = 1.0f) { m_pos += (m_front * amount); }
		inline void left(float amount = 1.0f) { m_pos -= (ml::normalize(ml::cross(m_front, m_up)) * amount); }
		inline void right(float amount = 1.0f) { m_pos += (ml::normalize(ml::cross(m_front, m_up)) * amount); }
		inline void back(float amount = 1.0f) { m_pos -= (m_front * amount); }
	};

	class ObserverCamera: public Camera {
		bool    moving    = false;
		bool    translate = false;
		float   lastY;
		float   lastX;
		Window& m_window;
		float   sensitivity = 0.01f;
		float   yaw         = 0.0f;
		float   pitch       = 0.0f;
		bool    firstMouse  = true;
		float   idk         = 0.0f;

	  public:
		Key modeSwitchKey = Key::F5;

		ObserverCamera(Window& window):
			Camera(45.0f, 800, 600), m_window(window) {
			EventSystem::onKeyPress.connect<&ObserverCamera::onKeyPress>(this);
			EventSystem::onMouseScroll.connect<&ObserverCamera::onScroll>(this);
			EventSystem::onMouseMove.connect<&ObserverCamera::onMouseMove>(this);
			EventSystem::onMouseClick.connect<&ObserverCamera::onMouseClick>(this);
		}

		virtual void onKeyPress(KeyEvent& event) {
			if(event.key == modeSwitchKey && event.action == KeyAction::Pressed) {
				//			toggleMode();
			}
			//if(event.key )
		}
		virtual void onScroll(ScrollEvent& event) {
			m_pos.z() += (float)event.yoffset;
			if(m_pos.z() > far) {
				m_pos.z() = far;
			}
			if(m_pos.z() < near) {
				m_pos.z() = near;
			}
		}

		inline ml::mat4 view() {
			return ml::lookAt(m_pos, ml::vec3(0.0f, 0.0f, 0.0f), m_up);
		}

		virtual void onMouseMove(MouseMoveEvent& event) {
			if(moving) {
				if(firstMouse) {
					lastX      = event.xpos;
					lastY      = event.ypos;
					firstMouse = false;
				}

				float xoffset = event.xpos - lastX;
				float yoffset = event.ypos - lastY;
				lastX         = event.xpos;
				lastY         = event.ypos;
				yaw -= xoffset * sensitivity;
				pitch += yoffset * sensitivity;
				idk += (xoffset + yoffset) * sensitivity;
				m_pos.x() = sin(yaw) * 10.0f;
				m_pos.y() = sin(pitch) * 10.0f;
				m_pos.z() = (cos(idk)) * 10.0f;
				//m_up.() = m_pos.z();
				//m_up = m_pos;
				//			m_up.y() = m_pos.z();
				//            m_up.z() = cos(pitch);

				//view = ml::lookAt(ml::vec3{0.0f, camX, camZ}, ml::vec3{0.0f,0.0f,0.0f}, ml::vec3{0.0f,camZ,0.0f}); // successful yaw
				//view = ml::lookAt(ml::vec3{camX, 0.0f, camZ}, ml::vec3{0.0f,0.0f,0.0f}, ml::vec3{0.0f,1.0f,0.0f}); // successful pitch
				//view = ml::lookAt(ml::vec3{camX, camX, camZ}, ml::vec3{0.0f,0.0f,0.0f}, ml::vec3{camZ,camZ,0.0f}); // ?
			}
		}
		virtual void onMouseClick(MouseClickEvent& event) {
			moving = ((event.state && event.button == MouseButton::Middle) || (event.button != MouseButton::Middle && moving));
		}
		void update() {
		}
	};

	class FPSCamera: public Camera {
		Window&      m_window;
		virtual void onKeyPress(KeyEvent& event) {
		}

	  public:
		FPSCamera(Window& window):
			Camera(45.0f, 800, 600), m_window(window) {
			//m_window
		}
	};

	struct FlyCamera: public Camera {
		Window& m_window;

		float speedModifier = 1.0f;
		float lastX;
		float lastY;
		float yaw        = 0.0f;
		float pitch      = 0.0f;
		bool  firstMouse = true;

	  public:
		float sensitivity = 0.1f;

		inline explicit FlyCamera(Window& window):
			Camera(45.0f, window.width(), window.height()), m_window(window),
			lastX(m_window.width() / 2), lastY(m_window.height() / 2) {
//#ifdef NDEBUG // Enable cursor if in debug mode
			m_window.setCursorMode(CursorMode::Disabled);
//#endif
			EventSystem::onMouseMove.connect<&FlyCamera::onMouseMove>(this);
		}

		void onMouseMove(MouseMoveEvent& event) {
			if(firstMouse) {
				lastX      = event.xpos;
				lastY      = event.ypos;
				firstMouse = false;
			}

			float xoffset = event.xpos - lastX;
			float yoffset = lastY - event.ypos;

			lastX = event.xpos;
			lastY = event.ypos;

			xoffset *= sensitivity;
			yoffset *= sensitivity;

			yaw += xoffset;
			pitch += yoffset;

			if(pitch > 89.0f)
				pitch = 89.0f;
			if(pitch < -89.0f)
				pitch = -89.0f;

			ml::vec3 direction;
			direction.x() = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
			direction.y() = sin(glm::radians(pitch));
			direction.z() = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
			m_front       = ml::normalize(direction);
		}

		void update() {
			if(m_window.keyIsPressed(Key::LeftShift)) {
				speedModifier = 0.10f;
			} else {
				speedModifier = 0.05f;
			}

			if(m_window.keyIsPressed(Key::W)) {
				front(speedModifier);
			}
			if(m_window.keyIsPressed(Key::A)) {
				left(speedModifier);
			}
			if(m_window.keyIsPressed(Key::S)) {
				back(speedModifier);
			}
			if(m_window.keyIsPressed(Key::D)) {
				right(speedModifier);
			}
			if(m_window.keyIsPressed(Key::LeftCtrl)) {
				down(speedModifier);
			}
			if(m_window.keyIsPressed(Key::Space)) {
				up(speedModifier);
			}
		}
	};
} // namespace gl

#endif //CAMERA_H
