#include "Painter.h"

#include <ituGL/asset/ShaderLoader.h>
#include <ituGL/shader/ShaderUniformCollection.h>
#include <ituGL/camera/Camera.h>
#include <ituGL/scene/SceneCamera.h>



#include "iostream"

//for scroll there needs to be a static value it's only used here so it'll do
namespace Input
{
    float scrollY = 10.0f;
}

Painter::Painter(Window& window, Renderer& renderer, std::shared_ptr<Texture2DObject> target)
{
    int width, height;
    window.GetDimensions(width, height);
    m_paintRenderPass = std::make_unique<PaintRenderPass>(width, height, renderer, target);

    m_paint = m_paintRenderPass->GetPaintPtr();
    m_brushRadius = m_paintRenderPass->GetBrushRadius();
    m_mousePosition = m_paintRenderPass->GetMousePosPtr();
    m_grow = m_paintRenderPass->GetGrowFloat();
    m_hair = m_paintRenderPass->GetHairFloat();
    m_mirror = m_paintRenderPass->GetMirror();
    m_trimLength = m_paintRenderPass->GetTrimFloat();

    m_mPressed = false;

    //add the paint render pass the the renderer
    renderer.AddRenderPass(std::move(m_paintRenderPass));

    //initialise scroll callback for incorporating scroll
    glfwSetScrollCallback(window.GetInternalWindow(), scroll_callback);


}

void Painter::Update(const Window& window, float deltaTime)
{
    UpdateBrushScale();
    //std::cout << "fps: " << 1.f/deltaTime << std::endl;

    if (window.IsMouseButtonPressed(Window::MouseButton(GLFW_MOUSE_BUTTON_LEFT))) {
        Paint(window);
    }
    //press b to grow beard
    if (window.IsKeyPressed(GLFW_KEY_B)) {
        *m_grow = 0.005;
        *m_paint = true;
    }
    //press h to grow hair
    if (window.IsKeyPressed(GLFW_KEY_H)) {
        *m_hair = 0.005;
        *m_paint = true;
    }
    // press and release m to activate mirroring shave
    if (window.IsKeyPressed(GLFW_KEY_M)) {
        m_mPressed = true;
    }
    if (window.IsKeyReleased(GLFW_KEY_M) && m_mPressed) {
        if (*m_mirror == 1) *m_mirror = 0;
        else *m_mirror = 1;
        m_mPressed = false;
    }
    // use numbers 1-0 and + - to adjust shave length
    if (window.IsKeyPressed(GLFW_KEY_1)) {
        *m_trimLength = 0.005;
    }
    if (window.IsKeyPressed(GLFW_KEY_2)) {
        *m_trimLength = 0.025;
    }
    if (window.IsKeyPressed(GLFW_KEY_3)) {
        *m_trimLength = 0.04;
    }
    if (window.IsKeyPressed(GLFW_KEY_4)) {
        *m_trimLength = 0.1;
    }
    if (window.IsKeyPressed(GLFW_KEY_5)) {
        *m_trimLength = 0.15;
    }
    if (window.IsKeyPressed(GLFW_KEY_6)) {
        *m_trimLength = 0.2;
    }
    if (window.IsKeyPressed(GLFW_KEY_7)) {
        *m_trimLength = 0.3;
    }
    if (window.IsKeyPressed(GLFW_KEY_8)) {
        *m_trimLength = 0.45;
    }
    if (window.IsKeyPressed(GLFW_KEY_9)) {
        *m_trimLength = 0.6;
    }
    if (window.IsKeyPressed(GLFW_KEY_0)) {
        *m_trimLength = 0.0;
    }
    if (window.IsKeyPressed(GLFW_KEY_KP_ADD)) {
        *m_trimLength = *m_trimLength + 0.0002;
    }
    if (window.IsKeyPressed(GLFW_KEY_KP_SUBTRACT)) {
        *m_trimLength = *m_trimLength - 0.0002;
    }
}


void Painter::UpdateBrushScale()
{
    *m_brushRadius = Input::scrollY/200.f;
}

void Painter::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    if (Input::scrollY + (float)yoffset >= 200.f) return;
    if (Input::scrollY + (float)yoffset <= 0.0f) return;
    Input::scrollY += (float)yoffset;
}

void Painter::Paint(const Window& window)
{
    int width, height;
    window.GetDimensions(width, height);

    glm::vec2 pos = window.GetMousePosition();
    *m_mousePosition = glm::vec2(pos[0], height - pos[1]);

    *m_paint = true;
}