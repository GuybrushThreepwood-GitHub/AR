

#ifndef __OPENGLCOMMON_H__
#define __OPENGLCOMMON_H__

namespace renderer
{
	/// default value of GL object
	const GLuint INVALID_OBJECT				= (GLuint)(~0);

	const int MAX_USEABLE_TEXUNITS = 16;

	const int MAX_USEABLE_LIGHTS = 8;

	/// VBO/PBO offset helper function
	#define BUFFER_OFFSET(i) ((char *)0 + (i))

	/// width of lines when drawing an axis representation
	const GLfloat AXIS_LINE_WIDTH			= 5.0f;
	/// width of lines when drawing a bounding box
	const GLfloat BOUNDINGBOX_LINE_WIDTH	= 5.0f;

	/// Calls and checks the GL error message of passed GL function
	#define CHECK_OPENGL_ERROR( glCmd ) \
			glCmd; \
			{ \
				GLenum error; \
				while( ( error = glGetError() ) != GL_NO_ERROR ) \
				{ \
					DBGLOG( "[%s:%d] '%s' failed with error %s\n", __FILE__, __LINE__, #glCmd, gluErrorString( error ) ); \
				} \
			}

	// Quick check function
#ifdef _DEBUG
	/// macro to check the current GL state
	#define GL_CHECK		if( renderer::CheckForGLError( __FILE__, __LINE__ ) ) \
							{ \
								DBG_ASSERT(0); \
							}
#else
	#define GL_CHECK
#endif //_DEBUG

	enum EViewState
	{
		VIEWSTATE_PERSPECTIVE=0,
		VIEWSTATE_ORTHOGRAPHIC
	};

	enum EPointOrigin
    {
		POINTORIGIN_TOPLEFT=0,
        POINTORIGIN_BOTTOMLEFT,
        POINTORIGIN_TOPRIGHT,
		POINTORIGIN_BOTTOMRIGHT
    };

	/// GL texture state tracker, structure
	struct TGLTexture
	{
		/// type of texture
		GLenum eTarget;
		/// Current texture object number
		GLuint nTextureID;
		/// Current S wrap mode
		GLfloat fTexSWrap;
		/// Current T wrap mode
		GLfloat fTexTWrap;

		/// Current texture minification filter
		GLfloat fTexMinFilter;
		/// Current texture magnification filter
		GLfloat fTexMagFilter;
		/// Current texture min lod value
		GLfloat fTexMinLod;
		/// Current texture max lod value
		GLfloat fTexMaxLod;
		/// Current mipmap base level
		GLfloat fTexBaseLevel;
		/// Current mipmap max level
		GLfloat fTexMaxLevel;
		/// Current texture lod bias
		GLfloat fTexLodBias;

	};

	/// GL texture unit state tracker, structure
	struct TGLTexUnitState
	{
		/// 1D texture state
		GLint nTex1DState;
		/// Current 1D texture object number
		GLuint nCurrent1DTexture;
		/// Current 1D texture env mode
		GLfloat nTex1DEnvMode;
		/// Current 1D texture gen mode
		GLfloat nTex1DGenMode;
		/// Current 1D texture minification filter
		GLfloat fTex1DMinFilter;
		/// Current 1D texture magnification filter
		GLfloat fTex1DMagFilter;
		/// Current 1D texture min lod value
		GLfloat fTex1DMinLod;
		/// Current 1D texture max lod value
		GLfloat fTex1DMaxLod;
		/// Current 1D mipmap base level
		GLfloat fTex1DBaseLevel;
		/// Current 1D mipmap max level
		GLfloat fTex1DMaxLevel;

		/// 2D texture state
		GLint nTex2DState;
		/// Current 2D texture object number
		GLuint nCurrent2DTexture;
		/// Current 2D texture env mode
		GLfloat nTex2DEnvMode;
		/// Current 2D texture gen mode
		GLfloat nTex2DGenMode;
		/// Current 2D texture minification filter
		GLfloat fTex2DMinFilter;
		/// Current 2D texture magnification filter
		GLfloat fTex2DMagFilter;
		/// Current 2D texture min lod value
		GLfloat fTex2DMinLod;
		/// Current 2D texture max lod value
		GLfloat fTex2DMaxLod;
		/// Current 2D mipmap base level
		GLfloat fTex2DBaseLevel;
		/// Current 2D mipmap max level
		GLfloat fTex2DMaxLevel;

		/// Texture Rectangle texture state
		GLint nTexRectangleState;
		/// Current Texture Rectangle texture object number
		GLuint nCurrentRectangleTexture;
		/// Current Texture Rectangle texture env mode
		GLfloat nTexRectangleEnvMode;
		/// Current Texture Rectangle texture gen mode
		GLfloat nTexRectangleGenMode;
		/// Current Texture Rectangle texture minification filter
		GLfloat fTexRectangleMinFilter;
		/// Current Texture Rectangle texture magnification filter
		GLfloat fTexRectangleMagFilter;
		/// Current Texture Rectangle texture min lod value
		GLfloat fTexRectangleMinLod;
		/// Current Texture Rectangle texture max lod value
		GLfloat fTexRectangleMaxLod;
		/// Current Texture Rectangle mipmap base level
		GLfloat fTexRectangleBaseLevel;
		/// Current Texture Rectangle mipmap max level
		GLfloat fTexRectangleMaxLevel;

		/// Texture Cubemap texture state
		GLint nTexCubemapState;
		/// Current Texture Cubemap texture object number
		GLuint nCurrentCubemapTexture;
		/// Current Texture Cubemap texture env mode
		GLfloat nTexCubemapEnvMode;
		/// Current Texture Cubemap texture gen mode
		GLfloat nTexCubemapGenMode;
		/// Current Texture Cubemap texture minification filter
		GLfloat fTexCubemapMinFilter;
		/// Current Texture Cubemap texture magnification filter
		GLfloat fTexCubemapMagFilter;
		/// Current Texture Cubemap texture min lod value
		GLfloat fTexCubemapMinLod;
		/// Current Texture Cubemap texture max lod value
		GLfloat fTexCubemapMaxLod;
		/// Current Texture Cubemap mipmap base level
		GLfloat fTexCubemapBaseLevel;
		/// Current Texture Cubemap mipmap max level
		GLfloat fTexCubemapMaxLevel;

	};

	/// GL texture unit state tracker, structure
	struct TGLLightState
	{
		void Init()
		{
			/// Light enable state
			bLightState=false;
			/// Current light ambient colour
			vAmbientColour = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
			/// Current light diffuse colour
			vDiffuseColour = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
			/// Current light specular colour
			vSpecularColour = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
			/// Current light emission colour
			vEmissionColour = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
			/// Current light position/direction
			vPosition = glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);
			/// Current light spot direction
			vSpotDirection = glm::vec3(0.0f, 0.0f, -1.0f);
			/// Current light spot exponent
			fSpotExponent = 0.0f;
			/// Current light spot cut off
			fSpotCutOff = 180.0f;
			/// Current light attenuation values
			vAttenuation = glm::vec3(1.0f, 0.0f, 0.0f);
		}

		/// Light enable state
		bool bLightState;
		/// Current light ambient colour
		glm::vec4 vAmbientColour;
		/// Current light diffuse colour
		glm::vec4 vDiffuseColour;
		/// Current light emission colour
		glm::vec4 vEmissionColour;
		/// Current light specular colour
		glm::vec4 vSpecularColour;
		/// Current light position/direction
		glm::vec4 vPosition;
		/// Current light spot direction
		glm::vec3 vSpotDirection;
		/// Current light spot exponent
		GLfloat fSpotExponent;
		/// Current light spot cut off
		GLfloat fSpotCutOff;
		/// Current light attenuation values
		glm::vec3 vAttenuation;
	};

