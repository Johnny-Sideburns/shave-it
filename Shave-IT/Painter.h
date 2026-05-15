#pragma once

#include <ituGL/renderer/Renderer.h>
#include <ituGL/texture/Texture2DObject.h>
#include <ituGL/shader/Material.h>
#include "renderer/PaintRenderPass.h"

#include <ituGL/application/Window.h>

/*
	Painter acts as the controls for the paint system/ shaver
	it creates the paintrenderpass and gets the relevant control pointers
*/

class Painter
{
public:
	Painter(Window& window, Renderer& renderer, std::shared_ptr<Texture2DObject> target);
	void Update(const Window& window, float deltaTime);
	void Paint(const Window& window);
	void UpdateBrushScale();

protected:

private:
	std::unique_ptr<PaintRenderPass> m_paintRenderPass;

	std::shared_ptr<bool> m_paint;
	std::shared_ptr<float> m_brushRadius;
	std::shared_ptr<float> m_grow;
	std::shared_ptr<float> m_hair;
	std::shared_ptr<float> m_trimLength;
	std::shared_ptr<int> m_mirror;

	std::shared_ptr<glm::vec2> m_mousePosition;
	bool m_mPressed;

	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
};