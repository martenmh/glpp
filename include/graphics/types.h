#ifndef TYPES_H
#define TYPES_H

#include "ml/ml.h"
#include "texture.h"

#include <map>
#include <tuple>
#include <vector>

namespace glpp {
	/** Conversion between the 3 types used in the application:
 *      C++ types used to call functions and write to the OpenGL API, eg. int
 *      OpenGL API types (defined in "GL/glew.h"), eg. GL_INT
 *      glsl shader types parsed by the Shader class
 ** Furthermore, each type is mapped with some useful info:
 *      size of the type in bytes
 *      unsigned char for if the type is normalized
 **/
	struct Type {
		const std::type_info& cpp_type;
		const unsigned short  opengl_type;
		const char*           glsl_type;
	};
	struct GLTypeInfo {
		const size_t        size;
		const unsigned char is_normalized;
	};
	enum class ShaderType : unsigned short {
		Vertex                 = GL_VERTEX_SHADER,
		Fragment               = GL_FRAGMENT_SHADER,
		Geometry               = GL_GEOMETRY_SHADER,
		Compute                = GL_COMPUTE_SHADER,
		TessellationControl    = GL_TESS_CONTROL_SHADER,
		TessellationEvaluation = GL_TESS_EVALUATION_SHADER
	};

// DRY element creation
#define GEN_TYPEMAP1(type, gl_type)                              GEN_TYPEMAP2(type, gl_type, #type)
#define GEN_TYPEMAP2(type, gl_type, glsl_type)                   GEN_TYPEMAP3(type, gl_type, glsl_type, sizeof(type))
#define GEN_TYPEMAP3(type, gl_type, glsl_type, size)             GEN_TYPEMAP4(type, gl_type, glsl_type, size, true)
#define GEN_TYPEMAP4(type, gl_type, glsl_type, size, normalized) GLTypeMap({Type{typeid(type), gl_type, glsl_type}, GLTypeInfo{size, normalized}})

#define SPREAD2(x) x[0], x[1]
#define SPREAD3(x) x[0], x[1], x[2]
#define SPREAD4(x) x[0], x[1], x[2], x[3]

// Function factory
#define GEN_SET_UNIFORM_VALUE(type, uniformFunc, ...) \
template<>\
    inline void setUniformValue<type>(int location, type& value) {\
    uniformFunc(location, __VA_ARGS__);\
}

	template<typename T>
	inline void setUniformValue(int location, T& value) {}
	/* Create all template specializations */
	GEN_SET_UNIFORM_VALUE(int, glUniform1i, value)
	GEN_SET_UNIFORM_VALUE(float, glUniform1f, value)
	GEN_SET_UNIFORM_VALUE(double, glUniform1d, value)
	GEN_SET_UNIFORM_VALUE(Texture, glUniform1i, value.bind())
	GEN_SET_UNIFORM_VALUE(ml::vec4, glUniform4f, SPREAD4(value))
	GEN_SET_UNIFORM_VALUE(ml::vec3, glUniform3f, SPREAD3(value))
	GEN_SET_UNIFORM_VALUE(ml::vec2, glUniform2f, SPREAD2(value))
	GEN_SET_UNIFORM_VALUE(ml::mat4, glUniformMatrix4fv, 1, GL_TRUE, value.data())

