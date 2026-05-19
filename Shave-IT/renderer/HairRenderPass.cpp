#include "HairRenderPass.h"

#include <ituGL/asset/ShaderLoader.h>
#include <ituGL/renderer/Renderer.h>
#include <ituGL/camera/Camera.h>

#include <vector>
#include <iostream>

HairRenderPass::HairRenderPass(Renderer& renderer, std::shared_ptr<Model> headModel, GLuint strandBuffer)
	: m_strandBuffer(strandBuffer)
{
	//InitShader();
	m_strandCount = 131072;
	m_segments = 12;
	InitShader(renderer);
	//glGenVertexArrays(1, &m_dummyVAO);

    //get the textures
    std::shared_ptr<TextureObject> tex;
    headModel->GetMaterial(0).GetUniformValue("ColorTexture", tex);
    m_diffuseTexture = std::dynamic_pointer_cast<Texture2DObject>(tex);

    headModel->GetMaterial(0).GetUniformValue("NormalTexture", tex);
    m_normalTexture = std::dynamic_pointer_cast<Texture2DObject>(tex);

    headModel->GetMaterial(0).GetUniformValue("SpecularTexture", tex);
    m_armTexture = std::dynamic_pointer_cast<Texture2DObject>(tex);
    headModel->GetMaterial(0).GetUniformValue("EnvironmentTexture", tex);
    m_environmentTexture = std::dynamic_pointer_cast<TextureCubemapObject>(tex);

    m_environmentMaxLod = std::make_shared<float>();
    float tap;
    headModel->GetMaterial(0).GetUniformValues("EnvironmentMaxLod", std::span<float>(&tap, 1));
    *m_environmentMaxLod = tap;

    m_color = std::make_shared<glm::vec3>();
    glm::vec3 tmp;
    headModel->GetMaterial(0).GetUniformValues("Color", std::span<glm::vec3>(&tmp, 1));
    *m_color = tmp;
}

void HairRenderPass::InitShader(Renderer& renderer)
{
    std::vector<const char*> vertexShaderPaths;
    vertexShaderPaths.push_back("shaders/version430.glsl");
    vertexShaderPaths.push_back("shaders/hair_render.vert");
    Shader vertexShader = ShaderLoader(Shader::VertexShader).Load(vertexShaderPaths);


    std::vector<const char*> fragmentShaderPaths;
    fragmentShaderPaths.push_back("shaders/version430.glsl");
    fragmentShaderPaths.push_back("shaders/utils.glsl");
    fragmentShaderPaths.push_back("shaders/lambert-ggx.glsl");
    fragmentShaderPaths.push_back("shaders/lighting.glsl");
    fragmentShaderPaths.push_back("shaders/default_pbr.frag");
    Shader fragmentShader = ShaderLoader(Shader::FragmentShader).Load(fragmentShaderPaths);

    m_shaderProgramPtr = std::make_shared<ShaderProgram>();
    m_shaderProgramPtr->Build(vertexShader, fragmentShader);

    ShaderProgram::Location worldMatrixLocation = m_shaderProgramPtr->GetUniformLocation("WorldMatrix");
    ShaderProgram::Location viewProjMatrixLocation = m_shaderProgramPtr->GetUniformLocation("ViewProjMatrix");
    ShaderProgram::Location cameraPositionLocation = m_shaderProgramPtr->GetUniformLocation("CameraPosition");
    ShaderProgram::Location segmentsLocation = m_shaderProgramPtr->GetUniformLocation("Segments");
    //texture locations
    ShaderProgram::Location diffuseLocation = m_shaderProgramPtr->GetUniformLocation("ColorTexture");
    ShaderProgram::Location normalLocation = m_shaderProgramPtr->GetUniformLocation("NormalTexture");
    ShaderProgram::Location specularLocation = m_shaderProgramPtr->GetUniformLocation("SpecularTexture");
    //environment locations
    ShaderProgram::Location environmentLocation = m_shaderProgramPtr->GetUniformLocation("EnvironmentTexture");
    ShaderProgram::Location mLodLocation = m_shaderProgramPtr->GetUniformLocation("EnvironmentMaxLod");
    ShaderProgram::Location colorLocation = m_shaderProgramPtr->GetUniformLocation("Color");

    

    // Register shader with renderer
    renderer.RegisterShaderProgram(m_shaderProgramPtr,
        [=](const ShaderProgram& shaderProgram, const glm::mat4& worldMatrix, const Camera& camera, bool cameraChanged)
        {
            if (cameraChanged)
            {
                shaderProgram.SetUniform(cameraPositionLocation, camera.ExtractTranslation());
                shaderProgram.SetUniform(viewProjMatrixLocation, camera.GetViewProjectionMatrix());
                shaderProgram.SetUniform(segmentsLocation, m_segments);
                shaderProgram.SetTexture(diffuseLocation, 0, *m_diffuseTexture);
                shaderProgram.SetTexture(normalLocation, 1, *m_normalTexture);
                shaderProgram.SetTexture(specularLocation, 2, *m_armTexture);
                shaderProgram.SetTexture(environmentLocation, 3, *m_environmentTexture);
                shaderProgram.SetUniform(mLodLocation, *m_environmentMaxLod);
                shaderProgram.SetUniform(colorLocation, *m_color);

            }
            shaderProgram.SetUniform(worldMatrixLocation, worldMatrix);
        },
        renderer.GetDefaultUpdateLightsFunction(*m_shaderProgramPtr)
    );

    
}

void HairRenderPass::Render()
{
    Renderer& renderer = GetRenderer();
    Camera camera = renderer.GetCurrentCamera();
    
    // get lights
    const auto& lights = renderer.GetLights();

    m_shaderProgramPtr->Use();

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_strandBuffer);

    //just picking 0 is a little hacky, I should find the real location later
    renderer.UpdateTransforms(m_shaderProgramPtr, 0);

    //for all lights
    bool first = true;
    unsigned int lightIndex = 0;
    while (renderer.UpdateLights(m_shaderProgramPtr, lights, lightIndex))
    {
        // Set the renderstates
        renderer.SetLightingRenderStates(first);
        // Draw instanced
        glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, m_segments * 2, m_strandCount);

        first = false;
    }
    
}