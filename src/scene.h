#ifndef GLPP_SCENE_H
#define GLPP_SCENE_H

#include "geometry.h"

/**
 * 2-Dimensional Scene
 */
class Scene {
    std::vector<std::unique_ptr<Shape1D>> shapes_1d;
    std::vector<std::unique_ptr<Shape3D>> shapes_3d;
    std::vector<Animation>              animations;

  protected:
    std::vector<std::shared_ptr<Shape>> shapes_2d;

  public:
    glpp::Window   window;
    glpp::Shader   shader;
    glpp::Observer2DCamera camera;
    glpp::Renderer renderer;
    CoordinateSystemBase coordinateSystem;

    Scene():
        window(1920, 1080, "Scene"), shader("/home/marten/projects/glpp/resources/scene_shaders"), camera(window, 10.0f), renderer(),
        coordinateSystem{
            .xMin = -10,
            .xMax = 10,
            .yMin = window.aspect() * -10,
            .yMax = window.aspect() * 10
        } {
        std::cout << window.aspect() << std::endl;
    }

//    /**
//     * Adds a shape to the scene
//     * The mesh of a shape is lazily evaluated,
//     * such that it does not have to be regenerated for every change before adding it to the scene.
//     */
//    template<Derived<Shape1D> T> requires Creatable<T>
//    void add(T& shape) {
//        shape.create();
//        shapes_1d.push_back(std::make_unique<T>(std::move(shape)));
//    }

    template<typename T> requires Creatable<T>
    void add(T& shape) {
        shape.create();
        shapes_2d.push_back(std::make_shared<T>(std::move(shape)));
    }

    template<typename T> requires Creatable<T>
    void add(T&& shape) {
        shape.create();
        shapes_2d.push_back(std::make_shared<T>(shape));
    }


    template<Derived<Shape3D> T> requires Creatable<T>
    void add(T& shape) {
        shape.create();
        shapes_3d.push_back(std::make_unique<T>(shape));
    }

    template<Derived<Shape> T, Derived<Shape>... Var>
    void add(T& shape, Var... args) {
        add<T>(shape);
        add<Var...>(args...);
    }

    /**
     * Remove a shape from the scene
     */
    template<Derived<Shape1D> T>
    void remove(T& shape){
        shapes_1d.erase(std::find(shapes_1d.begin(), shapes_1d.end(), shape));
    }
    template<Derived<Shape2D> T>
    void remove(T& shape){
        shapes_2d.erase(std::find(shapes_2d.begin(), shapes_2d.end(), shape));
    }
    template<Derived<Shape3D> T>
    void remove(T& shape){
        shapes_3d.erase(std::find(shapes_3d.begin(), shapes_3d.end(), shape));
    }

    template<Derived<Shape> T, Derived<Shape>... Var>
    void remove(T& shape, Var... args){
        remove(shape);
        remove(args...);
    }
    double startTime = 0.0, dx = 0.0;
    void render() {
        //		auto proj = camera.projection();
        //		auto view = camera.view();
        shader.use();
//			if(dx < animation.duration)
//				animations.erase(std::find(animations.begin(), animations.end(), animation));


        for(auto& shape : shapes_1d){
//            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//            glLineWidth(shape->borderThickness);
            renderer.drawLine(shape->mesh());

//            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }

        for(auto& shape : shapes_2d){

            shader.setUniform("mvp", ml::ortho(static_cast<float>(coordinateSystem.xMin), static_cast<float>(coordinateSystem.xMax), static_cast<float>(coordinateSystem.yMin), static_cast<float>(coordinateSystem.yMax), 0.1f, 1.0f) * shape->transformation());
            renderer.draw(shape->mesh());
//			if(shape->borderThickness > 0.0f){
//                glLineWidth(shape->borderThickness);
            renderer.draw(shape->mesh());
//			}
        }

        for(auto& shape : shapes_3d){
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
        startTime = startTime == 0 ? glfwGetTime() : startTime;
        dx = glfwGetTime() - startTime;
        // update animations
        for(auto& animation: animations)
            animation(dx);

        camera.update();
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



#endif //GLPP_SCENE_H
