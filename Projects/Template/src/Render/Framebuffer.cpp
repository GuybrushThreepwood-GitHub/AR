
/*===================================================================
	File: Framebuffer.cpp
=====================================================================*/

#include "Boot/Includes.h"

using renderer::Framebuffer;

namespace
{
	renderer::Shader fboShader;

	glm::vec2 vertList[] =
	{
		glm::vec2(-1.0f, -1.0f),
		glm::vec2(1.0f, -1.0f),
		glm::vec2(-1.0f, 1.0f),
		glm::vec2(1.0f, 1.0f)
	};
}

/////////////////////////////////////////////////////
/// Function: InititaliseFBOShader
/// Params: None
///
/////////////////////////////////////////////////////
void renderer::InititaliseFBOShader()
{
	fboShader.CreateFromFiles("data/shaders/21/fullscreen-quad.vert", "data/shaders/21/fullscreen-quad.frag", "");
}

/////////////////////////////////////////////////////
/// Function: CheckFramebufferStatus
/// Params: None
///
/////////////////////////////////////////////////////
int renderer::CheckFramebufferStatus( void )
{
	GLenum eStatus;
	eStatus = (GLenum)glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
	switch(eStatus) 
	{
		case GL_FRAMEBUFFER_COMPLETE:
		{
			DBGLOG( "Framebuffer status: GL_FRAMEBUFFER_COMPLETE\n" );
			return 0;
		}break;
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
		{
			// Not all framebuffer attachment points are framebuffer
			// attachment complete. This means that at least one
			// attachment point with a renderbuffer or texture 
			// attached has its attached object no longer in existence
			// or has an attached image with a width or height of 
			// zero, or the color attachment point has a 
			// non-color-renderable image attached, or the
			// depth attachment point has a non-depth-renderable
			// image attached, or the stencil attachment point has a
			// non-stencil-renderable image attached.
                    
			// Color-renderable formats include GL_RGBA4,
			// GL_RGB5_A1, and
			// GL_RGB565.
			// GL_DEPTH_COMPONENT16 is the only
			// depth-renderable format. 
			// GL_STENCIL_INDEX8 is the only
			// stencil-renderable format.
			DBGLOG( "Framebuffer status: GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT\n" );
			return 1;
		}break;
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
		{
			// No images are attached to the framebuffer.
			DBGLOG( "Framebuffer status: GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT\n" );
			return 1;
		}break;
		case GL_FRAMEBUFFER_UNSUPPORTED:
		{
			// The combination of internal formats of the attached
			// images violates an implementation-dependent set of
			// restrictions.
			DBGLOG( "Framebuffer status: GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT\n" );
			return 1;
		}break;
		case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
		{
			DBGLOG("Framebuffer status: GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER\n");
			return 1;
		}break;
		case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
		{
			DBGLOG( "Framebuffer status: GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER\n" );
			return 1;
		}break;

		default:
			DBG_ASSERT(0);
	}

	return 1;
}

/////////////////////////////////////////////////////
/// Default constructor
/// 
///
/////////////////////////////////////////////////////
Framebuffer::Framebuffer()
{
	Initialise();
}

/////////////////////////////////////////////////////
/// Default destructor
/// 
///
/////////////////////////////////////////////////////
Framebuffer::~Framebuffer()
{
	Release();
}

/////////////////////////////////////////////////////
/// Method: Initialise
/// Params: None
///
/////////////////////////////////////////////////////
void Framebuffer::Initialise( )
{
	nID = renderer::INVALID_OBJECT;
	vClearColour = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	vColour = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	nWidth = 0;
	nHeight = 0;
	nBPP = 0;
	bHasAlpha = false;
	bHasMipMaps = false;
	bDepthOnly = false;
	eInternalFormat = GL_RGBA8;
	eFormat = GL_RGB5_A1;
	eWrapMode = GL_CLAMP_TO_EDGE;

	eFullscreenFilter = GL_NEAREST;

	image = 0;
	depthBuffer = 0;
	bInitialised = false;
	nUnit = 0;
	bValidTexture = false;

	nPreviousWidth = 0;
	nPreviousHeight = 0;
	nPreviousBoundFrameBuffer = renderer::INVALID_OBJECT;

	nID = INVALID_OBJECT;
	nFramebufferID = renderer::INVALID_OBJECT;
	nDepth_rb = renderer::INVALID_OBJECT;
	nStencil_rb = renderer::INVALID_OBJECT;
}

