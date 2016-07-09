
/*===================================================================
	File: OpenGLCommon.cpp
	Library: RenderLib

	(C)Hidden Games
=====================================================================*/

#include "Boot/Includes.h"


/////////////////////////////////////////////////////
/// Function: CheckForGLError
/// Params: [in]szFile, [in]nLine
///
/////////////////////////////////////////////////////
GLint renderer::CheckForGLError( const char *szFile, GLint nLine )
{
	// Returns 1 if an OpenGL error occurred, 0 otherwise.
	GLint retCode = 0;
		
	if( renderer::OpenGL::IsInitialised() )
	{
		GLenum glErr;
			
		glErr = glGetError();
		while( glErr != GL_NO_ERROR )
		{
			DBGLOG( "GL Error in file %s @ line %d: %d\n", szFile, nLine, glErr );
			retCode = 1;
			// always stop in debug mode to check the state
#ifdef _DEBUG
			DBG_ASSERT(0);
#endif // _DEBUG
			
			glErr = glGetError();
			
		}
	}
	
	return retCode;
}

/////////////////////////////////////////////////////
/// Function: IsPowerOfTwo
/// Params: [in]nValue
///
/////////////////////////////////////////////////////
bool renderer::IsPowerOfTwo(int nValue)
{
	const int POW2_TABLE_SIZE = 16;

	int i = 0;
	int nTable[POW2_TABLE_SIZE];

	for (i = 0; i < POW2_TABLE_SIZE; i++)
		nTable[i] = static_cast<int>(std::pow(2.0, (double)i));

	for (i = 0; i < POW2_TABLE_SIZE; i++)
	{
		if (nValue == nTable[i])
			return true;
	}

	return false;
}

/////////////////////////////////////////////////////
/// Function: GetPreviousPowerOfTwo
/// Params: [in]nValue, [in]nMaxSize
///
/////////////////////////////////////////////////////
int renderer::GetPreviousPowerOfTwo(int nValue, int nMaxSize)
{
	int result = nMaxSize;

	// the loop below will never be low enough so set it here
	if (nValue > nMaxSize)
		return nMaxSize;

	// shift until the loop fails
	while (result >= nValue)
	{
		result >>= 1;
	}

	return result;
}

/////////////////////////////////////////////////////
/// Function: GetNextPowerOfTwo
/// Params: [in]nValue, [in]nMaxSize
///
/////////////////////////////////////////////////////
int renderer::GetNextPowerOfTwo(int nValue, int nMaxSize)
{
	int result = 1;

	// don't go higher than supported
	if (nValue > nMaxSize)
		return nMaxSize;

	if (nValue <= result)
		return 2;

	// shift until the loop fails
	while (result <= nValue)
	{
		result <<= 1;
	}

	return result;
}
