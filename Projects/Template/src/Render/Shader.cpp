
/*===================================================================
	File: Shader.cpp
=====================================================================*/

#include "Boot/Includes.h"

using renderer::Shader;

namespace
{
	std::array<Shader::TUniformBlock, Shader::TotalUniforms> uniforms = 
	{
			Shader::ModelViewProjectionMatrix,
			Shader::VarType_Mat4,
			-1,
			"ogl_ModelViewProjectionMatrix",

			Shader::ModelViewMatrix,
			Shader::VarType_Mat4,
			-1,
			"ogl_ModelViewMatrix",

			Shader::ViewMatrix,
			Shader::VarType_Mat4,
			-1,
			"ogl_ViewMatrix",

			Shader::NormalMatrix,
			Shader::VarType_Mat4,
			-1,
			"ogl_NormalMatrix",

			Shader::VertexColour,
			Shader::VarType_Vec4,
			-1,
			"ogl_VertexColour",

			Shader::TexUnit0,
			Shader::VarType_Int,
			-1,
			"ogl_TexUnit0",

			Shader::TexUnit1,
			Shader::VarType_Int,
			-1,
			"ogl_TexUnit1",

			Shader::TexUnit2,
			Shader::VarType_Int,
			-1,
			"ogl_TexUnit2",

			Shader::TexUnit3,
			Shader::VarType_Int,
			-1,
			"ogl_TexUnit3",
	};

	std::array<Shader::TAttrBlock, Shader::TotalAttribs> attribs =
	{
		Shader::Vertex,
		Shader::VarType_Vec3,
		-1,
		"base_v",

		Shader::TexCoord0,
		Shader::VarType_Vec2,
		-1,
		"base_uv0",

		Shader::TexCoord1,
		Shader::VarType_Vec2,
		-1,
		"base_uv1",

		Shader::TexCoord2,
		Shader::VarType_Vec2,
		-1,
		"base_uv2",

		Shader::TexCoord3,
		Shader::VarType_Vec2,
		-1,
		"base_uv3",

		Shader::Normal,
		Shader::VarType_Vec3,
		-1,
		"base_n",

		Shader::Tangent,
		Shader::VarType_Vec4,
		-1,
		"base_t",
	};
}

/////////////////////////////////////////////////////
/// Default Constructor
/// Params: None
///
/////////////////////////////////////////////////////
Shader::Shader()
{
	m_ProgramId = renderer::INVALID_OBJECT;
	m_PrevProgramId = 0;

	m_OpenGLContext = nullptr;
}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
Shader::~Shader()
{	

}

/////////////////////////////////////////////////////
/// Method: Initialise
/// Params: None
///
/////////////////////////////////////////////////////
void Shader::Initialise()
{
	Init();
}

/////////////////////////////////////////////////////
/// Method: Shutdown
/// Params: None
///
/////////////////////////////////////////////////////
void Shader::Shutdown()
{
	Release();
}

/////////////////////////////////////////////////////
/// Method: Init
/// Params: None
///
/////////////////////////////////////////////////////
void Shader::Init()
{
	m_ProgramId = renderer::INVALID_OBJECT;
	m_PrevProgramId = 0;

	m_OpenGLContext = nullptr;
}

/////////////////////////////////////////////////////
/// Method: Release
/// Params: None
///
/////////////////////////////////////////////////////
void Shader::Release()
{
	if (m_ProgramId != renderer::INVALID_OBJECT)
		renderer::RemoveShaderProgram(m_ProgramId);
}

/////////////////////////////////////////////////////
/// Method: IsValid
/// Params: None
///
/////////////////////////////////////////////////////
bool Shader::IsValid()
{
	return (m_ProgramId != renderer::INVALID_OBJECT);
}

