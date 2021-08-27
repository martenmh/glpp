#ifndef GLPP_SHAPE_H
#define GLPP_SHAPE_H

#include "geometry.h"

struct Transformable {
    ml::mat4 m_transformation = ml::mat4(1.0f);
  protected:
    bool shouldRegenerateTransformation = false;
    ml::vec3 translation = ml::vec3(0.0f), rotation = ml::vec3(0.0f), dilation = ml::vec3(1.0f);
  public:

    ml::mat4 transformation(){
        if(shouldRegenerateTransformation){
            m_transformation = ml::translate(translation);
            m_transformation *= ml::rotate(rotation);
            m_transformation *= ml::scale(dilation);
        }
        shouldRegenerateTransformation = false;
        return m_transformation;
    }

    /* Main functions */
    void translate(const ml::vec3& xyz){
        translation = xyz;
        shouldRegenerateTransformation = true;
    }
    void rotate(const ml::vec3& xyz){
        rotation = xyz;
        shouldRegenerateTransformation = true;
    }
    void dilate(const ml::vec3& xyz){
        dilation = xyz;
        shouldRegenerateTransformation = true;
    }

    /* Alternative versions, provide an easier API */
    void move(const ml::vec3& xyz) {
        translate(xyz);
    }
    void scale(const ml::vec3& xyz) {
        dilate(xyz);
    }
    void scale(float scale) {
        dilate(ml::vec3(scale, scale, scale));
    }

};

struct Shape : Transformable {

    glpp::Mesh m_mesh;

    /**
     * Creates the mesh of a shape.
     * The create function has to be implemented by derivatives.
     * create() is called when a shape is added to the scene, to lazily evaluate any changes.
     * @see Scene::add()
     */
//	virtual void create() = 0;

    /**
     * Create the Shape Mesh
     * Should be called by a derived Shape during the create() call
     * @param mesh
     * @see create
     */
    void create(glpp::Mesh&& mesh) {
        m_mesh = std::move(mesh);
    }

    [[nodiscard]] glpp::Mesh& mesh() {
        return m_mesh;
    }
    bool operator==(Shape& rhs) {
        return m_mesh == rhs.m_mesh && transformation() == rhs.transformation();
    }
};

struct Shape1D : public Shape {
  public:
    rgba color;
};

struct Shape2D : public Shape {
  public:
    void addMesh(glpp::Mesh&& mesh) {
        this->m_mesh.vertices.insert(this->m_mesh.vertices.end(), mesh.vertices.begin(), mesh.vertices.end());
        this->m_mesh.indices.insert(this->m_mesh.indices.end(), mesh.indices.begin(), mesh.indices.end());
        this->m_mesh.regenBuffers();
    }
    rgba borderColor = rgba();
    rgba fillColor;
    float borderThickness = 0.1f;
};

// TODO:
class Shape3D : public Shape {
  public:
};


ml::vec3 perpendicular(const ml::vec3& line){
    return ml::vec3{-line.y(), line.x(), 0.0f};
}



struct Point : ml::vec3 {
    using ml::vec3::vec3T;
};

struct Line: public Shape2D {
    Point start = ml::vec3(-0.5f, 0.0f, 0.0f);
    Point end = ml::vec3(1.0f, 0.0f, 0.0f);
//    Line(const ml::vec3& start, const ml::vec3& end): start(start), end(end){
//
//    }

    Point center() {
        return (end - start) / 2 + start;
    }

    Line&& operator+(ml::vec3 direction) {
        return std::move(Line {
            .start = start + direction,
            .end = end + direction
        });
    }

    Geometry create() {
        ml::vec3 line = end + start; // Translate line to origin
        std::cout << line << std::endl;
        std::cout << perpendicular(line) << std::endl;
        ml::vec3 thicknessUnitDirection = unit(perpendicular(line)) * (borderThickness / 2);
        std::cout << thicknessUnitDirection << std::endl;
        return glpp::createRectangle(start + thicknessUnitDirection, start - thicknessUnitDirection, end + thicknessUnitDirection, end - thicknessUnitDirection, fillColor);
//        Shape::create(createLine(start, end, Shape1D::color));
    }
};


struct Dot : public Shape2D {
    Point position;
    void create() {
//        createCircle(position, 0.1f, 10, rgba())
//        Shape::create()
    }
};


struct Polyline {
    std::vector<Point> points;
    Line operator[](size_t i){

    }
};


class Arrow : public Line {

    void create() {

    }
};

class Vector : public Arrow {

};




struct Angle: public Circle {
    Line& line1;
    Line& line2;
    Angle(Line& l1, Line& l2): line1(l1), line2(l2) {}

