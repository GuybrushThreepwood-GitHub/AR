
/*===================================================================
	File: ShaderShared.cpp
=====================================================================*/

#include "Boot/Includes.h"

namespace
{
	struct lstr
	{
		bool operator() (std::string s1, std::string s2) const
		{
			return( (s1.compare(s2)) < 0);
		}
	};

	struct ShaderStore
	{
		int refCount;

		GLuint vertShaderId;
		bool isVertexShaderAFile;
		char szVertexShaderFileOrString[MAX_SHADER_SIZE];

		GLuint fragShaderId;
		bool isFragmentShaderAFile;
		char szFragmentShaderFileOrString[MAX_SHADER_SIZE];

		GLuint geomShaderId;
		bool isGeomShaderAFile;
		char szGeomShaderFileOrString[MAX_SHADER_SIZE];

		GLuint programId;
	};

	typedef std::map< std::string, ShaderStore, struct lstr> TNameIntMap;
	typedef std::pair< std::string, ShaderStore> TNameIntPair;

	TNameIntMap ShaderMap;

	GLchar shaderSrcBuffer[MAX_SHADER_SIZE];
	GLchar tmpBuffer[MAX_SHADER_SIZE];
	char shaderLogBuffer[MAX_SHADER_LOG_SIZE];
}

/////////////////////////////////////////////////////
/// Function: ReloadShaderMap
/// Params: None
///
/////////////////////////////////////////////////////
void renderer::ReloadShaderMap()
{

}

/////////////////////////////////////////////////////
/// Function: ClearShaderMap
/// Params: None
///
/////////////////////////////////////////////////////
void renderer::ClearShaderMap()
{	
	auto it = ShaderMap.begin();
	
	while( it != ShaderMap.end() )
	{
#ifdef _DEBUG
		DBGLOG( "*WARNING* Shader %s still active\n", it->first.c_str() );
#endif // _DEBUG

		if( glIsShader(it->second.vertShaderId) )
		{
			glDetachShader(it->second.programId, it->second.vertShaderId);
			glDeleteShader(it->second.vertShaderId);
		}

		if( glIsShader(it->second.fragShaderId) )
		{
			glDetachShader(it->second.programId, it->second.fragShaderId);
			glDeleteShader(it->second.fragShaderId);
		}

		if (glIsShader(it->second.geomShaderId))
		{
			glDetachShader(it->second.programId, it->second.geomShaderId);
			glDeleteShader(it->second.geomShaderId);
		}

		if( glIsProgram(it->second.programId) )
			glDeleteProgram( it->second.programId );

		it++;
	}

	ShaderMap.clear();
}

/////////////////////////////////////////////////////
/// Function: RemoveShaderProgram
/// Params: [in]programId
///
/////////////////////////////////////////////////////
void renderer::RemoveShaderProgram( GLuint programId )
{
	auto it = ShaderMap.begin();
	
	while( it != ShaderMap.end() )
	{
		if( it->second.programId == programId )
		{
			it->second.refCount--;

			if( it->second.refCount < 1 )
			{
				if( glIsShader(it->second.vertShaderId) )
				{
					glDetachShader(it->second.programId, it->second.vertShaderId);
					glDeleteShader(it->second.vertShaderId);
				}

				if( glIsShader(it->second.fragShaderId) )
				{
					glDetachShader(it->second.programId, it->second.fragShaderId);
					glDeleteShader(it->second.fragShaderId);
				}

				if (glIsShader(it->second.geomShaderId))
				{
					glDetachShader(it->second.programId, it->second.geomShaderId);
					glDeleteShader(it->second.geomShaderId);
				}

				if( glIsProgram(it->second.programId) )
					glDeleteProgram( it->second.programId );

				ShaderMap.erase( it );

				return;
			}
		}

		it++;
	}

	ShaderMap.clear();
}

