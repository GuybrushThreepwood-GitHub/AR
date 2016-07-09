
#ifndef __FRAMEBUFFER_H__
#define __FRAMEBUFFER_H__

namespace renderer
{
	const short RTTFLAG_CREATE_COLOURBUFFER = 0x0001;
	const short RTTFLAG_CREATE_DEPTHBUFFER = 0x0002;
	const short RTTFLAG_CREATE_STENCILBUFFER = 0x0004;

	void InititaliseFBOShader();

	/// CheckFramebufferStatus - Checks the current state of the active framebuffer, and returns if it is valid
	/// \return integer - (SUCCESS: OK, FAIL: FAIL )
	int CheckFramebufferStatus();

	class Framebuffer
	{
		public:
			/// default constructor
			Framebuffer();
			/// default destructor
			~Framebuffer();

			/// Initialise - Sets up all the members variables for the class
			void Initialise();
			/// Release - Cleans up any texture or image memory from the class
			void Release();
			/// Create - Creates a GL texture object for receiving renders
			/// \param nTexWidth - texture Width
			/// \param nTexHeight - texture Height
			/// \param nTexInternalFormat - internal texture format
			/// \param nTexFormat - external texture format
			/// \param nTexType - texture type
			/// \param eTexWrapMode - texture wrapping mode
			/// \param bMipMap - generate mipmaps for the texture
			/// \return integer - (SUCCESS:0 or FAIL:1)
			int Create( int nTexWidth, int nTexHeight, short flags = (renderer::RTTFLAG_CREATE_COLOURBUFFER | renderer::RTTFLAG_CREATE_DEPTHBUFFER),
							GLenum nTexInternalFormat = GL_RGBA8, GLenum nTexFormat = GL_RGBA8, GLenum nTexType = GL_UNSIGNED_BYTE, 
							GLenum eTexWrapMode = GL_CLAMP_TO_EDGE, bool bMipMap = false );			

			/// CreateDepthOnly - Creates a GL texture object for receiving depth only renders
			/// \param nTexWidth - texture Width
			/// \param nTexHeight - texture Height
			/// \return integer - (SUCCESS:0 or FAIL:1)
			int CreateDepthOnly( int nTexWidth, int nTexHeight );		

			/// StartRenderToTexture - Prepares the texture for rendering
			/// \param mask - GL clear bits
			void StartRenderToTexture( GLbitfield mask = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
			// EndRenderToTexture - Cleans up the texture after rendering
			void EndRenderToTexture();

			/// Bind - Binds the texture
			/// \param nUnit - texture unit to bind the texture to
			void Bind( int nUnit );
			/// Bind - UnBinds the texture
			void UnBind();

			/// RenderToScreen - Renders the texture to a full screen quad
			/// \param nTexUnit - texture unit to bind to
			void RenderToScreen( int nTexUnit=0, bool restoreView=false );

			/// SetClearColour - Sets the clear colour
			/// \param vColour - colour to set
			void SetClearColour(const glm::vec4 &vColour)				{ vClearColour = vColour; }
			/// GetClearColour - Gets the clear colour
			/// \return Vec4 - returned colour
			glm::vec4 GetClearColour() const						{ return vClearColour; }
			/// SetQuadColour - Sets the colour of the full screen quad
			/// \param vNewColour - colour to set
			void SetQuadColour(const glm::vec4 &vNewColour)			{ vColour = vNewColour; }
			/// GetQuadColour - Gets the colour of the full screen quad
			/// \return Vec4 - returned colour
			glm::vec4 GetQuadColour() const						{ return vColour; }
			/// SetFullscreenFilter - Sets the filtering of the full screen textured quad
			/// \param eFilter - filter to set
			void SetFullscreenFilter( const GLenum eFilter )		{ eFullscreenFilter = eFilter; }
			/// GetFullscreenFilter - Gets the filtering of the full screen textured quad
			/// \return GLenum - returned filter
			GLenum GetFullscreenFilter( ) const				{ return eFullscreenFilter; }
			/// SetWidthHeight - Sets the render to texture, width and height
			/// \param nTexWidth - width of the texture
			/// \param nTexHeight - height of the texture 
			void SetWidthHeight( int nTexWidth, int nTexHeight );
			/// GetTexture - Returns the GL texture object id
			/// \return unsigned int - (SUCCESS: valid texture id FAIL: INVALID_OBJECT)
			GLuint GetTexture( )		{ return nID; }
			/// GetTarget - Returns the GL texture target
			/// \return GLenum - GL_TEXTURE_2D or GL_TEXTURE_RECTANGLE_ARB
			GLenum GetTarget( )		{ return eTarget; }
			/// IsValid - Returns whether the texture is valid to be used
			/// \return integer - (SUCCESS: OK FAIL: FAIL )
			int IsValid( )		{ return bValidTexture; }

			void SetUniform(renderer::Shader::EShaderUniformName uniform, const GLfloat *value);

			GLint GetUniformLocation(renderer::Shader::EShaderUniformName uniform);

			GLint GetAttributeLocation(renderer::Shader::EShaderAttribName attr);

			GLuint GetProgram();

		private:
			/// GL texture object id
			GLuint nID;
			/// clear colour
			glm::vec4 vClearColour;
			/// full screen colour
			glm::vec4 vColour;
			/// full screen uv coords
			glm::vec2 vUVCoords[4];
			/// GL FBO object id
			GLuint nFramebufferID;
			/// GL FBO depth render buffer object id
			GLuint nDepth_rb;
			/// GL FBO stencil render buffer object id
			GLuint nStencil_rb;
			/// width of the texture
			int nWidth;
			/// height of the texture
			int nHeight;
			/// bits-per-pixel of the texture
			int nBPP;
			/// flags whether the texture contains alpha information
			bool bHasAlpha;
			/// flags whether this FBO contains any mipmaps
			bool bHasMipMaps;
			/// depth only flag
			bool bDepthOnly;
			/// GL target
			GLenum eTarget;
			/// GL internal format
			GLenum eInternalFormat;
			/// GL external format
			GLenum eFormat;
			/// GL target
			GLenum eWrapMode;
			/// GL type
			GLenum eType;
			/// filter when applying to a fullscreen quad
			GLenum eFullscreenFilter;
			/// pointer to the actual image data
			unsigned char *image;
			/// pointer to the depth image data
			unsigned char *depthBuffer;
			/// whether the texture is initialised
			bool bInitialised;
			/// unit the texture is currently bound to
			int nUnit;
			/// flag whether the texture is valid, whether FBO or vanilla texture
			bool bValidTexture;
			/// previous viewport width before rendering to texture
			int nPreviousWidth;
			/// previous viewport height before rendering to texture
			int nPreviousHeight;
			/// if framebuffer drawing is nested, you need to know what framebuffer to switch back to
			GLuint nPreviousBoundFrameBuffer;

			GLint m_TextureSampler;

			GLint m_VertexAttr;
	};

} // namespace renderer

#endif // __FRAMEBUFFER_H__



