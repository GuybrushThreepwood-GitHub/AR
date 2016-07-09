
/*===================================================================
	File: OpenGL.cpp
	Library: Render

	(C)Hidden Games
=====================================================================*/

#include "Boot/Includes.h"

#ifdef USE_OPENGL41

using renderer::OpenGL;

//OpenGL* OpenGL::ms_Instance = nullptr;
std::unique_ptr<OpenGL> OpenGL::ms_Instance = nullptr;

namespace renderer
{
	const int VENDOR_VENDOR_SIZE = 64;
	const int VENDOR_VERSION_SIZE = 64;
	const int VENDOR_RENDERER_SIZE = 64;
	const int VENDOR_EXTENSIONS_SIZE = 8192;

} // namespace renderer

/////////////////////////////////////////////////////
/// Method: Initialise
/// Params: None
///
/////////////////////////////////////////////////////
void OpenGL::Initialise( void )
{
	DBG_ASSERT((ms_Instance == nullptr));

	//ms_Instance = new OpenGL;
	ms_Instance.reset(new OpenGL);
}

/////////////////////////////////////////////////////
/// Method: Shutdown
/// Params: None
///
/////////////////////////////////////////////////////
void OpenGL::Shutdown( void )
{
	/*if( ms_Instance )
	{
		delete ms_Instance;
		ms_Instance = nullptr;
	}*/
	ms_Instance.reset(nullptr);
	ms_Instance = nullptr;
}

/////////////////////////////////////////////////////
/// Default constructor
/// 
///
/////////////////////////////////////////////////////
OpenGL::OpenGL( )
{
	GLint i = 0;

	// initialise GL state values
	std::memset( &m_GLState, 0, sizeof(TGLState) );
	for( i = 0; i < MAX_USEABLE_TEXUNITS; i++ )
	{
		m_GLState.TexUnits[i].nCurrent1DTexture = renderer::INVALID_OBJECT;
		m_GLState.TexUnits[i].nCurrent2DTexture = renderer::INVALID_OBJECT;
		m_GLState.TexUnits[i].nCurrentRectangleTexture = renderer::INVALID_OBJECT;

		m_GLState.TexUnits[i].nCurrentCubemapTexture = renderer::INVALID_OBJECT;
	}

	m_GLState.bVBOActive			= false;

	m_GLState.nCurrentProgram		= renderer::INVALID_OBJECT;

	m_GLState.bTextureState			= true;
	m_GLState.nCurrentTexture		= renderer::INVALID_OBJECT;
	m_GLState.nCurrentFrameBuffer	= renderer::INVALID_OBJECT;

	m_GLState.bFillMode = true;
	m_GLState.bLightingState = false;

	for( i = 0; i < MAX_USEABLE_LIGHTS; i++ )
	{
		m_GLState.LightUnits[i].bLightState = false;
		m_GLState.LightUnits[i].vAmbientColour	= glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		if( i == 0 )
		{
			m_GLState.LightUnits[i].vDiffuseColour = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
			m_GLState.LightUnits[i].vSpecularColour = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		}
		else
		{
			m_GLState.LightUnits[i].vDiffuseColour = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
			m_GLState.LightUnits[i].vSpecularColour = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		}
		m_GLState.LightUnits[i].vEmissionColour = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

		m_GLState.LightUnits[i].vPosition = glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);
		m_GLState.LightUnits[i].vSpotDirection = glm::vec3(0.0f, 0.0f, -1.0f);
		m_GLState.LightUnits[i].fSpotExponent	= 0.0f;
		m_GLState.LightUnits[i].fSpotCutOff		= 180.0f;
		m_GLState.LightUnits[i].vAttenuation = glm::vec3(1.0f, 0.0f, 0.0f);
	}

	m_GLState.bDepthState	= true;
	m_GLState.eDepthFunc	= GL_LESS;
	m_GLState.bBlendState	= false;
	m_GLState.eBlendSrc		= GL_SRC_ALPHA;
	m_GLState.eBlendDest	= GL_ONE_MINUS_SRC_ALPHA;

	m_GLState.bCullState = true;
	m_GLState.eCullFace = GL_BACK;
	m_GLState.eFrontFace = GL_CCW;

	m_GLState.bFogState = false;
	m_GLState.eFogMode = 0;
	m_GLState.vFogColour = glm::vec3(0.0f, 0.0f, 0.0f);
	m_GLState.fFogNearClip = 0.0f;
	m_GLState.fFogFarClip = 1.0f;
	m_GLState.fFogDensity = 1.0f;

	m_GLState.vColour = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

	m_Width		= 640;
	m_Height	= 480;
	m_BPP		= 32;
	m_ZBuffer	= 24;
	m_AlphaBits = 8;


	m_ProjMatrix = glm::mat4(1.0f);
	m_ViewMatrix = glm::mat4(1.0f);
	m_ModlMatrix = glm::mat4(1.0f);
	m_ClipMatrix = glm::mat4(1.0f);

	m_eViewState = VIEWSTATE_PERSPECTIVE;
	m_FOV		= 60.0f;
	m_AspectRatio = 1.0f;
	m_NearClip = 1.0f;
	m_FarClip	= 1000.0f;

	m_vClearColour = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	m_ClearBits = (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_MaxTextureSize = 1;
	m_MaxTextureUnits = 1;
	m_MaxRenderBufferSize = 1;

	nTotalTriangleCount = 0;
	nTotalVertexCount = 0;
	nTotalMaterialCount = 0;
	nTotalTextureCount = 0;
}

/////////////////////////////////////////////////////
/// Default destructor
/// 
///
/////////////////////////////////////////////////////
OpenGL::~OpenGL( )
{
	Release();
}

