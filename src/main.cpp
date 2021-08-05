#include "glm/glm.hpp"
#include "graphics/camera.h"
#include "graphics/font.h"
#include "graphics/mesh.h"
#include "graphics/shaders.h"
#include "graphics/shapes.h"
#include "gui/window.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <iostream>
#include <list>
#include <random>

using namespace glpp;

Mesh plane(const ml::vec3& pos, const ml::vec2& size, rgba color, float textureID = 0.0f) {
	return Mesh({{{pos.x(), pos.y(), pos.z()}, color, {}, {0.0f, 0.0f}, textureID},
				 {{pos.x() + size.x(), pos.y(), pos.z()}, color, {}, {1.0f, 0.0f}, textureID},
				 {{pos.x(), pos.y() + size.y(), pos.z()}, color, {}, {0.0f, 1.0f}, textureID},
				 {{pos.x() + size.x(), pos.y() + size.y(), pos.z()}, color, {}, {1.0f, 1.0f}, textureID}},
				{0, 1, 2, 2, 1, 3});
}

struct Transformable {
	ml::mat4 transformation;

	void move(const ml::vec3& xyz) {
		ml::translate(transformation, xyz);
	}

	void scale(const ml::vec3& xyz) {
		transformation *= ml::scale(xyz);
	}
	void scale(float scale) {
		transformation *= ml::scale(scale);
	}
	void rotate(const ml::vec3& xyz) {
		transformation *= ml::rotate(1.0f, xyz);
	}
};

struct Shape: public ::Transformable {
	Mesh m_mesh;

  public:
	/**
     * Creates the mesh of a shape.
     * The create function has to be implemented by derivatives.
     * create() is called when a shape is added to the scene, to lazily evaluate any changes.
     * @see Scene::add()
     */
	virtual void create() = 0;

	/**
     * Create the Shape Mesh
     * Should be called by a derived Shape during the create() call
     * @param mesh
     * @see create
     */
	void create(const Mesh& mesh) {
		m_mesh = mesh;
	}

	[[nodiscard]] Mesh& mesh() {
		return m_mesh;
	}
	bool operator==(const Shape& rhs) {
		return m_mesh == rhs.m_mesh && transformation == rhs.transformation;
	}
};

class Animation {
  protected:
	using AnimationCallback = void (*)(ml::mat4& transformation, double dx);
	std::reference_wrapper<Shape> shapeRef;
	AnimationCallback             callback;

  public:
	/* Duration in seconds */
	float duration = INFINITY;

	Animation(float duration, Shape& shape, AnimationCallback callback):
		shapeRef(shape) {
		this->duration = duration;
		this->callback = callback;
	}

	void operator()(double dx) {
		callback(shapeRef.get().transformation, dx);
	}
	bool operator==(const Animation& rhs) {
		return this->callback == rhs.callback && this->duration == rhs.duration;
	}
};

Animation rotateAnimation(Shape& shape, float duration = INFINITY) {
	return Animation(duration, shape, [](ml::mat4& transformation, double dx) {
		transformation *= ml::rotate(static_cast<float>(dx), {1.0f, 0.0f, 0.0f});
	});
}

struct Triangle: public Shape {
	ml::vec3 A = ml::vec3(-1.0f, -1.0f, 0.0f),
			 B = ml::vec3(0.0f, 1.0f, 0.0f),
			 C = ml::vec3(1.0f, -1.0f, 0.0f);

    void create() override {
        Shape::create(Mesh(createTriangle(A, B, C, rgba{0.0f, 0.0f, 1.0f, 1.0f}, 0.0f)));
    }
};

struct Rectangle: public Shape {
    ml::vec3 position = ml::vec3(0.0f);
    float height = 0.4, width = 0.5;

    void create() override {
        Shape::create(createPlane2(position, {width, height}, rgba{0.0f, 1.0f, 0.0f, 1.0f}, 0));
    }
};

struct Square: public Shape {
	ml::vec3 position = ml::vec3(0.0f);
	float size = 1.0f;

    void create() override {
		Shape::create(createPlane2(position, {size, size}, rgba{1.0f, 0.0f, 0.0f, 1.0f}, 0));
	}
};

struct Circle: public Shape {
	ml::vec3 center = ml::vec3(0.0f);
	float    radius = 1.0f;

	explicit Circle(const ml::vec3& center = ml::vec3(0.0f), float radius = 1.0f): center(center), radius(radius) {}

	void create() override {
		Shape::create(Mesh(createCircle(center, radius, static_cast<int>(50 * radius))));
	}
};

