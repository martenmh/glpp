#include "glm/glm.hpp"
#include "graphics/camera.h"
#include "graphics/mesh.h"
#include "graphics/shaders.h"
#include "graphics/shapes.h"
#include "graphics/font.h"

#include "gui/window.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"


#include <iostream>
#include <list>
#include <random>
//template<typename T>
//concept bool is_vector =
//	requires(T t) {
//	t.length();
//	t.operator[];
//};

//template<typename T, typename M, size_t N>
//concept bool is_vector3 =
//	requires(T t) {
//	    is_number<M>() &&
//		N == 3 &&
//		(t.length() || t.size()) &&
//		t.operator[];
//};

// To abstract the vector implementation away the
//template<typename T>
//concept Vector3 =
//
//void f(Vector auto vec){
//
//}

//void f(is_vector3 auto vec) {
//}
using namespace glpp;

Mesh plane(const ml::vec3& pos, const ml::vec2& size, rgba color, float textureID = 0.0f) {
	return Mesh({{{pos.x(), pos.y(), pos.z()}, color, {}, {0.0f, 0.0f}, textureID},
				 {{pos.x() + size.x(), pos.y(), pos.z()}, color, {}, {1.0f, 0.0f}, textureID},
				 {{pos.x(), pos.y() + size.y(), pos.z()}, color, {}, {0.0f, 1.0f}, textureID},
				 {{pos.x() + size.x(), pos.y() + size.y(), pos.z()}, color, {}, {1.0f, 1.0f}, textureID}},
				{0, 1, 2, 2, 1, 3});
}

Mesh createCube(const ml::vec3& pos, const ml::vec3& size, rgba color) {
	return Mesh({{{pos.x(), pos.y(), pos.z()}, color, {}, {0.0f, 0.0f}},
				 {{pos.x() + size.x(), pos.y(), pos.z()}, color, {}, {1.0f, 0.0f}},
				 {{pos.x() + size.x(), pos.y() + size.y(), pos.z()}, color, {}, {1.0f, 1.0f}},
				 {{pos.x(), pos.y() + size.y(), pos.z()}, color, {}, {0.0f, 1.0f}},
				 {{pos.x(), pos.y(), pos.z() + size.z()}, color, {}, {0.0f, 1.0f}},
				 {{pos.x() + size.x(), pos.y(), pos.z() + size.z()}, color, {}, {1.0f, 1.0f}},
				 {{pos.x() + size.x(), pos.y() + size.y(), pos.z() + size.z()}, color, {}, {1.0f, 0.0f}},
				 {{pos.x(), pos.y() + size.y(), pos.z() + size.z()}, color, {}, {0.0f, 0.0f}}},
				{{
					0, 1, 3, 1, 2, 3, // Back Quad
					0, 1, 4, 1, 5, 4, // Right Quad
					2, 3, 7, 2, 6, 7, // Left Quad
					4, 5, 7, 5, 6, 7, // Front Quad
					0, 3, 4, 3, 4, 7, // Top Quad
					1, 2, 5, 2, 5, 6  // Bottom Quad
				}});
}
Mesh createUVSphere(const ml::vec3& center, float radius, int stacks, int segments, int texSlot = 0) {
	rgba                col{1.0f, 1.0f, 1.0f, 1.0f};
	std::vector<Vertex> vertices;
	std::vector<Index>  indices;

	const float sectorStep = 2 * M_PI / segments;
	const float stackStep  = M_PI / stacks;
	const float invR       = 1.0f / radius;
	for(int stack = 0; stack <= stacks; stack++) {
		const float phi = M_PI / 2 - stack * stackStep;
		const float xy  = radius * cosf(phi); // r * cos(u)
		const float z   = radius * sinf(phi); // r * sin(u)

		int k1 = stack * (segments + 1); // beginning of current stack
		int k2 = k1 + segments + 1;      // beginning of next stack

		// the first and last vertices have same position and normal, but different tex coords
		for(int segment = 0; segment <= segments; segment++, k1++, k2++) {
			const float theta = segment * sectorStep;

			const float x = xy * cosf(theta); // r * cos(u) * cos(v)
			const float y = xy * sinf(theta); // r * cos(u) * sin(v)
			vertices.push_back({{center.x() + x, center.y() + y, center.z() + z},
								col,
								{(short)(x * invR), (short)(y * invR), (short)(z * invR)},
								{static_cast<float>(segment) / static_cast<float>(segments), static_cast<float>(stack) / static_cast<float>(stacks)},
								(float)texSlot});

			if(stack != 0) {
				int in[] = {k1, k2, k1 + 1};
				indices.insert(indices.end(), std::begin(in), std::end(in));
			}
			if(stack != (stacks - 1)) {
				int in[] = {k1 + 1, k2, k2 + 1};
				indices.insert(indices.end(), std::begin(in), std::end(in));
			}
		}
	}
	return Mesh(vertices, indices);
}


