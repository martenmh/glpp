//#ifndef RENDERER_H
//#define RENDERER_H
//
//#include "drawsettings.h"
//
//#include "opengl.h"
//#include "m_shaders.h"
//#include "utils/include/color.h"
//
//#include <queue>
//
//using Index = GLuint;
//class Vertex;
//class Drawable;
//
//enum class PolygonMode {
//	Point,
//	Line,
//	Fill
//};
//
/////**
//// * Batch Renderer
//// */
////class Renderer {
////	PolygonMode m_polyMode;
////	std::vector<Index> indices;
////	std::vector<Vertex> vertices;
////	std::vector<Vertex> m_texVertices;
////
////	struct RenderData {
////		Vertex* vertices;
////		Index* indices;
////		int vertexCount, indexCount;
////		RenderData(Vertex* vertices, int vertexCount, Index* indices, int indexCount):
////			vertices{vertices}, indices{indices}, vertexCount{vertexCount}, indexCount{indexCount}{}
////		bool operator==(const RenderData& rhs) const{
////			return rhs.indices == indices && rhs.vertices == vertices &&
////                rhs.vertexCount == vertexCount && rhs.indexCount == indexCount;
////		}
////	};
////	std::vector<RenderData> m_renderQueue;
////
////	int m_vertexCount = 0, m_indexCount = 0;
////  public:
////	// TODO enable idle mode and don't rewrite the buffers on draw
////	void idle(bool setIdle){}
////	bool isIdle(){}
//////	VertexBuffer& getVertexBuffer(){ return m_vertexBuffer; }
//////	VertexArray& getVertexArray(){ return m_vertexArray; }
//////	VertexBufferLayout& getVertexBuffeLayout(){ return m_layout; }
////
////	Renderer();
////	~Renderer()= default;
////	PolygonMode getPolygonMode(){ return m_polyMode; }
////	void setPolygonMode(PolygonMode polyMode){
////		m_polyMode = polyMode;
////		switch(polyMode){
////		case PolygonMode::Point: glPolygonMode(GL_FRONT_AND_BACK,GL_POINT); break;
////		case PolygonMode::Line: glPolygonMode(GL_FRONT_AND_BACK,GL_LINE); break;
////		case PolygonMode::Fill: glPolygonMode(GL_FRONT_AND_BACK,GL_FILL); break;
////		}
////	}
////
////    //void clear() const;
////
////	//void draw(Cube& drawable, Shader& shader, GLuint* indices, VertexArray& va, VertexBuffer& vb, VertexBufferLayout& layout);
////	void render();
////	void draw(Vertex* vertices, int vertexCount, Index* indices, int indexCount);
////	// Draw a drawable using the default settings
////	void draw(Drawable& drawable);
////	// Draw a drawable with a specific shader program
////	void draw(Drawable& drawable, Shader& shader);
////	// Draw a drawable using a Texture and the default shader
////	void draw(Drawable& drawable, Texture& texture);
////	// Draw a drawable using a Texture and specific shader
////	void draw(Drawable& drawable, Texture& texture, Shader& shader);
////};
//
//#endif //RENDERER_H