	using GLTypeMap = std::pair<Type, GLTypeInfo>;
	/** Maps supported types to GLTypeInfo **/
	constexpr std::array<GLTypeMap, 14> typeConversionTable = {
		GEN_TYPEMAP1(bool, GL_BOOL),
		GEN_TYPEMAP2(unsigned char, GL_UNSIGNED_BYTE, "ubyte"),
		GEN_TYPEMAP1(short, GL_SHORT),
		GEN_TYPEMAP2(unsigned short, GL_UNSIGNED_SHORT, "ushort"),
		GEN_TYPEMAP1(int, GL_INT),
		GEN_TYPEMAP2(unsigned int, GL_UNSIGNED_INT, "uint"),
		GEN_TYPEMAP1(float, GL_FLOAT),
		GEN_TYPEMAP1(double, GL_DOUBLE),
		GEN_TYPEMAP3(ml::vec4, GL_FLOAT_VEC4, "vec4", 4 * 4),
		GEN_TYPEMAP3(ml::vec3, GL_FLOAT_VEC3, "vec3", 3 * 4),
		GEN_TYPEMAP3(ml::vec2, GL_FLOAT_VEC2, "vec2", 2 * 4),
		GEN_TYPEMAP3(ml::mat4, GL_FLOAT_MAT4, "mat4", 4 * 4 * 4),
		GEN_TYPEMAP3(Texture, GL_SAMPLER_2D, "sampler2D", 4),
		GEN_TYPEMAP3(float, GL_SAMPLER_2D, "sampler2D", 4)};

	const static std::unordered_map<std::string_view, ShaderType> fileExtToType = {
		{{"vs", ShaderType::Vertex},
		 {"fs", ShaderType::Fragment},
		 {"tes", ShaderType::TessellationEvaluation},
		 {"tcs", ShaderType::TessellationControl},
		 {"gs", ShaderType::Geometry},
		 {"cs", ShaderType::Compute}}};

#undef GEN_TYPEMAP1
#undef GEN_TYPEMAP2
#undef GEN_TYPEMAP3
#undef GEN_TYPEMAP4

	//#undef SPREAD2
	//#undef SPREAD3
	//#undef SPREAD4

	template<typename T>
	constexpr Type getTypes() {
		for(auto elem: typeConversionTable)
			if(elem.first.cpp_type == typeid(T))
				return elem.first;
	}

	/* Converts C++ type to GLTypeInfo of the equivalent type */
	template<typename T>
	constexpr GLTypeInfo getGLTypeInfo() {
		for(auto elem: typeConversionTable)
			if(elem.first.cpp_type == typeid(T))
				return elem.second;
	}

	/* Converts glsl type string to GLTypeInfo of the equivalent type */
	constexpr GLTypeInfo getGLTypeInfo(const char* glsl_str) {
		for(auto elem: typeConversionTable)
			if(elem.first.glsl_type == glsl_str)
				return elem.second;
	}

	/* Converts OpenGL type to GLTypeInfo of the equivalent type */
	constexpr GLTypeInfo getGLTypeInfo(unsigned short glVal) {
		for(auto elem: typeConversionTable)
			if(elem.first.opengl_type == glVal)
				return elem.second;
	}

	template<typename T>
	constexpr unsigned int getGLTypeID() {
		for(auto elem: typeConversionTable)
			if(elem.first.cpp_type == typeid(T))
				return elem.first.opengl_type;
	}
	template<typename T>
	constexpr size_t getGLTypeSize() {
		return getGLTypeInfo<T>();
	}
	template<typename T>
	constexpr bool isGLTypeNormalized() {
		return getGLTypeInfo<T>();
	}
	constexpr const std::type_info& getTypeFromStr(const char* str) {
		for(auto elem: typeConversionTable)
			if(elem.first.glsl_type == str)
				return elem.first.cpp_type;
	}
	constexpr unsigned short getGLTypeFromStr(const char* str) {
		for(auto elem: typeConversionTable)
			if(elem.first.glsl_type == str)
				return elem.first.opengl_type;
	}
	constexpr const std::type_info& getTypeFromGLType(unsigned short typeID) {
		for(auto elem: typeConversionTable)
			if(elem.first.opengl_type == typeID)
				return elem.first.cpp_type;
	}
	template<typename T>
	constexpr const char* getTypeNameFromType() {
		for(auto elem: typeConversionTable)
			if(elem.first.cpp_type == typeid(T))
				return elem.first.glsl_type;
	}
}

#endif //TYPES_H
