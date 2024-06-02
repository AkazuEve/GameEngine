#pragma once

#include <vector>

extern std::vector<void*> registeredElements;

enum UI_ELEMENT_TYPE {
	UI_DEBUG = 0,
	UI_MODEL = 1,
	UI_PROFILER = 2,
	UI_RENDERER = 3
};

class UIElement {
public:
	UIElement(UI_ELEMENT_TYPE type) : m_type(type) {
		registeredElements.push_back(this);
	}
	~UIElement() {
		registeredElements.erase(std::find(registeredElements.begin(), registeredElements.end(), this));
	}
	UI_ELEMENT_TYPE GetType() {
		return m_type;
	}
	virtual void OnUIRender() = 0;
private:
	UI_ELEMENT_TYPE m_type;
};