/////////////////////////////////////////////////////
/// Method: Release
/// Params: None 
///
/////////////////////////////////////////////////////
void Framebuffer::Release( )
{
	nWidth = 0;
	nHeight = 0;
	nBPP = 0;
	bHasAlpha = false;
	bHasMipMaps = false;
	bDepthOnly = false;
	eInternalFormat = GL_RGBA8;
	eFormat = GL_RGB5_A1;
	eWrapMode = GL_CLAMP_TO_EDGE;

	// remove any texture data
	if( nID != renderer::INVALID_OBJECT )
	{
		glDeleteTextures(1, &nID);
		nID = renderer::INVALID_OBJECT;
	}

	if( nDepth_rb != renderer::INVALID_OBJECT )
	{
		if( glIsRenderbuffer(nDepth_rb) )
			glDeleteRenderbuffers( 1, &nDepth_rb );
		nDepth_rb = renderer::INVALID_OBJECT;
	}

	if( nStencil_rb != renderer::INVALID_OBJECT )
	{
		if( glIsRenderbuffer(nStencil_rb) )
			glDeleteRenderbuffers( 1, &nStencil_rb );
		nDepth_rb = renderer::INVALID_OBJECT;
	}

	if( nFramebufferID != renderer::INVALID_OBJECT )
	{
		if( glIsFramebuffer(nFramebufferID) )
			glDeleteFramebuffers( 1, &nFramebufferID );
		nFramebufferID = renderer::INVALID_OBJECT;
	}

	if( image )
	{
		delete image;
		image = 0;
	}

	if( depthBuffer )
	{
		delete depthBuffer;
		depthBuffer = 0;
	}
	
	bInitialised = false;
	nUnit = 0;
	bValidTexture = false;
}

