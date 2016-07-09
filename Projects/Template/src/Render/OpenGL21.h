

#ifndef __OPENGL21_H__
#define __OPENGL21_H__

#ifdef USE_OPENGL21

namespace renderer
{
	class OpenGL
	{
		public:
			/// default constructor
			OpenGL();
			/// default destructor
			~OpenGL();

			/// Initialise - Sets up just the opengl default values
			void Init( void );
			/// cleanup
			/// Release - Clears out all allocated data by the class
			void Release( void );
        
            /// ResetGLState - Puts the GL state back to its initial settings
            void ResetGLState();
        
			// standard GL setup methods

			/// UseProgram - Uses the passed program
			void UseProgram( GLuint program );
			/// ClearProgram - clears the current program
			void ClearProgram();

			/// DisableVBO - Disables VBO bound buffers
			void DisableVBO( void );

			/// EnableTexturing - Enables all texturing
			void EnableTexturing( void );
			/// DisableTexturing - Disables all texturing
			void DisableTexturing( void );

			/// SetCullState - Sets the state of culling
			/// \param bCull - cull flag
			/// \param eFace - which face to cull
			void SetCullState( bool bCull, GLenum eFace );
			/// BindTexture - binds a new texture
			/// \param nTexID - GL texture object
			void BindTexture( GLuint nTexID );
			/// BindUnitTexture - binds a new texture to a tex unit
			/// \param nTexUnit - which texture unit
			/// \param eTarget - what type of target 1D/2D/3D
			/// \param nTexID - GL texture object
			/// \param bForceBind - makes the texture change regardless of whether it's already the bound texture
			void BindUnitTexture( GLint nTexUnit, GLenum eTarget, GLuint nTexID, bool bForceBind = false );
			/// BindFrameBufferObject - binds a new framebuffer object
			/// \param nFrameBufferID - GL framebuffer object
			void BindFrameBufferObject( GLuint nFrameBufferID );
			/// ClearUnitTextures - clears all textures for all units
			void ClearUnitTextures( void );
			/// ClearUnitTexture - clears all textures on a texture unit
			/// \param nTexUnit - which texture unit
			void ClearUnitTexture( GLint nTexUnit );
			/// DisableUnitTexture - disables a texture target on a texture unit
			/// \param nTexUnit - which texture unit
			void DisableUnitTexture( GLint nTexUnit );
			/// ClearFrameBufferObject - unbinds any current Frame Buffer Object
			void ClearFrameBufferObject( void );

			/// DepthMode - Sets the current depth test state
			/// \param bDepthFlag - depth test flag
			/// \param eDepthFunc - depth test function
			void DepthMode( bool bDepthFlag, GLenum eDepthFunc );
			/// BlendMode - Sets the current blend test state
			/// \param bBlendFlag - blend flag
			/// \param eBlendSrc - source blend function
			/// \param eBlendDest - destination blend function
			void BlendMode( bool bBlendFlag, GLenum eBlendSrc, GLenum eBlendDest );


			/// GetGLProperties - Goes through some standard glGet*() commands and prints their values
			void GetGLProperties( void );

			/// ClearScreen - Does a standard clear of the screen
			void ClearScreen( void );
			/// ClearColour - Sets the clear colour of the screen
			/// \param fRed - red clear colour
			/// \param fGreen - green clear colour
			/// \param fBlue - blue clear colour
			/// \param fAlpha - alpha clear colour
			void ClearColour( GLfloat fRed, GLfloat fGreen, GLfloat fBlue, GLfloat fAlpha );
			/// SetClearBits - Sets the bitfield for the glClear call
			/// \param bits - clear bits
			void SetClearBits( GLbitfield bits );
			/// SetupPerspectiveView - Sets the width and height of the GL viewport
			/// \param nWidth - width of the view
			/// \param nHeight - height of the view
			/// \param bForceUpdate - force the reset of the matrix setup
			void SetupPerspectiveView( GLint nWidth, GLint nHeight, bool bForceUpdate = false );
			/// SetupOrthographicView - Sets the width and height of the GL ortho viewport
			/// \param nWidth - width of the view
			/// \param nHeight - height of the view
			/// \param bForceUpdate - force the reset of the matrix setup
			void SetupOrthographicView( GLint nWidth, GLint nHeight, bool bForceUpdate = false );
 
