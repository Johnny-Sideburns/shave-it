#include <ituGL/camera/Camera.h>
#include <ituGL/renderer/Renderer.h>
#include <ituGL/asset/ShaderLoader.h>

#include "PaintRenderPass.h"

#include "iostream"

PaintRenderPass::PaintRenderPass(int width, int height, Renderer& renderer, std::shared_ptr<Texture2DObject> target, int drawcallCollectionIndex)
    : m_drawcallCollectionIndex(drawcallCollectionIndex),
    m_width(width), m_height(height), m_paintTexture(target)
{   
    //create all the member variables
    m_mousePosition = std::make_shared<glm::vec2>(0.0f);
    m_brushWorldPos = std::make_shared<glm::vec3>(0.0f);
    m_brushRadius = std::make_shared<float>(0.0f);
    m_grow = std::make_shared<float>(0.0f);
    m_hair = std::make_shared<float>(0.0f);
    m_trimLength = std::make_shared<float>(0.0f);
    m_paint = std::make_shared<bool>(false);
    m_mirror = std::make_shared<int>(0);

    //init shader programs
    //std::cout << "hit init" << std::endl;
    InitHitShaderProgram(renderer);

    //std::cout << "brush init" << std::endl;
    InitUVShaderProgram(renderer);

    //std::cout << "canvas init" << std::endl;
    InitCanvasShaderProgram(renderer);

    //std::cout << "paint init" << std::endl;
    InitPaintShaderProgram(renderer);

    //init textures
    InitTextures(width, height);

    //init framebuffers
    InitUVFramebuffer();
    InitDepthFramebuffer();
    InitCanvasFramebuffer();
    InitPaintFramebuffer();

}

void PaintRenderPass::InitDepthFramebuffer()
{
    m_depthFramebuffer.Bind();

    m_depthFramebuffer.SetTexture(FramebufferObject::Target::Draw, FramebufferObject::Attachment::Depth, *m_depthTexture);
    
    // Set the normals texture as color attachment 0
    m_depthFramebuffer.SetTexture(FramebufferObject::Target::Draw, FramebufferObject::Attachment::Color0, *m_normalsTexture);

    // Set the draw buffers used by the framebuffer (all attachments except depth)
    m_depthFramebuffer.SetDrawBuffers(std::array<FramebufferObject::Attachment, 1>(
        {
            FramebufferObject::Attachment::Color0,
        }));
    

    FramebufferObject::Unbind();
    assert(m_depthFramebuffer.IsValid());
}

void PaintRenderPass::InitUVFramebuffer()
{
    m_uvFramebuffer.Bind();

    m_uvFramebuffer.SetTexture(FramebufferObject::Target::Draw, FramebufferObject::Attachment::Depth, *m_depthTexture);

    // Set the uv texture as color attachment 0
    m_uvFramebuffer.SetTexture(FramebufferObject::Target::Draw, FramebufferObject::Attachment::Color0, *m_brushTexture);

    // Set the draw buffers used by the framebuffer (all attachments except depth)
    m_uvFramebuffer.SetDrawBuffers(std::array<FramebufferObject::Attachment, 1>(
        {
            FramebufferObject::Attachment::Color0,
        }));

    FramebufferObject::Unbind();
    assert(m_uvFramebuffer.IsValid());
}

void PaintRenderPass::InitCanvasFramebuffer()
{
    m_canvasFramebuffer.Bind();

    m_canvasFramebuffer.SetTexture(FramebufferObject::Target::Draw, FramebufferObject::Attachment::Depth, *m_depthTexture);

    // Set the uv texture as color attachment 0
    m_canvasFramebuffer.SetTexture(FramebufferObject::Target::Draw, FramebufferObject::Attachment::Color0, *m_canvasTexture);

    // Set the draw buffers used by the framebuffer (all attachments except depth)
    m_canvasFramebuffer.SetDrawBuffers(std::array<FramebufferObject::Attachment, 1>(
        {
            FramebufferObject::Attachment::Color0,

        }));

    FramebufferObject::Unbind();
    assert(m_canvasFramebuffer.IsValid());
}