/////////////////////////////////////////////////////
/// Method: Create
/// Params: [in]nTexWidth, [in]nTexHeight, [in]eTexTarget, [in]nTexInternalFormat, [in]nTexFormat, [in]nTexType, [in]eTexWrapMode, [in]bMipMap
///
/////////////////////////////////////////////////////
int Framebuffer::Create( int nTexWidth, int nTexHeight, short flags, GLenum nTexInternalFormat, GLenum nTexFormat, GLenum nTexType, GLenum eTexWrapMode, bool bMipMap )
{
	GLenum activeMinFilter=GL_LINEAR, activeMagFilter=GL_LINEAR;

	bHasMipMaps = bMipMap;
	eInternalFormat = nTexInternalFormat;
	eFormat = nTexFormat;
	eType = nTexType;
	nWidth = nTexWidth;
	nHeight = nTexHeight;
	eWrapMode = eTexWrapMode;
	bHasMipMaps = bMipMap;

	eTarget = GL_TEXTURE_2D;

	// some sanity checks
	/*if( !GLEW_OES_rgb8_rgba8 )
	{
		// probably on iOS or Android where 8 bpp for render buffers doesn't exist
		if( eInternalFormat == GL_RGB8 )
			eInternalFormat = GL_RGB;
		else if( eInternalFormat == GL_RGBA8 )
			eInternalFormat = GL_RGBA;

		if( eFormat == GL_RGB8 )
			eFormat = GL_RGBA;
		else if ( eFormat == GL_RGBA8 )
			eFormat = GL_RGBA;
	}*/

	// depth format
	GLenum eDepthFormat = GL_DEPTH_COMPONENT16;

	//if( GLEW_OES_depth32 )
	//{
	//	eDepthFormat = GL_DEPTH_COMPONENT32;
	//}
	//else
	//if( GLEW_OES_depth24 )
	//{
	//	eDepthFormat = GL_DEPTH_COMPONENT24;
	//}
	//else
		eDepthFormat = GL_DEPTH_COMPONENT16;

	/*int nIsHeightPOW2 = renderer::IsPowerOfTwo( nTexHeight );
	int nIsWidthPOW2 = renderer::IsPowerOfTwo( nTexWidth );

	// no NPOT extension, downscale the width and height
	if( !nIsHeightPOW2 )
		nHeight = renderer::GetPreviousPowerOfTwo( nTexHeight, renderer::OpenGL::GetInstance()->GetMaxRenderBufferSize() );
	if( !nIsWidthPOW2 )
		nWidth = renderer::GetPreviousPowerOfTwo( nTexWidth, renderer::OpenGL::GetInstance()->GetMaxRenderBufferSize() );

	if( !nIsHeightPOW2 || !nIsWidthPOW2 )
		DBGLOG( "FRAMEBUFFER: *WARNING*  RTT had NPOT size and GL_ARB_texture_non_power_of_two is not available\n" );
		*/
	GL_CHECK

	// create colour buffer, depth and vanilla texture ids
	glGenFramebuffers( 1, &nFramebufferID );
	glGenTextures( 1, &nID );
	glGenRenderbuffers(1, &nDepth_rb );
  
	// if this function is called during another bound framebuffer, need to know what ID to switch back to
	nPreviousBoundFrameBuffer = renderer::OpenGL::GetInstance()->GetActiveFrameBuffer();
	
	renderer::OpenGL::GetInstance()->BindFrameBufferObject( nFramebufferID );

	// init texture
	renderer::OpenGL::GetInstance()->BindUnitTexture( 0, eTarget, nID );
	glTexImage2D( eTarget, 0, eInternalFormat, nWidth, nHeight, 0, eFormat, eType, 0 );
		
	GL_CHECK

	// check for mipmaps, cannot mipmap rectangular textures
	if( bHasMipMaps )
	{
		// generate mipmaps for the target
		glGenerateMipmap( eTarget );
	}
	else
	{
		// default to bilinear
		activeMagFilter = GL_LINEAR;
		activeMinFilter = GL_LINEAR;
	}

	GL_CHECK
		
	glTexParameterf( eTarget, GL_TEXTURE_MIN_FILTER, (GLfloat)activeMinFilter );
	glTexParameterf( eTarget, GL_TEXTURE_MAG_FILTER, (GLfloat)activeMagFilter );

	glTexParameterf( eTarget, GL_TEXTURE_WRAP_S, (GLfloat)eWrapMode );
	glTexParameterf( eTarget, GL_TEXTURE_WRAP_T, (GLfloat)eWrapMode );

	if( flags & renderer::RTTFLAG_CREATE_COLOURBUFFER )
		glFramebufferTexture2D( GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, eTarget, nID, 0 );

	GL_CHECK

	// bind the depth render buffer and attach its ID
	if( flags & renderer::RTTFLAG_CREATE_DEPTHBUFFER )
	{
		glBindRenderbuffer( GL_RENDERBUFFER, nDepth_rb );
		glRenderbufferStorage( GL_RENDERBUFFER, eDepthFormat, nWidth, nHeight );
		glFramebufferRenderbuffer( GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, nDepth_rb );

		GL_CHECK
	}

	DBGLOG( "FRAMEBUFFER: Created FBO - TextureID (%d) FrameBufferID(%d) DepthRenderBufferID (%d)\n", nID, nFramebufferID, nDepth_rb );

	if( CheckFramebufferStatus() == 0 )
	{
		if( nPreviousBoundFrameBuffer > 0 &&
		   nPreviousBoundFrameBuffer != renderer::INVALID_OBJECT )
		{
			// clear the texture bind, and bind the frame buffer
			renderer::OpenGL::GetInstance()->BindUnitTexture( 0, eTarget, 0 );
			
			// rebind any previous framebuffer
			renderer::OpenGL::GetInstance()->BindFrameBufferObject( nPreviousBoundFrameBuffer );
		}
		else
		{
			// make sure the frame buffer isn't enabled while returning
			renderer::OpenGL::GetInstance()->ClearFrameBufferObject();
		}

		fboShader.Bind();

		// cache the uniforms and attributes
		m_TextureSampler = fboShader.GetUniformLocation(renderer::Shader::TexUnit0);
		if (m_TextureSampler != -1)
			glUniform1i(m_TextureSampler, 0);

		m_VertexAttr = fboShader.GetAttributeLocation(renderer::Shader::Vertex);

		fboShader.UnBind();
		
		bValidTexture = true;
		return 0;
	}

	if( nPreviousBoundFrameBuffer > 0 &&
	   nPreviousBoundFrameBuffer != renderer::INVALID_OBJECT )
	{
		// clear the texture bind, and bind the frame buffer
		renderer::OpenGL::GetInstance()->BindUnitTexture( 0, eTarget, 0 );
		
		// rebind any previous framebuffer
		renderer::OpenGL::GetInstance()->BindFrameBufferObject( nPreviousBoundFrameBuffer );
	}
	else
	{
		// make sure the frame buffer isn't enabled while returning
		renderer::OpenGL::GetInstance()->ClearFrameBufferObject();
	}
	bValidTexture = false;

	return 1;
}

