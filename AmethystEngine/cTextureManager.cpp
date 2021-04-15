#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>
#include <memory>

#ifdef _DEBUG
#define DEBUG_NEW new (_NORMAL_BLOCK , __FILE__ , __LINE__)
#else
#define DBG_NEW
#endif

#include <sstream>
#include "cTextureManager.hpp"

// change to set texture file extension or something of the sort, the base path will always be the same
// though the file extension could change currently this only handles bmps.
void cTextureManager::SetBasePath(std::string basepath) {
	this->m_basePath = basepath;
	return;
}


bool cTextureManager::Create2DTextureFromBMPFile(std::string textureFileName, bool bGenerateMIPMap) {
	std::string fileToLoadFullPath = this->m_basePath + "/" + textureFileName;


	CTextureFromBMP* pTempTexture = new CTextureFromBMP();
	if (!pTempTexture->CreateNewTextureFromBMPFile2(textureFileName, fileToLoadFullPath, /*textureUnit,*/ bGenerateMIPMap)) {
		this->m_appendErrorString("Can't load ");
		this->m_appendErrorString(fileToLoadFullPath);
		this->m_appendErrorString("\n");
		return false;
	}

	// Texture is loaded OK
	//this->m_nextTextureUnitOffset++;

	this->m_map_TexNameToTexture[textureFileName] = pTempTexture;

	return true;
}


void cTextureManager::m_appendErrorString(std::string nextErrorText) {
	std::stringstream ss;
	ss << this->m_lastError << nextErrorText;
	this->m_lastError = ss.str();
	return;
}

GLuint cTextureManager::getTextureIDFromName(std::string textureFileName) {
	std::map< std::string, CTextureFromBMP* >::iterator itTexture
		= this->m_map_TexNameToTexture.find(textureFileName);
	// Found it?
	if (itTexture == this->m_map_TexNameToTexture.end()) {
		return 0;
	}
	// Reutrn texture number (from OpenGL genTexture)
	return itTexture->second->getTextureNumber();
}


void cTextureManager::m_appendErrorStringLine(std::string nextErrorTextLine) {
	std::stringstream ss;
	ss << this->m_lastError << std::endl;
	ss << nextErrorTextLine << std::endl;
	this->m_lastError = ss.str();
	return;
}


// Picks a random texture from the textures loaded
std::string cTextureManager::PickRandomTexture(void) {
	if (this->m_map_TexNameToTexture.empty()) {
		// There are no textures loaded, yet.
		return "";
	}

	int textureIndexRand = rand() % (this->m_map_TexNameToTexture.size() + 1);
	if (textureIndexRand >= this->m_map_TexNameToTexture.size()) {
		textureIndexRand = 0;
	}

	std::map< std::string, CTextureFromBMP* >::iterator itTex = this->m_map_TexNameToTexture.begin();
	for (unsigned int count = 0; count != textureIndexRand; count++, itTex++) {}

	return itTex->second->getTextureName();
}


bool cTextureManager::CreateCubeTextureFromBMPFiles(
	std::string cubeMapName,
	std::string posX_fileName, std::string negX_fileName,
	std::string posY_fileName, std::string negY_fileName,
	std::string posZ_fileName, std::string negZ_fileName,
	bool bIsSeamless, std::string& errorString)
{
	std::string posX_fileName_FullPath = this->m_basePath + "/" + posX_fileName;
	std::string negX_fileName_FullPath = this->m_basePath + "/" + negX_fileName;
	std::string posY_fileName_FullPath = this->m_basePath + "/" + posY_fileName;
	std::string negY_fileName_FullPath = this->m_basePath + "/" + negY_fileName;
	std::string posZ_fileName_FullPath = this->m_basePath + "/" + posZ_fileName;
	std::string negZ_fileName_FullPath = this->m_basePath + "/" + negZ_fileName;

	GLenum errorEnum;
	std::string errorDetails;
	CTextureFromBMP* pTempTexture = new CTextureFromBMP();
	if (!pTempTexture->CreateNewCubeTextureFromBMPFiles(
		cubeMapName,
		posX_fileName_FullPath, negX_fileName_FullPath,
		posY_fileName_FullPath, negY_fileName_FullPath,
		posZ_fileName_FullPath, negZ_fileName_FullPath,
		bIsSeamless, errorEnum, errorString, errorDetails))
	{
		this->m_appendErrorString("Can't load ");
		this->m_appendErrorString(cubeMapName);
		this->m_appendErrorString(" because:\n");
		this->m_appendErrorString(errorString);
		this->m_appendErrorString("\n");
		this->m_appendErrorString(errorDetails);
		errorString += ("\n" + errorDetails);
		return false;
	}//if ( ! pTempTexture->CreateNewCubeTextureFromBMPFiles()

	// Texture is loaded OK
	//this->m_nextTextureUnitOffset++;

	this->m_map_TexNameToTexture[cubeMapName] = pTempTexture;

	return true;
}

