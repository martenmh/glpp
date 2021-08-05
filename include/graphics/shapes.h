#ifndef SHAPES_H
#define SHAPES_H

#include "math-lib2/include/ml/graphics/color.h"
#include "ml/vector.h"
#include "renderer.h"
#include "shapetraits.h" // Defines all shape traits, eg. Drawable, Transformable.

namespace glpp {
	using VertexIndexPair = std::pair<std::vector<Vertex>, std::vector<Index>>;
	using MeshSplit       = std::pair<VertexIndexPair, VertexIndexPair>;

	inline Index maxIndex(std::vector<Index>& indices) {
		if(indices.empty())
			return 0;
		Index maxIndex = *std::max_element(indices.begin(), indices.end());
		if(maxIndex == 0)
			return 0;

		return maxIndex + 1;
	}

	/* Creators that add onto existing indices and vertices */
	void createPlane(const ml::vec3& pos, const ml::vec2& size,
					 std::vector<Vertex>& vertices, std::vector<Index>& indices);
	void createLine(const ml::vec3& from, const ml::vec3& to, float thickness,
					rgba                 color,
					std::vector<Vertex>& vertices, std::vector<Index>& indices);

	void createCircle(const ml::vec3& center, float radius, int segments,
					  std::vector<Vertex>& vertices, std::vector<Index>& indices);

	/* Creators that create new vertices and indices */
	VertexIndexPair createPlane(const ml::vec3& center, const ml::vec2& size, rgba color, float textureID);
	VertexIndexPair createLine(const ml::vec3& from, const ml::vec3& to, float thickness);
	VertexIndexPair createCircle(const ml::vec3& center, float radius, int segments);
	VertexIndexPair createUVSphere(const ml::vec3& center, float radius, int stacks, int segments, int texSlot);
	VertexIndexPair createCircle(const ml::vec3& center, float radius, int stacks, int segments);

	/** ------------------------- 2 Dimensional Shapes ------------------------- **/
	struct Triangle: public Drawable2D {
		using Drawable2D::Drawable2D;
		enum Side {
			Left   = 0,
			Middle = 10,
			Right  = 20
		};
		Triangle(const ml::vec3& pos, const ml::vec2& size, rgba color, Side side = Middle, float textureID = 0.0f):
			Drawable2D{{{{pos.x(), pos.y(), 0.0f}, color, {}, {0.0f, 0.0f}, textureID},
						{{pos.x() + size.y(), pos.y(), 0.0f}, color, {}, {1.0f, 0.0f}, textureID},
						{{pos.x() + size.x() * (static_cast<float>(side) / 10), pos.y() + size.y(), 0.0f}, color, {}, {0.5f, 1.0f}, textureID}},
					   {0, 1, 2}} {}

		Triangle(const std::array<ml::vec3, 3>& pos, rgba color, float textureID = 0.0f):
			Drawable2D{{{{pos[0].x(), pos[0].y(), 0.0f}, color, {}, {0.0f, 0.0f}, textureID},
						{{pos[1].x(), pos[1].y(), 0.0f}, color, {}, {1.0f, 0.0f}, textureID},
						{{pos[2].x(), pos[2].y(), 0.0f}, color, {}, {0.5f, 1.0f}, textureID}},
					   {0, 1, 2}} {}
	};

	struct Rectangle: public Drawable2D {
		using Drawable2D::Drawable2D;

		Rectangle(const ml::vec3& pos, const ml::vec2& size, rgba color, float textureID = 0.0f):
			Drawable2D{createPlane(pos, size, color, textureID)} {
		}
	};

	struct Curve: public Drawable2D {
		using Drawable2D::Drawable2D;
	};
	struct BrezierCurve: public Curve {
	};

	struct HermiteCurve: public Curve {
	};

	struct ChaikinCurve: public Curve {
	};

	struct CubicBSplineCurve: public Curve {
	};

	struct ClampedCubicBSplineCurve: public Curve {
	};

	struct NurbsCurve: public Curve {
	};

	struct Line: public Drawable2D {
	};