	/// GL state tracking structure
	struct TGLState
	{
		/// Active shader program
		GLuint nCurrentProgram;

		/// Current vertex array client state
		bool bVertexArrayState;
		/// Current texture array client state
		bool bTextureArrayState;
		/// Current colour array client state
		bool bColourArrayState;
		/// Current normal array client state
		bool bNormalArrayState;

		bool bVBOActive;

		/// Current texture enable/disable state
		bool bTextureState;
		/// Current fillmode enable/disable state
		bool bFillMode;
		/// Flag for light enabled/disabled
		bool bLightingState;

		/// Current texture active
		GLuint nCurrentTexture;
		/// 32 texture units tracking list
		TGLTexUnitState TexUnits[MAX_USEABLE_TEXUNITS];
		/// Current active framebuffer 
		GLuint nCurrentFrameBuffer;

		/// Current light states
		TGLLightState LightUnits[MAX_USEABLE_LIGHTS];

		/// Current depth enable/disable state
		bool bDepthState;
		/// Current depth function
		GLenum eDepthFunc;
		/// Current blend enable/disable state
		bool bBlendState;
		/// Current blend source function
		GLenum eBlendSrc;
		/// Current blend destination function
		GLenum eBlendDest;

		/// Current cull enable/disable state
		bool bCullState;
		/// Current cull face function
		GLenum eCullFace;