void PaintRenderPass::InitPaintFramebuffer()
{
    m_paintFramebuffer.Bind();

    m_paintFramebuffer.SetTexture(FramebufferObject::Target::Draw, FramebufferObject::Attachment::Depth, *m_depthTexture);

    // Set the uv texture as color attachment 0
    m_paintFramebuffer.SetTexture(FramebufferObject::Target::Draw, FramebufferObject::Attachment::Color0, *m_paintTexture);

    // Set the draw buffers used by the framebuffer (all attachments except depth)
    m_paintFramebuffer.SetDrawBuffers(std::array<FramebufferObject::Attachment, 1>(
        {
            FramebufferObject::Attachment::Color0,

        }));

    FramebufferObject::Unbind();
    assert(m_paintFramebuffer.IsValid());
}

void PaintRenderPass::InitTextures(int width, int height)
{
    // bind depth buffer I might have overdone it with these
    m_depthTexture = std::make_shared<Texture2DObject>();
    m_depthTexture->Bind();
    m_depthTexture->SetImage(0, width, height, TextureObject::FormatDepth, TextureObject::InternalFormatDepth);
    m_depthTexture->SetParameter(TextureObject::ParameterEnum::MinFilter, GL_NEAREST);
    m_depthTexture->SetParameter(TextureObject::ParameterEnum::MagFilter, GL_NEAREST);

    // Bind the normals texture "this is currently not in use"
    m_normalsTexture = std::make_shared<Texture2DObject>();
    m_normalsTexture->Bind();
    m_normalsTexture->SetImage(0, width, height, TextureObject::FormatRGBA, TextureObject::InternalFormatRGBA16F);
    m_normalsTexture->SetParameter(TextureObject::ParameterEnum::MinFilter, GL_NEAREST);
    m_normalsTexture->SetParameter(TextureObject::ParameterEnum::MagFilter, GL_NEAREST);

    // brush texture
    m_brushTexture = std::make_shared<Texture2DObject>();
    m_brushTexture->Bind();
    m_brushTexture->SetImage(0, width, height, TextureObject::FormatRGBA, TextureObject::InternalFormatRGBA8);
    m_brushTexture->SetParameter(TextureObject::ParameterEnum::MinFilter, GL_NEAREST);
    m_brushTexture->SetParameter(TextureObject::ParameterEnum::MagFilter, GL_NEAREST);

    // and the canvas texture
    m_canvasTexture = std::make_shared<Texture2DObject>();
    m_canvasTexture->Bind();
    m_canvasTexture->SetImage(0, width, height, TextureObject::FormatRGBA, TextureObject::InternalFormatRGBA8);
    m_canvasTexture->SetParameter(TextureObject::ParameterEnum::MinFilter, GL_NEAREST);
    m_canvasTexture->SetParameter(TextureObject::ParameterEnum::MagFilter, GL_NEAREST);

    Texture2DObject::Unbind();
}

//the DepthShaderProgram is for an inital pass to get information for the mouse position and normal
void PaintRenderPass::InitHitShaderProgram(Renderer& renderer)
{
    // Load and build shader
    Shader vertexShader = ShaderLoader(Shader::VertexShader).Load("shaders/hit.vert");
    Shader fragmentShader = ShaderLoader(Shader::FragmentShader).Load("shaders/hit.frag");
    m_hitShaderProgramPtr = std::make_shared<ShaderProgram>();
    m_hitShaderProgramPtr->Build(vertexShader, fragmentShader);

    // Get transform related uniform locations
    ShaderProgram::Location worldMatrixLocation = m_hitShaderProgramPtr->GetUniformLocation("WorldMatrix");
    ShaderProgram::Location worldViewProjMatrixLocation = m_hitShaderProgramPtr->GetUniformLocation("WorldViewProjMatrix");
    
    renderer.RegisterShaderProgram(m_hitShaderProgramPtr,
        [=](const ShaderProgram& shaderProgram, const glm::mat4& worldMatrix, const Camera& camera, bool cameraChanged)
        {
            shaderProgram.SetUniform(worldMatrixLocation, worldMatrix);
            shaderProgram.SetUniform(worldViewProjMatrixLocation, camera.GetViewProjectionMatrix() * worldMatrix);
        },
        nullptr
        
    );
}