/////////////////////////////////////////////////////
/// Function: PrintInfoLog
/// Params: [in]objectId
///
/////////////////////////////////////////////////////
void renderer::PrintInfoLog( GLuint objectId )
{
	GLint infologLength = 0;
	int charsWritten  = 0;

	if( glIsShader(objectId) )
		glGetShaderiv( objectId, GL_INFO_LOG_LENGTH, &infologLength );
	else
		glGetProgramiv( objectId, GL_INFO_LOG_LENGTH, &infologLength );

	GL_CHECK 

	if( infologLength > 0 )
	{
		if( glIsShader(objectId) )
		{
			glGetShaderInfoLog( objectId, infologLength, (GLsizei *)&charsWritten, &shaderLogBuffer[0] );
			if( shaderLogBuffer != nullptr &&
				strlen(shaderLogBuffer) > 0 )
			{
				DBGLOG("Shader InfoLog:\n%s\n", shaderLogBuffer);
				DBGLOG("************************\n");
			}
		}
		else
		{
			glGetProgramInfoLog( objectId, infologLength, (GLsizei *)&charsWritten, &shaderLogBuffer[0] );
			if (shaderLogBuffer != nullptr &&
				strlen(shaderLogBuffer) > 0)
			{
				DBGLOG( "Program InfoLog:\n%s\n", shaderLogBuffer );
				DBGLOG("************************\n");
			}
		}
	}
	GL_CHECK
}

/////////////////////////////////////////////////////
/// Function: LoadShader
/// Params: [in]shaderType, [in]filename
///
/////////////////////////////////////////////////////
GLuint renderer::LoadShader(GLenum shaderType, const std::string& szFilename)
{
	GLuint shaderId = renderer::INVALID_OBJECT;

	SDL_RWops *pFile = nullptr;
	std::size_t fileSize = 0;

	std::memset( shaderSrcBuffer, 0, sizeof(GLchar)*MAX_SHADER_SIZE );

	// check file is valid
	if (szFilename.length() == 0 )
		return renderer::INVALID_OBJECT;

	// valid shader type
	if( shaderType != GL_VERTEX_SHADER &&
		shaderType != GL_FRAGMENT_SHADER 
#ifdef USE_OPENGL41
		&& shaderType != GL_GEOMETRY_SHADER 
#endif
		)
	{
		DBGLOG( "SHADERS: *ERROR* invalid shader type\n" );
		return renderer::INVALID_OBJECT;
	}

	// open the shader for reading
	pFile = SDL_RWFromFile(szFilename.c_str(), "rt");
	if (pFile == nullptr)
	{
		DBGLOG( "SHADERS: *ERROR* Failed loading shader: %s\n", szFilename.c_str() );
		return renderer::INVALID_OBJECT;
	}

	fileSize = SDL_RWsize(pFile);

#ifdef USE_OPENGL21
	// append string here
	sprintf( shaderSrcBuffer, "#version 120\n" );
#else
	// append string here
	sprintf( shaderSrcBuffer, "#version 410\n" );
#endif

	std::size_t currentOffset = std::strlen( shaderSrcBuffer );

	// Get the shader from a file
	SDL_RWseek(pFile, RW_SEEK_SET, 0);
	std::size_t readSize = SDL_RWread(pFile, &shaderSrcBuffer[currentOffset], sizeof(char), fileSize);
	shaderSrcBuffer[currentOffset+readSize] = '\0';

	// close file
	SDL_RWclose(pFile);
	pFile = nullptr;

	// create
	shaderId = glCreateShader( shaderType );
	GLint len = static_cast<GLint>( std::strlen(shaderSrcBuffer) );
	GLchar* ptr = &shaderSrcBuffer[0];
	glShaderSource( shaderId, 1, (const GLchar**)&ptr, &len );

	GL_CHECK

	// compile it
	GLint compiled = false;
	glCompileShader( shaderId );

	GL_CHECK

	glGetShaderiv( shaderId, GL_COMPILE_STATUS, (GLint *)&compiled );

#ifdef _DEBUG
	PrintInfoLog( shaderId );
#endif // _DEBUG

	if( !compiled )
	{
		return renderer::INVALID_OBJECT;
	}

	return shaderId;
}