/////////////////////////////////////////////////////
/// Method: ResetGLState
/// Params: None
///
/////////////////////////////////////////////////////
void OpenGL::ResetGLState()
{
    GLint i = 0;
    
	// initialise GL state values
	std::memset( &m_GLState, 0, sizeof(TGLState) );
	for( i = 0; i < MAX_USEABLE_TEXUNITS; i++ )
	{
		m_GLState.TexUnits[i].nCurrent1DTexture = renderer::INVALID_OBJECT;
		m_GLState.TexUnits[i].nCurrent2DTexture = renderer::INVALID_OBJECT;
		m_GLState.TexUnits[i].nCurrentRectangleTexture = renderer::INVALID_OBJECT;
        
		m_GLState.TexUnits[i].nCurrentCubemapTexture = renderer::INVALID_OBJECT;
	}
    
	m_GLState.bVBOActive			= false;
    
	m_GLState.nCurrentProgram		= renderer::INVALID_OBJECT;
    
	m_GLState.bTextureState			= true;
	m_GLState.nCurrentTexture		= renderer::INVALID_OBJECT;
	m_GLState.nCurrentFrameBuffer	= renderer::INVALID_OBJECT;
    
	m_GLState.bFillMode = true;
	m_GLState.bLightingState = false;
    
	for( i = 0; i < MAX_USEABLE_LIGHTS; i++ )
	{
		m_GLState.LightUnits[i].bLightState = false;
		m_GLState.LightUnits[i].vAmbientColour = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		if( i == 0 )
		{
			m_GLState.LightUnits[i].vDiffuseColour = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
			m_GLState.LightUnits[i].vSpecularColour = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		}
		else
		{
			m_GLState.LightUnits[i].vDiffuseColour = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
			m_GLState.LightUnits[i].vSpecularColour = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		}
		m_GLState.LightUnits[i].vEmissionColour = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        
		m_GLState.LightUnits[i].vPosition = glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);
		m_GLState.LightUnits[i].vSpotDirection = glm::vec3(0.0f, 0.0f, -1.0f);
		m_GLState.LightUnits[i].fSpotExponent	= 0.0f;
		m_GLState.LightUnits[i].fSpotCutOff		= 180.0f;
		m_GLState.LightUnits[i].vAttenuation = glm::vec3(1.0f, 0.0f, 0.0f);
	}
    
	m_GLState.bDepthState	= true;
	m_GLState.eDepthFunc	= GL_LESS;
	m_GLState.bBlendState	= false;
	m_GLState.eBlendSrc		= GL_SRC_ALPHA;
	m_GLState.eBlendDest	= GL_ONE_MINUS_SRC_ALPHA;
    
	m_GLState.bCullState = true;
	m_GLState.eCullFace = GL_BACK;
	m_GLState.eFrontFace = GL_CCW;
    
	m_GLState.bFogState = false;
	m_GLState.eFogMode = 0;
	m_GLState.vFogColour = glm::vec3(0.0f, 0.0f, 0.0f);
	m_GLState.fFogNearClip = 0.0f;
	m_GLState.fFogFarClip = 1.0f;
	m_GLState.fFogDensity = 1.0f;
    
	m_GLState.vColour = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    
	m_Width		= 640;
	m_Height	= 480;
	m_BPP		= 32;
	m_ZBuffer	= 24;
	m_AlphaBits = 8;
    
	m_ProjMatrix = glm::mat4(1.0f);
	m_ViewMatrix = glm::mat4(1.0f);
	m_ModlMatrix = glm::mat4(1.0f);
	m_ClipMatrix = glm::mat4(1.0f);
    
	m_eViewState = VIEWSTATE_PERSPECTIVE;
	m_FOV		= 60.0f;
	m_AspectRatio = 1.0f;
	m_NearClip = 1.0f;
	m_FarClip	= 1000.0f;
    
	m_vClearColour = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	m_ClearBits = (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
////
	UseProgram( 0 );
	m_GLState.nCurrentProgram = 0;
    
	// now do some simple OpenGL initialisation
	
	glClearColor( m_vClearColour.r, m_vClearColour.g, m_vClearColour.b, m_vClearColour.a );
	//SetClearBits( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    
	glEnable( GL_CULL_FACE );
	glCullFace( GL_BACK );
	//SetCullState( true, GL_BACK );
	glFrontFace(GL_CCW);
    
	glEnable( GL_DEPTH_TEST );
    
	glActiveTexture( GL_TEXTURE0 );
	glBindTexture( GL_TEXTURE_2D, 0 );
    
	//SetNearFarClip( 1.0f, 10000.0f );
	//SetFieldOfView( 60.0f );
    
	glDepthMask( GL_TRUE );
	glDepthFunc( GL_LESS );
	//DepthMode( true, GL_LESS );
    
	glDisable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	//BlendMode( false, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
   
}

