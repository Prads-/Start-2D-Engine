#pragma once

#include <vector>

#include "Button.h"
#include "StartEngine.h"

class GraphRenderer {
private:
	Image *font;
	StartEngine *engine;
	std::vector<Button*> nodes;

public:
	GraphRenderer(StartEngine *engine);
	~GraphRenderer();

	void Render();
};