/////////////////////////////////////////////////////
/// Method: CreateDepthOnly
/// Params: [in]nTexWidth, [in]nTexHeight
///
/////////////////////////////////////////////////////
int Framebuffer::CreateDepthOnly( int nTexWidth, int nTexHeight)
{
//	if( !GLEW_OES_depth_texture )
//		return Create( nTexWidth, nTexHeight, (renderer::RTTFLAG_CREATE_COLOURBUFFER | renderer::RTTFLAG_CREATE_DEPTHBUFFER), GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, GL_CLAMP_TO_EDGE, false );

	// if RGBA is used, to get Z data it can be packed differently
/*
const highp vec4 packFactors = vec4(256.0 * 256.0 * 256.0, 256.0 * 256.0, 256.0, 1.0);
const highp vec4 cutoffMask  = vec4(0.0, 1.0/256.0, 1.0/256.0, 1.0/256.0);

void main() {
    highp vec4 packedVal = vec4(fract(packFactors*gl_FragCoord.z));
    gl_FragColor = packedVal - packedVal.xxyz*cutoffMask;
}
*/

	bHasMipMaps = 0;

	eInternalFormat = GL_DEPTH_COMPONENT; // this is always GL_DEPTH_COMPONENT
	eFormat = GL_DEPTH_COMPONENT;
	eType = GL_UNSIGNED_INT; // _SHORT for 16 bit _INT for 24-32?
	nWidth = nTexWidth;
	nHeight = nTexHeight;

	eWrapMode = GL_CLAMP_TO_EDGE;

	eTarget = GL_TEXTURE_2D;

	// filtering mode
	GLenum eFilterMode = GL_LINEAR;

	/*int nIsHeightPOW2 = renderer::IsPowerOfTwo( nTexHeight );
	int nIsWidthPOW2 = renderer::IsPowerOfTwo( nTexWidth );

	// no NPOT extension, downscale the width and height
	if( !nIsHeightPOW2 )
		nHeight = renderer::GetPreviousPowerOfTwo( nTexHeight, renderer::OpenGL::GetInstance()->GetMaxRenderBufferSize() );
	if( !nIsWidthPOW2 )
		nWidth = renderer::GetPreviousPowerOfTwo( nTexWidth, renderer::OpenGL::GetInstance()->GetMaxRenderBufferSize() );

	if( !nIsHeightPOW2 || !nIsWidthPOW2 )
		DBGLOG( "FRAMEBUFFER: *WARNING*  RTT had NPOT size and GL_ARB_texture_non_power_of_two is not available\n" );
	*/

	// 
	glGenFramebuffers( 1, &nFramebufferID );
	glGenTextures( 1, &nID );
	
	// if this function is called during another bound framebuffer, need to know what ID to switch back to
	nPreviousBoundFrameBuffer = renderer::OpenGL::GetInstance()->GetActiveFrameBuffer();
	
	renderer::OpenGL::GetInstance()->BindFrameBufferObject( nFramebufferID );

	// init texture
	renderer::OpenGL::GetInstance()->BindUnitTexture( 0, eTarget, nID );
	glTexImage2D( eTarget, 0, eInternalFormat, nWidth, nHeight, 0, eFormat, eType, 0 );

	GL_CHECK
		
	// depth cannot be mipmapped, so force the correct filters
	glTexParameterf( eTarget, GL_TEXTURE_MIN_FILTER, (GLfloat)eFilterMode );
	glTexParameterf( eTarget, GL_TEXTURE_MAG_FILTER, (GLfloat)eFilterMode );

	glTexParameterf( eTarget, GL_TEXTURE_WRAP_S, (GLfloat)eWrapMode );
	glTexParameterf( eTarget, GL_TEXTURE_WRAP_T, (GLfloat)eWrapMode );
	
	//glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);
	
	// No colour buffer to draw to or read from
#if defined(BASE_PLATFORM_WINDOWS) || defined(BASE_PLATFORM_MAC)
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
#endif
	
	// Set up depth_tex for render-to-texture
	glFramebufferTexture2D( GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, eTarget, nID, 0 );

	GL_CHECK
		
	DBGLOG( "FRAMEBUFFER: Created FBO Depth - TextureID (%d) FrameBufferID(%d)\n", nID, nFramebufferID );

	if( CheckFramebufferStatus() == 0 )
	{
		bValidTexture = true;
		bDepthOnly = true;
		
		if( nPreviousBoundFrameBuffer > 0 &&
		   nPreviousBoundFrameBuffer != renderer::INVALID_OBJECT )
		{
			// clear the texture bind, and bind the frame buffer
			renderer::OpenGL::GetInstance()->BindUnitTexture( 0, eTarget, 0 );
			
			// rebind any previous framebuffer
			renderer::OpenGL::GetInstance()->BindFrameBufferObject( nPreviousBoundFrameBuffer );
		}
		else
		{
			// make sure the frame buffer isn't enabled while returning
			renderer::OpenGL::GetInstance()->ClearFrameBufferObject( );
		}
		
		fboShader.Bind();

		// cache the uniforms and attributes
		m_TextureSampler = fboShader.GetUniformLocation(renderer::Shader::TexUnit0);
		if (m_TextureSampler != -1)
			glUniform1i(m_TextureSampler, 0);

		m_VertexAttr = fboShader.GetAttributeLocation(renderer::Shader::Vertex);

		fboShader.UnBind();

		return 0;
	}

	bValidTexture = false;
	
	if( nPreviousBoundFrameBuffer > 0 &&
	   nPreviousBoundFrameBuffer != renderer::INVALID_OBJECT )
	{
		// clear the texture bind, and bind the frame buffer
		renderer::OpenGL::GetInstance()->BindUnitTexture( 0, eTarget, 0 );
		
		// rebind any previous framebuffer
		renderer::OpenGL::GetInstance()->BindFrameBufferObject( nPreviousBoundFrameBuffer );
	}
	else
	{
		// make sure the frame buffer isn't enabled while returning
		renderer::OpenGL::GetInstance()->ClearFrameBufferObject( );
		
	}
	
	return 1;
}
/////////////////////////////////////////////////////
/// Method: StartRenderToTexture
/// Params: None
///
/////////////////////////////////////////////////////
void Framebuffer::StartRenderToTexture( GLbitfield mask )
{
	if( !bValidTexture )
		return;

	// FBO
	if( nFramebufferID != renderer::INVALID_OBJECT )
	{
		// if this function is called during another bound framebuffer, need to know what ID to switch back to
		nPreviousBoundFrameBuffer = renderer::OpenGL::GetInstance()->GetActiveFrameBuffer();

		// clear the texture bind, and bind the frame buffer
		renderer::OpenGL::GetInstance()->BindUnitTexture( 0, eTarget, 0 );

		renderer::OpenGL::GetInstance()->BindFrameBufferObject( nFramebufferID );
	}

	glClearColor( vClearColour.r, vClearColour.g, vClearColour.b, vClearColour.a );	

	renderer::OpenGL::GetInstance()->GetWidthHeight( &nPreviousWidth, &nPreviousHeight );

	renderer::OpenGL::GetInstance()->SetViewport( nWidth, nHeight );

	glClear( mask );

	GL_CHECK
}

