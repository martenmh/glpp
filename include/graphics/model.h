#ifndef ENGINE_MODEL_H
#define ENGINE_MODEL_H

//#include "utils/logging.h"

#include "graphics/mesh.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <stb_image.h>
#include <string>
#include <vector>

namespace glpp {
	class Model {
		// model data
		std::vector<Mesh> meshes;
		std::string       directory;

		void                 loadModel(const std::string& path);
		void                 processNode(aiNode* node, const aiScene* scene);
		std::vector<Vertex>  processVertices(aiMesh* mesh);
		std::vector<Index>   processIndices(aiMesh* mesh);
		Mesh                 processMesh(aiMesh* mesh, const aiScene* scene);
		std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, TextureType textureType);

	  public:
		explicit Model(const std::string& path) {
			loadModel(path);
		}
		inline void draw(Shader& shader) {
			for(auto& mesh: meshes)
				mesh.draw(shader);
		}
	};

	void Model::loadModel(const std::string& path) {
		Assimp::Importer import;
		const aiScene*   scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
		if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
			logging::error(format("ERROR::ASSIMP::{}", import.GetErrorString()));
			return;
		}
		directory = path.substr(0, path.find_last_of('/'));
		processNode(scene->mRootNode, scene);
	}
	void Model::processNode(aiNode* node, const aiScene* scene) {
		for(unsigned int i = 0; i < node->mNumMeshes; i++) {
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			meshes.push_back(processMesh(mesh, scene));
		}
		for(unsigned int i = 0; i < node->mNumChildren; i++) {
			processNode(node->mChildren[i], scene);
		}
	}
	std::vector<Vertex> Model::processVertices(aiMesh* mesh) {
		std::vector<Vertex> vertices;
		// Fill vertices
		for(unsigned int i = 0; i < mesh->mNumVertices; i++) {
			Vertex vertex;
			auto&  positions = mesh->mVertices[i];
			auto&  normals   = mesh->mNormals[i];
			auto&  texCoords = mesh->mTextureCoords[0][i];

			vertex.positions = {positions.x, positions.y, positions.z};
			vertex.normals   = {static_cast<short>(normals.x),
                              static_cast<short>(normals.y),
                              static_cast<short>(normals.z)};
			if(mesh->mTextureCoords[0]) {
				vertex.texCoords = {texCoords.x, texCoords.y};
			}
			vertices.push_back(vertex);
		}
		return vertices;
	}
	std::vector<Index> Model::processIndices(aiMesh* mesh) {
		std::vector<Index> indices;
		for(unsigned int i = 0; i < mesh->mNumFaces; i++) {
			aiFace face = mesh->mFaces[i];
			for(unsigned int j = 00; j < face.mNumIndices; j++) {
				indices.push_back(face.mIndices[j]);
			}
		}
		return indices;
	}
	Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene) {
		std::vector<Vertex>  vertices = processVertices(mesh);
		std::vector<Index>   indices  = processIndices(mesh);
		std::vector<Texture> textures;
		if(mesh->mMaterialIndex >= 0) {
			aiMaterial*          material     = scene->mMaterials[mesh->mMaterialIndex];
			std::vector<Texture> diffuseMaps  = loadMaterialTextures(material, aiTextureType_DIFFUSE, TextureType::Diffuse);
			std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, TextureType::Specular);
			textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
			textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
		}

		return Mesh(vertices, indices, textures);
	}

	std::vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, TextureType textureType) {
		std::vector<Texture> textures;
		for(unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
			aiString str;
			mat->GetTexture(type, i, &str);
			//			Texture texture(directory + str.C_Str());
			//			texture.type = textureType;
			//			textures.push_back(texture);
		}
		return textures;
	}
}

#endif //ENGINE_MODEL_H