	struct Circle: public Drawable2D {
		using Drawable2D::Drawable2D;

	  public:
		Vertex center;
		float  radius;
		Circle(const ml::vec3 center, float radius, int segments):
			Drawable2D{createCircle(center, radius, segments)} {
		}
	};

	struct Polygon: public Drawable2D {
		using Drawable2D::Drawable2D;
	};

	struct Polyline: public Drawable2D {
		using Drawable2D::Drawable2D;
	};
	struct Path: public Drawable2D {
		using Drawable2D::Drawable2D;

		//void moveTo(const ml::vec3& );
	};

	/** ------------------------- 3 Dimensional Shapes ------------------------- **/
	struct UVSphere: public Drawable3D {
		using Drawable3D::Drawable3D;
		UVSphere(const ml::vec3 center, float radius, int stacks, int segments, int texSlot = 1):
			Drawable3D{createUVSphere(center, radius, stacks, segments, texSlot)} {
		}
	};

	struct IcoSphere: public Drawable3D {
		using Drawable3D::Drawable3D;
		//IcoSphere
	};

	struct Cube: public Drawable3D {
		using Drawable3D::Drawable3D;
		Cube(const ml::vec3& pos, const ml::vec3& size, rgba color, float textureID):
			Drawable3D(
				{{{pos.x(), pos.y(), pos.z()}, color, {}, {0.0f, 0.0f}, textureID},
				 {{pos.x() + size.x(), pos.y(), pos.z()}, color, {}, {1.0f, 0.0f}, textureID},
				 {{pos.x() + size.x(), pos.y() + size.y(), pos.z()}, color, {}, {1.0f, 1.0f}, textureID},
				 {{pos.x() + size.x(), pos.y() + size.y(), pos.z()}, color, {}, {1.0f, 1.0f}, textureID},
				 {{pos.x(), pos.y() + size.y(), pos.z()}, color, {}, {0.0f, 1.0f}, textureID},
				 {{pos.x(), pos.y(), pos.z()}, color, {}, {0.0f, 0.0f}, textureID},
				 {{pos.x(), pos.y(), pos.z() + size.z()}, color, {}, {0.0f, 0.0f}, textureID},
				 {{pos.x() + size.x(), pos.y(), pos.z() + size.z()}, color, {}, {1.0f, 0.0f}, textureID},
				 {{pos.x() + size.x(), pos.y() + size.y(), pos.z() + size.z()}, color, {}, {1.0f, 1.0f}, textureID},
				 {{pos.x() + size.x(), pos.y() + size.y(), pos.z() + size.z()}, color, {}, {1.0f, 1.0f}, textureID},
				 {{pos.x(), pos.y() + size.y(), pos.z() + size.z()}, color, {}, {0.0f, 1.0f}, textureID},
				 {{pos.x(), pos.y(), pos.z() + size.z()}, color, {}, {0.0f, 0.0f}, textureID},
				 {{pos.x(), pos.y() + size.y(), pos.z() + size.z()}, color, {}, {1.0f, 0.0f}, textureID},
				 {{pos.x(), pos.y() + size.y(), pos.z()}, color, {}, {1.0f, 1.0f}, textureID},
				 {{pos.x(), pos.y(), pos.z()}, color, {}, {0.0f, 1.0f}, textureID},
				 {{pos.x(), pos.y(), pos.z() + size.z()}, color, {}, {0.0f, 1.0f}, textureID},
				 {{pos.x(), pos.y(), pos.z()}, color, {}, {0.0f, 0.0f}, textureID},
				 {{pos.x(), pos.y() + size.y(), pos.z() + size.z()}, color, {}, {1.0f, 1.0f}, textureID},
				 {{pos.x() + size.x(), pos.y() + size.y(), pos.z() + size.z()}, color, {}, {1.0f, 0.0f}, textureID},
				 {{pos.x() + size.x(), pos.y() + size.y(), pos.z()}, color, {}, {1.0f, 1.0f}, textureID},
				 {{pos.x() + size.x(), pos.y(), pos.z()}, color, {}, {0.0f, 1.0f}, textureID},
				 {{pos.x() + size.x(), pos.y(), pos.z()}, color, {}, {0.0f, 1.0f}, textureID},
				 {{pos.x() + size.x(), pos.y(), pos.z() + size.z()}, color, {}, {0.0f, 0.0f}, textureID},
				 {{pos.x() + size.x(), pos.y() + size.y(), pos.z() + size.z()}, color, {}, {1.0f, 0.0f}, textureID},
				 {{pos.x(), pos.y(), pos.z()}, color, {}, {0.0f, 1.0f}, textureID},
				 {{pos.x() + size.x(), pos.y(), pos.z()}, color, {}, {1.0f, 1.0f}, textureID},
				 {{pos.x() + size.x(), pos.y(), pos.z() + size.z()}, color, {}, {1.0f, 0.0f}, textureID},
				 {{pos.x() + size.x(), pos.y(), pos.z() + size.z()}, color, {}, {1.0f, 0.0f}, textureID},
				 {{pos.x(), pos.y(), pos.z() + size.z()}, color, {}, {0.0f, 0.0f}, textureID},
				 {{pos.x(), pos.y(), pos.z()}, color, {}, {0.0f, 1.0f}, textureID},
				 {{pos.x(), pos.y() + size.y(), pos.z()}, color, {}, {0.0f, 1.0f}, textureID},
				 {{pos.x() + size.x(), pos.y() + size.y(), pos.z()}, color, {}, {1.0f, 1.0f}, textureID},
				 {{pos.x() + size.x(), pos.y() + size.y(), pos.z() + size.z()}, color, {}, {1.0f, 0.0f}, textureID},
				 {{pos.x() + size.x(), pos.y() + size.y(), pos.z() + size.z()}, color, {}, {1.0f, 0.0f}, textureID},
				 {{pos.x(), pos.y() + size.y(), pos.z() + size.z()}, color, {}, {0.0f, 0.0f}, textureID},
				 {{pos.x(), pos.y() + size.y(), pos.z()}, color, {}, {0.0f, 1.0}, textureID}},
				{expand<unsigned int>(0, 36)}) {
		}
		Cube(const ml::vec3& pos, const ml::vec3& size, rgba color):
			Drawable3D({{{pos.x(), pos.y(), pos.z()}, color, {}, {0.0f, 0.0f}},
						{{pos.x() + size.x(), pos.y(), pos.z()}, color, {}, {1.0f, 0.0f}},
						{{pos.x() + size.x(), pos.y() + size.y(), pos.z()}, color, {}, {1.0f, 1.0f}},
						{{pos.x(), pos.y() + size.y(), pos.z()}, color, {}, {0.0f, 1.0f}},
						{{pos.x(), pos.y(), pos.z() + size.z()}, color, {}, {0.0f, 1.0f}},
						{{pos.x() + size.x(), pos.y(), pos.z() + size.z()}, color, {}, {1.0f, 1.0f}},
						{{pos.x() + size.x(), pos.y() + size.y(), pos.z() + size.z()}, color, {}, {1.0f, 0.0f}},
						{{pos.x(), pos.y() + size.y(), pos.z() + size.z()}, color, {}, {0.0f, 0.0f}}},
					   {{
						   0, 1, 3, 1, 2, 3, // Back Quad
						   0, 1, 4, 1, 5, 4, // Right Quad
						   2, 3, 7, 2, 6, 7, // Left Quad
						   4, 5, 7, 5, 6, 7, // Front Quad
						   0, 3, 4, 3, 4, 7, // Top Quad
						   1, 2, 5, 2, 5, 6  // Bottom Quad
					   }}) {
		}
	};

