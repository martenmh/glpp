//
// Created by marten on 06-02-20.
//

#ifndef LIBRARY_SHADERS_H
#define LIBRARY_SHADERS_H

#include "types.h"
#include "utils/include/format.h"

#include <filesystem>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>
//#include "debug.h"
#include "types.h"
#include "utils/include/tokenreader.h"

namespace glpp {
	/**
 * ShaderData structure, contains all necessary data for each shader
 */
	struct ShaderData {
		GLuint      ID = 0; // todo make ID private
		std::string source;

		GLushort type;
		enum class SourceType {
			Path,
			Code
		} sourceType;

		ShaderData(const std::string& shaderSource, ShaderType shaderType):
			source(shaderSource),
			type(static_cast<GLushort>(shaderType)),
			sourceType{shaderSource.ends_with("glsl") ? SourceType::Path : SourceType::Code} {}
	};

	class Shader {
		std::vector<ShaderData> m_shaders;
		// Uniform info contains the OpenGL type ID and the id for OpenGL
		struct UniformInfo {
			unsigned short opengl_type;
			int            id;
		};
		// Maps all uniform variable names to a opengl type
		std::unordered_map<std::string, UniformInfo> m_uniforms;
		GLuint                                       m_rendererID = 0;
		bool                                         m_compiled   = false;

	  protected:
		void compileShader(const std::string& shader, GLuint& shaderID);
		void checkShaderStatus(GLuint& shaderID, GLushort status, GLushort shaderType);
		void checkCompileErrors(GLuint shader, std::string type);
		int  getUniformLocation(const std::string& name) const;
		/* Parse Uniforms from source code to m_uniforms */
		void parseUniforms();

	  public:
		Shader() = default;
		explicit Shader(std::filesystem::path directory);
		explicit Shader(const std::vector<ShaderData>& shaders);
		~Shader();

		GLuint&     getID() { return m_rendererID; }
		ShaderData& getShader(ShaderType shaderType);
		bool        shaderExists(ShaderType shaderType);

		void addShader(ShaderData&& shader);
		template<typename... T>
		void addShader(T... args) { addShader(ShaderData{args...}); }

		void use() const;
        void bind() const;
		void unbind() const;

		/* Set the value of a uniform found in the source code */
		template<typename T>
		inline void setUniform(const std::string& name, T value) {
			if(m_uniforms.find(name) == m_uniforms.end()) {
				std::cout << fout("Uniform: '" + name + "' not found in m_shaders.").set_fcolor(red) << std::endl;
				return;
			}
			Type typeInfo = getTypes<T>();
			//        if(typeInfo.opengl_type != m_uniforms.at(name).opengl_type) {
			//            throw std::runtime_error("Value of type: {} does not fit Uniform of type: {}"_format(
			//                typeid(T).name(), getTypeNameFromType<T>()));
			//        }
			setUniformValue(m_uniforms.at(name).id, value);
		}
	};

	/** Shader class implementations: **/
	Shader::Shader(std::filesystem::path directory) {
		m_rendererID = glCreateProgram();

		for(auto& shaderFile: std::filesystem::directory_iterator(directory)) {
			const std::filesystem::path& filePath = shaderFile.path();
			std::string                  fileName = filePath.filename();
			auto                         words    = split(fileName, ".");
			for(std::string_view& word: words) {
				auto search = fileExtToType.find(std::string(word));
				if(search != fileExtToType.end())
					addShader(ShaderData(filePath, search->second));
			}
		}
		glLinkProgram(m_rendererID);
		parseUniforms();
		m_compiled = true;
	}
	Shader::Shader(const std::vector<ShaderData>& shaders) {
		m_rendererID = glCreateProgram();

		for(auto& shader: shaders) {
			addShader(shader);
		}
		glLinkProgram(m_rendererID);
		m_shaders.insert(m_shaders.end(), shaders.begin(), shaders.end());
		parseUniforms();
		m_compiled = true;
	}

	void Shader::compileShader(const std::string& shader, GLuint& shaderID) {
		const char* shaderStr = shader.c_str();
		glShaderSource(shaderID, 1, &shaderStr, NULL);
		glCompileShader(shaderID);
	}

