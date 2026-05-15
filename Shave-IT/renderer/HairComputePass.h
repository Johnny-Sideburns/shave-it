#include <ituGL/shader/ShaderProgram.h>
#include <ituGL/renderer/RenderPass.h>

#include <ituGL/texture/Texture2DObject.h>
#include <ituGL/texture/FramebufferObject.h>

#include <ituGL/geometry/Model.h>

#include <memory>

class Renderer;

class HairComputePass : public RenderPass
{
public:
    HairComputePass(std::shared_ptr<Model> headModel, std::shared_ptr<Texture2DObject> mask, std::shared_ptr<Texture2DObject> paint);

    void Render() override;
    const GLuint GetStrandBuffer() const { return m_strandBuffer; }


private:
    void InitComputeShader();

    std::shared_ptr<ShaderProgram> m_computeShaderProgram;
    GLuint m_strandBuffer;
    GLuint m_vertexBuffer;
    GLuint m_indexBuffer;
    GLuint m_counterBuffer;
    
    ShaderProgram::Location m_hairLocation;
    ShaderProgram::Location m_paintLocation;

    std::shared_ptr<Texture2DObject> m_hairTexture;
    std::shared_ptr<Texture2DObject> m_paintTexture;
};