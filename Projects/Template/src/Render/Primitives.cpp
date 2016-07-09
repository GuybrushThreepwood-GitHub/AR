
/*===================================================================
	File: Primitives.cpp
=====================================================================*/

#include "Boot/Includes.h"

using renderer::Primitives;

/////////////////////////////////////////////////////
/// Default constructor
/// 
///
/////////////////////////////////////////////////////
Primitives::Primitives()
{

}

/////////////////////////////////////////////////////
/// Default destructor
/// 
///
/////////////////////////////////////////////////////
Primitives::~Primitives()
{

}

/////////////////////////////////////////////////////
/// Function: InitialisePrimitives
/// Params: None
///
/////////////////////////////////////////////////////
void Primitives::InitialisePrimitives()
{
	ShutdownPrimitives();

	m_PrimitiveShader.CreateFromFiles("data/shaders/21/primitives.vert", "data/shaders/21/primitives.frag", "");

	m_PrimitiveShader.Bind();
		m_ModelViewProjectionMatrix = m_PrimitiveShader.GetUniformLocation(renderer::Shader::ModelViewProjectionMatrix);
		m_VertexColour = m_PrimitiveShader.GetUniformLocation(renderer::Shader::VertexColour);

		m_VertexAttribLocation = m_PrimitiveShader.GetAttributeLocation(renderer::Shader::Vertex);
	m_PrimitiveShader.UnBind();
}

/////////////////////////////////////////////////////
/// Method: ShutdownFreetype
/// Params: None
///
/////////////////////////////////////////////////////
void Primitives::ShutdownPrimitives()
{
	m_PrimitiveShader.Shutdown();
}

/////////////////////////////////////////////////////
/// Function: DrawSphere
/// Params: [in]radius
///
/////////////////////////////////////////////////////
void Primitives::DrawSphere(float radius, const glm::vec4& colour)
{
	renderer::OpenGL::GetInstance()->DisableVBO();

	if (m_PrimitiveShader.GetProgram() == renderer::INVALID_OBJECT )
		return;

	m_PrimitiveShader.Bind();

	if (m_VertexColour != -1)
		glUniform4f(m_VertexColour, colour.r, colour.g, colour.b, colour.a);

	// grab all matrices
	glm::mat4 projMatrix	= renderer::OpenGL::GetInstance()->GetProjectionMatrix();
	glm::mat4 viewMatrix	= renderer::OpenGL::GetInstance()->GetViewMatrix();
	glm::mat4 modelMatrix	= renderer::OpenGL::GetInstance()->GetModelMatrix();

	glm::mat4 modelViewMatrix = viewMatrix*modelMatrix;

	glEnableVertexAttribArray(m_VertexAttribLocation);

	glm::vec2 vaPoints[16];
	
	const float k_segments = 16.0f;
	const float k_increment = 2.0f * glm::pi<float>() / k_segments;
	float theta = 0.0f;

	for (int i = 0; i < static_cast<int>(k_segments); ++i)
	{
		glm::vec2 v = glm::vec2(std::cos(theta), std::sin(theta)) * radius;
		vaPoints[i] = v;
		theta += k_increment;
	}

	glVertexAttribPointer(m_VertexAttribLocation, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), vaPoints);

	if( m_ModelViewProjectionMatrix != -1 )
		glUniformMatrix4fv(m_ModelViewProjectionMatrix, 1, GL_FALSE, glm::value_ptr(projMatrix*modelViewMatrix));

	glDrawArrays(GL_LINE_LOOP, 0, 16 );

	glm::mat4 objMatrix = glm::rotate( modelViewMatrix, 90.0f, glm::vec3(0.0f, 1.0f, 0.0f) );

	if (m_ModelViewProjectionMatrix != -1)
		glUniformMatrix4fv(m_ModelViewProjectionMatrix, 1, GL_FALSE, glm::value_ptr(projMatrix*objMatrix));

	glDrawArrays(GL_LINE_LOOP, 0, 16 );

	objMatrix = glm::rotate( modelViewMatrix, 45.0f, glm::vec3(0.0f, 1.0f, 0.0f) );

	if (m_ModelViewProjectionMatrix != -1)
		glUniformMatrix4fv(m_ModelViewProjectionMatrix, 1, GL_FALSE, glm::value_ptr(projMatrix*objMatrix));

	glDrawArrays(GL_LINE_LOOP, 0, 16 );

	objMatrix = glm::rotate( modelViewMatrix, -45.0f, glm::vec3(0.0f, 1.0f, 0.0f) );

	if (m_ModelViewProjectionMatrix != -1)
		glUniformMatrix4fv(m_ModelViewProjectionMatrix, 1, GL_FALSE, glm::value_ptr(projMatrix*objMatrix));
		
	glDrawArrays(GL_LINE_LOOP, 0, 16 );

	if (m_VertexAttribLocation != -1)
		glDisableVertexAttribArray(m_VertexAttribLocation);

	m_PrimitiveShader.UnBind();
}

/////////////////////////////////////////////////////
/// Function: DrawLine
/// Params: [in]start, [in]vBoxMax
///
/////////////////////////////////////////////////////
void Primitives::DrawLine(const glm::vec3 &start, const glm::vec3 &end, const glm::vec4& colour)
{
	if (m_PrimitiveShader.GetProgram() == renderer::INVALID_OBJECT)
		return;

	glm::vec3 vaSegmentPoints[2];

	renderer::OpenGL::GetInstance()->DisableVBO();

	m_PrimitiveShader.Bind();

	if (m_VertexColour != -1)
		glUniform4f(m_VertexColour, colour.r, colour.g, colour.b, colour.a);

	// grab all matrices
	glm::mat4 projMatrix	= renderer::OpenGL::GetInstance()->GetProjectionMatrix();
	glm::mat4 viewMatrix	= renderer::OpenGL::GetInstance()->GetViewMatrix();
	glm::mat4 modelMatrix	= renderer::OpenGL::GetInstance()->GetModelMatrix();

	glm::mat4 modelViewMatrix = viewMatrix*modelMatrix;

	glEnableVertexAttribArray(m_VertexAttribLocation);
	
	if (m_ModelViewProjectionMatrix != -1)
		glUniformMatrix4fv(m_ModelViewProjectionMatrix, 1, GL_FALSE, glm::value_ptr(projMatrix*modelViewMatrix));

	vaSegmentPoints[0] = start;
	vaSegmentPoints[1] = end;

	glVertexAttribPointer(m_VertexAttribLocation, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), vaSegmentPoints);
	glDrawArrays(GL_LINES, 0, 2 );

	if (m_VertexAttribLocation != -1)
		glDisableVertexAttribArray(m_VertexAttribLocation);

	m_PrimitiveShader.UnBind();
}