			/// SetNearFarClip - Sets the near and far clip planes
			/// \param fNearClip - near clip value
			/// \param fFarClip - far clip value
			void SetNearFarClip( GLfloat fNearClip, GLfloat fFarClip );
			/// SetFieldOfView - Sets the field of view
			/// \param fAngle - angle to set the view
			void SetFieldOfView( GLfloat fAngle );
			/// SetViewport - Sets the viewport
			/// \param nWidth - width of viewport
			/// \param nHeight - height of viewport
			void SetViewport( GLint nWidth, GLint nHeight );
			/// SetPerspective - Sets the perspective view
			/// \param fFOV - field of view angle
			/// \param fAspectRatio - aspect ratio
			/// \param fNearClip - near clip value
			/// \param fFarClip - far clip value
			void SetPerspective( GLfloat fFOV, GLfloat fAspectRatio, GLfloat fNearClip, GLfloat fFarClip, bool force=false );

			/// SaveDepthBlendAlphaState - Saves the current depth, blend and alpha state to a structure
			/// \param pState - pointer to state storage
			void SaveDepthBlendAlphaState( TGLDepthBlendAlphaState* pState );
			/// SetDepthBlendAlphaState - Sets the depth, blend and alpha state from a structure
			/// \param pState - new state
			void SetDepthBlendAlphaState( TGLDepthBlendAlphaState* pState );
			
			/// SaveFogState - Saves the current fog state to a structure
			/// \param pState - pointer to state storage
			void SaveFogState( TGLFogState* pState );
			/// SetFogState - Sets the fog state from a structure
			/// \param pState - new state
			void SetFogState( TGLFogState* pState );

			/// SaveViewState - Saves the current view state to a structure
			/// \param pState - pointer to view state storage
			void SaveViewState( TViewState* pState );
			/// SetViewState - Sets the a view state from a structure
			/// \param viewState - new view state
			void SetViewState( TViewState* pViewState );

			/// SaveCullState - Saves the current cull state to a structure
			/// \param pState - pointer to view state storage
			void SaveCullState( TGLCullFaceState* pState );
			/// SetCullState - Sets the cull state from a structure
			/// \param cullState - new cull state
			void SetCullState( TGLCullFaceState* pCullState );


			/// SetMaxTextureSize - Sets the max texture units
			/// \param nMaxTextureUnits - max texture units
			void SetMaxTextureUnits( GLint nMaxTextureUnits )				{ m_MaxTextureUnits = nMaxTextureUnits; }
			/// GetMaxTextureSize - Gets the current max texture units
			/// \return integer - returned max texture units
			GLuint GetMaxTextureUnits( void )								{ return m_MaxTextureUnits; }

			/// SetMaxTextureSize - Sets the max texture size
			/// \param nMaxTextureSize - max texture size
			void SetMaxTextureSize( GLint nMaxTextureSize )				{ m_MaxTextureSize = nMaxTextureSize; }
			/// GetMaxTextureSize - Gets the current max texture size
			/// \return integer - returned max texture size
			GLint GetMaxTextureSize( void )								{ return m_MaxTextureSize; }

			/// SetMaxRenderBufferSize - If supported the max render buffer size gets set
			/// \param nMaxRenderBufferSize - max render buffer size
			void SetMaxRenderBufferSize( GLint nMaxRenderBufferSize )		{ m_MaxRenderBufferSize = nMaxRenderBufferSize; }
			/// GetMaxRenderBufferSize - Gets the current max allowed render buffer size
			/// \return integer - max size
			GLint GetMaxRenderBufferSize( void )							{ return m_MaxRenderBufferSize; }

			/// SetClipPlane
			/// \param plane - plane id
			/// \param eq - plane equation
			void SetClipPlane( GLenum plane, const glm::vec4& eq );

