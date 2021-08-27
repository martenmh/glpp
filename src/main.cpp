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

#include "plotting/coordinates.h"


#include <iostream>
#include <list>
#include <random>

#include "animation.h"
#include "shape.h"
#include "scene.h"

using namespace glpp;

class Function {
	bool show = true;
	std::string expression;
	float operator()(float x) {}
};

/**
 * Simple recursive descent parser that parses into a walkable tree
 */
class ExpressionParser {
	std::vector<Function> functions;
	/**
	 * A walkable tree to
	 */
	class AST {
	    class Node{
            enum class Op {
                Assign,
                Multiply,
                Divide,
                Add,
                Subtract,
                Power,
                Negate,
                Function
            } operation;
			std::vector<Node> nodes;
		};
		Node root;
	};
	ExpressionParser() {
	}


};


Mesh plane(const ml::vec3& pos, const ml::vec2& size, rgba color, float textureID = 0.0f) {
	return Mesh({{{pos.x(), pos.y(), pos.z()}, color, {}, {0.0f, 0.0f}, textureID},
				 {{pos.x() + size.x(), pos.y(), pos.z()}, color, {}, {1.0f, 0.0f}, textureID},
				 {{pos.x(), pos.y() + size.y(), pos.z()}, color, {}, {0.0f, 1.0f}, textureID},
				 {{pos.x() + size.x(), pos.y() + size.y(), pos.z()}, color, {}, {1.0f, 1.0f}, textureID}},
				{0, 1, 2, 2, 1, 3});
}




std::ostream& operator<<(std::ostream& os, Mesh& mesh){
    for(auto& vertex : mesh.vertices){
		os << "Position:" << vertex.positions << std::endl;
        os << "Normal  : " << vertex.normals << std::endl;
	}

    for(Index index : mesh.indices){
        os << index << ", " << std::endl;
    }
}



/**
 * Return the first common point and all other points
 * @tparam N The amount of lines
 * @param array
 * @return
 */
std::pair<ml::vec3, std::vector<ml::vec3>> splitCommonPoint(std::vector<ml::vec3> points){
	ml::vec3 commonPoint;
	for(auto it = points.begin(); it != points.end() - 1; ++it) {
		for(auto jt = it + 1; jt != points.end(); jt++) {
		    if(*it == *jt) {
                commonPoint = *it;
                points.erase(jt); // erase 'jt' first, as 'it' would shift the 'jt' iterator
                points.erase(it);
                return {commonPoint, points};
			}
		}
	}
}

// Constants
constexpr const ml::vec3 Origin = {0.0f, 0.0f, 0.0f};
constexpr const ml::vec3 Left = {-1.0f, 0.0f, 0.0f};
constexpr const ml::vec3 Right = {1.0f, 0.0f, 0.0f};
constexpr const ml::vec3 Up = {0.0f, 1.0f, 0.0f};
constexpr const ml::vec3 Down = {0.0f, -1.0f, 0.0f};
const rgba Red = rgba(0.851, 0.298, 0.098);
const rgba Blue = rgba(0.149, 0.349, 0.60);
const rgba Yellow = rgba(0.949, 0.702, 0.098);
const rgba Black = rgba(0.0f, 4/255.0f, 1.0f/255.0f);
const rgba Transparent = rgba(0.0f, 0.0f, 0.0f, 0.0f);

ml::vec3 vertexMedianNormal(const std::array<Line, 2>& lines){
//    auto [commonVertex, uncommonVertex] = splitCommonPoint(lines);
//
//    ml::vec3 l1 = uncommonVertex[0] - commonVertex,
//        l2 = uncommonVertex[1] - commonVertex;
//    ml::vec3 midpoint = {(l1.x() + l2.x()) / 2,
//                         (l1.y() + l2.y()) / 2,
//                         (l1.z() + l2.z()) / 2};
//    ml::vec3 unit = midpoint / midpoint.length();
//    return -unit;
}

ml::vec3 vertexMedianNormal(ml::vec3 prev, ml::vec3 curr, ml::vec3 next){
    ml::vec3 l1 = prev - curr,
        l2 = next - curr;
    ml::vec3 midpoint = {(l1.x() + l2.x()) / 2,
                         (l1.y() + l2.y()) / 2,
                         (l1.z() + l2.z()) / 2};
    ml::vec3 unit = midpoint / midpoint.length();
    return -unit;
}

ml::vec3 vertexMedianNormal(ml::vec3 curr, ml::vec3 next){
    ml::vec3 l1 = Origin,
        l2 = next - curr;
    ml::vec3 midpoint = {(l1.x() + l2.x()) / 2,
                         (l1.y() + l2.y()) / 2,
                         (l1.z() + l2.z()) / 2};
    ml::vec3 unit = midpoint / midpoint.length();
    return -unit;
}