/////////////////////////////////////////////////////
/// Method: EndRenderToTexture
/// Params: None
///
/////////////////////////////////////////////////////
void Framebuffer::EndRenderToTexture( )
{
	if( !bValidTexture )
		return;

	GL_CHECK

	// reset viewport
	renderer::OpenGL::GetInstance()->SetViewport( nPreviousWidth, nPreviousHeight );

	// FBO
	if( nFramebufferID != renderer::INVALID_OBJECT )
	{
		// unbind the framebuffer
		renderer::OpenGL::GetInstance()->ClearFrameBufferObject( );

		// bind the texture
		renderer::OpenGL::GetInstance()->BindUnitTexture( 0, eTarget, nID );

		// mip the texture?
		if( bHasMipMaps )
		{
			// generate mipmaps for the target
			glGenerateMipmap( eTarget );
		}
	}

	if( nPreviousBoundFrameBuffer > 0 &&
	   nPreviousBoundFrameBuffer != renderer::INVALID_OBJECT )
	{
		// clear the texture bind, and bind the frame buffer
		renderer::OpenGL::GetInstance()->BindUnitTexture( 0, eTarget, 0 );

		// rebind any previous framebuffer
		renderer::OpenGL::GetInstance()->BindFrameBufferObject( nPreviousBoundFrameBuffer );
	}

	GL_CHECK
}

