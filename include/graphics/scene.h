#ifndef ENGINE_SCENE_H
#define ENGINE_SCENE_H

#include "camera.h"
#include "mesh.h"

namespace glpp {
	class Scene {
		Camera&           m_camera;
		std::vector<Mesh> m_meshes;
	};
}

#endif //ENGINE_SCENE_H
