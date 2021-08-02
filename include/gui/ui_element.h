#ifndef ELEMENT_H
#define ELEMENT_H

#include "event.h"

namespace glpp {
	struct Rectangle {
		int          x, y;
		unsigned int width, height;
	};

	class GUI_Element {
		Rectangle m_rect;

	  public:
		GUI_Element*                              parent = nullptr;
		std::vector<std::shared_ptr<GUI_Element>> children;

		virtual GUI_Element(Rectangle rectangle, GUI_Element* parent) = 0;
		virtual void onDraw()                                         = 0; /* Calls all children's onDraw(Event) */
		virtual void onEvent(Event)                                   = 0; /* Calls all children's onEvent(Event) */
	};

	class GUI_Button: public GUI_Element {
	  public:
		GUI_Button(Rectangle rectangle, GUI_Element* parent):
			m_rect(rectangle), parent(parent) {
		}
		void onDraw() {
			// Draws all triangles of the shape an
		}
	};
}

#endif //ELEMENT_H