/////////////////////////////////////////////////////
/// Method: Bind
/// Params: [in]nTexUnit
///
/////////////////////////////////////////////////////
void Framebuffer::Bind( int nTexUnit )
{
	if( !bValidTexture )
		return;

	if( nID != renderer::INVALID_OBJECT )
	{
		renderer::OpenGL::GetInstance()->BindUnitTexture( nTexUnit, eTarget, nID );
		nUnit = nTexUnit;
	}
}

/////////////////////////////////////////////////////
/// Method: UnBind
/// Params: None
///
/////////////////////////////////////////////////////
void Framebuffer::UnBind( )
{
	if( !bValidTexture )
		return;

	// unbind the framebuffer
	renderer::OpenGL::GetInstance()->DisableUnitTexture( nUnit );
}

/////////////////////////////////////////////////////
/// Method: RenderToScreen
/// Params: [in]nTexUnit, [in]nScreenWidth, [in]nScreenHeight
///
/////////////////////////////////////////////////////
void Framebuffer::RenderToScreen(int nTexUnit, bool restoreView)
{
	if( !bValidTexture )
		return;

	renderer::OpenGL::GetInstance()->DisableVBO();
	renderer::OpenGL::GetInstance()->DepthMode( true, GL_ALWAYS );
	renderer::OpenGL::GetInstance()->BlendMode( false, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	fboShader.Bind();

	TViewState SavedView;
	if (restoreView)
		renderer::OpenGL::GetInstance()->SaveViewState( &SavedView );

	// bind the texture
	Bind( nTexUnit );

	if( eFullscreenFilter != GL_NEAREST )
	{
		glTexParameterf( eTarget, GL_TEXTURE_MIN_FILTER, (GLfloat)eFullscreenFilter );
		glTexParameterf( eTarget, GL_TEXTURE_MAG_FILTER, (GLfloat)eFullscreenFilter );
	}
	else
	{
		glTexParameterf( eTarget, GL_TEXTURE_MIN_FILTER, (GLfloat)GL_NEAREST );
		glTexParameterf( eTarget, GL_TEXTURE_MAG_FILTER, (GLfloat)GL_NEAREST );
	}
	
	if( m_VertexAttr != -1 )
	{
		glEnableVertexAttribArray(m_VertexAttr);
		glVertexAttribPointer(m_VertexAttr, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), &vertList[0]);
	}

	glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
	
	// unbind the texture, so it affects nothing after it
	UnBind();

	if (m_VertexAttr != -1)
	{
		glDisableVertexAttribArray(m_VertexAttr);
	}

	fboShader.UnBind();

	// reset view
	if (restoreView)
		renderer::OpenGL::GetInstance()->SetViewState( &SavedView );
}

/////////////////////////////////////////////////////
/// Method: SetWidthHeight
/// Params: [in]nTexWidth, [in]nTexHeight
///
/////////////////////////////////////////////////////
void Framebuffer::SetWidthHeight( int nTexWidth, int nTexHeight )
{
	nWidth = nTexWidth;
	nHeight = nTexHeight;
}

/////////////////////////////////////////////////////
/// Method: SetUniform
/// Params: [in]uniform, [in]value
///
/////////////////////////////////////////////////////
void Framebuffer::SetUniform(renderer::Shader::EShaderUniformName uniform, const GLfloat *value)
{
	fboShader.SetUniform(uniform, value);

	GL_CHECK
}

/////////////////////////////////////////////////////
/// Method: GetUniformLocation
/// Params: [in]uniform
///
/////////////////////////////////////////////////////
GLint Framebuffer::GetUniformLocation(renderer::Shader::EShaderUniformName uniform)
{
	return fboShader.GetUniformLocation(uniform);
}

/////////////////////////////////////////////////////
/// Method: GetAttributeLocation
/// Params: None
///
/////////////////////////////////////////////////////
GLint Framebuffer::GetAttributeLocation(renderer::Shader::EShaderAttribName attr)
{
	return fboShader.GetAttributeLocation(attr);
}

GLuint Framebuffer::GetProgram()
{
	return fboShader.GetProgram();
}