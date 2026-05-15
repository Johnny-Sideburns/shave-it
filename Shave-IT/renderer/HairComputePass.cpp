#include <ituGL/renderer/Renderer.h>
#include <ituGL/asset/ShaderLoader.h>

#include "HairComputePass.h"

#include "iostream"

HairComputePass::HairComputePass(std::shared_ptr<Model> headModel, std::shared_ptr<Texture2DObject> mask, std::shared_ptr<Texture2DObject> paint)
	: m_vertexBuffer(std::as_const(headModel->GetMesh()).GetVertexBuffer(0).GetHandle()),
	m_indexBuffer(std::as_const(headModel->GetMesh()).GetElementBuffer(0).GetHandle()),
	m_hairTexture(mask), m_paintTexture(paint)
{
	//std::cout << headModel->GetMesh().GetVertexBufferCount() << std::endl;
	//std::cout << headModel->GetMesh().GetSubmeshCount() << std::endl;

	InitComputeShader();	
}

void HairComputePass::InitComputeShader() {

	Shader computeShader = ShaderLoader(Shader::ComputeShader).Load("shaders/hair.comp");

	m_computeShaderProgram = std::make_shared<ShaderProgram>();
	m_computeShaderProgram->Build(computeShader);

	m_computeShaderProgram->Use();

	m_hairLocation = m_computeShaderProgram->GetUniformLocation("HairTexture");
	m_paintLocation = m_computeShaderProgram->GetUniformLocation("PaintTexture");
	
	// and set them in the computeshaderProgram	
	m_computeShaderProgram->SetTexture(m_hairLocation, 0, *m_hairTexture);
	m_computeShaderProgram->SetTexture(m_paintLocation, 1, *m_paintTexture);

	// initiate strandbuffer
	glGenBuffers(1, &m_strandBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_strandBuffer);

	// the storage buffer for all the strands needs to be massive
	// 4 vec3 and 2 floats currently, multiplied by the amount of hair strands
	// might be able to compact it better
	glBufferData(GL_SHADER_STORAGE_BUFFER, (sizeof(glm::vec4) * 4 + sizeof(float) * 2) * 131072, nullptr, GL_DYNAMIC_DRAW);

	// initiate atomic counter
	glGenBuffers(1, &m_counterBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_counterBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(GLuint), nullptr, GL_DYNAMIC_DRAW);
	
}

void HairComputePass::Render()
{
	m_computeShaderProgram->Use();

	// Bind buffers
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_strandBuffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_strandBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_vertexBuffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_vertexBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_indexBuffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, m_indexBuffer);

	// set atomic counter to 0 and bind it
	GLuint zero = 0;
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_counterBuffer);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(GLuint), &zero);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, m_counterBuffer);

	// bind textures
	m_computeShaderProgram->SetTexture(m_hairLocation, 0, *m_hairTexture);
	m_computeShaderProgram->SetTexture(m_paintLocation, 1, *m_paintTexture);

	
	glDispatchCompute(2048 , 1, 1);

	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

}