// NOTE: This should really be changed to be a loop based off of things passed in and textures should be given a definition
// as to what any given range of textures will contain...
void cTextureManager::SetUpTextureBindingsForObject(cModelObject* pCurrentObject, GLint shaderProgID) {
	if (pCurrentObject->v_textureNames.size() == 0)
	{
		GLuint ID = mpTexture["Default"];
		glActiveTexture(GL_TEXTURE0);		// Texture Unit 0
		glBindTexture(GL_TEXTURE_2D, ID);	// Texture now assoc with texture unit 0


		GLint textSamp00_UL = glGetUniformLocation(shaderProgID, "textSamp00");
		glUniform1i(textSamp00_UL, 0);	// Texture unit 0

		GLint textSamp01_UL = glGetUniformLocation(shaderProgID, "textSamp01");
		glUniform1i(textSamp01_UL, 0);	// Texture unit 0

		GLint tex0_ratio_UL = glGetUniformLocation(shaderProgID, "tex_0_1_ratio");
		glUniform2f(tex0_ratio_UL, 1.0f, 0.0f);
	}
	else if (pCurrentObject->v_textureNames.size() == 1)
	{
		GLuint ID = mpTexture[pCurrentObject->v_textureNames[0]];
		glActiveTexture(GL_TEXTURE0);				// Texture Unit 0
		glBindTexture(GL_TEXTURE_2D, ID);	// Texture now assoc with texture unit 0


		GLint textSamp00_UL = glGetUniformLocation(shaderProgID, "textSamp00");
		glUniform1i(textSamp00_UL, 0);	// Texture unit 0

		GLint textSamp01_UL = glGetUniformLocation(shaderProgID, "textSamp01");
		glUniform1i(textSamp01_UL, 0);	// Texture unit 0

		GLint tex0_ratio_UL = glGetUniformLocation(shaderProgID, "tex_0_1_ratio");
		glUniform2f(tex0_ratio_UL, pCurrentObject->v_texureBlendRatio[0], 0.0f);
	}
	else if (pCurrentObject->v_textureNames.size() == 2)
	{
		GLuint ID0 = mpTexture[pCurrentObject->v_textureNames[0]];
		glActiveTexture(GL_TEXTURE0);				// Texture Unit 0
		glBindTexture(GL_TEXTURE_2D, ID0);	// Texture now assoc with texture unit 0

		GLuint ID1 = mpTexture[pCurrentObject->v_textureNames[1]];
		glActiveTexture(GL_TEXTURE1);				// Texture Unit 0
		glBindTexture(GL_TEXTURE_2D, ID1);	// Texture now assoc with texture unit 0


		GLint textSamp00_UL = glGetUniformLocation(shaderProgID, "textSamp00");
		glUniform1i(textSamp00_UL, 0);	// Texture unit 0

		GLint textSamp01_UL = glGetUniformLocation(shaderProgID, "textSamp01");
		glUniform1i(textSamp01_UL, 1);	// Texture unit 1

		GLint tex0_ratio_UL = glGetUniformLocation(shaderProgID, "tex_0_1_ratio");
		glUniform2f(tex0_ratio_UL, pCurrentObject->v_texureBlendRatio[0], pCurrentObject->v_texureBlendRatio[1]);
	}

	return;
}