
#ifndef __SHADERSHARED_H__
#define __SHADERSHARED_H__

// forward declare
namespace renderer { class Shader; }

const int MAX_SHADER_SIZE		= (16*1024);
const int MAX_SHADER_LOG_SIZE	= (32*1024);

namespace renderer
{
	enum EShaderType
	{
		SHADERTYPE_UNKNOWN=0,
		
		SHADERTYPE_VERTEX_SHADER=1,
		SHADERTYPE_FRAGMENT_SHADER=2,
		SHADERTYPE_GEOMETRY_SHADER=3,

		SHADERTYPE_MISSING_UNSUPPORTED=9999,
	};


	/// ReloadShaderMap - attempts to reload all shader programs into their designated slots
	void ReloadShaderMap();
	/// ClearTextureMap - clears the shader name map
	void ClearShaderMap();
	/// RemoveShaderProgram - removes a shader program from the map
	/// \param programId - program object id
	void RemoveShaderProgram( GLuint programId );

	/// PrintInfoLog - prints a shader or program log
	void PrintInfoLog( GLuint objectId );

	/// LoadShader - Loads a shader from a file
	/// \param shaderType - what type of shader
	/// \param filename - name of file to load
	/// \return integer - (SUCCESS: GL shader id, FAIL: INVALID_OBJECT)
	GLuint LoadShader( GLenum shaderType, const std::string& filename );

	/// LoadShaderFilesForProgram - Loads a vertex and fragment shader to create a program
	/// \param vertexShader - vertex shader file
	/// \param fragmentShader - fragment shader file
	/// \param geometryShader - geometry shader file
	/// \return integer - (SUCCESS: GL shader id, FAIL: INVALID_OBJECT)
	GLuint LoadShaderFilesForProgram(const std::string& vertexShader, const std::string& fragmentShader, const std::string& geometryShader);

} // namespace renderer

#endif // __SHADERSHARED_H__

