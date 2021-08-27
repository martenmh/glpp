#ifndef GLPP_ANIMATION_H
#define GLPP_ANIMATION_H


class Animation {
  protected:
    using AnimationCallback = void (*)(Shape&, double dx);
    Shape* shapeRef;
    AnimationCallback             callback;

  public:
    /* Duration in seconds */
    float duration = INFINITY;

    Animation(float duration, Shape* shape, AnimationCallback callback):
        shapeRef(shape) {
        this->duration = duration;
        this->callback = callback;
    }

    void operator()(double dx) {
        std::cout << shapeRef->transformation() << std::endl;
        callback(*shapeRef, dx);
    }

    bool operator==(const Animation& rhs) {
        return this->callback == rhs.callback && this->duration == rhs.duration;
    }
};

Animation rotateAnimation(Shape* shape, float duration = INFINITY) {
    return Animation(duration, shape, [](Shape& shape, double dx) {
      shape.rotate({0.0f, 0.0f, static_cast<float>(dx)});
    });
}

#endif //GLPP_ANIMATION_H
