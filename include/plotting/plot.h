//
// Created by marten on 06-09-20.
//

#ifndef PLOT_H
#define PLOT_H

#include <graphics/shapes.h>
#include <graphics/shapetraits.h>

namespace glpp {
	// Virtual class
	class Plottable {
	};

	enum class AngularUnit {
		Radian,
		Degree
	};

	class HeatMap {};
	template<typename T>
	class HeightMap {
		int width, height;
		HeightMap(int height, int width) {}
		std::vector<T> elems;
		void           draw() {
            int elemWidth = width / elems.size();
		}
	};

	rgba Black = {0.0f, 0.0f, 0.0f, 1.0f};
	rgba Red   = {1.0f, 0.0f, 0.0f, 1.0f};
	rgba Green = {0.0f, 1.0f, 0.0f, 1.0f};
	rgba Blue  = {0.0f, 0.0f, 1.0f, 1.0f};
	rgba White = {1.0f, 1.0f, 1.0f, 1.0f};

	template<typename T>
	class Plottable2D: public Drawable2D {
	  protected:
	using Drawable2D::m_vertices;
	using Drawable2D::m_indices;
	ml::vec3<T> m_pos;
		ml::vec2<T> m_size;

	  public:
		Plottable2D(const ml::vec3<T>& position, const ml::vec2<T>& size):
			m_pos{position}, m_size{size} {}
		virtual void plotPoint(const ml::vec2<T>&)                    = 0;
		virtual void plotLine(const ml::vec2<T>&, const ml::vec2<T>&) = 0;
		virtual void plotFunction()                                   = 0;
		// ... etc
	};
	template<typename T>
	class Plottable3D: public Drawable3D {
		virtual void plotPoint(const ml::vec3<T>&)                    = 0;
		virtual void plotLine(const ml::vec3<T>&, const ml::vec3<T>&) = 0;
		virtual void plotFunction()                                   = 0;
		// plotCurve
		// plotPath...
		// ... etc
	};

	template<typename T = float>
	class CartesianPlane: public Plottable2D<T> {
		using Plottable2D<T>::m_vertices;
		using Plottable2D<T>::m_indices;
		using Plottable2D<T>::m_size;
		using Plottable2D<T>::m_pos;
		T m_xDiff;
		T m_xStepSize;
		T m_yDiff;
		T m_yStepSize;
		T xAxisPos = 0, yAxisPos = 0;
		T prevXMin, prevXMax;
		T prevYMin, prevYMax;
		T prevXStep = 1;
		T prevYStep = 1;

		void cutoff() {
		}
		// Recalculate all necessary variables if a change was made to any plane settings.
		void update() {
			yAxisPos    = findAxisPos(yMin, yMax);
			xAxisPos    = findAxisPos(xMin, xMax);
			m_xDiff     = xMax - xMin / xStep;
			m_yDiff     = yMax - yMin / yStep;
			m_xStepSize = m_size.x() / m_xDiff;
			m_yStepSize = m_size.y() / m_yDiff;
			// If necessary, cut off any plotted points for the new plot
			cutoff();
		}

		T findAxisPos(T min, T max) {
			if((min < 0 && max > 0))
				return abs(min) - max;
			else if(min == 0)
				return min;
			else if(max == 0)
				return max;
			else
				return 0;
		}

	  public:
		static_assert(std::is_arithmetic_v<T>, "T has to be an arithmetic type.");
		T xMin, xMax;
		T yMin, yMax;
		T xStep = 1;
		T yStep = 1;

		std::vector<std::pair<T (*)(T), rgb>> functions;

		void plotLine(const ml::vec2<T>& from, const ml::vec2<T>& to) {
			createLine({m_pos.x() + (from.x() * m_xStepSize), m_pos.y() + (from.y() * m_yStepSize), m_pos.z()},
					   {m_pos.x() + (to.x() * m_xStepSize), m_pos.y() + (to.y() * m_yStepSize), m_pos.z()},
					   0.02, rgba(1.0f, 1.0f, 1.0f), m_vertices, m_indices);
			//createLine({pos.x(), pos.y(), pos.z()}, ml::vec3{pos.x() + size.x(), pos.y(), pos.z()}, thickness, Black, 0.0f, m_vertices, m_indices);
		}

		void plotPoint(const ml::vec2<T>& point) {
			createCircle({m_pos.x() + (point.x() * m_xStepSize), // Make X relative
						  m_pos.y() + (point.y() * m_yStepSize), // Make Y relative
						  m_pos.z()},
						 0.02, 12, m_vertices, m_indices);
		}
		void plotFunction() override {}
		void plotFunction(T (*func)(T), rgb lineCol = Black.toRGB()) {
			//functions.emplace_back(func, lineCol);
			float thickness = 0.001;
			for(int x = xMin; x < xMax; x += xStep) {
				float y     = func(x);
				float nextX = x + xStep;
				float nextY = func(nextX);
				plotLine({x, y}, {nextX, nextY});
			}
		}

