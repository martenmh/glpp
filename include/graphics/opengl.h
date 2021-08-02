#ifndef OPENGL_H
#define OPENGL_H

#include "shaders.h"
#include "texture.h"

namespace glpp {
	namespace RendererSettings {
		inline const uint32_t maxVertexCount = 1024;

		inline const uint32_t maxIndexCount = maxVertexCount;
		inline Shader         defaultShader;
		inline Texture        defaultTexture;
	}

/*
 * Functions related to the TextureManager
 */
	namespace TextureManager {
		inline const uint32_t                        maxTextureSlots = GL_MAX_TEXTURE_IMAGE_UNITS;
		static std::array<Texture*, maxTextureSlots> textures;
		static uint32_t                              usedTextures = 0; // texture 0 is reserved for colors
		static std::vector<Texture*>                 unactiveTextures;

		inline Slot getSlot(Texture* texture) {
			auto it = std::find(textures.begin(), textures.end(), texture);
			if(it == textures.end()) {
				return 0;
			}
			return std::distance(textures.begin(), it) + 1;
		}
		/*
	 * Add a texture to the textures array or if it's full to the unactiveTextures
	 */
		Slot addTexture(Texture* texture);
		/*
	 * Remove a texture to textures or unactiveTextures,
	 * if a texture is removed from the textures array and the unactiveTextures is not empty
	 * the first element from the unactiveTextures will be inserted into the textures array and activated
	 */
		void removeTexture(Texture* texture);
		/* Activates a texture from the unactiveTextures,
	 * unactivates the last texture in the textures array
	 * and puts it in the unactiveTextures
	 */
		Slot activate(Texture* texture);
		/* Binds all textures for Rendering
	 */
		void bindAll();
	}

inline void initGL() {
		//	glewExperimental = true; // Needed in core profile
		//	if(glewInit() != GLEW_OK) {
		//		throw std::runtime_error("Failed to initialize GLEW\n");
		//	}
		// glad: load all OpenGL function pointers
		// ---------------------------------------
		if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
			std::cout << "Failed to initialize GLAD" << std::endl;
			//return -1;
		}
		// Initialize default settings
		RendererSettings::defaultShader  = Shader("../../res/default/m_shaders");
		RendererSettings::defaultTexture = Texture("../../res/default/textures/grid.png");
	}
}

#endif //OPENGL_H