		/// Current front face direction
		GLenum eFrontFace;

		/// Current fog enable/disable state
		bool bFogState;
		/// Current fog colour
		glm::vec3 vFogColour;
		/// Current fog type
		GLenum eFogMode;
		/// Current fog near clip (linear model) 
		GLfloat fFogNearClip;
		/// Current fog far clip (linear model)
		GLfloat fFogFarClip;
		/// Current fog density (exp/exp2 model)
		GLfloat fFogDensity;

		// colour
		glm::vec4	vColour;
	};

	/// GL depth, blend and alpha state tracking structure
	struct TGLDepthBlendAlphaState
	{
		/// Current depth enable/disable state
		bool bDepthState;
		/// Current depth function
		GLenum eDepthFunc;
		/// Current blend enable/disable state
		bool bBlendState;
		/// Current blend source function
		GLenum eBlendSrc;
		/// Current blend destination function
		GLenum eBlendDest;

	};

	/// GL cull face state tracking structure
	struct TGLCullFaceState
	{
		/// Current cull enable/disable state
		bool bCullState;
		/// Current cull face function
		GLenum eCullFace;

	};

	/// GL cull face state tracking structure
	struct TGLFogState
	{
		/// Current fog enable/disable state
		bool bFogState;
		/// Current fog type
		GLenum eFogMode;
		/// Current fog colour
		glm::vec3 vFogColour;
		/// Current fog near clip (linear model) 
		GLfloat fFogNearClip;
		/// Current fog far clip (linear model)
		GLfloat fFogFarClip;
		/// Current fog density (exp/exp2 model)
		GLfloat fFogDensity;
	};

	/// view state tracking structure
	struct TViewState
	{
		EViewState	eViewState;
		/// field of view holder
		GLfloat	fFOV;
		/// near clip value
		GLfloat	fNearClip;
		/// far clip value
		GLfloat	fFarClip;
		/// aspect ratio
		GLfloat	fAspectRatio;
		/// width value
		GLint		nWidth;
		/// height value
		GLint		nHeight;

	};

	enum EShaderAttribs
	{
		SHADERATTRIB_VERT = 0,
		SHADERATTRIB_TEXCOORDS,
		SHADERATTRIB_COLOUR
	};

	struct InterleavedData
	{
		float v[3];		//	12 bytes
		float uv[2];	//	8 bytes
		float n[3];		//	12 bytes
		// ------------------------
		// total			32 bytes
		float t[4];		//  16 bytes
		// total			48 bytes
		float pad[4];
	};

	/// CheckForGLError - Checks the GL state machine for any errors 
	/// \param szFile - current file
	/// \param nLine - current line
	GLint CheckForGLError( const char *szFile, GLint nLine );

	/// IsPowerOfTwo - Checks to see if the value passed is a valid power of two number
	/// \param nValue - value to check
	/// \return boolean - ( SUCCESS: true or FAIL: false )
	bool IsPowerOfTwo(int nValue);
	/// GetPreviousPowerOfTwo - Gets the previous valid power of two number from the value passed to the function
	/// \param nValue - value to get previous value from
	/// \param nMaxSize - max value allowed (should be a driver limit)
	/// \return integer - Previous power of two number
	int GetPreviousPowerOfTwo(int nValue, int nMaxSize);
	/// GetNextPowerOfTwo - Gets the next valid power of two number from the value passed to the function
	/// \param nValue - value to get next value from
	/// \param nMaxSize - max value allowed (should be a driver limit)
	/// \return integer - Next power of two number
	int GetNextPowerOfTwo(int nValue, int nMaxSize);

} // namespace renderer


#endif // __OPENGL_H__