//the uv shader program 
void PaintRenderPass::InitUVShaderProgram(Renderer& renderer)
{
    // Load and build shader
    Shader vertexShader = ShaderLoader(Shader::VertexShader).Load("shaders/uvlook.vert");
    Shader fragmentShader = ShaderLoader(Shader::FragmentShader).Load("shaders/uvpaint.frag");
    m_uvShaderProgramPtr = std::make_shared<ShaderProgram>();
    m_uvShaderProgramPtr->Build(vertexShader, fragmentShader);

    // Get transform related uniform locations
    ShaderProgram::Location worldMatrixLocation = m_uvShaderProgramPtr->GetUniformLocation("WorldMatrix");
    ShaderProgram::Location mouseWorldPositionLocation = m_uvShaderProgramPtr->GetUniformLocation("BrushWorldPos");
    ShaderProgram::Location brushLocation = m_uvShaderProgramPtr->GetUniformLocation("BrushRadius");
    ShaderProgram::Location growLocation = m_uvShaderProgramPtr->GetUniformLocation("Grow");
    ShaderProgram::Location hairLocation = m_uvShaderProgramPtr->GetUniformLocation("Hair");


    renderer.RegisterShaderProgram(m_uvShaderProgramPtr,
        [=](const ShaderProgram& shaderProgram, const glm::mat4& worldMatrix, const Camera& camera, bool cameraChanged)
        {
            shaderProgram.SetUniform(worldMatrixLocation, worldMatrix);
            shaderProgram.SetUniform(mouseWorldPositionLocation, *GetBrushWorldPos());
            shaderProgram.SetUniform(brushLocation, *GetBrushRadius());
            shaderProgram.SetUniform(growLocation, *GetGrowFloat());
            shaderProgram.SetUniform(hairLocation, *GetHairFloat());
        },
        nullptr
    );

}

void PaintRenderPass::InitCanvasShaderProgram(Renderer& renderer)
{
    // Load and build shader
    Shader vertexShader = ShaderLoader(Shader::VertexShader).Load("shaders/canvas.vert");
    Shader fragmentShader = ShaderLoader(Shader::FragmentShader).Load("shaders/canvas.frag");
    m_canvasShaderProgramPtr = std::make_shared<ShaderProgram>();
    m_canvasShaderProgramPtr->Build(vertexShader, fragmentShader);

    ShaderProgram::Location PaintLocation = m_canvasShaderProgramPtr->GetUniformLocation("PaintTexture");
    ShaderProgram::Location BrushLocation = m_canvasShaderProgramPtr->GetUniformLocation("BrushTexture");
    ShaderProgram::Location mirrorLocation = m_canvasShaderProgramPtr->GetUniformLocation("Mirror");
    ShaderProgram::Location lengthLocation = m_canvasShaderProgramPtr->GetUniformLocation("TrimLength");

    renderer.RegisterShaderProgram(m_canvasShaderProgramPtr,
        [=](const ShaderProgram& shaderProgram, const glm::mat4& worldMatrix, const Camera& camera, bool cameraChanged)
        {
            shaderProgram.SetUniform(mirrorLocation, *GetMirror());
            shaderProgram.SetUniform(lengthLocation, *GetTrimFloat());
            shaderProgram.SetTexture(BrushLocation, 0, *GetBrushTexture());
            shaderProgram.SetTexture(PaintLocation, 1, *GetPaintTexture());
        },
        nullptr
    );

}

void PaintRenderPass::InitPaintShaderProgram(Renderer& renderer)
{
    // Load and build shader
    Shader vertexShader = ShaderLoader(Shader::VertexShader).Load("shaders/canvas.vert");
    Shader fragmentShader = ShaderLoader(Shader::FragmentShader).Load("shaders/paint.frag");
    m_paintShaderProgramPtr = std::make_shared<ShaderProgram>();
    m_paintShaderProgramPtr->Build(vertexShader, fragmentShader);

    ShaderProgram::Location CanvasLocation = m_paintShaderProgramPtr->GetUniformLocation("CanvasTexture");

    renderer.RegisterShaderProgram(m_paintShaderProgramPtr,
        [=](const ShaderProgram& shaderProgram, const glm::mat4& worldMatrix, const Camera& camera, bool cameraChanged)
        {
            shaderProgram.SetTexture(CanvasLocation, 1, *GetCanvasTexture());
        },
        nullptr
    );

}