/////////////////////////////////////////////////////
/// Method: Init
/// Params: None
///
/////////////////////////////////////////////////////
void OpenGL::Init( void )
{
	ogl_LoadFunctions();

	char szRenderer[renderer::VENDOR_RENDERER_SIZE];
	char szVersion[renderer::VENDOR_VERSION_SIZE];
	char szVendor[renderer::VENDOR_VENDOR_SIZE];
	
	snprintf( szRenderer, renderer::VENDOR_RENDERER_SIZE, "Renderer: %s\n", glGetString( GL_RENDERER ) );
	snprintf( szVersion, renderer::VENDOR_VERSION_SIZE, "Version: %s\n", glGetString( GL_VERSION ) );

	snprintf( szVendor, renderer::VENDOR_VENDOR_SIZE, "Vendor: %s\n", glGetString( GL_VENDOR ) );

	DBGLOG( "\n" );
	DBGLOG( "OpenGL driver info\n" );
	DBGLOG( "--------------------------------------\n" );
	DBGLOG( "%s", szRenderer );
	DBGLOG( "--------------------------------------\n" );
	DBGLOG( "%s", szVersion );
	DBGLOG( "--------------------------------------\n" );
	DBGLOG( "%s", szVendor );
	DBGLOG( "--------------------------------------\n" );
	DBGLOG( "extensions:\n" );

	GLint numExt, i;
	glGetIntegerv(GL_NUM_EXTENSIONS, &numExt);
	for (i = 0; i < numExt; i++) {

		const char* currExtension = (const char*)glGetStringi(GL_EXTENSIONS, i);
		DBGLOG("%s\n", glGetStringi(GL_EXTENSIONS, i));
	}
	DBGLOG( "--------------------------------------\n" );
	DBGLOG( "\n" );

	glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE, &m_MaxRenderBufferSize);
	DBGLOG("GL_MAX_RENDERBUFFER_SIZE: (%d)\n", m_MaxRenderBufferSize);

	UseProgram( 0 );
	m_GLState.nCurrentProgram = 0;

	// now do some simple OpenGL initialisation
	
	glClearColor( m_vClearColour.r, m_vClearColour.g, m_vClearColour.b, m_vClearColour.a );		
	SetClearBits( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glEnable( GL_CULL_FACE );
	glCullFace( GL_BACK );
	SetCullState( true, GL_BACK );		
	glFrontFace(GL_CCW);

	glEnable( GL_DEPTH_TEST );

	glActiveTexture( GL_TEXTURE0 );
	glBindTexture( GL_TEXTURE_2D, 0 );

	SetNearFarClip( 1.0f, 10000.0f );
	SetFieldOfView( 60.0f );

	glDepthMask( GL_TRUE );
	glDepthFunc( GL_LESS );
	DepthMode( true, GL_LESS );

	glDisable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	BlendMode( false, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

//	renderer::InitialisePrimitives();

	// check for errors
	GL_CHECK
}

/////////////////////////////////////////////////////
/// Method: Release
/// Params:
///
/////////////////////////////////////////////////////
void OpenGL::Release( void )
{
	DBGLOG( "----- OpenGL Shutdown -----\n" );

	UseProgram(0);

	DBGLOG( "----- OpenGL Shutdown Complete-----\n" );
}

/////////////////////////////////////////////////////
/// Method: UseProgram
/// Params: [in]program
///
/////////////////////////////////////////////////////
void OpenGL::UseProgram( GLuint program )
{
	//if( m_GLState.nCurrentProgram == program )
	//	return;

	if( program == 0 ||
		program == renderer::INVALID_OBJECT )
	{
		ClearProgram();
		return;
	}

	if( glIsProgram(program) )
	{
		glUseProgram( program );
		m_GLState.nCurrentProgram = program;
	}
	else
	{
		ClearProgram();
		return;
	}

	// check for errors
	GL_CHECK
}
			
/////////////////////////////////////////////////////
/// Method: ClearProgram
/// Params: None
///
/////////////////////////////////////////////////////
void OpenGL::ClearProgram()
{
	m_GLState.nCurrentProgram = renderer::INVALID_OBJECT;
	glUseProgram(0);

	// check for errors
	GL_CHECK
}

/////////////////////////////////////////////////////
/// Method: DisableVBO
/// Params: None
///
/////////////////////////////////////////////////////
void OpenGL::DisableVBO( void )
{
	// already off?
	//if( m_GLState.bVBOActive == false )
	//	return;

	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	m_GLState.bVBOActive = false;

	// check for errors
	GL_CHECK
}

/////////////////////////////////////////////////////
/// Method: SetCullState
/// Params: [in]bCull, [in]eFace
///
/////////////////////////////////////////////////////
void OpenGL::SetCullState( bool bCull, GLenum eFace )
{
	if( m_GLState.bCullState != bCull )
	{
		if( bCull )
			glEnable( GL_CULL_FACE );
		else
			glDisable( GL_CULL_FACE );

		m_GLState.bCullState = bCull;
	}

	if( m_GLState.eCullFace != eFace )
	{
		glCullFace( eFace );

		m_GLState.eCullFace = eFace;
	}

	// check for errors
	GL_CHECK
}

/////////////////////////////////////////////////////
/// Method: BindFrameBufferObject
/// Params: [in]nFrameBufferID
///
/////////////////////////////////////////////////////
void OpenGL::BindFrameBufferObject( GLuint nFrameBufferID )
{
	if( nFrameBufferID == INVALID_OBJECT )
		return;

	if( nFrameBufferID != renderer::INVALID_OBJECT )
	{
		glBindFramebuffer( GL_FRAMEBUFFER, nFrameBufferID );
		m_GLState.nCurrentFrameBuffer = nFrameBufferID;	
	}

	// check for errors
	GL_CHECK
}

/////////////////////////////////////////////////////
/// Method: ClearUnitTextures
/// Params: None
///
/////////////////////////////////////////////////////
void OpenGL::ClearUnitTextures( void )
{
	GLint i = 0;

	// go through each unit and set it to nothing
	for( i = 0; i < m_MaxTextureUnits; i++ )
	{
		GLenum eActiveTexUnit = GL_TEXTURE0 + i;

		// make sure multitexturing is allowed
		glActiveTexture( eActiveTexUnit );

		glBindTexture( GL_TEXTURE_CUBE_MAP, 0 );
		m_GLState.TexUnits[i].nCurrentCubemapTexture = 0;

		glBindTexture( GL_TEXTURE_2D, 0 );
		m_GLState.TexUnits[i].nCurrent2DTexture = 0;

		m_GLState.nCurrentTexture = 0;
	}

	// check for errors
	GL_CHECK
}

/////////////////////////////////////////////////////
/// Method: ClearUnitTexture
/// Params: [in]nTexUnit
///
/////////////////////////////////////////////////////
void OpenGL::ClearUnitTexture( GLint nTexUnit )
{
	// make sure it's a valid unit
	if( nTexUnit >= 0 /*&& nTexUnit < m_MaxTextureUnits*/ )
	{
		// make sure multitexturing is allowed, or if it's only unit 0 allow it
		// which unit are we working on
		GLenum eActiveTexUnit = GL_TEXTURE0 + nTexUnit;

		glActiveTexture( eActiveTexUnit );	

		glBindTexture( GL_TEXTURE_CUBE_MAP, 0 );
		m_GLState.TexUnits[nTexUnit].nCurrentCubemapTexture = 0;

		glBindTexture( GL_TEXTURE_2D, 0 );
		m_GLState.TexUnits[nTexUnit].nCurrent2DTexture = 0;

		m_GLState.nCurrentTexture = 0;
	}

	// check for errors
	GL_CHECK
}

/////////////////////////////////////////////////////
/// Method: DisableUnitTexture
/// Params: [in]nTexUnit
///
/////////////////////////////////////////////////////
void OpenGL::DisableUnitTexture( GLint nTexUnit )
{
	// make sure it's a valid unit
	if( nTexUnit >= 0 /*&& nTexUnit < m_MaxTextureUnits*/ )
	{
		ClearUnitTexture( nTexUnit );

		// make sure multitexturing is allowed, or if it's only unit 0 allow it
		// which unit are we working on
		//GLenum eActiveTexUnit = GL_TEXTURE0 + nTexUnit;

		//glActiveTexture( eActiveTexUnit );	
		//glBindTexture( GL_TEXTURE_2D, 0 );
	}

	// check for errors
	GL_CHECK
}

/////////////////////////////////////////////////////
/// Method: ClearFrameBufferObject
/// Params: None
///
/////////////////////////////////////////////////////
void OpenGL::ClearFrameBufferObject( void )
{
	// unbind any framebuffer
	glBindFramebuffer( GL_FRAMEBUFFER, 0 );
	m_GLState.nCurrentFrameBuffer = 0;

	// check for errors
	GL_CHECK
}

/////////////////////////////////////////////////////
/// Method: DepthMode
/// Params: [in]bDepthFlag, [in]eDepthFunc
///
/////////////////////////////////////////////////////
void OpenGL::DepthMode( bool bDepthFlag, GLenum eDepthFunc )
{
	if( m_GLState.bDepthState != bDepthFlag )
	{
		glDepthMask( bDepthFlag );

		m_GLState.bDepthState = bDepthFlag;
	}

	if( m_GLState.eDepthFunc != eDepthFunc )
	{
		glDepthFunc( eDepthFunc );

		m_GLState.eDepthFunc = eDepthFunc;
	}

	// check for errors
	GL_CHECK
}

/////////////////////////////////////////////////////
/// Method: BlendMode
/// Params: [in]bBlendFlag, [in]eBlendSrc, [in]eBlendDest
///
/////////////////////////////////////////////////////
void OpenGL::BlendMode( bool bBlendFlag, GLenum eBlendSrc, GLenum eBlendDest )
{
	if( m_GLState.bBlendState != bBlendFlag )
	{
		if( bBlendFlag )
			glEnable( GL_BLEND );
		else
			glDisable( GL_BLEND );

		m_GLState.bBlendState = bBlendFlag;
	}

	if( m_GLState.eBlendSrc != eBlendSrc || m_GLState.eBlendDest != eBlendDest )
	{
		glBlendFunc( eBlendSrc, eBlendDest );

		m_GLState.eBlendSrc = eBlendSrc;
		m_GLState.eBlendDest = eBlendDest;
	}

	// check for errors
	GL_CHECK
}

/////////////////////////////////////////////////////
/// Method: SetColour4f
/// Params: [in]r, [in]g, [in]b, [in]a
///
/////////////////////////////////////////////////////
void OpenGL::SetColour( GLfloat r, GLfloat g, GLfloat b, GLfloat a )
{
	/*if( m_GLState.vColour4f.R != r || 
		m_GLState.vColour4f.G != g ||
		m_GLState.vColour4f.B != b ||
		m_GLState.vColour4f.A != a )*/
	{
		m_GLState.vColour = glm::vec4(r, g, b, a);

		if( m_GLState.nCurrentProgram != 0 &&
			m_GLState.nCurrentProgram != renderer::INVALID_OBJECT )
		{
			GLint ogl_VertexColour = glGetUniformLocation(m_GLState.nCurrentProgram, "ogl_VertexColour");
			if( ogl_VertexColour != -1 )
			{
				glUniform4f( ogl_VertexColour, m_GLState.vColour.r, m_GLState.vColour.g, m_GLState.vColour.b, m_GLState.vColour.a );
			}
		}
	}

	// check for errors
	GL_CHECK
}

/////////////////////////////////////////////////////
/// Method: ClearScreen
/// Params: None
///
/////////////////////////////////////////////////////
void OpenGL::ClearScreen( void )
{
	// Set background clearing colour
	glClearColor( m_vClearColour.r, m_vClearColour.g, m_vClearColour.b, m_vClearColour.a );		
	
	// Clear the window with current clearing colour
	glClear( m_ClearBits );

	// always reset to perspective
	glViewport( 0, 0, m_Width, m_Height );

	// check for errors
	GL_CHECK
}

/////////////////////////////////////////////////////
/// Method: ClearColour
/// Params: [in]fRed, [in]fGreen, [in]fBlue, [in]fAlpha
///
/////////////////////////////////////////////////////
void OpenGL::ClearColour( GLfloat fRed, GLfloat fGreen, GLfloat fBlue, GLfloat fAlpha )
{
	m_vClearColour = glm::vec4(fRed, fGreen, fBlue, fAlpha);

	// check for errors
	GL_CHECK
}

/////////////////////////////////////////////////////
/// Method: SetClearBits
/// Params: [in]bits
///
/////////////////////////////////////////////////////
void OpenGL::SetClearBits( GLbitfield bits )
{
	m_ClearBits = bits;
}

/////////////////////////////////////////////////////
/// Method: SetupPerspectiveView
/// Params: [in]nWidth, [in]nHeight
///
/////////////////////////////////////////////////////
void OpenGL::SetupPerspectiveView( GLint nWidth, GLint nHeight, bool bForceUpdate )
{
	if( nHeight == 0 )
		nHeight = 1;

	if( nWidth == 0 )
		nWidth = 1;

	// don't change if it's already there
	if( !bForceUpdate )
	{
		if( m_Width == nWidth &&
			m_Height == nHeight && 
			m_eViewState == VIEWSTATE_PERSPECTIVE )
			return;
	}

	m_Width = nWidth;
	m_Height = nHeight;

	m_eViewState = VIEWSTATE_PERSPECTIVE;

	// set perspective
    glm::mat4 scale = glm::mat4(1.0f);
    glm::mat4 rotate = glm::mat4(1.0f);
    
    m_ProjMatrix = scale * glm::perspective(m_FOV, m_AspectRatio, m_NearClip, m_FarClip) * rotate;

	// reset modelview
	m_ModlMatrix = glm::mat4(1.0f);
	m_ViewMatrix = glm::mat4(1.0f);
	
	m_Viewport[0] = 0;
	m_Viewport[1] = 0;
	m_Viewport[2] = nWidth;
	m_Viewport[3] = nHeight;

	// check for errors
	GL_CHECK
}

/////////////////////////////////////////////////////
/// Method: SetupOrthographicView
/// Params: [in]nWidth, [in]nHeight
///
/////////////////////////////////////////////////////
void OpenGL::SetupOrthographicView( GLint nWidth, GLint nHeight, bool bForceUpdate )
{
	if( nHeight == 0 )
		nHeight = 1;

	if( nWidth == 0 )
		nWidth = 1;

	// don't change if it's already there
	if( !bForceUpdate )
	{
		if( m_Width == nWidth &&
			m_Height == nHeight && 
			m_eViewState == VIEWSTATE_ORTHOGRAPHIC )
			return;
	}

	m_Width = nWidth;
	m_Height = nHeight;

	m_eViewState = VIEWSTATE_ORTHOGRAPHIC;

    glm::mat4 scale = glm::mat4(1.0f);
    glm::mat4 rotate = glm::mat4(1.0f);
    
    m_ProjMatrix = scale * glm::ortho( 0.0f, static_cast<GLfloat>(m_Width), 0.0f, static_cast<GLfloat>(m_Height), m_NearClip, m_FarClip ) * rotate;
	
	// this translation seems to correct pixel shifting according to the GL tips section in the red book
	m_ModlMatrix = glm::translate( glm::mat4(1.0f), glm::vec3(0.4f, 0.4f, 0.0f) );
	m_ViewMatrix = glm::mat4(1.0f);
	
	m_Viewport[0] = 0;
	m_Viewport[1] = 0;
	m_Viewport[2] = nWidth;
	m_Viewport[3] = nHeight;

	// check for errors
	GL_CHECK
}

/////////////////////////////////////////////////////
/// Method: SetNearFarClip
/// Params: [in]fNearClip, [in]fFarClip
///
/////////////////////////////////////////////////////
void OpenGL::SetNearFarClip( GLfloat fNearClip, GLfloat fFarClip )
{
	if( fFarClip < fNearClip )
		DBGLOG( "OPENGL: *WARNING* the far clip distance has been set less than the near clip\n" );

	m_NearClip = fNearClip;
	m_FarClip = fFarClip;

	// check for errors
	GL_CHECK
}

/////////////////////////////////////////////////////
/// Method: SetFieldOfView
/// Params: [in]fAngle
///
/////////////////////////////////////////////////////
void OpenGL::SetFieldOfView( GLfloat fAngle )
{
	if( fAngle > 270.0f )
	{
		DBGLOG( "OPENGL: *WARNING* the FOV angle has been requested greater than 270degs forcing it to 270deg\n" );
		fAngle = 270.0f;
	}

	m_FOV = fAngle;

	// check for errors
	GL_CHECK
}

/////////////////////////////////////////////////////
/// Method: SetViewport
/// Params: [in]nWidth, [in]nHeight
///
/////////////////////////////////////////////////////
void OpenGL::SetViewport( GLint nWidth, GLint nHeight )
{
	if( nHeight == 0 )
		nHeight = 1;

	if( nWidth == 0 )
		nWidth = 1;

	// don't change if it's already there
	if( m_Width == nWidth &&
		m_Height == nHeight )
		return;

	m_Width = nWidth;
	m_Height = nHeight;

	glViewport( 0, 0, nWidth, nHeight );
	m_Viewport[0] = 0;
	m_Viewport[1] = 0;
	m_Viewport[2] = nWidth;
	m_Viewport[3] = nHeight;

	// check for errors
	GL_CHECK
}

/////////////////////////////////////////////////////
/// Method: SetPerspective
/// Params: [in]fFOV, [in]fAspectRatio, [in]fNearClip, [in]fFarClip
///
/////////////////////////////////////////////////////
void OpenGL::SetPerspective( GLfloat fFOV, GLfloat fAspectRatio, GLfloat fNearClip, GLfloat fFarClip, bool force )
{
	// don't change if it's already there
	if( force == false &&
		m_FOV == fFOV &&
		m_NearClip == fNearClip &&
		m_FarClip == fFarClip &&
		m_AspectRatio == fAspectRatio &&
		m_eViewState == VIEWSTATE_PERSPECTIVE )
		return;

	m_FOV = fFOV;
	m_NearClip = fNearClip;
	m_FarClip = fFarClip;
	m_AspectRatio = fAspectRatio;
	m_eViewState = VIEWSTATE_PERSPECTIVE;

	// set perspective
    glm::mat4 scale = glm::mat4(1.0f);
    glm::mat4 rotate = glm::mat4(1.0f);
    
    m_ProjMatrix = scale * glm::perspective(m_FOV, m_AspectRatio, m_NearClip, m_FarClip) * rotate;

	// reset modelview
	m_ModlMatrix = glm::mat4(1.0f);
	m_ViewMatrix = glm::mat4(1.0f);
	
	// check for errors
	GL_CHECK
}

/////////////////////////////////////////////////////
/// Method: SaveDepthBlendAlphaState
/// Params: [in/out]pState
///
/////////////////////////////////////////////////////
void OpenGL::SaveDepthBlendAlphaState( renderer::TGLDepthBlendAlphaState *pState )
{
	DBG_ASSERT( pState != 0 );

	if( pState )
	{
		// depth
		pState->bDepthState		= m_GLState.bDepthState;
		pState->eDepthFunc		= m_GLState.eDepthFunc;

		// blend
		pState->bBlendState		= m_GLState.bBlendState;
		pState->eBlendSrc		= m_GLState.eBlendSrc;
		pState->eBlendDest		= m_GLState.eBlendDest;
	}

	// check for errors
	GL_CHECK
}

/////////////////////////////////////////////////////
/// Method: SetDepthBlendAlphaState
/// Params: [in]State
///
/////////////////////////////////////////////////////
void OpenGL::SetDepthBlendAlphaState( renderer::TGLDepthBlendAlphaState* pState )
{
	DBG_ASSERT( pState != 0 );

	DepthMode( pState->bDepthState, pState->eDepthFunc );

	BlendMode( pState->bBlendState, pState->eBlendSrc, pState->eBlendDest );

	// check for errors
	GL_CHECK
}

/////////////////////////////////////////////////////
/// Method: SaveFogState
/// Params: [in/out]pState
///
/////////////////////////////////////////////////////
void OpenGL::SaveFogState( renderer::TGLFogState *pState )
{
	if( pState )
	{
		// fog
		pState->bFogState		= m_GLState.bFogState;
		pState->vFogColour		= m_GLState.vFogColour;
		pState->eFogMode		= m_GLState.eFogMode;
		pState->fFogNearClip	= m_GLState.fFogNearClip;
		pState->fFogFarClip		= m_GLState.fFogFarClip;
		pState->fFogDensity		= m_GLState.fFogDensity;
	}

	// check for errors
	GL_CHECK
}

/////////////////////////////////////////////////////
/// Method: SetFogState
/// Params: [in]pState
///
/////////////////////////////////////////////////////
void OpenGL::SetFogState( renderer::TGLFogState* pState )
{
	DBG_ASSERT( pState != 0 );

	// check for errors
	GL_CHECK
}

/////////////////////////////////////////////////////
/// Method: SaveViewState
/// Params: [in/out]pState
///
/////////////////////////////////////////////////////
void OpenGL::SaveViewState( renderer::TViewState* pState )
{
	if( pState )
	{
		pState->eViewState = m_eViewState;
		pState->fFOV = m_FOV;
		pState->fNearClip = m_NearClip;
		pState->fFarClip = m_FarClip;
		pState->nWidth = m_Width;
		pState->nHeight = m_Height;
		pState->fAspectRatio = m_AspectRatio;
	}

	// check for errors
	GL_CHECK
}

/////////////////////////////////////////////////////
/// Method: SetViewState
/// Params: [in]viewState
///
/////////////////////////////////////////////////////
void OpenGL::SetViewState( renderer::TViewState* pViewState )
{
	DBG_ASSERT( pViewState != 0 );

	m_eViewState = pViewState->eViewState;
	m_FOV = pViewState->fFOV;
	m_NearClip	= pViewState->fNearClip;
	m_FarClip = pViewState->fFarClip;
	m_Width = pViewState->nWidth;
	m_Height = pViewState->nHeight;

	if( m_Height == 0 )
		m_Height = 1;

	if( m_Width == 0 )
		m_Width = 1;

	if( m_eViewState == VIEWSTATE_PERSPECTIVE )
		SetupPerspectiveView( m_Width, m_Height );

	if( m_eViewState == VIEWSTATE_ORTHOGRAPHIC )
		SetupOrthographicView( m_Width, m_Height );

	// check for errors
	GL_CHECK
}

/////////////////////////////////////////////////////
/// Method: SaveCullState
/// Params: [in]viewState
///
/////////////////////////////////////////////////////
void OpenGL::SaveCullState( renderer::TGLCullFaceState* pState )
{
	if( pState )
	{
		pState->bCullState	= m_GLState.bCullState;
		pState->eCullFace	= m_GLState.eCullFace;
	}

	// check for errors
	GL_CHECK
}

/////////////////////////////////////////////////////
/// Method: SetCullState
/// Params: [in]cullState
///
/////////////////////////////////////////////////////
void OpenGL::SetCullState( renderer::TGLCullFaceState* pCullState )
{
	DBG_ASSERT( pCullState != 0 );

	SetCullState( pCullState->bCullState, pCullState->eCullFace );

	// check for errors
	GL_CHECK
}

/////////////////////////////////////////////////////
/// Method: Flip
/// Params: None
///
/////////////////////////////////////////////////////
void OpenGL::Flip( void )
{
//	glFinish();

	// FIXME: clear all units for the start of next frame?
	//ClearUnitTextures();

	GL_CHECK

	// reset the counters
	nTotalTriangleCount = 0;
	nTotalVertexCount = 0;
	nTotalMaterialCount = 0;
	nTotalTextureCount = 0;
}

/////////////////////////////////////////////////////
/// Method: ExtractFrustum
/// Params: None
///
/////////////////////////////////////////////////////
void OpenGL::ExtractFrustum( void )
{
	GLfloat	t;

	const float *pMdlMtx = (const float*)glm::value_ptr(m_ModlMatrix);
	const float *pProjMtx = (const float*)glm::value_ptr(m_ProjMatrix);
	float *pClipMtx = (float*)glm::value_ptr(m_ClipMatrix);

	// combine the two matrices (proj*mdlview)
	pClipMtx[0] = pMdlMtx[0]*pProjMtx[0] + pMdlMtx[1]*pProjMtx[4] + pMdlMtx[2]*pProjMtx[8] + pMdlMtx[3]*pProjMtx[12];
	pClipMtx[1] = pMdlMtx[0]*pProjMtx[1] + pMdlMtx[1]*pProjMtx[5] + pMdlMtx[2]*pProjMtx[9] + pMdlMtx[3]*pProjMtx[13];
	pClipMtx[2] = pMdlMtx[0]*pProjMtx[2] + pMdlMtx[1]*pProjMtx[6] + pMdlMtx[2]*pProjMtx[10] + pMdlMtx[3]*pProjMtx[14];
	pClipMtx[3] = pMdlMtx[0]*pProjMtx[3] + pMdlMtx[1]*pProjMtx[7] + pMdlMtx[2]*pProjMtx[11] + pMdlMtx[3]*pProjMtx[15];

	pClipMtx[4] = pMdlMtx[4]*pProjMtx[0] + pMdlMtx[5]*pProjMtx[4] + pMdlMtx[6]*pProjMtx[8] + pMdlMtx[7]*pProjMtx[12];
	pClipMtx[5] = pMdlMtx[4]*pProjMtx[1] + pMdlMtx[5]*pProjMtx[5] + pMdlMtx[6]*pProjMtx[9] + pMdlMtx[7]*pProjMtx[13];
	pClipMtx[6] = pMdlMtx[4]*pProjMtx[2] + pMdlMtx[5]*pProjMtx[6] + pMdlMtx[6]*pProjMtx[10] + pMdlMtx[7]*pProjMtx[14];
	pClipMtx[7] = pMdlMtx[4]*pProjMtx[3] + pMdlMtx[5]*pProjMtx[7] + pMdlMtx[6]*pProjMtx[11] + pMdlMtx[7]*pProjMtx[15];

	pClipMtx[8] = pMdlMtx[8]*pProjMtx[0] + pMdlMtx[9]*pProjMtx[4] + pMdlMtx[10]*pProjMtx[8] + pMdlMtx[11]*pProjMtx[12];
	pClipMtx[9] = pMdlMtx[8]*pProjMtx[1] + pMdlMtx[9]*pProjMtx[5] + pMdlMtx[10]*pProjMtx[9] + pMdlMtx[11]*pProjMtx[13];
	pClipMtx[10] = pMdlMtx[8]*pProjMtx[2] + pMdlMtx[9]*pProjMtx[6] + pMdlMtx[10]*pProjMtx[10] + pMdlMtx[11]*pProjMtx[14];
	pClipMtx[11] = pMdlMtx[8]*pProjMtx[3] + pMdlMtx[9]*pProjMtx[7] + pMdlMtx[10]*pProjMtx[11] + pMdlMtx[11]*pProjMtx[15];

	pClipMtx[12] = pMdlMtx[12]*pProjMtx[0] + pMdlMtx[13]*pProjMtx[4] + pMdlMtx[14]*pProjMtx[8] + pMdlMtx[15]*pProjMtx[12];
	pClipMtx[13] = pMdlMtx[12]*pProjMtx[1] + pMdlMtx[13]*pProjMtx[5] + pMdlMtx[14]*pProjMtx[9] + pMdlMtx[15]*pProjMtx[13];
	pClipMtx[14] = pMdlMtx[12]*pProjMtx[2] + pMdlMtx[13]*pProjMtx[6] + pMdlMtx[14]*pProjMtx[10] + pMdlMtx[15]*pProjMtx[14];
	pClipMtx[15] = pMdlMtx[12]*pProjMtx[3] + pMdlMtx[13]*pProjMtx[7] + pMdlMtx[14]*pProjMtx[11] + pMdlMtx[15]*pProjMtx[15];

	// the right plane
	m_Frustum[0][0] = pClipMtx[3] - pClipMtx[0];
	m_Frustum[0][1] = pClipMtx[7] - pClipMtx[4];
	m_Frustum[0][2] = pClipMtx[11] - pClipMtx[8];
	m_Frustum[0][3] = pClipMtx[15] - pClipMtx[12];

	// normalise
	t = glm::sqrt(m_Frustum[0][0] * m_Frustum[0][0] + m_Frustum[0][1] * m_Frustum[0][1] + m_Frustum[0][2] * m_Frustum[0][2]);
	m_Frustum[0][0] /= t;
	m_Frustum[0][1] /= t;
	m_Frustum[0][2] /= t;
	m_Frustum[0][3] /= t;

	// the left plane
	m_Frustum[1][0] = pClipMtx[3] + pClipMtx[0];
	m_Frustum[1][1] = pClipMtx[7] + pClipMtx[4];
	m_Frustum[1][2] = pClipMtx[11] + pClipMtx[8];
	m_Frustum[1][3] = pClipMtx[15] + pClipMtx[12];

	// normalise
	t = glm::sqrt(m_Frustum[1][0] * m_Frustum[1][0] + m_Frustum[1][1] * m_Frustum[1][1] + m_Frustum[1][2] * m_Frustum[1][2]);
	m_Frustum[1][0] /= t;
	m_Frustum[1][1] /= t;
	m_Frustum[1][2] /= t;
	m_Frustum[1][3] /= t;

	// bottom plane
	m_Frustum[2][0] = pClipMtx[3] + pClipMtx[1];
	m_Frustum[2][1] = pClipMtx[7] + pClipMtx[5];
	m_Frustum[2][2] = pClipMtx[11] + pClipMtx[9];
	m_Frustum[2][3] = pClipMtx[15] + pClipMtx[13];

	// normalise
	t = glm::sqrt(m_Frustum[2][0] * m_Frustum[2][0] + m_Frustum[2][1] * m_Frustum[2][1] + m_Frustum[2][2] * m_Frustum[2][2]);
	m_Frustum[2][0] /= t;
	m_Frustum[2][1] /= t;
	m_Frustum[2][2] /= t;
	m_Frustum[2][3] /= t;

	// top plane
	m_Frustum[3][0] = pClipMtx[3] - pClipMtx[1];
	m_Frustum[3][1] = pClipMtx[7] - pClipMtx[5];
	m_Frustum[3][2] = pClipMtx[11] - pClipMtx[9];
	m_Frustum[3][3] = pClipMtx[15] - pClipMtx[13];

	// normalise
	t = glm::sqrt(m_Frustum[3][0] * m_Frustum[3][0] + m_Frustum[3][1] * m_Frustum[3][1] + m_Frustum[3][2] * m_Frustum[3][2]);
	m_Frustum[3][0] /= t;
	m_Frustum[3][1] /= t;
	m_Frustum[3][2] /= t;
	m_Frustum[3][3] /= t;

	// far plane
	m_Frustum[4][0] = pClipMtx[3] - pClipMtx[2];
	m_Frustum[4][1] = pClipMtx[7] - pClipMtx[6];
	m_Frustum[4][2] = pClipMtx[11] - pClipMtx[10];
	m_Frustum[4][3] = pClipMtx[15] - pClipMtx[14];

	// normalise
	t = glm::sqrt(m_Frustum[4][0] * m_Frustum[4][0] + m_Frustum[4][1] * m_Frustum[4][1] + m_Frustum[4][2] * m_Frustum[4][2]);
	m_Frustum[4][0] /= t;
	m_Frustum[4][1] /= t;
	m_Frustum[4][2] /= t;
	m_Frustum[4][3] /= t;

	// near plane
	m_Frustum[5][0] = pClipMtx[3] + pClipMtx[2];
	m_Frustum[5][1] = pClipMtx[7] + pClipMtx[6];
	m_Frustum[5][2] = pClipMtx[11] + pClipMtx[10];
	m_Frustum[5][3] = pClipMtx[15] + pClipMtx[14];

	// normalise
	t = glm::sqrt(m_Frustum[5][0]*m_Frustum[5][0] + m_Frustum[5][1]*m_Frustum[5][1] + m_Frustum[5][2]*m_Frustum[5][2] );
	m_Frustum[5][0] /= t;
	m_Frustum[5][1] /= t;
	m_Frustum[5][2] /= t;
	m_Frustum[5][3] /= t;
}

/////////////////////////////////////////////////////
/// Method: PointInFrustum
/// Params: [in]X, [in]Y, [in]Z
///
/////////////////////////////////////////////////////
bool OpenGL::PointInFrustum( GLfloat X, GLfloat Y, GLfloat Z )
{
	GLint nPlaneNum;

	for( nPlaneNum = 0; nPlaneNum < 6; nPlaneNum++ )
		if(m_Frustum[nPlaneNum][0] * X + m_Frustum[nPlaneNum][1] * Y + 
			m_Frustum[nPlaneNum][2] * Z + m_Frustum[nPlaneNum][3] <= 0 )
			return false;

	return true;
}

/////////////////////////////////////////////////////
/// Method: SphereInFrustum
/// Params: [in]X, [in]Y, [in]Z, [in]fRadius
///
/////////////////////////////////////////////////////
bool OpenGL::SphereInFrustum( GLfloat X, GLfloat Y, GLfloat Z, GLfloat fRadius )
{
	GLint nPlaneNum;

	for( nPlaneNum = 0; nPlaneNum < 6; nPlaneNum++ )
		if(m_Frustum[nPlaneNum][0] * X + m_Frustum[nPlaneNum][1] * Y + 
			m_Frustum[nPlaneNum][2] * Z + m_Frustum[nPlaneNum][3] <= -fRadius )
			return false;

	return true;
}

/////////////////////////////////////////////////////
/// Method: CubeInFrustum
/// Params: [in]X, [in]Y, [in]Z, [in]fSize
///
/////////////////////////////////////////////////////
bool OpenGL::CubeInFrustum( GLfloat X, GLfloat Y, GLfloat Z, GLfloat fSize )
{
	for(GLint i = 0; i < 6; i++ )
	{
		if(m_Frustum[i][0] * (X - fSize) + m_Frustum[i][1] * (Y - fSize) + m_Frustum[i][2] * (Z - fSize) + m_Frustum[i][3] > 0)
		   continue;
		if(m_Frustum[i][0] * (X + fSize) + m_Frustum[i][1] * (Y - fSize) + m_Frustum[i][2] * (Z - fSize) + m_Frustum[i][3] > 0)
		   continue;
		if(m_Frustum[i][0] * (X - fSize) + m_Frustum[i][1] * (Y + fSize) + m_Frustum[i][2] * (Z - fSize) + m_Frustum[i][3] > 0)
		   continue;
		if(m_Frustum[i][0] * (X + fSize) + m_Frustum[i][1] * (Y + fSize) + m_Frustum[i][2] * (Z - fSize) + m_Frustum[i][3] > 0)
		   continue;
		if(m_Frustum[i][0] * (X - fSize) + m_Frustum[i][1] * (Y - fSize) + m_Frustum[i][2] * (Z + fSize) + m_Frustum[i][3] > 0)
		   continue;
		if(m_Frustum[i][0] * (X + fSize) + m_Frustum[i][1] * (Y - fSize) + m_Frustum[i][2] * (Z + fSize) + m_Frustum[i][3] > 0)
		   continue;
		if(m_Frustum[i][0] * (X - fSize) + m_Frustum[i][1] * (Y + fSize) + m_Frustum[i][2] * (Z + fSize) + m_Frustum[i][3] > 0)
		   continue;
		if(m_Frustum[i][0] * (X + fSize) + m_Frustum[i][1] * (Y + fSize) + m_Frustum[i][2] * (Z + fSize) + m_Frustum[i][3] > 0)
		   continue;

		// If we get here, it isn't in the frustum
		return false;
	}

	return true;
}

/////////////////////////////////////////////////////
/// Method: GetDepthMode
/// Params: [out]bDepthFlag, [out]eDepthFunc
///
/////////////////////////////////////////////////////
void OpenGL::GetDepthMode( bool *bDepthFlag, GLenum *eDepthFunc )
{
	if( bDepthFlag )
		*bDepthFlag = m_GLState.bDepthState;
	if( eDepthFunc )
		*eDepthFunc = m_GLState.eDepthFunc;
}

/////////////////////////////////////////////////////
/// Method: GetBlendMode
/// Params: [out]bBlendFlag, [out]eBlendSrc, [out]eBlendDest
///
/////////////////////////////////////////////////////
void OpenGL::GetBlendMode( bool *bBlendFlag, GLenum *eBlendSrc, GLenum *eBlendDest )
{
	if( bBlendFlag )
		*bBlendFlag = m_GLState.bBlendState;
	if( eBlendSrc )
		*eBlendSrc = m_GLState.eBlendSrc;
	if( eBlendDest )
		*eBlendDest = m_GLState.eBlendDest;
}

/////////////////////////////////////////////////////
/// Method: GetWidthHeight
/// Params: [out]nWidth, [out]nHeight
///
/////////////////////////////////////////////////////
void OpenGL::GetWidthHeight( GLint *nWidth, GLint *nHeight )
{
	if( nWidth )
		*nWidth = m_Width;

	if( nHeight )
		*nHeight = m_Height;
}

/////////////////////////////////////////////////////
/// Method: GetClearColour
/// Params: [out]fRed, [out]fGreen, [out]fBlue
///
/////////////////////////////////////////////////////
void OpenGL::GetClearColour( GLfloat *fRed, GLfloat *fGreen, GLfloat *fBlue )
{
	if( fRed )
	{
		*fRed = m_vClearColour.r;
	}
	if( fGreen )
	{
		*fGreen = m_vClearColour.g;
	}
	if( fBlue )
	{
		*fBlue = m_vClearColour.b;
	}
}

/////////////////////////////////////////////////////
/// Method: GetFOVNearFarClip
/// Params: [out]fFOV, [out]fNearClip, [out]fFarClip
///
/////////////////////////////////////////////////////
void OpenGL::GetFOVNearFarClip( GLfloat *fFOV, GLfloat *fNearClip, GLfloat *fFarClip )
{
	if( fFOV )
	{
		*fFOV = m_FOV;
	}
	if( fNearClip )
	{
		*fNearClip = m_NearClip;
	}
	if( fFarClip )
	{
		*fFarClip = m_FarClip;
	}
}

/////////////////////////////////////////////////////
/// Method: GetActiveTexture
/// Params: [in]nTexUnit, [in]eTarget
///
/////////////////////////////////////////////////////
GLuint OpenGL::GetActiveTexture( GLint nTexUnit, GLenum eTarget )
{
	// make sure it's a valid unit
	if( nTexUnit >= 0 /*&& nTexUnit < m_MaxTextureUnits*/ )
	{
		// make sure multitexturing is allowed, or if it's only unit 0 allow it
		if( eTarget == GL_TEXTURE_2D )
		{					
			return m_GLState.TexUnits[nTexUnit].nCurrent2DTexture;
		}
		else if( eTarget == GL_TEXTURE_CUBE_MAP )
		{					
			return m_GLState.TexUnits[nTexUnit].nCurrentCubemapTexture;
		}
	}

	return renderer::INVALID_OBJECT;
}

/////////////////////////////////////////////////////
/// Method: GetActiveFrameBuffer
/// Params: None
///
/////////////////////////////////////////////////////
GLuint OpenGL::GetActiveFrameBuffer( void )
{
	return m_GLState.nCurrentFrameBuffer;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // USE_OPENGL41


