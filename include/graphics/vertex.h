#ifndef VERTEX_H
#define VERTEX_H

#include "math-lib2/include/ml/graphics/color.h"
#include "ml/ml.h"
#include "opengl.h"

namespace glpp {
	// Get the offset of a struct and it's member
	template<typename T1, typename T2>
	inline size_t offset_of(T1 T2::*member) {
		T2 object{};
		return size_t(&(object.*member)) - size_t(&object);
	}
	struct Normal {
		unsigned x : 10;
		unsigned y : 10;
		unsigned z : 10;
	};
	/**
 * Vertex Attribute Structure
 * Defines the Vertex Buffer Layout for use in the m_shaders
 */
	struct Vertex {
		ml::vec3         positions;
		rgba             color = {1.0f, 1.0f, 1.0f, 1.0f};
		ml::vec3T<short> normals; // TODO: could be stored in a 32 bit number https://www.khronos.org/opengl/wiki/Vertex_Specification_Best_Practices
		ml::vec2         texCoords   = {0.0f, 0.0f};
		Slot             textureSlot = 0;
		constexpr Vertex()           = default;
		explicit Vertex(const ml::vec3& positions):
			positions(positions) {}
		Vertex(const ml::vec3& positions, const rgba& color, const ml::vec3T<short>& normals, const ml::vec2& textureCoordinates = {0.0f, 0.0f}, float textureID = 0.0f):
			positions(positions), color(color), normals(normals), texCoords(textureCoordinates), textureSlot(0) {}
	};
}

#endif //VERTEX_H
