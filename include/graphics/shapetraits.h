#ifndef SHAPETRAITS_H
#define SHAPETRAITS_H

#include <utility>

#include "vertex.h"
#include "ml/transform.h"

namespace glpp {
	template<typename T>
	std::vector<T> expand(T first, T second, typename std::make_signed<T>::type interval = T(1)) {
		typedef typename std::make_signed<T>::type SignedT;
		static_assert(std::is_arithmetic<T>(), "T should be arithmetic.");
		std::vector<T> result;
		result.reserve(std::abs(SignedT(second - first)));
		if(first > second)
			interval = SignedT(-interval);
		for(T i = first; i < second; i += interval) {
			result.push_back(i);
		}
		return result;
	}
	/**
 * Defines a structure to be transformable
 */
//	class Transformable {
//		// Local origin
//		ml::vec3 m_origin;
//
//	  public:
//		//ml::Transform<float> transform;
//
//		void setOrigin(const ml::vec3& pos) {
//			m_origin = pos;
//		}
//		void setPosition(const ml::vec3& axis) {
//		}
//		/**
//	 * Move the transformable using the current positions + the new positions
//	 * @param axis the XYZ axis to move by
//	 */
//		void move(const ml::vec3& axis) {
//			//transform.translate(axis);
//		}
//
//		/**
//	 * Rotate the ml::vec3ransformable
//	 * @param axis the XYZ axis to rotate in degrees
//	 */
//		void rotate(const ml::vec3& axis) {
//			//transform.rotate(ml::radians(1.0f), ml::radians(axis));
//		}
//		void rotate(float angle, const ml::vec3& axis) {
//			//transform.rotate(angle, ml::radians(axis));
//		}
//		/**
//	 * Scale the ml::vec3ransformable
//	 * @param axis the XYZ axis to scale
//	 */
//		void scale(const ml::vec3& axis) {
//			//transform.scale(axis);
//		}
//	};

	/**
 * Defines a structure to be drawable by the renderer
 */
	class Drawable {
	  protected:
		//DrawSettings m_settings;
		std::vector<Vertex> m_vertices;
		std::vector<Index>  m_indices;

	  public:
		std::vector<Vertex>& vertices() { return m_vertices; }
		std::vector<Index>&  indices() { return m_indices; }
		Drawable() = default;
		Drawable(std::vector<Vertex> vertices):
			m_vertices(std::move(vertices)) {
		}
		Drawable(std::vector<Vertex> vertices, std::vector<Index> indices):
			m_vertices(std::move(vertices)), m_indices(std::move(indices)) {
		}
		//	virtual Vertex& draw(Renderer& renderer, const DrawSettings& drawSettings){
		//		draw(renderer);
		//	}
		//virtual Vertex& draw(Renderer& renderer)=0;
	};

//	class Drawable2D: public Drawable, public Transformable {
//		Texture* texture;
//		Slot     slot;
//		void     updateTextureSlot() {
//            if(texture && slot != texture->getSlot()) {
//                slot = texture->getSlot();
//                for(auto& vertex: m_vertices) {
//                    vertex.textureSlot = slot;
//                }
//            }
//		}
//
//	  public:
//		void setSlot(Slot slot) {
//			for(auto& vertex: m_vertices) {
//				vertex.textureSlot = slot;
//			}
//		}
//		void setColor(rgba color) {}
//		void setBorderColor(rgba color) {}
//		void setBorderSize() {}
//		void setTexture(Texture& tex) {
//			TextureManager::addTexture(&tex);
//			texture = &tex;
//		}
//		ml::vec3& getPosition(int index) {
//		}
//
//		//	Vertex& draw(Renderer& renderer) override {
//		//		//updateTextureSlot();
//		//		renderer.draw(vertices.data(), vertices.size(),
//		//                       indices.data(), indices.size());
//		//	}
//		Drawable2D() = default;
//
//		Drawable2D(std::pair<std::vector<Vertex>, std::vector<Index>> data):
//			Drawable(data.first, data.second), slot(m_vertices[0].textureSlot) {
//		}
//		Drawable2D(std::vector<Vertex> vertices, std::vector<GLuint> indices):
//			Drawable(std::move(vertices), std::move(indices)), slot(m_vertices[0].textureSlot) {
//		}
//	};
//
//	class Drawable3D: public Drawable, public Transformable {
//	  public:
//		//	Vertex& draw(Renderer& renderer) override {
//		//		renderer.draw(vertices.data(), vertices.size(),
//		//					  indices.data(), indices.size());
//		//	}
//		Drawable3D(std::vector<Vertex> vertices, std::vector<GLuint> indices):
//			Drawable(std::move(vertices), std::move(indices)) {
//		}
//
//		Drawable3D() = default;
//
//		Drawable3D(std::vector<Vertex> vertices):
//			Drawable(vertices) {
//		}
//		Drawable3D(std::pair<std::vector<Vertex>, std::vector<Index>> data):
//			Drawable(data.first, data.second) {
//		}
//	};
//
//	class Point: public Transformable {
//		ml::vec3& point;
//		ml::vec3& origin;
//	};
}

#endif //SHAPETRAITS_H
