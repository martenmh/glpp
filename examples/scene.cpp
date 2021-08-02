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
	void operator()(float dx){

	}
};

class MathRenderer : protected Renderer {
	void draw(){

	}
    void draw(ml::vec2 vec){

    }

	void play(Animation anim){
		float dx = 0.0f;
		do {
            anim(dx);
		} while(anim.duration < dx)

	}
};

class Shapes {

};

class Triangle : protected Shapes {

};
class Square : protected Shapes {

};

struct Circle : protected Shapes {
	ml::vec3 position;
	float radius;
    ml::vec3 getCenter(){

	}
	ml::vec3 getRadius(){

	}
};

class Line : protected Shapes {
    ml::vec3 start, end;

};



int main() {
    // Use namespace glpp in main
    using namespace glpp;
    constexpr const int height = 1080;
    constexpr const int width  = 1920;

    ml::mat4 projection(1), view(1), model(1);

    RenderWindow<MathRenderer> window(width, height, "Engine", 5.0f, 5.0f, arr);
    Shader                        shader("/home/marten/projects/glpp/resources/default/shaders/mvp/");
    Camera                        camera(100, 100);

    Mesh mesh3 = plane({-0.5f, -0.5f, -0.5f}, {100.0f, 100.0f}, {1.0f, 1.0f, 1.0f});

    camera.back(10);
    int i = 0;
    bool show_demo_window = true;

    while(window.isOpen()) {
        window.pollEvents();
        window.clear({0.05f, 0.05f, 0.05f, 1.0f});

        if(window.keyIsPressed(Key::Q) || window.keyIsPressed(Key::ESCAPE))
            window.close();

        shader.setUniform("mvp", projection * view * model);
        shader.use();

        model = ml::scale(0.4f);


        window.display();
    }
    return 0;
}