	struct Object {
		//    std::vector<Vertex> vertices;
		//    template<int N>
		//    inline Object(const std::array<Vertex, N>& array){
		//        //memcpy()
		//    }
		//    inline void push_back(const std::array<Vertex, N>& array){
		////        vertices.reserve()
		////        memcpy()
		//    }
		//    inline void* data(){
		//        //return vertices.data();
		//    }
	};

	using CutLine = std::pair<ml::vec2, ml::vec2>;

	//TODO: Create Meshes and replace vertices & indices by mesh.
	//TODO: replace to meshtools.h?
	// Splits the incoming vertices indices and returns the split part.
	inline VertexIndexPair meshSlice2D(std::vector<Vertex>& inVertices, std::vector<Index>& inIndices, CutLine ab) {
		auto& a = ab.first;
		auto& b = ab.second;

		for(auto i = inIndices.begin(); i != (inIndices.end() - 1); i++) {
			auto a = inVertices[*i].positions.xy();
			auto b = inVertices[*i + 1].positions.xy();
			//auto x = ml::intersect({a, b}, ab);
			//if(x)
			//logging::debug(strfmt("({}, {}) ({}, {}), ({}, {}) ({}, {}) => ({}, {})", a.x(), a.y(), b.x(), b.y(), ab.first.x(), ab.first.y(), ab.second.x(), ab.second.y(), x->x(), x->y()));
		}

		return {inVertices, inIndices};
	}
	// Splits and results both parts
	//inline MeshSplit meshSlice2D(const std::vector<Vertex>& vertices, const std::vector<Index>& indices, ml::vec2 pos, ml::vec2 rotation){}
	// Splits the incoming vertices indices and returns the split part.
	inline VertexIndexPair meshSlice3D(std::vector<Vertex>& inVertices, std::vector<Index>& inIndices, ml::vec3 pos, ml::vec3 rotation) {}
	// Splits and results both parts
	inline MeshSplit meshSlice3D(const std::vector<Vertex>& vertices, const std::vector<Index>& indices, ml::vec3 pos, ml::vec3 rotation) {}