/////////////////////////////////////////////////////
/// Function: LoadShaderFilesForProgram
/// Params: [in]vertexShader, [in]fragmentShader
///
/////////////////////////////////////////////////////
GLuint renderer::LoadShaderFilesForProgram(const std::string& vertexShader, const std::string& fragmentShader, const std::string& geometryShader)
{
	if (vertexShader.length() == 0 &&
		vertexShader.length() == 0)
		return renderer::INVALID_OBJECT;

	GLuint vsId = renderer::INVALID_OBJECT;
	GLuint fsId = renderer::INVALID_OBJECT;
	GLuint gsId = renderer::INVALID_OBJECT;

	std::string name;

	if( vertexShader.length() > 0 )
		name += std::string(vertexShader);

	if (fragmentShader.length() > 0)
		name += std::string(fragmentShader);

	if (geometryShader.length() > 0)
		name += std::string(geometryShader);

	auto it = ShaderMap.find( name );

	// did iterator not hit the end
	if( it != ShaderMap.end() )
	{
		it->second.refCount++;
		return static_cast<GLuint>(it->second.programId);
	}

	if (vertexShader.length() > 0)
		vsId = renderer::LoadShader( GL_VERTEX_SHADER, vertexShader );

	if (fragmentShader.length() > 0)
		fsId = renderer::LoadShader( GL_FRAGMENT_SHADER, fragmentShader );

#ifdef USE_OPENGL41
	if (geometryShader.length() > 0)
		gsId = renderer::LoadShader(GL_GEOMETRY_SHADER, geometryShader);
#endif

	if( vsId != renderer::INVALID_OBJECT ||
		fsId != renderer::INVALID_OBJECT ||
		gsId != renderer::INVALID_OBJECT )
	{
		// create and attach
		GLuint progId = glCreateProgram();

		if( vsId != renderer::INVALID_OBJECT )
			glAttachShader( progId, vsId );
	
		if( fsId != renderer::INVALID_OBJECT )
			glAttachShader( progId, fsId );

		if (gsId != renderer::INVALID_OBJECT)
			glAttachShader(progId, gsId);

		// Link the program object 
		glLinkProgram( progId );
			
		GL_CHECK

		// was linked
		GLuint linked = false;
		glGetProgramiv( progId, GL_LINK_STATUS, (GLint *)&linked );

#ifdef _DEBUG
		GLint status;
		glValidateProgram(progId);
		
		glGetProgramiv(progId, GL_VALIDATE_STATUS, &status);
		if (status == GL_FALSE)
		{
			DBGLOG( "SHADERS: status %d\n", status );
			return renderer::INVALID_OBJECT;
		}
		
		DBGLOG("v: %s | f:%s | g:%s\n", vertexShader.c_str(), fragmentShader.c_str(), geometryShader.c_str());
		renderer::PrintInfoLog( progId );
#endif // _DEBUG

		if( !linked )
		{
			return renderer::INVALID_OBJECT;
		}

		GL_CHECK

		ShaderStore addProgram;
		addProgram.refCount = 1;
		addProgram.programId = progId;
		addProgram.vertShaderId = vsId;
		addProgram.fragShaderId = fsId;
		addProgram.geomShaderId = gsId;
		addProgram.isFragmentShaderAFile = true;
		addProgram.isVertexShaderAFile = true;
		addProgram.isGeomShaderAFile = true;

		if (vertexShader.length() > 0)
			snprintf( addProgram.szVertexShaderFileOrString, MAX_SHADER_SIZE, "%s", vertexShader );

		if (fragmentShader.length() > 0)
			snprintf( addProgram.szFragmentShaderFileOrString, MAX_SHADER_SIZE, "%s", fragmentShader );

		if (geometryShader.length() > 0)
			snprintf( addProgram.szGeomShaderFileOrString, MAX_SHADER_SIZE, "%s", geometryShader );

		ShaderMap.insert( TNameIntPair( name, addProgram ) );

		return progId;
	}
	
	return renderer::INVALID_OBJECT;
}
