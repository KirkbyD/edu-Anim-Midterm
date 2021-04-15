#pragma once
#include <fstream>
#include <vector>
#include <string>
#include <sstream>

#include "cModelObject.hpp"
#include "cMesh.hpp"
#include "cVAOManager.hpp"
#include "cModelLoader.hpp"
#include "cShaderManager.hpp"
#include "cLightManager.hpp"
#include "cGameObjectFactory.hpp"
#include "cLightObject.hpp"
#include "cError.hpp"
#include "Globals.hpp"
#include "cPhysicsManager.hpp"
#include "cAnimationManager.hpp"
#include "cAnimationComponent.hpp"


class cFileManager {
	cFileManager();
	static cFileManager stonFileMngr;

	cError errmngr;
	cGameObjectFactory fact_game_obj;

	std::string _filePath = "data/config/";
	std::string _fileExt = ".ini";
	std::string _luaExt = ".txt";

	std::string _graphics_path = "models/";
	std::string _lights_path = "lights/";
	std::string _audio_path = "audio/";
	std::string _complex_object_path = "complex/";
	std::string _physics_path = "physics/";
	std::string _script_path = "scripts/lua/";

	std::string _model_File = "models/models";
	std::string _lights_File = "lights/lights";
	std::string _mesh_File = "models/meshes";
	std::string _texture_File = "textures/textures";
	std::string _skybox_texture_File = "textures/skybox_textures";
	std::string _audio_File = "audio/audio";
	std::string _script_File = "scripts/lua/animascripts";
	std::string _physics_library_path = "physics/PhysicsLibraries.json";

	std::string _texture_path = "data/textures";
	std::string _skybox_texture_path = "data/textures/skyboxtextures";

	std::string _shaders_File = "shaders/shaders";
	std::string _complexObj_File = "complex/complex";

	std::string _entity_path = "complex/Entities.json";

	std::string _config_node;
	std::string _lhs, _rhs;
	std::vector<std::string> _vlhs, _vrhs;

	std::ifstream _input;
	std::ofstream _output;

	std::stringstream _data_stream;

	std::vector<std::string> _data;
	std::string _tempData;

	std::vector<float> vf;
	float f = 0.0f;

	int n = 0;

	void LoadData(std::string filename);

	void FormatModelData();
	void FormatLightData();
	void FormatComplexModelData();

	// Converts the string into floats and populates the vf ( vector float ) vector with the values.
	void ConvertStringToFloatArray(std::string cs);

	// Converts the string into a float and stores it in the f ( float ) float with the value.
	void ConvertStringToFloat(std::string cs);

	void ConvertStringToInt(std::string cs);

	// Resets the conversion variables.
	void ResetConversionVars();

	// IF NOT FOUND RETURNS 0 ( NULL )
	eShapeTypes DetermineShapeType(std::string type, std::map<std::string, eShapeTypes>& mpShapeTypes);
	std::string GetShapeTypeString(eShapeTypes shapeType);
	bool CheckDecimal(float num);


#pragma region Entity_Structs
	// sTransformComponent
	struct sTransformComponent {
		float scale;
		glm::vec3 position;
		glm::vec3 orientation;
	};
	
	// sGraphicsComponent
	// 
	// Contains all graphics only related information
	// necessary to create a cModelObject (TODO convert to graphics component) instance.
	struct sGraphicsComponent {
		std::string name;
		std::string mesh;
		std::string texture;
		float scale;
	};

	// sAnimationCompnent
	struct sAnimationComponent {
		struct sMemberAnimations {
			std::string File;
			std::string AnimationName;
		};
		std::string filename;
		std::string friendlyname;
		float scale;
		std::vector<sMemberAnimations> animationFiles;
	};

	// sPhysicsComponent
	// 
	// Contains all physics related information
	// necessary to create a iPhysicsComponent instance.
	struct sPhysicsComponent {
		//Gather all potential physics variables here, dispatch correct factory command based on shape type.
		nPhysics::eComponentType shape;
		glm::vec3 position;
		glm::vec3 velocity;
		glm::vec3 normal;
		float mass;
		float radius;
		std::string Bone;

		// Default Constructor
		// Sets all value's to 0
		sPhysicsComponent() 
			: shape(nPhysics::eComponentType::unknown)
			, position(0.f)
			, velocity(0.f)
			, normal(0.f)
			, mass(0.f)
			, radius(0.f)
			, Bone("") { }
	};

	// sEntity
	// 
	// Contains all information regarding one entity.
	// necessary to create a ComplexOPbject(Entity) instance.
	struct sEntity {
		std::string Name;
		sTransformComponent TransformComponent;
		std::vector<sPhysicsComponent> PhysicsComponents;
		std::vector<sGraphicsComponent> GraphicsComponents;
		std::vector< sAnimationComponent> AnimationComponents;
	};
	
	// sEntitySerializer
	// 
	// Contains a json serializer (TODO) and deserializer for all entity types.
	// Holds the information of every entity loaded into the program.
	struct sEntitySerializer : iSerializable {
		std::vector<sEntity> Entities;

		// Inherited via iSerializable
		// Convert information of in program entities to json for saving.
		// TODO
		virtual bool serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>* writer) override {
			return false;
		}

		// Deserialize entity's and populate this struct's data.
		virtual bool deserialize(const rapidjson::Document* document) override;
	};
#pragma endregion

	sEntitySerializer EntitySerializer;

public:
	static cFileManager* GetFileManager();

	std::string GetModelFile();
	std::string GetMeshFile();
	std::string GetTextureFile();
	std::string GetSkyboxTextureFile();
	std::string GetLightsFile();
	std::string GetShadersFile();
	std::string GetAudioFile();
	std::string GetComplexObjFile();

	bool LoadPhysicsLibraryStruct(sPhysicsLibs* PhysicsDLLs);
	void LoadModelsOntoGPU(cModelLoader* pModelLoader, cVAOManager* pVAOManager, const GLuint& shaderProgID, std::string filename, std::map<std::string, cMesh*>& mpMesh);
	void LoadAnimationsOntoGPU(cAnimationManager* pAnimaManager, cVAOManager* pVAOManager, const GLuint& shaderProgID);
	void LoadTexturesOntoGPU(cBasicTextureManager* pTextureManager, std::string filename, std::map<std::string, GLuint>& mpTex);
	void LoadSkyboxTexturesOntoGPU(cBasicTextureManager* pTextureManager, std::string filename, std::map<std::string, GLuint>& mpTex);
	void LoadShaders(std::string filename, std::map<std::string, cShaderManager::cShader>& mpVertexShader, std::map<std::string, cShaderManager::cShader>& mpFragmentShader);

	void BuildObjects(std::string filename);
	void BuildEntities(cPhysicsManager* pPhysicsManager, cAnimationManager* pAnimationManager);

	// SAVES ALL THREE OBJECT TYPES
	bool SaveData();
	bool SaveData(std::string modelsFilename, std::string lightsFilename, std::string complexFilename);

	// LUA SCRIPTING
	void LoadLuaScripts();
};