bool startConnectedToEnd(Mesh& mesh){
    Index startIndex = 0;
    Index endIndex = mesh.vertices.size() - 1;
    bool connected = false, startConnected = false, endConnected = false;
    for(int i = 0; !connected && i < mesh.indicesSize(); i += 3){
        startConnected = mesh.indices[i] == startIndex ||
                         mesh.indices[i + 1] == startIndex ||
                         mesh.indices[i + 2] == startIndex;

        endConnected = mesh.indices[i] == endIndex ||
                       mesh.indices[i + 1] == endIndex ||
                       mesh.indices[i + 2] == endIndex;
        connected = startConnected & endConnected;
    }
    return connected;
}

/**
 * Uses the normals to extend any polygon with a border of thickness
 * @param mesh
 * @param thickness
 */
void addBorder(Mesh& mesh, float thickness){
    const int meshSize = mesh.vertices.size();
    bool startConnected = startConnectedToEnd(mesh);

    for(int i = 0; i < meshSize - 1; i++){

        ml::vec3 normal;
        if(i == 0 && startConnected){
            normal = vertexMedianNormal(mesh.vertices.back().positions, mesh.vertices[i].positions, mesh.vertices[i + 1].positions);
        } else if(i == 0){
            normal = vertexMedianNormal(mesh.vertices[i].positions, mesh.vertices[i + 1].positions);
        } else {
            normal = vertexMedianNormal(mesh.vertices[i - 1].positions, mesh.vertices[i].positions, mesh.vertices[i + 1].positions);
        }

        auto nn = ml::vec3{static_cast<float>(normal.x()) * thickness, static_cast<float>(normal.y()) * thickness, static_cast<float>(normal.z()) * thickness};
        auto     outerPosition = mesh.vertices[i].positions + nn;

        if(i-1 == 0 && startConnected){
            normal = vertexMedianNormal(mesh.vertices.back().positions, mesh.vertices[i - 1].positions, mesh.vertices[i].positions);
        } else if(i-1 == 0){
            normal = vertexMedianNormal(mesh.vertices[i - 1].positions, mesh.vertices[i].positions);
        } else {
            normal = vertexMedianNormal(mesh.vertices[i - 2].positions, mesh.vertices[i - 1].positions, mesh.vertices[i].positions);
        }
        auto prevVertex = mesh.vertices[i - 1];
        nn = ml::vec3{static_cast<float>(normal.x()) * thickness, static_cast<float>(normal.y()) * thickness, static_cast<float>(normal.z()) * thickness};
        auto     prevOuterPosition = prevVertex.positions + nn;

        mesh.vertices.push_back(Vertex{
            prevVertex.positions, rgba{1.0f, 0.0f, 0.0f, 1.0f}, {}, {normal}, 0
        });
        mesh.vertices.push_back(Vertex{
            prevOuterPosition, rgba{1.0f, 0.0f, 0.0f, 1.0f}, {}, {normal}, 0
        });

        mesh.vertices.push_back(Vertex{
            mesh.vertices[i].positions, rgba{1.0f, 0.0f, 0.0f, 1.0f}, {}, {normal}, 0
        });
        mesh.vertices.push_back(Vertex{
            outerPosition, rgba{1.0f, 0.0f, 0.0f, 1.0f}, {}, {normal}, 0
        });

        // Copy previous inner and outer vertex


        Index innerVertexIndex = mesh.vertices.size() - 2;
        Index prevInnerVertexIndex = mesh.vertices.size() - 4;

        Index outerVertexIndex = mesh.vertices.size() - 1;
        Index prevOuterVertexIndex = mesh.vertices.size() - 3;

        Index indices[] = {
            prevOuterVertexIndex, prevInnerVertexIndex, innerVertexIndex,
            innerVertexIndex, outerVertexIndex, prevOuterVertexIndex
        };
        mesh.indices.insert(mesh.indices.end(), std::begin(indices), std::end(indices));
    }
    mesh.regenBuffers();
}

