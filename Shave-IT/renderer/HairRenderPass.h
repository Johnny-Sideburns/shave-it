#include <ituGL/shader/ShaderProgram.h>
#include <ituGL/renderer/RenderPass.h>
#include <ituGL/geometry/Model.h>

#include <ituGL/texture/Texture2DObject.h>
#include <ituGL/texture/TextureCubemapObject.h>
#include <ituGL/texture/FramebufferObject.h>
#include <memory>
class Renderer;

class HairRenderPass : public RenderPass
{
public:
	HairRenderPass(Renderer& renderer, std::shared_ptr<Model> headModel, GLuint strandBuffer);
	void InitShader(Renderer& renderer);

	void Render() override;

private:
	GLuint m_strandBuffer;
	GLuint m_dummyVAO;

	int m_strandCount;
	int m_segments;

	std::shared_ptr<ShaderProgram> m_shaderProgramPtr;

	ShaderProgram::Location m_viewProjLocation;
	ShaderProgram::Location m_segmentsLocation;
	ShaderProgram::Location m_cameraPositionLocation;

	std::shared_ptr<Texture2DObject> m_diffuseTexture;
	std::shared_ptr<Texture2DObject> m_normalTexture;
	std::shared_ptr<Texture2DObject> m_armTexture;
	std::shared_ptr<TextureCubemapObject> m_environmentTexture;
	std::shared_ptr<float> m_environmentMaxLod;
	std::shared_ptr<glm::vec3> m_color;
};