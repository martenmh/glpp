#ifndef TEXT_H
#define TEXT_H

#include "font.h"
#include "shapes.h"

#include <utility>

namespace glpp {
std::array<Vertex, 6> createText(float xpos, float ypos, float h, float w, rgba color, float textureID) {
	Vertex v0({xpos, ypos + h}, color, {}, {0.0f, 0.0f}, textureID);
	Vertex v1({xpos, ypos}, color, {}, {0.0f, 1.0f}, textureID);
	Vertex v2({xpos + w, ypos}, color, {}, {1.0f, 1.0f}, textureID);
	Vertex v3({xpos, ypos + h}, color, {}, {0.0f, 0.0f}, textureID);
	Vertex v4({xpos + w, ypos}, color, {}, {1.0f, 1.0f}, textureID);
	Vertex v5({xpos + w, ypos + h}, color, {}, {1.0f, 0.0f}, textureID);
	return {{v0, v1, v2, v3, v4, v5}};
}

struct Text: public Drawable {
	Font                font;
	std::string         text;
	float               x, y;
	float               height, width, scale;
	rgba                color;
	std::vector<Vertex> vertices;
	std::vector<Index>  indices;

  public:
	Text(std::string text, const std::filesystem::path& fontPath, rgba color, float x, float y, float scale):
		font(fontPath, scale, 0), text(std::move(text)), x(x), y(y), color(color), scale(scale) {
		// iterate through all characters
		int n = 0;
		vertices.reserve(6 * text.size());
		std::string::const_iterator c;
		for(c = text.begin(); c != text.end(); c++) {
			Character ch = font.getCharacter(*c);

			float xpos = x + ch.bearing.x() * scale;
			float ypos = y - (ch.size.y() - ch.bearing.y()) * scale;

			float w = ch.size.x() * scale;
			float h = ch.size.y() * scale;

			std::array<Vertex, 6> bla = createText(xpos, ypos, h, w, color, ch.textureID);
			memcpy(&vertices[n], &bla[0], 6);

			x += (ch.offset >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
			n += 6;
		}
	}
//	Vertex& draw(Renderer& renderer) override {}
//	Vertex& draw(Renderer& renderer, const DrawSettings& settings) override {
//		renderer.draw(vertices.data(), vertices.size(), indices.data(), indices.size());
		//		settings.shader->bind();
		//		settings.shader->setUniform("textColor", color);
		//		glActiveTexture(GL_TEXTURE0);
		//		renderer.getVertexArray().bind();
		//
		//		// iterate through all characters
		//		std::string::const_iterator c;
		//		for(c = text.begin(); c != text.end(); c++) {
		//			Character ch = font.getCharacter(*c);
		//
		//			float xpos = x + ch.bearing.x * scale;
		//			float ypos = y - (ch.size.y - ch.bearing.y) * scale;
		//
		//			float w = ch.size.x * scale;
		//			float h = ch.size.y * scale;
		//
		//			std::array<Vertex, 6> vertices = createText(xpos, ypos, h, w, color, ch.textureSlot);
		//			x += (ch.offset >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
		//		}
		//		renderer.getVertexArray().unbind();
		//		glBindTexture(GL_TEXTURE_2D, 0);
	}
};
}

#endif //TEXT_H
