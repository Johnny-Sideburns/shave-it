#pragma once

#include <ituGL/shader/ShaderProgram.h>
#include <ituGL/renderer/RenderPass.h>

#include <ituGL/texture/Texture2DObject.h>
#include <ituGL/texture/FramebufferObject.h>
#include "../TextureIdMap.h"

#include <memory>

class Renderer;

class PaintRenderPass : public RenderPass
{
public:
    PaintRenderPass(int width, int height, Renderer& renderer, std::shared_ptr<Texture2DObject> target , int drawcallCollectionIndex = 0);

    void Render() override;

    const std::shared_ptr<Texture2DObject> GetDepthTexture() const { return m_depthTexture; }
    const std::shared_ptr<Texture2DObject> GetPaintTexture() const { return m_paintTexture; }
    const std::shared_ptr<Texture2DObject> GetNormalsTexture() const { return m_normalsTexture; }
    const std::shared_ptr<Texture2DObject> GetBrushTexture() const { return m_brushTexture; }
    const std::shared_ptr<Texture2DObject> GetCanvasTexture() const { return m_canvasTexture; }

    const std::shared_ptr<glm::vec3> GetBrushWorldPos() const { return m_brushWorldPos; }
    const std::shared_ptr<glm::vec2> GetMousePosPtr() const { return m_mousePosition; }
    const std::shared_ptr<float> GetBrushRadius() const { return m_brushRadius; }
    const std::shared_ptr<float> GetGrowFloat() const { return m_grow; }
    const std::shared_ptr<float> GetHairFloat() const { return m_hair; }
    const std::shared_ptr<float> GetTrimFloat() const { return m_trimLength; }
    const std::shared_ptr<bool> GetPaintPtr() const { return m_paint; }
    const std::shared_ptr<int> GetMirror() const { return m_mirror; }

private:
    void Paint();

    void InitCanvasShaderProgram(Renderer& renderer);
    void InitPaintShaderProgram(Renderer& renderer);
    void InitUVShaderProgram(Renderer& renderer);
    void InitHitShaderProgram(Renderer& renderer);

    void SetBrushWorldPos(Renderer& renderer);
    void RenderUV(Renderer& renderer);
    void RenderCanvas(Renderer& renderer);
    void RenderPaint(Renderer& renderer);

    void RenderGeneric(Renderer& renderer, FramebufferObject& frameBuffer, std::shared_ptr<ShaderProgram> shaderProgram);


    void InitTextures(int width, int height);
    
    void InitUVFramebuffer();
    void InitDepthFramebuffer();
    void InitCanvasFramebuffer();
    void InitPaintFramebuffer();

    void DebugDraw(FramebufferObject& frameBuffer);


private:
    int m_drawcallCollectionIndex;

    std::shared_ptr<Texture2DObject> m_depthTexture;
    std::shared_ptr<Texture2DObject> m_paintTexture;
    std::shared_ptr<Texture2DObject> m_brushTexture;
    std::shared_ptr<Texture2DObject> m_normalsTexture;
    std::shared_ptr<Texture2DObject> m_canvasTexture;
    
    std::shared_ptr<ShaderProgram> m_uvShaderProgramPtr;
    std::shared_ptr<ShaderProgram> m_hitShaderProgramPtr;
    std::shared_ptr<ShaderProgram> m_canvasShaderProgramPtr;
    std::shared_ptr<ShaderProgram> m_paintShaderProgramPtr;

    std::shared_ptr<glm::vec3> m_brushWorldPos;

    std::shared_ptr<glm::vec2> m_mousePosition;
    std::shared_ptr<float> m_brushRadius;
    std::shared_ptr<float> m_grow;
    std::shared_ptr<float> m_hair;
    std::shared_ptr<float> m_trimLength;
    std::shared_ptr<bool> m_paint;
    std::shared_ptr<int> m_mirror;

    FramebufferObject m_uvFramebuffer;
    FramebufferObject m_depthFramebuffer;
    FramebufferObject m_canvasFramebuffer;
    FramebufferObject m_paintFramebuffer;

    int m_width;
    int m_height;

};