	VertexIndexPair createPlane(const ml::vec3& pos, const ml::vec2& size, rgba color, float textureID) {
		return VertexIndexPair({{{pos.x(), pos.y(), pos.z()}, color, {}, {0.0f, 0.0f}, textureID},
								{{pos.x() + size.x(), pos.y(), pos.z()}, color, {}, {1.0f, 0.0f}, textureID},
								{{pos.x(), pos.y() + size.y(), pos.z()}, color, {}, {0.0f, 1.0f}, textureID},
								{{pos.x() + size.x(), pos.y() + size.y(), pos.z()}, color, {}, {1.0f, 1.0f}, textureID}},
							   {0, 1, 2, 2, 1, 3});
	}
	VertexIndexPair createLine(const ml::vec3& from, const ml::vec3& to, float thickness) {
	}

	void createLine(const ml::vec3& from, const ml::vec3& to, float thickness, rgba color, std::vector<Vertex>& vertices, std::vector<Index>& indices) {
		Index maxI = maxIndex(indices);
		vertices.insert(vertices.end(), {{{from.x() + thickness, from.y() + thickness, from.z()}, color, {}, {0.0f, 0.0f}, 0.0f},
										 {{to.x() + thickness, to.y() + thickness, to.z()}, color, {}, {1.0f, 0.0f}, 0.0f},
										 {{to.x() - thickness, to.y() - thickness, to.z()}, color, {}, {0.0f, 1.0f}, 0.0f},
										 {{from.x() - thickness, from.y() - thickness, from.z()}, color, {}, {1.0f, 1.0f}, 0.0f}});
		indices.insert(indices.end(), {maxI + 0, maxI + 1, maxI + 2, maxI + 2, maxI + 0, maxI + 3});
	}

	void createCircle(const ml::vec3& center, float radius, int segments, std::vector<Vertex>& vertices, std::vector<Index>& indices) {
		rgba  col{1.0f, 1.0f, 1.0f, 1.0f};
		Index maxI = maxIndex(indices);
		vertices.push_back({center, col, {}});
		for(unsigned int i = 0; i <= segments; i++) {
			float       theta = 2.0f * M_PI * float(i) / float(segments); // Angle
			const float x     = sin(theta);
			const float y     = cos(theta);
			vertices.push_back({{center.x() + (radius * x), center.y() + (-radius * y), center.z()},
								col,
								{},
								{x, y},
								{}});
			indices.insert(indices.end(), {maxI, maxI + i, maxI + i + 1});
		}
		// Connect to first segment
		//int in[3] = {0,segments, 1};
		//indices.insert(indices.end(), std::begin(in), std::end(in));
	}
	VertexIndexPair createCircle(const ml::vec3& center, float radius, int segments) {
		std::vector<Vertex> vertices;
		std::vector<Index>  indices;
		createCircle(center, radius, segments, vertices, indices);
		return {vertices, indices};
	}