			/// Flip - Swaps the backbuffer with the frontbuffer
			void Flip( void );

			// Frustum calls
			/// ExtractFrustum - This grabs the current modelview and projection and calculates the frustum box
			void ExtractFrustum( void );
			/// PointInFrustum - Checks to see if a point is within the current frustum
			/// \param X - x position of the point
			/// \param Y - y position of the point
			/// \param Z - z position of the point
			/// \return boolean ( SUCCESS:true or FAIL:false )
			bool PointInFrustum( GLfloat X, GLfloat Y, GLfloat Z );
			/// SphereInFrustum - Checks to see if a sphere is within the current frustum
			/// \param X - center x point of the sphere
			/// \param Y - center y point of the sphere
			/// \param Z - center z point of the sphere
			/// \param fRadius - radius of the sphere
			/// \return boolean ( SUCCESS:true or FAIL:false )
			bool SphereInFrustum( GLfloat X, GLfloat Y, GLfloat Z, GLfloat fRadius );
			/// CubeInFrustum - Checks to see if a cube is within the current frustum
			/// \param X - center x point of the cube
			/// \param Y - center y point of the cube
			/// \param Z - center z point of the cube
			/// \param fSize - size of the cube
			/// \return boolean ( SUCCESS:true or FAIL:false )
			bool CubeInFrustum( GLfloat X, GLfloat Y, GLfloat Z, GLfloat fSize );

			GLuint GetCurrentProgram()			{ return m_GLState.nCurrentProgram; }

			/// GetTextureState - Gets the current texturing enabled/disabled state
			/// \return boolean - ( true or false )
			bool GetTextureState( void )		{ return m_GLState.bTextureState; }
			/// GetLightingState - Gets the current lighting enabled/disabled state
			/// \return boolean - ( true or false )
			bool GetLightingState( void )		{ return m_GLState.bLightingState; }
			/// GetDepthMode - Gets the current depth test state 
			/// \param bDepthFlag - depth test flag holder
			/// \param eDepthFunc - depth test function holder
			void GetDepthMode( bool *bDepthFlag, GLenum *eDepthFunc );
			/// GetBlendMode - Gets the current blend test state
			/// \param bBlendFlag - blend flag holder
			/// \param eBlendSrc - source blend function holder
			/// \param eBlendDest - destination blend function holder
			void GetBlendMode( bool *bBlendFlag, GLenum *eBlendSrc, GLenum *eBlendDest );
			/// GetAlphaMode - Gets the current alpha test state
			/// \param bAlphaFlag - alpha flag holder
			/// \param eAlphaFunc - alpha test function holder
			/// \param fAlphaClamp - alpha test clamp holder
			void GetAlphaMode( bool *bAlphaFlag, GLenum *eAlphaFunc, GLfloat *fAlphaClamp );
			/// GetFillMode - Gets the current fillmode state
			/// \return boolean - ( true or false )
			bool GetFillMode( void )		{ return m_GLState.bFillMode; }
			/// GetCullState - Gets the current cull state
			/// \return boolean - ( true or false )
			bool GetCullState( void )		{ return m_GLState.bCullState; }
			/// GetCullFace - Gets the current cull face
			/// \return GLenum - current culling face (should be either GL_FRONT, GL_BACK or GL_FRONT_AND_BACK)
			GLenum GetCullFace( void )		{ return m_GLState.eCullFace; }
			/// GetWidthHeight - Gets the width and height of the current view
			/// \param nWidth - parameter to hold the returned width
			/// \param nHeight - parameter to hold the returned height
			void GetWidthHeight( GLint *nWidth, GLint *nHeight );
			/// GetClearColour - Gets the current GL clear screen colour
			/// \param fRed - parameter to hold the returned red value
			/// \param fGreen - parameter to hold the returned green value
			/// \param fBlue - parameter to hold the returned blue value
			void GetClearColour( GLfloat *fRed, GLfloat *fGreen, GLfloat *fBlue );
			/// GetClearBits - Gets the glClear mask bits
			GLbitfield GetClearBits( )		{ return m_ClearBits; }

