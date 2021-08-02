//
// Created by marten on 13-08-20.
//

#ifndef MESH_H
#define MESH_H

#include "common.h"

#include <utility>
//#include "assimp/Importer.hpp"
//#include "assimp/postprocess.h"
//#include "assimp/scene.h"
//#include "shapes.h"
#include "gui/window.h"
#include "vertex.h"
#include "graphics/shaders.h"

namespace glpp {

	using VertexIndexPair = std::pair<std::vector<Vertex>, std::vector<Index>>;

	/** The Mesh class contains all data on the CPU to be sent to the GPU.
    * 	Vertices, Indices, Textures and Transformations.
    * 	Meshes can be created from a file or from builtin classes
    */
	class Mesh {
		unsigned int VBO, VAO, EBO;

	  public:
		std::vector<Vertex>  vertices;
		std::vector<Index>   indices;
		std::vector<Texture> textures;
		ml::mat4             transformation;

		ml::vec3 center();

		size_t indicesSize() {
			return indices.size();
		}
		void setTexture(const Texture& texture) {
			textures.push_back(texture);
		}
		Mesh(std::vector<Vertex> vertices, std::vector<Index> indices, std::vector<Texture> textures) {
			this->vertices = std::move(vertices);
			this->indices  = std::move(indices);
			this->textures = std::move(textures);
			genBuffers();
		}
		Mesh(std::vector<Vertex> vertices, std::vector<Index> indices) {
			this->vertices = std::move(vertices);
			this->indices  = std::move(indices);
			genBuffers();
		}
		Mesh(VertexIndexPair pair) {
			vertices = std::move(pair.first);
			indices  = std::move(pair.second);
			genBuffers();
		}
		Mesh(const std::filesystem::path& path) {
			loadFromFile(path);
			genBuffers();
		}
		~Mesh() {
			std::cout << "Calling Mesh destructor" << std::endl;
//			glASSERT(glDeleteVertexArrays(1, &VAO));
//			glASSERT(glDeleteBuffers(1, &VBO));
//			glASSERT(glDeleteBuffers(1, &EBO));
		}
		void bind() {
			glASSERT(glBindVertexArray(VAO));
		}

		void draw(Shader& shader);

	  private:
		void genBuffers();
		void loadFromFile(const std::filesystem::path& path) {
		}
	};

	// TODO: figure out vertex normals, and remove this
	class VertexMesh {
		unsigned int VBO, VAO;

	  public:
		std::vector<Vertex>  vertices;
		std::vector<Texture> textures;
		ml::mat4             transformation;

		void setTexture(const Texture& texture) {
			textures.push_back(texture);
		}

		VertexMesh(std::vector<Vertex> vertices_) {
			vertices = std::move(vertices_);
			genBuffers();
		}
		VertexMesh(VertexIndexPair pair) {
			vertices = std::move(pair.first);
			genBuffers();
		}
		VertexMesh(const std::filesystem::path& path) {
			loadFromFile(path);
			genBuffers();
		}
		~VertexMesh() {
			glASSERT(glDeleteVertexArrays(1, &VAO));
			glASSERT(glDeleteBuffers(1, &VBO));
		}
		void bind() {
			glASSERT(glBindVertexArray(VAO));
		}
		void draw(Shader& shader) {
		}

	  private:
		void genBuffers();

		void loadFromFile(const std::filesystem::path& path) {
		}
	};

	void Mesh::genBuffers() {
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);
		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Index) * indices.size(), indices.data(), GL_STATIC_DRAW);

		// positions
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offset_of(&Vertex::positions));
		glEnableVertexAttribArray(0);
		// color attribute
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offset_of(&Vertex::color));
		glEnableVertexAttribArray(1);
		// texture coord attribute
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offset_of(&Vertex::texCoords));
		glEnableVertexAttribArray(2);
		// texCoords
		glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offset_of(&Vertex::textureSlot));
		glEnableVertexAttribArray(3);
		// Normals
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offset_of(&Vertex::normals));
		glEnableVertexAttribArray(4);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	ml::vec3 Mesh::center() {
		//TODO: Calculate center
		//		ml::vec3 result;
		//		for(auto& vertex : vertices){
		//			result *= vertex.positions;
		//		}
	}

	void Mesh::draw(Shader& shader) {
		unsigned int diffuseNr  = 1;
		unsigned int specularNr = 1;
//		for(unsigned int i = 0; i < textures.size(); i++) {
//			glActiveTexture(GL_TEXTURE0 + i); // activate proper texture unit before binding
//			// retrieve texture number (the N in diffuse_textureN)
//			std::string number;
//			TextureType type = textures[i].type;
//			std::string name;
//			if(type == TextureType::Diffuse) {
//				number = std::to_string(diffuseNr++);
//				name   = "diffuse";
//			} else if(type == TextureType::Specular) {
//				number = std::to_string(specularNr++);
//				name   = "specular";
//			}
//			//		shader.setUniform("material_{}{}"_format(name, number), textures[i].bind());
//		}
//		glActiveTexture(GL_TEXTURE0);

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}

    class Renderer {
      public:
        size_t width, height;
        explicit Renderer() {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        }
        void draw(Mesh& mesh) {
            mesh.bind();
			glASSERT(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0));
        }
        void draw(VertexMesh& mesh) {
            mesh.bind();
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        void render() {
        }
    };


    template <typename T>
    concept RendererDerivative = std::is_base_of<Renderer, T>::value;

	/**
	 * A wrapper class around the Window and Renderer classes
	 * @tparam RenderEngine, a possible Render Engine Backend for the RenderWindow
	 * Requires RenderEngine to be a derivative of Renderer using RendererDerivative Concept
	 * @see Window
	 * @see Renderer
	 * @see RendererDerivate
	 */
    template<RendererDerivative RenderEngine = Renderer>
    class RenderWindow: public Window, public RenderEngine {
      public:
		/**
		 * @tparam VarArgs Variable arguments for the templated RenderEngine base class
		 * @param width
		 * @param height
		 * @param label
		 * @param args
		 */
		template<typename... VarArgs>
        RenderWindow(size_t width, size_t height, const std::string& label, VarArgs... args):
            Window(width, height, label), RenderEngine(args...) {
		}

        Window& window(){
			return this;
		}

		RenderEngine& renderer(){
			return this;
		}
    };
}

#endif //MESH_H