	Mesh createTriangle(const ml::vec3& A, const ml::vec3& B, const ml::vec3& C, rgba color, float textureID = 0.0f){
        return Mesh(VertexIndexPair{{{{A.x(), A.y(), A.z()}, color, {}, {0.0f, 0.0f}, textureID},
                            {{B.x(), B.y(), B.z()}, color, {}, {1.0f, 0.0f}, textureID},
                               {{C.x(), C.y(), C.z()}, color, {}, {0.5f, 1.0f}, textureID}},
                        {0, 1, 2}});
	}

    Mesh createPlane2(const ml::vec3& center, const ml::vec2& size, rgba color, float textureID) {
        return Mesh(VertexIndexPair({
                                {{center.x() - size.x(), center.y() - size.y(), center.z()}, color, {}, {0.0f, 0.0f}, textureID},
                                {{center.x() + size.x(), center.y() - size.y(), center.z()}, color, {}, {1.0f, 0.0f}, textureID},
                                {{center.x() - size.x(), center.y() + size.y(), center.z()}, color, {}, {0.0f, 1.0f}, textureID},
                                {{center.x() + size.x(), center.y() + size.y(), center.z()}, color, {}, {1.0f, 1.0f}, textureID}},
                               {0, 1, 2, 2, 1, 3}));
    }

	VertexIndexPair createICOSphere(const ml::vec3& center, float radius, int stacks, int segments, int texSlot) {
	}

	//VertexIndexPair createUVSphere(const ml::vec3& center, float radius, int stacks, int segments, int texSlot){
	//	rgba col {1.0f, 1.0f, 1.0f, 1.0f};
	//	std::vector<Vertex> vertices;
	//	std::vector<Index> indices;
	//
	//	const float sectorStep = 2 * M_PI / segments;
	//	const float stackStep = M_PI / stacks;
	//	const float invR = 1.0f / radius;
	//	for(int stack = 0; stack <= stacks; stack++)
	//	{
	//		const float phi = M_PI / 2 - stack * stackStep;
	//		const float xy = radius * cosf(phi);             // r * cos(u)
	//		const float z = radius * sinf(phi);              // r * sin(u)
	//
	//		int k1 = stack * (segments + 1); // beginning of current stack
	//		int k2 = k1 + segments + 1;      // beginning of next stack
	//
	//		// the first and last vertices have same positions and normal, but different tex coords
	//		for(int segment = 0; segment <= segments; segment++, k1++, k2++)
	//		{
	//			const float theta = segment * sectorStep;
	//
	//			const float x = xy * cosf(theta);             // r * cos(u) * cos(v)
	//			const float y = xy * sinf(theta);             // r * cos(u) * sin(v)
	//			vertices.push_back({
	//				{center.x() + x, center.y() + y, center.z() + z},
	//				col,
	//				{(short)(x * invR), (short)(y * invR), (short)(z * invR)},
	//				{(float)(segment / segments), (float)(stack / stacks)},
	//				(float)texSlot});
	//
	//			if(stack != 0){
	//				int in[] = {k1, k2, k1 + 1};
	//				indices.insert(indices.end(), std::begin(in), std::end(in));
	//			}
	//			if(stack != (stacks - 1)){
	//				int in[] = {k1 + 1, k2, k2 + 1};
	//				indices.insert(indices.end(), std::begin(in), std::end(in));
	//			}
	//		}
	//	}
	//	return {std::move(vertices), std::move(indices)};
	//}
}

#endif //SHAPES_H
