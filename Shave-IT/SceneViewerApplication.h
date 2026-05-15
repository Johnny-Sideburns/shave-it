#pragma once

#include <ituGL/application/Application.h>

#include <ituGL/scene/Scene.h>
#include <ituGL/renderer/Renderer.h>
#include <ituGL/camera/CameraController.h>
#include <ituGL/utils/DearImGui.h>


#include "Painter.h"

class TextureCubemapObject;
class Material;

class SceneViewerApplication : public Application
{
public:
    SceneViewerApplication();

protected:
    void Initialize() override;
    void Update() override;
    void Render() override;
    void Cleanup() override;

private:
    void InitializeCamera();
    void InitializeLights();
    void InitializeMaterial();
    void InitializeMaterialGeo();
    void InitializeModels();
    void InitializeRenderer();

    void RenderGUI();

private:
    // Use compute else use Geometry shader, will work on lower open gl version, but can be harder on integrated graphics cards.
    bool m_use_compute;

    // Helper object for debug GUI
    DearImGui m_imGui;

    // Camera controller
    CameraController m_cameraController;

    // Global scene
    Scene m_scene;

    // Renderer
    Renderer m_renderer;

    // Skybox texture
    std::shared_ptr<TextureCubemapObject> m_skyboxTexture;

    // Default material
    std::shared_ptr<Material> m_defaultMaterial;
    std::shared_ptr<Material> m_hairMaterial;

    // member variables for other classes
    std::shared_ptr<Painter> m_painter;
    std::shared_ptr<Texture2DObject> m_target;
    std::shared_ptr<Texture2DObject> m_mask;
    std::shared_ptr<Model> m_headModel;

};