struct Line: protected Shape {
	ml::vec3 start, end;
	void     create() override {
	}
	//    Line(const ml::vec3& start, const ml::vec3& end): start(start), end(end){
	//
	//	}
};

//class LengthLine : public Line {
//
//};
//
//class Brace : public Line {
//
//};
//
//class Arrow : public Line {
//
//};

class Angle: Shape {
};


template <class T, class U>
concept Derived = std::is_base_of<U, T>::value;

/**
 * 2-Dimensional Scene
 */
class Scene {
	static const constexpr auto vertexSource   = R"(
        #version 410 core
        layout(location = 0) in vec3 a_Position;
        layout(location = 1) in vec4 a_Color;
        out vec4 v_Color;

        uniform mat4 mvp;

        void main(){
            gl_Position = mvp * vec4(a_Position, 1.0);
            v_Color = a_Color;
        }
    )";
	static const constexpr auto fragmentSource = R"(
        #version 410 core
        out vec4 o_Color;
        in vec4 v_Color;

        void main(){
            o_Color = v_Color;
        }
    )";

	std::vector<std::unique_ptr<Shape>> shapes;
	std::vector<Animation>              animations;

  public:
	Window   window;
	Shader   shader;
	Camera   camera;
	Renderer renderer;

	Scene():
		window(1920, 1080, "Scene"), shader("/home/marten/projects/glpp/resources/default/shaders/mvp/"), camera(100, 100), renderer() {
	}

	/**
	 * Adds a shape to the scene
	 *
	 * The mesh of a shape is lazily evaluated,
	 * such that it does not have to be regenerated for every change before adding it to the scene.
	 */
	template<Derived<Shape> T>
	void add(T& shape) {
		shape.create();
		shapes.push_back(std::make_unique<T>(shape));
	}

    template<Derived<Shape> T, Derived<Shape>... Var>
    void add(T& shape, Var... args) {
        add(shape);
		add(args...);
    }

	/**
	 * Remove a shape from the scene
	 */
    template<Derived<Shape> T>
    void remove(T& shape){
        shapes.erase(std::find(shapes.begin(), shapes.end(), shape));
	}

    template<Derived<Shape> T, Derived<Shape>... Var>
    void remove(T& shape, Var... args){
        shapes.erase(std::find(shapes.begin(), shapes.end(), shape));
		remove(args...);
    }

	void render() {
		//		auto proj = camera.projection();
		//		auto view = camera.view();

		shader.use();
		for(auto& shape: shapes) {
			shader.setUniform("mvp", ml::mat4(1.0f));
			renderer.draw(shape->mesh());
		}
		//        Renderer::draw()
	}

	void play(Animation animation) {
		animations.push_back(animation);
		//        float dx = 0.0f;
		//        do {
		//            anim(dx);
		//        } while(anim.duration < dx);
	}

    bool update() {
    }

	/**
     * Wait on the current animations existing in the buffer
     * Clears the animation buffer of all finished animations
     */
	void wait() {
		double longestDuration = std::max_element(animations.begin(), animations.end(), [](auto& lhs, auto& rhs) {
									 return lhs.duration < rhs.duration;
								 })->duration;

		if(longestDuration == INFINITY)
			logging::warning("Waiting on an animation that has an infinite duration.");

		double dx        = 0.0;
		double startTime = glfwGetTime();
		// render loop
		do {
			dx = glfwGetTime() - startTime;
			for(auto& animation: animations) {
				animation(dx);
				if(dx < animation.duration)
					animations.erase(std::find(animations.begin(), animations.end(), animation));
				//					animations.erase(animation);
			}

			this->render();
		} while(dx < longestDuration);
		// remove all finished animations
		animations.clear();
	}

	virtual void setup() = 0;
};

class ai {
	float d;
};

struct sss: public ai {
	int x;
};

struct MyScene: Scene {
	void setup() override {
        auto circle = ::Circle();
//		add(circle);
		auto square = ::Square();
		auto triangle = ::Triangle();
		auto rectangle = ::Rectangle();
//		add(square);
		add(circle, rectangle, triangle);
        add(triangle);
//        add(circle);
//		play(rotateAnimation(circle));
//		wait();
	}
};

int main() {
	MyScene scene;
	scene.setup();
	while(scene.window.isOpen()) {
		scene.window.pollEvents();

		if(scene.window.keyIsPressed(Key::Q) || scene.window.keyIsPressed(Key::ESCAPE)) {
			scene.window.close();
		}

		scene.window.clear({0.05f, 0.05f, 0.05f, 1.0f});
		scene.update();
		scene.render();
		scene.window.display();
		//		break;
	}
}