		struct LineSettings {
			bool  enabled;
			rgba  color;
			float thickness;
		};

		LineSettings grid;
		LineSettings border;
		LineSettings xAxis;
		LineSettings yAxis;

		// Set the background grid
		void setGrid(bool enable, rgba color, float thickness) {}
		// Set the X-axis and Y-axis simultaneously.
		void setAxes(bool enable, rgba color, float thickness) {}
		void setXAxis(bool enable, rgba color, float thickness) {}
		void setYAxis(bool enable, rgba color, float thickness) {}
		//void setNumbers(bool enamble, Font font, rgba color){}
		// Set the border of the cartesian plane
		void setBorder(bool enable, rgba color, float thickness) {}
		// Set the background color
		void setBackground(bool enable, rgba color) {}

		// Creates an endless grid
		CartesianPlane(const ml::vec3& pos, const ml::vec2& size) {}

		CartesianPlane(const ml::vec3& pos, const ml::vec2& size, int xMin, int xMax, int yMin, int yMax, int xStep = 1, int yStep = 1):
			Plottable2D<T>(pos, size), xMin{xMin}, xMax{xMax}, yMin{yMin}, yMax{yMax}, xStep{xStep}, yStep{yStep} {
			update();
			float thickness = 0.005;
			plotLine({0.0f, yMin},
					 {0.0f, yMax});

			meshSlice2D(m_vertices, m_indices, {{xMin - 50.0f, 2.0f}, {xMax, 2.0f}});

			plotLine({xMin, 0.0f},
					 {xMax, 0.0f});
			plotPoint({-5.0f, 5.0f});
			plotPoint({-5.0f, -5.0f});
			plotPoint({5.0f, -5.0f});
			plotPoint({5.0f, 5.0f});
		}

		bool hasChanged() {
			return !(prevXMin == xMin &&
					 prevXMax == xMax &&
					 prevYMin == yMin &&
					 prevYMax == yMax &&
					 prevXStep == xStep &&
					 prevYStep == yStep);
		}

		void draw() {
		}

		//	Vertex& render(Renderer& renderer) override {}

		Vertex& draw(Renderer& renderer) override {
			if(hasChanged())
				update();
			// Filter all vertices that are not in the min-max range
			// If necessarry re-sort the vertices, such that all unbounded?
			// (not in min-max range) vertices are not in the renderer.draw() vertices.size() range
			renderer.draw(m_vertices.data(), m_vertices.size(),
						  m_indices.data(), m_indices.size());
		}
	};

	class PolarPlane {
		void plotPoint(float radius, float angle) {
		}
	};

	class PolarGrid {
		void plotPoint(float radius, float angle) {
		}
	};

	class Animation {
		struct Frame: public Transformable {
		};
		std::vector<Frame> frames;
		Frame&             operator[](int i) { return frames[i]; }
		//void addFrame(){}
	};
	template<typename T = float>
	class CartesianGraph {
		T xMin, xMax;
		T yMin, yMax;
		T xStep = 1;
		T yStep = 1;
		T zMin, zMax;
		T zStep = 1;

		// Take in a double variable function for (x,y)
		std::vector<std::pair<T (*)(T, T), rgb>> functions;
		//
		CartesianGraph(int xMin, int xMax, int yMin, int yMax, int zMin, int zMax) {}
	};

	template<typename T>
	struct has_subscript {
		using value = std::true_type;
	};

	template<typename T>
	constexpr unsigned short plotable_dim() {
		if constexpr(has_subscript<T>::value) {
			if constexpr(has_subscript<typename std::result_of<decltype(T::operator[])>::type>::value) {
				return 3;
			}
			return 2;
		}
		return 0;
	}

	enum class PlotMode {
		HeatMap,
		HeightMap,
		CartesianMap,
		PolarMap
	};

	template<typename T>
	void plot2d(const T& val, PlotMode plotMode) {
		switch(plotMode) {
		case PlotMode::HeatMap: break;
		case PlotMode::HeightMap: break;
		case PlotMode::CartesianMap: break;
		case PlotMode::PolarMap: break;
		}
	}
	template<typename T>
	void plot3d(const T& val, PlotMode plotMode) {
		switch(plotMode) {
		case PlotMode::HeatMap: break;
		case PlotMode::HeightMap: break;
		case PlotMode::CartesianMap: break;
		case PlotMode::PolarMap: break;
		}
	}

	template<typename T>
	void plot(const T& val, PlotMode plotMode) {
		unsigned short dim = plotable_dim<T>();
		if(dim == 2)
			plot2d(val, plotMode);
		if(dim == 3)
			plot2d(val, plotMode);
	}
}


#endif //PLOT_H