void addBorder2(Mesh& mesh, float thickness){
    const int meshSize = mesh.vertices.size();
	bool startConnected = startConnectedToEnd(mesh);
    ml::vec3 currPos;
	for(int i = 0; i < meshSize; i++){

		ml::vec3 normal, prevNormal, prevPos;
        if(i == 0 && startConnected){
            prevNormal = vertexMedianNormal(mesh.vertices[meshSize - 1].positions, mesh.vertices.back().positions, mesh.vertices[i].positions);
            normal = vertexMedianNormal(mesh.vertices.back().positions, mesh.vertices[i].positions, mesh.vertices[i + 1].positions);
			prevPos = mesh.vertices.back().positions;
        } else if(i == 0){
            normal = vertexMedianNormal(mesh.vertices[i].positions, mesh.vertices[i + 1].positions);
			continue;
        } else {
			prevNormal = normal;
            prevPos = mesh.vertices[i - 1].positions;
            normal = vertexMedianNormal(mesh.vertices[i - 1].positions, mesh.vertices[i].positions, mesh.vertices[i + 1].positions);
        }
        currPos = mesh.vertices[i].positions;
        auto outerPosition = currPos + normal * thickness;
		auto prevOuterPosition = prevPos + prevNormal * thickness;

		// set border between current point and previous point
        mesh.vertices.push_back(Vertex{
            prevPos, rgba{1.0f, 0.0f, 0.0f, 1.0f}, {}, {normal}, 0
        });
        mesh.vertices.push_back(Vertex{
            prevOuterPosition, rgba{1.0f, 0.0f, 0.0f, 1.0f}, {}, {normal}, 0
        });

        mesh.vertices.push_back(Vertex{
            currPos, rgba{1.0f, 0.0f, 0.0f, 1.0f}, {}, {normal}, 0
        });
        mesh.vertices.push_back(Vertex{
            outerPosition, rgba{1.0f, 0.0f, 0.0f, 1.0f}, {}, {normal}, 0
        });

        // Copy previous inner and outer vertex


        Index innerVertexIndex = mesh.vertices.size() - 2;
        Index prevInnerVertexIndex = mesh.vertices.size() - 4;

        Index outerVertexIndex = mesh.vertices.size() - 1;
        Index prevOuterVertexIndex = mesh.vertices.size() - 3;

        Index indices[] = {
            prevOuterVertexIndex, prevInnerVertexIndex, innerVertexIndex,
            innerVertexIndex, outerVertexIndex, prevOuterVertexIndex
        };
        mesh.indices.insert(mesh.indices.end(), std::begin(indices), std::end(indices));

	}
    mesh.regenBuffers();
}

struct LinearEquation {
	float a = 1, b = 0;
};

struct BezierCurve {
	std::vector<Point> points;

	void create() {

	}
};


struct MyScene: Scene {
	float xpos, ypos;
	bool hasStart = false;

	ml::vec2T<double> start;
    ml::vec2T<double> end;
    void onMouseMove(MouseMoveEvent e) {
        xpos = e.xpos; ypos = e.ypos;
    }
	void onMouseClick(MouseClickEvent e) {
        CoordinateMapper mapWindowToCoordinates = CoordinateMapper(createWindowCoordinates(window),
                                                  CoordinateSystemBase{
                                                      .xMin = -2.5,
                                                      .xMax = 2.5,
                                                      .yMin = -2,
                                                      .yMax = 2
                                                  });
		if(e.button == MouseButton::Left) {
			std::cout << window.height() << ", " << window.width() << std::endl;
			std::cout << xpos << ", " << ypos << std::endl;
            std::cout << mapWindowToCoordinates({xpos, ypos}) << std::endl;
            if(!hasStart) {
				start = mapWindowToCoordinates({xpos, ypos});
				std::cout << "start" << start << std::endl;
				hasStart = true;
			} else {
                end = mapWindowToCoordinates({xpos, ypos});
                std::cout << "end" << end << std::endl;
                auto line1 = Line{
                    .start = ml::vec3{start, 0.0f},
                    .end   = ml::vec3{end, 0.0f}
				};
                line1.fillColor       = Blue;
                line1.borderThickness = 0.025f;
                add(line1);
				hasStart = false;
			}

		}
	}

