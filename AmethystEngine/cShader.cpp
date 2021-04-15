#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>
#include <memory>

#ifdef _DEBUG
#define DEBUG_NEW new (_NORMAL_BLOCK , __FILE__ , __LINE__)
#else
#define DBG_NEW
#endif

#include "cShaderManager.hpp"
#include "GLCommon.hpp"

cShaderManager::cShader::cShader() {
	this->ID = 0;
	this->shaderType = cShader::UNKNOWN;
	return;
}

cShaderManager::cShader::~cShader() { }

std::string cShaderManager::cShader::getShaderTypeString() {
	switch (this->shaderType) {
	case cShader::VERTEX_SHADER:
		return "VERTEX_SHADER";
		break;
	case cShader::FRAGMENT_SHADER:
		return "FRAGMENT_SHADER";
		break;
	case cShader::UNKNOWN:
	default:
		return "UNKNOWN_SHADER_TYPE";
		break;
	}
	return "UNKNOWN_SHADER_TYPE";
}


// Look up the uniform inside the shader, then save it, if it finds it
bool cShaderManager::cShaderProgram::LoadUniformLocation(std::string variableName) {
	GLint uniLocation = glGetUniformLocation(this->ID, variableName.c_str());
	// Did it find it (not -1)
	if (uniLocation == -1)
		return false;
	// Save it
	this->mapUniformName_to_UniformLocation[variableName.c_str()] = uniLocation;
	return true;
}

// Look up the uniform location and save it.
int cShaderManager::cShaderProgram::getUniformID_From_Name(std::string name) {
	std::map< std::string /*name of uniform variable*/, int /* uniform location ID */ >::iterator itUniform = this->mapUniformName_to_UniformLocation.find(name);

	if (itUniform == this->mapUniformName_to_UniformLocation.end())
		return -1;		// OpenGL uniform not found value
	return itUniform->second;		// second if the "int" value
}