void PaintRenderPass::Render()
{
    if (*m_paint) {
        Paint();
        *m_paint = false;
        *m_grow = 0.0;
        *m_hair = 0.0;
    }

    //DebugDraw(m_paintFramebuffer);
    FramebufferObject::Unbind();

}

void PaintRenderPass::Paint()
{
    Renderer& renderer = GetRenderer();

    SetBrushWorldPos(renderer);
    RenderUV(renderer);
    RenderCanvas(renderer);
    RenderPaint(renderer);
}

//The render uv pass, the naming is a bit of an artifact, this actually paints from world coordinates to model uv-space
void PaintRenderPass::RenderUV(Renderer& renderer)
{
    RenderGeneric(renderer, m_uvFramebuffer, m_uvShaderProgramPtr);
}
//Render Canvas takes information from the uv pass and the painttexture to prepare what goes on the painttexture
void PaintRenderPass::RenderCanvas(Renderer& renderer)
{
    RenderGeneric(renderer, m_canvasFramebuffer, m_canvasShaderProgramPtr);
}
//Render Canvas basically just copies the canvas onto the paintTexture
void PaintRenderPass::RenderPaint(Renderer& renderer)
{
    RenderGeneric(renderer, m_paintFramebuffer, m_paintShaderProgramPtr);
}

//renderpass for getting mouse position and normal from depth and normal -texture respectively
void PaintRenderPass::SetBrushWorldPos(Renderer& renderer)
{
    const Camera& camera = renderer.GetCurrentCamera();
    RenderGeneric(renderer, m_depthFramebuffer, m_hitShaderProgramPtr);

    //get mouse position in pixel terms (with y flipped)
    float x = m_mousePosition->x;
    float y = m_mousePosition->y;
    
    //bind the depthbuffer
    glReadBuffer(GL_NONE);

    float depth;
    glReadPixels(x, y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);

    // NDC
    float ndcX = (2.0f * x) / m_width - 1.0f;
    float ndcY = (2.0f * y) / m_height - 1.0f;
    float ndcZ = depth * 2.0f - 1.0f;

    // Clip space
    glm::vec4 clip(ndcX, ndcY, ndcZ, 1.0f);

    glm::vec4 world = glm::inverse(camera.GetViewProjectionMatrix()) * clip;
    world /= world.w;
    
    *m_brushWorldPos = glm::vec3(world.x, world.y, world.z);

    //debug print position
    //std::cout << m_brushWorldPos->x << "," << m_brushWorldPos->y << "," << m_brushWorldPos->z << std::endl;

}

void PaintRenderPass::RenderGeneric(Renderer& renderer, FramebufferObject& frameBuffer, std::shared_ptr<ShaderProgram> shaderProgram)
{
    const auto& drawcallCollection = renderer.GetDrawcalls(m_drawcallCollectionIndex);

    frameBuffer.Bind();
    shaderProgram->Use();

    renderer.GetDevice().Clear(true, Color(0.0f, 0.0f, 0.0f, 1.0f), true, 1.0f);

    for (const Renderer::DrawcallInfo& drawcallInfo : drawcallCollection)
    {
        renderer.UpdateTransforms(shaderProgram, drawcallInfo.GetWorldMatrixIndex());
        drawcallInfo.GetVAO().Bind();
        drawcallInfo.GetDrawcall().Draw();

    }
}
//debugdraw has been a usefull tool to see what was being drawn at different stages
void PaintRenderPass::DebugDraw(FramebufferObject& frameBuffer)
{
    frameBuffer.Bind(FramebufferObject::Target::Read);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glViewport(0, 0, m_width, m_height);

    glBlitFramebuffer(
        0, 0, m_width, m_height,
        0, 0, m_width, m_height,
        GL_COLOR_BUFFER_BIT,
        GL_NEAREST
    );
}