			/// GetFOVNearFarClip - Returns the current field of view, near and far clip values
			/// \param fFOV - parameter to hold the returned field of view value
			/// \param fNearClip - parameter to hold the returned near clip value
			/// \param fFarClip - parameter to hold the returned far clip value
			void GetFOVNearFarClip( GLfloat *fFOV, GLfloat *fNearClip, GLfloat *fFarClip );
			/// GetActiveTexture - Gets the current active texture on a unit
			/// \param nTexUnit - unit to grab current texture from
			/// \param eTarget - texture target
			/// \return GLuint - ( SUCCESS: GL texture id or FAIL: INVALID_OBJECT
			GLuint GetActiveTexture( GLint nTexUnit, GLenum eTarget );
			/// GetActiveFrameBuffer - Gets the current active texture on a unit
			/// \return GLuint - ( SUCCESS: GL framebuffer id or FAIL: INVALID_OBJECT
			GLuint GetActiveFrameBuffer( void );

			/// GetTotalTrianglesDrawn - Gets the total amount of triangles drawn since the last flip
			/// \return integer - (SUCCESS: number of triangles drawn since last flip)
			GLint GetTotalTrianglesDrawn( void )	{ return nTotalTriangleCount; }
			/// GetTotalVerticesDrawn - Gets the total amount of vertices drawn since the last flip
			/// \return integer - (SUCCESS: number of vertices drawn since last flip)
			GLint GetTotalVerticesDrawn( void )		{ return nTotalVertexCount; }

			/// GetViewport - Gets the last known viewport extends
			GLint* GetViewport()					{ return (&m_Viewport[0]); }

			/// SetProjectionMatrix - Set the projection matrix
			/// \param m - matrix to set
			void SetProjectionMatrix( glm::mat4& m )	{ m_ProjMatrix = m; }
			/// GetProjectionMatrix - Gets the last known projection matrix
			glm::mat4& GetProjectionMatrix()			{ return (m_ProjMatrix); }

			/// SetViewMatrix - Sets the view matrix
			/// \param m - matrix to set
			void SetViewMatrix( glm::mat4& m )		{ m_ViewMatrix = m; }
			/// GetProjectionMatrix - Gets the last known view matrix
			glm::mat4& GetViewMatrix()				{ return (m_ViewMatrix); }

			/// SetModelMatrix - Sets the model matrix
			/// \param m - matrix to set
			void SetModelMatrix(glm::mat4& m)		{ m_ModlMatrix = m; }
			/// GetModelMatrix - Gets the last known model  matrix
			glm::mat4& GetModelMatrix()				{ return (m_ModlMatrix); }

			/// GetClipMatrix - Gets the last known clip matrix
			glm::mat4& GetClipMatrix()				{ return (m_ClipMatrix); }

			/// Tool preference structure holder
			//TToolPreferences toolPrefs;
			/// Current total triangle count
			GLint nTotalTriangleCount;
			/// Current total vertex count
			GLint nTotalVertexCount;
			/// Current total material count
			GLint nTotalMaterialCount;
			/// Current total texture count
			GLint nTotalTextureCount;

			static void Initialise( void );
			static void Shutdown( void );

			static OpenGL *GetInstance(void)
			{
				DBG_ASSERT((ms_Instance != nullptr));

				return ms_Instance.get();
			}
			static bool IsInitialised(void)
			{
				return(ms_Instance != nullptr);
			}

		private:
			//static OpenGL* ms_Instance;
			static std::unique_ptr<OpenGL> ms_Instance;

			/// State tracking structure
			TGLState	m_GLState;

			/// Current Pixel Format value
			GLint		m_PixelFormat;
			/// Window width value
			GLint		m_Width;
			/// Window height value
			GLint		m_Height;
			/// GL Bits per Pixel value
			GLint	m_BPP;
			/// GL Z buffer value
			GLint	m_ZBuffer;
			/// GL alpha bits value
			GLint	m_AlphaBits;
			/// GL depth bits value
			GLint	m_DepthBits;