	void Shader::addShader(ShaderData&& shader) {
		auto shaderTypePos = std::find_if(m_shaders.begin(), m_shaders.end(), [&shader](const ShaderData& elem) -> bool {
			return elem.type == shader.type;
		});
		ASSERT(shaderTypePos == m_shaders.end(),
			   "Duplicate shader types, shader with type of ShaderType::{} already exists in current shader program."_format(shader.type));

		shader.ID = glCreateShader(shader.type);
		std::string shaderSource;
		try {
			// TODO: optimize for ShaderData::SourceType::Code
			if(shader.sourceType == ShaderData::SourceType::Path) {
				shaderSource = fileToStr(shader.source);
			} else {
				shaderSource = shader.source;
			}
			compileShader(shaderSource, shader.ID);
			checkShaderStatus(shader.ID, GL_COMPILE_STATUS, shader.type);
		} catch(std::ifstream::failure& e) {
			std::cout << "ERROR::SHADER:FILE_NOT_SUCCESSFULLY_READ" << std::endl;
		}
		glAttachShader(m_rendererID, shader.ID);
		m_shaders.emplace_back(std::move(shader));
	}

	void Shader::checkShaderStatus(GLuint& shaderID, unsigned short status, unsigned short shaderType) {
		GLint result = GL_FALSE;

		// Check Shader
		glGetShaderiv(shaderID, status, &result);

		if(result == GL_FALSE) {
			int length;
			glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &length);
			char* message = (char*)alloca(length * sizeof(char));
			glGetShaderInfoLog(shaderID, length, NULL, message);
			// TODO: throw custom exception
			std::cout << "ERROR::" << (shaderType == GL_VERTEX_SHADER ? "VERTEX_SHADER" : "FRAGMENT_SHADER")
					  << "::COMPILATION_FAILED " << message << std::endl;
		}
	}
	void Shader::use() const {
		glUseProgram(m_rendererID);
	}
    void Shader::bind() const {
        glUseProgram(m_rendererID);
    }
	void Shader::unbind() const {
		glUseProgram(0);
	}

	void Shader::parseUniforms() {
		std::string source;
		for(auto& shader: m_shaders) {
			source += shader.sourceType == ShaderData::SourceType::Path ?
                          fileToStr(shader.source) :
                          shader.source;
		}

		TokenReader reader(source);
		reader.addIgnoreLine("//");
		reader.addIgnoreUntil("/*", "*/");
		while(reader.gotoNext("uniform") != std::string::npos) {
			std::string typeName = reader++;
			std::string varName  = reader++;
			// Skip the uniform if it's only used a single time
			//		if(reader.getCount(varName) == 1)
			//			continue;
			// TODO: clean the varName for arrays and set the type to array

			// erase character if it is a semicolon (as glsl uniform expressions end with 'foo;')
			varName.erase(std::remove_if(varName.begin(), varName.end(), [](char c) { return c == ';'; }), varName.end());

			// Ignore the uniform if it's only used a single time
			reader++;
			if(reader.findNext(varName) == std::string::npos) {
				continue;
			}
			m_uniforms[varName] = {getGLTypeFromStr(typeName.c_str()), getUniformLocation(varName)};
		}
	}

	int Shader::getUniformLocation(const std::string& name) const {
		int location = glGetUniformLocation(m_rendererID, name.c_str());
		if(location == -1) {
			std::cerr << "Uniform: '" << name << "' doesn't exist" << std::endl;
		}
		return location;
	}

	Shader::~Shader() {
		for(auto& shader: m_shaders) {
			glDetachShader(m_rendererID, shader.ID);
			glDeleteShader(shader.ID);
		}
		glDeleteProgram(m_rendererID);
	}

	ShaderData& Shader::getShader(ShaderType shaderType) {
		return *std::find_if(m_shaders.begin(), m_shaders.end(), [=](ShaderData type) { return type.type == static_cast<GLushort>(shaderType); });
	}
	bool Shader::shaderExists(ShaderType shaderType) {
		return std::find_if(m_shaders.begin(), m_shaders.end(), [=](ShaderData type) { return type.type == static_cast<GLushort>(shaderType); }) != m_shaders.end();
	}

	void Shader::checkCompileErrors(GLuint shader, std::string type) {
		GLint  success;
		GLchar infoLog[1024];
		if(type != "PROGRAM") {
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
			if(!success) {
				glGetShaderInfoLog(shader, 1024, NULL, infoLog);
				std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n"
						  << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
			}
		} else {
			glGetProgramiv(shader, GL_LINK_STATUS, &success);
			if(!success) {
				glGetProgramInfoLog(shader, 1024, NULL, infoLog);
				std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n"
						  << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
			}
		}
	}
}

#endif //LIBRARY_SHADERS_H
