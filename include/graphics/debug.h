//
// Created by marten on 06-02-20.
//

#ifndef LIBRARY_DEBUG_H
#define LIBRARY_DEBUG_H

#include "common.h"
#include "utils/include/log.h"

#include <cassert>

/************* Debugging *************/
#include <iostream>

namespace glpp {

#define glASSERT(x) \
	glClearError(); \
	x;              \
	assert(glLogCall(#x, __FILE__, __LINE__) == GL_NO_ERROR)

#define ASSERT(condition, message)   \
	do {                             \
		if(!(condition)) {           \
			logging::error("{}:{}: {}"_format(__FILE__, __LINE__, message)); \
			std::terminate();        \
		}                            \
	} while(false)

	// > opengl 4.3
	inline void debugMessageCallback(GLenum source, GLenum type, GLuint m_rendererID, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
		std::cout << "ERROR\nSource: " << source << "\nType: " << type << "\nID: " << m_rendererID << "\nSeverity: " << severity << "\nLength: " << length << "\nMessage: " << message;
	}

	/** Debugging usign glGetError() **/
	/** Clear all errors given by OpenGL **/
	inline static void glClearError() {
		while(glGetError() != GL_NO_ERROR)
			;
	}

	/** Log the OpenGL error **/
	static GLenum glLogCall(const char* function, const char* file, int line) {
		switch(glGetError()) {
		case GL_NO_ERROR:
			return GL_NO_ERROR;
		case GL_INVALID_ENUM:
			logging::error("ERROR: GL Invalid Enum");
			break;
		case GL_INVALID_VALUE:
			logging::error("ERROR: GL Invalid Value");
			break;
		case GL_INVALID_OPERATION:
			logging::error("ERROR: GL Invalid Operation");
			break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			logging::error("ERROR: GL Invalid Framebuffer Operation");
			break;
		case GL_OUT_OF_MEMORY:
			logging::error("ERROR: GL Out Of Memory.");
			break;
		case GL_STACK_UNDERFLOW:
			logging::error("ERROR: GL Stack Underflow.");
			break;
		case GL_STACK_OVERFLOW:
			logging::error("ERROR: GL Stack Overflow.");
			break;
		}
		logging::error(format("Error in {} {}. On line: '{}'", function, file, line));
		return glGetError();
	}

	/************* END Debugging *************/

} // glpp

#endif //LIBRARY_DEBUG_H