	void setup() override {

//		auto atr = Rectangle {
//            .position = {2.0f, 2.0f},
//			.height = 2.0f,
//			.width = 3.0f
//		};
//		add(atr);
//        auto atrr = Rectangle {
//            .position = {2.0f, 2.0f},
//            .height = 2.0f,
//            .width = 3.0f
//        };
//		atrr.fillColor = Blue;
//        atrr.scale(3.0f);
//        add(atrr);

		auto tr = Triangle{
			.A = {-0.5f, -0.5f, 0.0f},
			.B = {0.0f, 0.5f, 0.0f},
			.C = {0.5f, -0.5f, 0.0f}};
        tr.borderThickness = 0.02f;
		tr.line[0].fillColor = Red;
        tr.line[1].fillColor = Blue;
        tr.line[2].fillColor = Yellow;
		tr.fillColor = Blue;
		tr.scale(3.0f);

		add(tr);

        auto circleA = Circle{
            .center = scaledA,
            .radius = 0.1f
        };
        auto circleB = Circle{
            .center = scaledB,
            .radius = 0.1f
        };
        auto circleC = Circle{
            .center = scaledC,
            .radius = 0.1f
        };
//		add(circleA);
//        add(circleB);
//        add(circleC);

//		auto circle = Circle{
//			.radius = 2.0f};
        //        tr.rotate({0.0f, 0.0f, ml::radians(35.0f)});

        //		tr.move(Left);
        //		circle.move(Right);

//		add(circle);

		// -2.2...2.2
//		auto line1 = Line{
//			.start = Origin,
//			.end   = Up};
//		line1.fillColor       = Yellow;
//		line1.borderThickness = 0.02f;
//        {
//		auto line2 = Line{
//			.start = Origin,
//			.end   = Left};
//		line2.fillColor       = Red;
//		line2.borderThickness = 0.02f;
//		add(line2);
//	}
//
////		auto a = Angle(line1, line2);
////        a.radius = 0.02f;
////        add(a);
//        add(line1);
        auto coordinate = CoordinateSystem();

        for(float x = coordinate.xMin; x < coordinate.xMax; x += coordinate.xInterval) {
            auto l = Line{
                .start = ml::vec3(x, -5.0f, 0.0f),
                .end = ml::vec3(x, 5.0f, 0.0f)
            };
            l.fillColor = Yellow;
            l.borderThickness = 0.02f;

//            l.create();
			add(l);
//            Shape::create(std::move(l.m_mesh));
//            addMesh(std::move(l.m_mesh));
//			std::cout << this->m_mesh.vertices.size() << std::endl;
//            std::cout << this->m_mesh.indices.size() << std::endl;
        }
        for(float x = coordinate.xMin; x < coordinate.xMax; x += coordinate.xInterval) {
            auto l = Line{
                .start = ml::vec3(-10.0f, x, 0.0f),
                .end = ml::vec3(10.0f, x, 0.0f)
            };
            l.fillColor = Yellow;
            l.borderThickness = 0.02f;

//            l.create();
            add(l);
//            Shape::create(std::move(l.m_mesh));
//            addMesh(std::move(l.m_mesh));
//			std::cout << this->m_mesh.vertices.size() << std::endl;
//            std::cout << this->m_mesh.indices.size() << std::endl;
        }

		add(coordinate);
//		coordinate(0, 0);
		EventSystem::onMouseMove.connect<&MyScene::onMouseMove>(this);
		EventSystem::onMouseClick.connect<&MyScene::onMouseClick>(this);

	}
};

//#include <immintrin.h>

/*
 * Reflect(line)
 * Reflect(point)
 * Rotate(angle, Origin = Center_Origin)
 * Translate(pointA, pointB)
 * ==
 * .isCongruent(other)
 */


#define SETUP_SCENE(SCENE_TYPE) \
	SCENE_TYPE scene;\
    scene.setup();\
    while(scene.window.isOpen()) {\
        scene.window.pollEvents();\
        if(scene.window.keyIsPressed(Key::Q) || scene.window.keyIsPressed(Key::ESCAPE)) {\
            scene.window.close();\
        }\
        scene.window.clear({0.05f, 0.05f, 0.05f, 1.0f});\
        scene.update();\
        scene.render();\
        scene.window.display();\
    }

#include "plotting/tokenizer.h"
#include "plotting/parser.h"
#include "plotting/evaluation.h"
#include "plotting/AST.h"


void printTokens(std::ostream& os, std::queue<Token>& tokens){
	std::queue<Token> temp = tokens;
	while(!temp.empty()) {
		os << temp.front() << std::endl;
        temp.pop();
	}
}

void eval(std::string& input){
	auto tokens = tokenize(input);
	printTokens(std::cout, tokens);
	AST tree = parse(tokens);
    printBT(std::cout, &tree);
    auto result = interpret(tree);
    std::cout << result << std::endl;

}

void REPL(){
	std::string input;
    do{
		if(!input.empty())
            eval(input);

        std::cout << "> ";
		getline(std::cin, input);
//        std::cin >> input;
        std::cout << std::endl;
	} while(input != ":q");
}


int main() {
////	__m256 evens = _mm256_set_ps(2.0, 4.0, 6.0, 8.0, 10.0, 12.0, 14.0, 16.0);
////	auto result = _mm256_add_ps(evens, evens);
//
//	MyScene scene;
//	scene.setup();
//	while(scene.window.isOpen()) {
//		scene.window.pollEvents();
//
//		if(scene.window.keyIsPressed(Key::Q) || scene.window.keyIsPressed(Key::ESCAPE)) {
//			scene.window.close();
//		}
//
//		scene.window.clear({0.05f, 0.05f, 0.05f, 1.0f});
////        scene.window.clear({0.95f, 0.95f, 0.95f, 1.0f});
//		scene.update();
//		scene.render();
//		scene.window.display();
//	}
//    REPL();
    SETUP_SCENE(MyScene);

}

