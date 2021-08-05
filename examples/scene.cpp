#include "glm/glm.hpp"
#include "graphics/camera.h"
#include "graphics/mesh.h"
#include "graphics/shaders.h"
#include "graphics/shapes.h"
#include "graphics/font.h"

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

class Animation{
  public:
    using AnimationCallback = ml::mat4 (*)(ml::mat4& transformation, float dx);

	/* Duration in seconds */
	float duration = INFINITY;
	Shape& shapeRef;
    AnimationCallback callback;

	void operator()(float dx){
		callback(shapeRef.transformation, callback);
	}
};

Animation rotateAnimation(Shape& shape, float duration = INFINITY){
    return Animation(duration, shape, [](ml::mat4& transformation, float dx){
        ml::rotate(dx, {1.0f, 0.0f, 0.0f});
	});
}

class Shape {
	ml::mat4 transformation;
};

class Triangle : protected Shape {

};

class Square : protected Shape {

};

struct Circle : protected Shape {
	ml::vec3 center;
	float radius;
};

class Line : protected Shape {
    ml::vec3 start, end;

};

class LengthLine : public Line {

};

class Brace : public Line {

};

class Arrow : public Line {

};

class Angle : Shape {

};



/**
 * 2-Dimensional Scene
 */
class Scene : protected Renderer {
    std::vector<Shape> shapes;

    void render() {
		
        Renderer::draw()
    }
    void play(){
        float dx = 0.0f;
        do {
            anim(dx);
        } while(anim.duration < dx);

    }

	/**
	 * Wait on an animation
	 */
	void wait(){
        Animation currentAnimation;
		if(currentAnimation.duration == INFINITY){

		}
	}
    virtual void setup() {

    }
};

class MyScene : public Scene {

    void setup() override {
        auto myCircle = Circle{.center = {0.0f, 0.0f}, .radius = 1.0f};
		this->shapes.push_back(myCircle);

		auto myCircleAnimation = rotateAnimation(myCircle, 5.0f);
		this->play(myCircleAnimation);
	}
};


int main() {
    // Use namespace glpp in main
    using namespace glpp;
    constexpr const int height = 1080;
    constexpr const int width  = 1920;

    ml::mat4 projection(1), view(1), model(1);

    RenderWindow<MyScene> window(width, height, "Engine");
    Shader                        shader("/home/marten/projects/glpp/resources/default/shaders/mvp/");
    Camera                        camera(100, 100);

    Mesh mesh3 = plane({-0.5f, -0.5f, -0.5f}, {100.0f, 100.0f}, {1.0f, 1.0f, 1.0f});

    camera.back(10);
    int i = 0;
    bool show_demo_window = true;

    window.setup();
    while(window.isOpen()) {
        window.pollEvents();
        window.clear({0.05f, 0.05f, 0.05f, 1.0f});

        if(window.keyIsPressed(Key::Q) || window.keyIsPressed(Key::ESCAPE))
            window.close();

        shader.setUniform("mvp", projection * view * model);
        shader.use();

        model = ml::scale(0.4f);

        window.update();
        window.render();
    }
    return 0;
}
