#ifndef FONT_H
#define FONT_H

#include <ft2build.h>
#include FT_FREETYPE_H

#include <filesystem>
#include <map>

namespace glpp {

	struct Character {
		unsigned int  textureID; // ID handle of the glyph texture
		ml::vec2i size;      // Size of glyph
		ml::vec2i bearing;   // Offset from baseline to left/top of glyph
		signed long   offset;    // Offset to advance to the next glyph
	};

	class Font {
		std::map<char, Character> m_characters;
		FT_Library                m_ft;
		FT_Face                   m_face;
		int                       m_faceIndex;

	  public:
		Character& getCharacter(char c) {
			return m_characters.at(c);
		}
		// if width is set to 0 the glyph width will be dynamically calculated
		inline Font(const std::filesystem::path& fontPath, int height, int width = 0) {
			if(FT_Init_FreeType(&m_ft)) {
				std::cerr << "ERROR::FREETYPE: Could not initialize FreeType Library" << std::endl;
			}
			if(FT_New_Face(m_ft, "/usr/share/fonts/liberation/LiberationMono-Regular.ttf", 0, &m_face)) {
				std::cerr << "ERROR::FREETYPE: Failed to load font" << fontPath.c_str() << std::endl;
			}

			glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction

			for(unsigned char c = 0; c < 128; c++) {
				FT_Set_Pixel_Sizes(m_face, width, height);
				// load character glyph
				if(FT_Load_Char(m_face, c, FT_LOAD_RENDER))
					std::cerr << "ERROR::FREETYPE: Failed to load Glyph" << c << std::endl;

				// generate texture
				unsigned int texture;
				glGenTextures(1, &texture);
				glBindTexture(GL_TEXTURE_2D, texture);
				glTexImage2D(
					GL_TEXTURE_2D,
					0,
					GL_RED,
					m_face->glyph->bitmap.width,
					m_face->glyph->bitmap.rows,
					0,
					GL_RED,
					GL_UNSIGNED_BYTE,
					m_face->glyph->bitmap.buffer);
				// set texture options
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				// now store character for later use
				Character character = {
					texture,
					ml::vec2i(m_face->glyph->bitmap.width, m_face->glyph->bitmap.rows),
					ml::vec2i(m_face->glyph->bitmap_left, m_face->glyph->bitmap_top),
					m_face->glyph->advance.x};
				m_characters.insert(std::pair<char, Character>(c, character));
			}

			FT_Done_Face(m_face);
			FT_Done_FreeType(m_ft);
		}
	};

}

#endif //FONT_H