    void create() {
        auto [commonPoint, others] = splitCommonPoint({line1.start, line1.end, line2.start, line2.end});
        // center vectors
        ml::vec3 x = others[0] - commonPoint;
        ml::vec3 y = others[1] - commonPoint;

        auto angle = ::angle(x, y);
        auto startAngle = ::angle({1.0f, 0.0f, 0.0f}, x);

        float degreeAngle = ml::degrees(angle);
        radius = degreeAngle < 60.0f ?
                 radius / 10 / sqrt(degreeAngle/60) :
                 radius / 10;
//        Shape::create(createCircle(commonPoint, radius, 200 * radius, fillColor, startAngle, angle));
//        line1.start.angle()
//		createCircle()
//        if(lineIntersection(line1, line2)){
//
//		}
    }
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


template <class T, class U>
concept Derived = std::is_base_of<U, T>::value;

template<typename T>
concept Creatable = requires(T a) {
    a.create();
};


/**
 * Get the angle between 2 vectors
 * @return the angle in radians
 */
float angle(ml::vec3 p1, ml::vec3 p2) {
    return acos(ml::dot(p1, p2) / (p1.length() * p2.length()));
}

struct PolarPlane {
    Point origin = Origin;
    float xMax = 10.0f, xMin = -10.0f, yMin = -6.0f, yMax = 6.0f;

    Point operator()(float radius, float angle) {

    }
};

struct Grid {

};
struct CoordinateSystem : Shape2D {
    Point origin = Origin;
    float xMax = 10.0f, xMin = -10.0f, yMin = -6.0f, yMax = 6.0f;
    float xInterval = 1.0f, yInterval = 1.0f;

    Point operator()(size_t x, size_t y) const {
        if(x > xMax || x < yMin || y > yMax || y < yMax) {
            return origin;
        }

        return Point(origin.x() + x * xInterval,
                     origin.y() + y * yInterval);
    }

    void create() {
        if(xMin > xMax || yMin > yMax || xInterval < 0 || yInterval < 0) {
            assert("Coordinate System is ill defined.");
        }

        this->m_mesh = Mesh();

        for(float x = xMin; x < xMax; x += xInterval) {
            auto l = Line{
                .start = {0.0f, -10.0f, origin.z()},
                .end = {0.0f, 10.0f, origin.z()}
            };
            l.fillColor = Yellow;
            l.borderThickness = 0.02f;

            l.create();
            Shape::create(std::move(l.m_mesh));
//            addMesh(std::move(l.m_mesh));
//			std::cout << this->m_mesh.vertices.size() << std::endl;
//            std::cout << this->m_mesh.indices.size() << std::endl;
        }
        this->m_mesh.genBuffers();
        this->m_mesh.regenBuffers();
//		Shape::create()
    }
};
struct Triangle: public Shape2D {
    // Points
    ml::vec3 A = ml::vec3(-1.0f, -1.0f, 0.0f),
        B = ml::vec3(0.0f, 1.0f, 0.0f),
        C = ml::vec3(1.0f, -1.0f, 0.0f);
    // Lines
    std::array<Line, 3> line;

//	Triangle(){
//		updateLines();
//	}
    void updateLines(){
        line[0].start = A; line[0].end = B; // AB
        line[1].start = B; line[1].end = C; // BC
        line[2].start = C; line[2].end = A; // CA
    }

    Point centroid() {
        return Point {
            (A.x() + B.x() + C.x()) / 3,
            (A.y() + B.y() + C.y()) / 3,
            0.0f
        };
    }

    LinearEquation computeLinearEquation(Line& line) {
        float x1 = line.start.x(), y1 = line.start.y();
        float x2 = line.end.x(), y2 = line.end.y();

        float dy = y2 - y1;
        float dx = x2 - x1;
        float a = dy / dx;
        float b = (x2 * y1 - x1 * y2) / dx;
        return {a, b};
    }
    ml::vec2 intersection() {

    }
    // Intersection of 2 lines using determinants
    ml::vec2 intersection(Line& line1, Line& line2) {
        float x1 = line1.start.x(), y1 = line1.start.y();
        float x2 = line1.end.x(), y2 = line1.end.y();
        float x3 = line2.start.x(), y3 = line2.start.y();
        float x4 = line2.end.x(), y4 = line2.end.y();

        float D = det(ml::mat2{
            det(ml::mat2({x1,1,x2,1})),det(ml::mat2({y1,1,y2,1})),
            det(ml::mat2({x3,1,x4,1})), det(ml::mat2({y3,1,y4,1}))
        });
        float x = det(ml::mat2{
            det(ml::mat2({x1,y1,x2,y2})),det(ml::mat2({x1,1,x2,1})),
            det(ml::mat2({x3,y3,x4,y4})), det(ml::mat2({x3,1,x4,1}))
        }) / D;
        float y = det(ml::mat2{
            det(ml::mat2({x1,y1,x2,y2})),det(ml::mat2({y1,1,y2,1})),
            det(ml::mat2({x3,y3,x4,y4})), det(ml::mat2({y3,1,y4,1}))
        }) / D;

        return {x, y};
    }

//    ml::vec2 intersection() {
//
//    }


