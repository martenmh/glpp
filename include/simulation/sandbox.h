#ifndef ENGINE_SANDBOX_H
#define ENGINE_SANDBOX_H

namespace glpp {
	/**
 * The sandbox class is a 2d or 3d
 */
	class Sandbox {
		size_t   m_height, m_width, m_depth;
		ml::vec3 m_pos;

		Sandbox(size_t height, size_t width, size_t depth = 0) {
		}
	};
}

#endif //ENGINE_SANDBOX_H
