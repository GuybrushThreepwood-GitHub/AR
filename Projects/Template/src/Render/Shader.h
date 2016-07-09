
#ifndef __SHADER_H__
#define __SHADER_H__

namespace renderer
{
	class Shader
	{
		public:
			enum EShaderUniformName
			{
				ModelViewProjectionMatrix = 0,
				ModelViewMatrix,
				ViewMatrix,
				NormalMatrix,
				VertexColour,

				TexUnit0,
				TexUnit1,
				TexUnit2,
				TexUnit3,

				TotalUniforms
			};

			enum EShaderAttribName
			{
				Vertex,
				TexCoord0,
				TexCoord1,
				TexCoord2,
				TexCoord3,
				Normal,
				Tangent,

				TotalAttribs
			};

			enum EShaderVarType
			{
				VarType_Int,

				VarType_Vec2,
				VarType_Vec3,
				VarType_Vec4,

				VarType_Mat4,

			};

			struct TUniformBlock
			{
				EShaderUniformName uniformIdent;
				EShaderVarType varType;
				GLint uniformLocation;
				std::string uniformString;
			};

			struct TAttrBlock
			{
				EShaderAttribName attrIdent;
				EShaderVarType varType;
				GLint attrLocation;
				std::string attrString;
			};

		public:
			/// Shader
			Shader();
			~Shader();

			void Initialise();
			void Shutdown();

			bool IsValid();

			void CreateFromFiles(const std::string& vertexShader, const std::string& fragmentShader, const std::string& geometryShader, renderer::OpenGL* openGLContext = nullptr);

			void Bind();
			void UnBind();

			void SetUniform(EShaderUniformName uniform, const GLfloat *value);
			GLint GetUniformLocation(EShaderUniformName uniform);

			GLint GetAttributeLocation(EShaderAttribName attr);

			GLuint GetProgram()		{ return m_ProgramId; }

		private:
			void Init();
			void Release();

			void CacheUniforms();
			void CacheAttributes();

		private:
			renderer::OpenGL* m_OpenGLContext;

			GLuint m_ProgramId;

			// uniforms
			std::map<std::string, Shader::TUniformBlock> m_UniformMap;
			typedef std::pair< std::string, Shader::TUniformBlock> TUniformMapPair;

			// attribs
			std::map<std::string, Shader::TAttrBlock> m_AttrMap;
			typedef std::pair< std::string, Shader::TAttrBlock> TAttrMapPair;

			GLuint m_PrevProgramId;
	};
	
} // namespace renderer

#endif // __SHADER_H__