/////////////////////////////////////////////////////
/// Method: CreateFromFiles
/// Params: [in]vertexShader, [in]fragmentShader, [in]geometryShader, [in]openGLContext
///
/////////////////////////////////////////////////////
void Shader::CreateFromFiles(const std::string& vertexShader, const std::string& fragmentShader, const std::string& geometryShader, renderer::OpenGL* openGLContext)
{
	Release();

	if (openGLContext == nullptr)
		m_OpenGLContext = renderer::OpenGL::GetInstance();
	else
		m_OpenGLContext = openGLContext;
	DBG_ASSERT(m_OpenGLContext != nullptr);

	m_ProgramId = renderer::LoadShaderFilesForProgram(vertexShader, fragmentShader, geometryShader);
	DBG_ASSERT(m_ProgramId != renderer::INVALID_OBJECT);

	CacheUniforms();

	CacheAttributes();
}

/////////////////////////////////////////////////////
/// Method: Bind
/// Params: None
///
/////////////////////////////////////////////////////
void Shader::Bind()
{
	if (m_ProgramId != renderer::INVALID_OBJECT)
	{
		m_PrevProgramId = m_OpenGLContext->GetCurrentProgram();
		m_OpenGLContext->UseProgram(m_ProgramId);
	}
}
	
/////////////////////////////////////////////////////
/// Method: UnBind
/// Params: None
///
/////////////////////////////////////////////////////
void Shader::UnBind()
{
	if (m_PrevProgramId != renderer::INVALID_OBJECT)
		m_OpenGLContext->UseProgram(m_PrevProgramId);
	else
		m_OpenGLContext->UseProgram(0);
}	

/////////////////////////////////////////////////////
/// Method: SetUniform
/// Params: [in]uniform, [in]value
///
/////////////////////////////////////////////////////
void Shader::SetUniform(EShaderUniformName uniform, const GLfloat *value)
{
	GLint location = m_UniformMap[uniforms[uniform].uniformString].uniformLocation;
	renderer::Shader::EShaderVarType type = uniforms[uniform].varType;
	if (location != -1)
	{
		if (type == Shader::VarType_Vec2)
			glUniform2fv(location, 1, value);
		else
		if (type == Shader::VarType_Vec3)
			glUniform3fv(location, 1, value);
		else
		if (type == Shader::VarType_Vec4)
			glUniform4fv(location, 1, value);
		else
		if (type == Shader::VarType_Mat4)
			glUniformMatrix4fv(location, 1, GL_FALSE, value);
	}

	GL_CHECK
}
	
/////////////////////////////////////////////////////
/// Method: GetUniformLocation
/// Params: [in]uniform
///
/////////////////////////////////////////////////////
GLint Shader::GetUniformLocation(EShaderUniformName uniform)
{
	GLint location = m_UniformMap[uniforms[uniform].uniformString].uniformLocation;
	renderer::Shader::EShaderVarType type = uniforms[uniform].varType;

	return location;
}

/////////////////////////////////////////////////////
/// Method: GetAttributeLocation
/// Params: None
///
/////////////////////////////////////////////////////
GLint Shader::GetAttributeLocation(EShaderAttribName attr)
{
	GLint location = m_AttrMap[attribs[attr].attrString].attrLocation;
	renderer::Shader::EShaderVarType type = attribs[attr].varType;

	return location;
}

/////////////////////////////////////////////////////
/// Method: CacheUniforms
/// Params: None
///
/////////////////////////////////////////////////////
void Shader::CacheUniforms()
{
	m_UniformMap.empty();
	for (auto &u : uniforms) {

		Shader::TUniformBlock newBlock;
		newBlock.uniformIdent = u.uniformIdent;
		newBlock.varType = u.varType;
		newBlock.uniformLocation = glGetUniformLocation(m_ProgramId, u.uniformString.c_str());
		newBlock.uniformString = u.uniformString;

		m_UniformMap.insert(TUniformMapPair(u.uniformString, newBlock));
	}
}

/////////////////////////////////////////////////////
/// Method: CacheAttributes
/// Params: None
///
/////////////////////////////////////////////////////
void Shader::CacheAttributes()
{
	m_AttrMap.empty();
	for (auto &a : attribs) {

		Shader::TAttrBlock newBlock;
		newBlock.attrIdent = a.attrIdent;
		newBlock.varType = a.varType;
		newBlock.attrLocation = glGetAttribLocation(m_ProgramId, a.attrString.c_str());
		newBlock.attrString = a.attrString;

		m_AttrMap.insert(TAttrMapPair(a.attrString, newBlock));
	}
}