    bool isLeft(Line& AB, Point c){
        return std::signbit(det(ml::mat2{
            AB.end.x()-AB.start.x(), c.x()-AB.start.x(),
            AB.end.y()-AB.start.y(), c.y()-AB.start.y()
        }));
    }

    void create() {
        updateLines();
        Mesh baseMesh = createTriangle(A, B, C, fillColor);

        std::array<ml::vec3, 3> outerDirections;
        std::array<ml::vec2, 3> intersections;

        for(unsigned int i = 0; i < line.size(); i++) {
            Line& currentLine = line[i];
            Line& nextLine    = (i == line.size() - 1) ? line[0] : line[i + 1];

            Point innerPoint = nextLine.end;

            // create a unit vector that is perpendicular to the currentLine
            const auto perpendicularPoint = ml::unit(perpendicular(currentLine.end - currentLine.start)) * borderThickness;
            // make sure it points to the outer of the triangle
            const bool perpendicularToOuter = isLeft(currentLine, perpendicularPoint) != isLeft(currentLine, innerPoint);
            outerDirections[i] = perpendicularToOuter ? perpendicularPoint : ml::vec3(-perpendicularPoint);

        }

        for(unsigned int i = 0; i < line.size(); i++) {
            Line& currentLine = line[i];
            size_t nextLineIndex = (i == line.size() - 1) ? 0 : i + 1;
            Line& nextLine = line[nextLineIndex];

            Line a = Line {
                .start =currentLine.start + outerDirections[i],
                .end = currentLine.end + outerDirections[i]
            };
            Line b = Line {
                .start =nextLine.start + outerDirections[nextLineIndex],
                .end = nextLine.end + outerDirections[nextLineIndex]
            };
            intersections[i] = intersection(a, b);
            std::cout << "insersection: " << intersections[i] << std::endl;
        }


        scaledA = ml::vec3(intersections[0], 0.0f);
        scaledB = ml::vec3(intersections[1], 0.0f);
        scaledC = ml::vec3(intersections[2], 0.0f);

        std::cout << "insersectionA: " << scaledA << std::endl;
        std::cout << "insersectionB: " << scaledB << std::endl;
        std::cout << "insersectionC: " << scaledC << std::endl;


        if(line[0].fillColor == line[1].fillColor && line[0].fillColor == line[2].fillColor) {
            baseMesh.vertices.push_back(Vertex{scaledA, Red, {}, {}, 0});
            baseMesh.vertices.push_back(Vertex{scaledB, Red, {}, {}, 0});
            baseMesh.vertices.push_back(Vertex{scaledC, Red, {}, {}, 0});

            Index indices[] = {
                3, 0, 1, 4, 3, 1,
                3, 5, 0, 0, 2, 5,
                2, 5, 1, 5, 1, 4};
            baseMesh.indices.insert(baseMesh.indices.end(), std::begin(indices), std::end(indices));
        } else {
            baseMesh.vertices.push_back(Vertex{scaledA, line[0].fillColor, {}, {}, 0});
            baseMesh.vertices.push_back(Vertex{scaledB, line[0].fillColor, {}, {}, 0});
            baseMesh.vertices.push_back(Vertex{A, line[0].fillColor, {}, {}, 0});
            baseMesh.vertices.push_back(Vertex{B, line[0].fillColor, {}, {}, 0});

            baseMesh.vertices.push_back(Vertex{scaledB, line[1].fillColor, {}, {}, 0});
            baseMesh.vertices.push_back(Vertex{scaledC, line[1].fillColor, {}, {}, 0});
            baseMesh.vertices.push_back(Vertex{B, line[1].fillColor, {}, {}, 0});
            baseMesh.vertices.push_back(Vertex{C, line[1].fillColor, {}, {}, 0});

            baseMesh.vertices.push_back(Vertex{scaledC, line[2].fillColor, {}, {}, 0});
            baseMesh.vertices.push_back(Vertex{scaledA, line[2].fillColor, {}, {}, 0});
            baseMesh.vertices.push_back(Vertex{C, line[2].fillColor, {}, {}, 0});
            baseMesh.vertices.push_back(Vertex{A, line[2].fillColor, {}, {}, 0});

            Index indices[] = {
                3, 4, 5, 6, 5, 4,
                7, 8, 9, 10, 9, 8,
                11, 12, 13, 14, 13, 12};
            baseMesh.indices.insert(baseMesh.indices.end(), std::begin(indices), std::end(indices));
        }
        baseMesh.regenBuffers();
        Shape::create(std::move(baseMesh));
    }

//    void create() {
//        updateLines();
//		auto origin = centroid();
//        Mesh baseMesh = createTriangle(A, B, C, fillColor);
//		auto toCenter = ml::translate(ml::vec3(-origin));
//        auto toOrigin = ml::translate(origin);
//        ml::mat4 toScale = ml::scale(1 + borderThickness);
//        auto operation = toScale;
//
//        ml::vec4 scaledA = operation * ml::vec4(A);
//        ml::vec4 scaledB = operation * ml::vec4(B);
//        ml::vec4 scaledC = operation * ml::vec4(C);
//
//		if(line[0].fillColor == line[1].fillColor && line[0].fillColor == line[2].fillColor) {
//			baseMesh.vertices.push_back(Vertex{scaledA.xyz(), borderColor, {}, {}, 0});
//			baseMesh.vertices.push_back(Vertex{scaledB.xyz(), borderColor, {}, {}, 0});
//			baseMesh.vertices.push_back(Vertex{scaledC.xyz(), borderColor, {}, {}, 0});
//
//			Index indices[] = {
//				3, 0, 1, 4, 3, 1,
//				3, 5, 0, 0, 2, 5,
//				2, 5, 1, 5, 1, 4};
//			baseMesh.indices.insert(baseMesh.indices.end(), std::begin(indices), std::end(indices));
//		} else {
//            baseMesh.vertices.push_back(Vertex{scaledA.xyz(), line[0].fillColor, {}, {}, 0});
//            baseMesh.vertices.push_back(Vertex{scaledB.xyz(), line[0].fillColor, {}, {}, 0});
//            baseMesh.vertices.push_back(Vertex{A, line[0].fillColor, {}, {}, 0});
//            baseMesh.vertices.push_back(Vertex{B, line[0].fillColor, {}, {}, 0});
//
//            baseMesh.vertices.push_back(Vertex{scaledB.xyz(), line[1].fillColor, {}, {}, 0});
//            baseMesh.vertices.push_back(Vertex{scaledC.xyz(), line[1].fillColor, {}, {}, 0});
//            baseMesh.vertices.push_back(Vertex{B, line[1].fillColor, {}, {}, 0});
//            baseMesh.vertices.push_back(Vertex{C, line[1].fillColor, {}, {}, 0});
//
//            baseMesh.vertices.push_back(Vertex{scaledC.xyz(), line[2].fillColor, {}, {}, 0});
//            baseMesh.vertices.push_back(Vertex{scaledA.xyz(), line[2].fillColor, {}, {}, 0});
//            baseMesh.vertices.push_back(Vertex{C, line[2].fillColor, {}, {}, 0});
//            baseMesh.vertices.push_back(Vertex{A, line[2].fillColor, {}, {}, 0});
//
//            Index indices[] = {
//                3,4,5, 6,5,4,
//				7,8,9, 10,9,8,
//				11,12,13,14,13,12
//			};
//            baseMesh.indices.insert(baseMesh.indices.end(), std::begin(indices), std::end(indices));
//		}
//        baseMesh.regenBuffers();
//
//        Shape::create(std::move(baseMesh));
//    }
};


/**
 *  ___________________
 * | \ ______________/ |
 * |  |             |  |
 * |  |  position   |  | height
 * |  |_____________|  |
 * |_/_______________\_|
 *         width
 */
struct Rectangle: public Shape2D {
    ml::vec3 position = ml::vec3(0.0f);
    float height = 0.4, width = 0.5;

    void create() {
        auto baseMesh = createPlane2(position, {width, height}, rgba{0.0f, 1.0f, 0.0f, 1.0f}, 0);
        borderThickness = 0.2f;
        if(borderThickness > 0.0f){
            addBorder(baseMesh, borderThickness);
        }
        Shape::create(std::move(baseMesh));
    }
};


struct Ray {

};


struct Square: public Shape2D {
    ml::vec3 position = ml::vec3(0.0f);
    float size = 1.0f;

    void create() {
        Shape::create(createPlane2(position, {size, size}, rgba{1.0f, 0.0f, 0.0f, 1.0f}, 0));
    }
};

struct Circle: Shape2D {
    ml::vec3 center = ml::vec3(0.0f);
    float    radius = 1.0f;

    void create() {
        Shape::create(Mesh(createCircle(center, radius, static_cast<int>(200 * radius))));
    }
};


#endif //GLPP_SHAPE_H