void insertion_sort(std::vector<int>& array, int i = 1) {
	int key, j;
    key = array[i];
    j   = i;
    while(j > 0 && array[j - 1] > key) {
        array[j] = array[j - 1];
        j--;
    }
    array[j] = key;

}

class Renderer3D: public glpp::Renderer {
  public:
	enum class Shade {
		Smooth,
		Flat
	};

	void setShade(Shade shade) {
		if(shade == Shade::Smooth) {
			//GL_POLYGON_SMOOTH
			glEnable(GL_LINE_SMOOTH);
		} else {
			glDisable(GL_LINE_SMOOTH);
		}
	}
	Renderer3D() {
		glEnable(GL_DEPTH_TEST);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
};

class SortingRenderer: public glpp::Renderer {
	std::vector<glpp::Mesh> m_meshes;
	float                   m_height, m_width;

  public:
	explicit SortingRenderer(size_t height, size_t width, const std::vector<int>& initialArr):
		m_height(height), m_width(width) {
		float horizontalPosition = -2.5f;
		float elementWidth       = m_width / initialArr.size();
		float maxHeight          = *std::max_element(initialArr.begin(), initialArr.end());
		//        m_meshes.push_back(plane({-2.5f, -2.5f, 0.0f}, {5.0f, 5.0f}, {1.0f, 0.0f, 0.0f}));
		for(auto value: initialArr) {
			float elementHeight = m_height * (value / maxHeight);
			m_meshes.push_back(plane({horizontalPosition, -2.5f, 0.0f}, {elementWidth, elementHeight}, {1.0f, 0.0f, 0.0f}));
			horizontalPosition += elementWidth;
		}
	}

