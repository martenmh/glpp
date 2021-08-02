#ifndef ENGINE_COMMON_H
#define ENGINE_COMMON_H

// Include GLEW. Always include it before gl.h and glfw3.h, since it's a bit magic.
#include "glad/glad.h"
// Include GLFW
#include <GLFW/glfw3.h>

namespace glpp {
	using Index = GLuint;
}

#endif //ENGINE_COMMON_H