			/// last viewport
			GLint m_Viewport[4];

			/// projection matrix
			glm::mat4	m_ProjMatrix;
			/// view matrix
			glm::mat4	m_ViewMatrix;
			/// modelview matrix
			glm::mat4	m_ModlMatrix;
			/// clip matrix
			glm::mat4	m_ClipMatrix;

			/// GL clear colour
			glm::vec4	m_vClearColour;
			/// Clear bits
			GLbitfield m_ClearBits;
			/// GL view type
			EViewState	m_eViewState;
			/// GL field of view
			GLfloat	m_FOV;
			/// GL aspect ratio
			GLfloat m_AspectRatio;
			/// GL near clip
			GLfloat	m_NearClip;
			/// GL far clip
			GLfloat	m_FarClip;
			/// GL frustum values, automatically filled each frame
			GLfloat	m_Frustum[6][4];

			/// Max texture size supported by the driver
			GLint m_MaxTextureSize;
			/// Max texture units supported by driver
			GLint m_MaxTextureUnits;
			/// Max render buffer size supported by driver
			GLint m_MaxRenderBufferSize;
	};

/////////////////////////////////////////////////////
/// Method: BindTexture
/// Params: [in]nTexID
///
/////////////////////////////////////////////////////
inline void OpenGL::BindTexture( GLuint nTexID )
{
	if( nTexID == INVALID_OBJECT )
		return;

	BindUnitTexture( 0, GL_TEXTURE_2D, nTexID );

	// check for errors
	GL_CHECK
}

/////////////////////////////////////////////////////
/// Method: BindUnitTexture
/// Params: None
///
/////////////////////////////////////////////////////
inline void OpenGL::BindUnitTexture( GLint nTexUnit, GLenum eTarget, GLuint nTexID, bool bForceBind )
{
	if( nTexID == INVALID_OBJECT )
		return;

	// make sure it's a valid unit
	if( nTexUnit >= 0 /*&& nTexUnit < m_MaxTextureUnits*/ )
	{
		// which unit are we working on
		GLenum eActiveTexUnit = GL_TEXTURE0 + nTexUnit;

		if( eTarget == GL_TEXTURE_2D )
		{					
			glActiveTexture( eActiveTexUnit );

			if( bForceBind )
			{
				glBindTexture( GL_TEXTURE_2D, nTexID );
				m_GLState.TexUnits[nTexUnit].nCurrent2DTexture = nTexID;

				if( nTexUnit == 0 )
					m_GLState.nCurrentTexture = nTexID;
			}
			else
			{
//				if( m_GLState.TexUnits[nTexUnit].nCurrent2DTexture != nTexID )
				{
					glBindTexture( GL_TEXTURE_2D, nTexID );
					m_GLState.TexUnits[nTexUnit].nCurrent2DTexture = nTexID;

					if( nTexUnit == 0 )
						m_GLState.nCurrentTexture = nTexID;
				}
			}

		}
		else if( eTarget == GL_TEXTURE_CUBE_MAP )
		{					
			glActiveTexture( eActiveTexUnit );

			if( bForceBind )
			{
				glBindTexture( GL_TEXTURE_CUBE_MAP, nTexID );
				m_GLState.TexUnits[nTexUnit].nCurrentCubemapTexture = nTexID;

				if( nTexUnit == 0 )
					m_GLState.nCurrentTexture = nTexID;
			}
			else
			{
//					if( m_GLState.TexUnits[nTexUnit].nCurrentCubemapTexture != nTexID )
				{	
					glBindTexture( GL_TEXTURE_CUBE_MAP, nTexID );
					m_GLState.TexUnits[nTexUnit].nCurrentCubemapTexture = nTexID;

					if( nTexUnit == 0 )
						m_GLState.nCurrentTexture = nTexID;
				}
			}
		}
	}

	// check for errors
	GL_CHECK
}

} // namespace renderer

#endif // USE_OPENGL21

#endif // __OPENGL21_H__


