#ifndef TEXTURE_H
#define TEXTURE_H

#include "debug.h"
#include <string>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace glpp {
	using Slot = float;

	class Texture;
	namespace TextureManager {
		Slot addTexture(Texture* texture);
		void removeTexture(Texture* texture);
		Slot activate(Texture* texture);
		Slot getSlot(Texture* texture);
		void bindAll();
	}

// Defines the type of the texture
	enum class TextureType {
		Diffuse,
		Specular
	};

	class Texture {
		unsigned int   m_rendererID = 0;
		std::string    m_filePath;
		unsigned char* m_buffer = nullptr;
		int            m_width = 0, m_height = 0, m_bpp = 0;
		Slot           m_slot;

	  public:
		TextureType type;
		// Defines what happens when the texture wraps
		enum class TextureWrap {
			Repeat,          // Repeat the texture
			Repeat_Mirrored, // Repeat the texture mirrored
			Clamp_Edge,      // Stretch to the edge
			Clamp_Border     // Everything Outside the border uses a color
		};

		enum TextureFilter {
			Smooth = GL_LINEAR,
			Pixel  = GL_NEAREST
		};

		// Texture Settings
		bool        useMipmap;
		TextureWrap xWrap, yWrap;

		Texture() = default;
		//	Texture(float id);
		explicit Texture(const std::string& path, bool flip = true, TextureWrap xWrap = TextureWrap::Repeat,
						 TextureWrap yWrap = TextureWrap::Repeat, TextureFilter filter = TextureFilter::Smooth,
						 bool useMipmap = true, TextureFilter mipmapFilter = TextureFilter::Smooth);

		// Delete the copy constructor/assignment
        Texture(const Texture &) = delete;
        Texture &operator=(const Texture&) = delete;

		Texture(Texture&& other)  noexcept : m_rendererID(other.m_rendererID){
            other.m_rendererID = 0;
			xWrap = other.xWrap;
			yWrap = other.yWrap;
            m_slot = other.m_slot;
            type = other.type;
			useMipmap = other.useMipmap;
		}

		void release(){
            glDeleteTextures(1, &m_rendererID);
            m_rendererID = 0;
		}

        Texture& operator=(Texture&& other) {
			if(this != &other) {
				release();
				m_rendererID = other.m_rendererID;
				other.m_rendererID = 0;
				xWrap              = other.xWrap;
				yWrap              = other.yWrap;
				m_slot             = other.m_slot;
				type               = other.type;
				useMipmap          = other.useMipmap;
			}
        }

		~Texture();

		void         resize(int height, int width);
		unsigned int getID() const { return m_rendererID; }

		void         setSlot(unsigned int slot) { m_slot = slot; }
		unsigned int getSlot() { return m_slot; }

		/*
     * Activate the current texture if not already
     * Note: This may deactivate another texture
     */
		inline void activate() { TextureManager::activate(this); }

		// Managed by the TextureManager
		unsigned int bind(unsigned int slot = 0);
		void         unbind() const;

		inline int getWidth() const { return m_width; }
		inline int getHeight() const { return m_width; }
	};

	Texture::Texture(const std::string& path, bool flip, Texture::TextureWrap xWrap, Texture::TextureWrap yWrap,
					 Texture::TextureFilter filter, bool useMipmap, Texture::TextureFilter mipmapFilter):
		m_filePath(path) {
		stbi_set_flip_vertically_on_load(flip);

		glGenTextures(1, &m_rendererID);
		glBindTexture(GL_TEXTURE_2D, m_rendererID);
		// Specify some necessary attributes
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		float borderColor[] = {1.0f, 1.0f, 0.0f, 1.0f};
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
		// Load data on CPU
		m_buffer = stbi_load(path.c_str(), &m_width, &m_height, &m_bpp, 0);

		if(m_buffer) {
			// Send data to GPU
			//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_buffer);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, m_buffer);
			glGenerateMipmap(GL_TEXTURE_2D);
		} else {
			std::cerr << "Failed to load texture: " << path << std::endl;
			return;
		}

		//glBindTexture(GL_TEXTURE_2D, 0);

		// Free data from CPU
		stbi_image_free(m_buffer);
	}

	unsigned int Texture::bind(unsigned int slot) {
		m_slot = slot;
		glActiveTexture(GL_TEXTURE0 + slot); // Specify texture slot
		glBindTexture(GL_TEXTURE_2D, m_rendererID);
		return slot;
	}
	void Texture::unbind() const {
		glActiveTexture(GL_TEXTURE0 + m_slot); // Specify texture slot
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	Texture::~Texture() { release(); }
	void Texture::resize(int height, int width) {
		//stbir_resize_uint8()
	}
}

#endif //TEXTURE_H
