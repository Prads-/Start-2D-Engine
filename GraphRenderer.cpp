#include "GraphRenderer.h"

GraphRenderer::GraphRenderer(StartEngine *engine) {
	this->engine = engine;
	font = new Image("Images/font.png");

	auto node = new Button(engine, Vector2i(200, 200), false, [] {});
	node->setCaption("Hello world", 5, 5, font, 0, 0xFFFF0000, 0XFFFFFF00);
	node->setShowBorder(true);
	nodes.push_back(node);
}

void GraphRenderer::Render() {
	engine->clearScreen();

	for (auto node : nodes) {
		node->draw();
	}

	engine->presentScreen();
}

GraphRenderer::~GraphRenderer() {
	if (font) delete font;
	for (auto node : nodes) {
		delete node;
	}
	nodes.clear();
}