	void draw(const std::vector<int>& arr) {
		//		if(arr.size() != m_meshes.size()) {
		//		}
		int i = 0;
		m_meshes.clear();
        float horizontalPosition = -2.5f;
        float elementWidth       = m_width / arr.size();
        float maxHeight          = *std::max_element(arr.begin(), arr.end());
        //        m_meshes.push_back(plane({-2.5f, -2.5f, 0.0f}, {5.0f, 5.0f}, {1.0f, 0.0f, 0.0f}));
		int j = 0;
        auto defaultColor = rgba{1.0f, 1.0f, 1.0f, 1.0f};
        auto selectedColor = rgba{1.0f, 0.0f, 0.0f, 1.0f};
        for(auto value: arr) {
			j++;
            float elementHeight = m_height * (value / maxHeight);
//			m_meshes[i] = plane({horizontalPosition, -2.5f, 0.0f}, {elementWidth, elementHeight}, {1.0f, 0.0f, 0.0f});
            m_meshes.push_back(plane({horizontalPosition, -2.5f, 0.0f}, {elementWidth, elementHeight}, i == j ? selectedColor : defaultColor));
            horizontalPosition += elementWidth;
        }

		for(Mesh& mesh: m_meshes) {
			glpp::Renderer::draw(mesh);
		}

		//        Mesh mesh2 = plane({-0.5f,-0.5f,0.0f}, {1.0f, 1.0f}, {1.0f, 0.0f, 0.0f});
		//        glpp::Renderer::draw(mesh2);
	}
};

template<size_t N>
void draw(int (&arr)[N]) {
}

void randomizeArray(std::vector<int>& arr, float passesMultiplier = 1.0f){
    std::default_random_engine         generator(std::chrono::system_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<int> distribution(1, arr.size());
    for(int i = 0; i < arr.size() * passesMultiplier; i++) {
        auto firstIndex  = distribution(generator);
        auto secondIndex = distribution(generator);
        std::iter_swap(arr.begin() + firstIndex, arr.begin() + secondIndex);
    }
}

std::vector<int> createUnsortedArray(size_t n, float passesMultiplier = 1.0f) {
	// Create a sorted array
	std::vector<int> arr(n - 1);
	for(int i = i; i < n; i++)
		arr[i] = i+1;

	// Randomize sorted array
    randomizeArray(arr);
	return arr;
}

int callback(ImGuiInputTextCallbackData* data){
    std::cout << "Writing: " << data->Buf << std::endl;
}

class VectorRenderer : protected Renderer {
	void draw(ml::vec2 vec){

	}
};

int main() {
	// Use namespace glpp in main
	using namespace glpp;
	constexpr const int height = 1080;
	constexpr const int width  = 1920;

	ml::mat4 projection(1), view(1), model(1);

	auto arr = createUnsortedArray(2000, 100);
	for(auto value: arr) {
		std::cout << value << std::endl;
	}
	RenderWindow<SortingRenderer> window(width, height, "Engine", 5.0f, 5.0f, arr);
	Shader                        shader("/home/marten/projects/glpp/resources/default/shaders/mvp/");
	Camera                        camera(100, 100);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window.getGLFWwindow(), true);
    const char* glsl_version = "#version 410";
    ImGui_ImplOpenGL3_Init(glsl_version);
    // Setup Dear ImGui style


	Mesh mesh3 = plane({-0.5f, -0.5f, -0.5f}, {100.0f, 100.0f}, {1.0f, 1.0f, 1.0f});

	camera.back(10);
    int i = 0;
	bool show_demo_window = true;
    float startTime = glfwGetTime();
	while(window.isOpen()) {
		window.pollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();


        window.clear({0.05f, 0.05f, 0.05f, 1.0f});
//        window.clear({0.95f, 0.95f, 0.95f, 1.0f});


        if(window.keyIsPressed(Key::Q) || window.keyIsPressed(Key::ESCAPE))
			window.close();

		shader.bind();
		shader.use();

        // feed inputs to dear imgui, start new frame
//        ImGui_ImplOpenGL3_NewFrame();
//        ImGui_ImplGlfw_NewFrame();
//        ImGui::NewFrame();

		model = ml::scale(0.4f);
		//        model = ml::translate<float>({0.0f, -2.0f, 0.0f});
		//		view       = camera.view();
		//		projection = ml::ortho(0.0f, 10.0f, 0.0f, 10.0f, 0.1f, 100.0f);

		shader.setUniform("mvp", projection * view * model);
		shader.use();
//        std::cout  << "si: " << (glfwGetTime() - startTime) << ", " << startTime << ", " << i << std::endl;
//        std::cout << "si: " << startTime << ", " << i << std::endl;
//        startTime = glfwGetTime();

        insertion_sort(arr, i);
        // render your GUI
//        ImGui::Begin("Demo window");
//        ImGui::Button("Hello!");
//        ImGui::End();

		window.draw(arr);

        ImGui::ShowDemoWindow();
        ImGui::InputTextMultiline("", "Hello", 5, {500, 1000}, ImGuiInputTextFlags_AllowTabInput, callback,nullptr);
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        window.display();
	}
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

	return 0;
}
