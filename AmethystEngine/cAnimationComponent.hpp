#pragma once
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <map>
#include <vector>
#include <array>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "sModelDrawInfo.hpp"
#include "iGameObject.hpp"
#include <queue>


class cAnimationComponent : public iGameObject {
public:
	// Is STATIC const, so can be assigned a value here
	// (non static const have to be set in the cpp file)
	static const int MAX_BONES_PER_VERTEX = 4;

private:
	struct sVertexBoneData	{
		//std::array<unsigned int, MAX_BONES_PER_VERTEX> ids;
		std::array<float, MAX_BONES_PER_VERTEX> ids;
		std::array<float, MAX_BONES_PER_VERTEX> weights;

		void AddBoneData(unsigned int BoneID, float Weight);
	};

	struct sBoneInfo {
		std::string boneName;
		glm::mat4 BoneOffset;
		glm::mat4 FinalTransformation;
		glm::mat4 ObjectBoneTransformation;
	};

	struct sAnimationInfo {
		std::string friendlyName;
		std::string fileName;
		const aiScene* pAIScene;
	};

	std::queue<std::string> AnimaQueue;
	float AnimaTime;

	std::vector<sModelDrawInfo*> _vec_meshes;

	Assimp::Importer* pImporter;
	std::string fileName;
	std::string prevAnimName;
	const aiScene* pScene;
	glm::mat4 mGlobalInverseTransformation;
	std::map<std::string, sAnimationInfo> mapAnimationFriendlyNameTo_pScene;
	std::map<std::string, unsigned> m_mapBoneNameToBoneIndex;
	std::vector<std::vector<sVertexBoneData>> vecVertexBoneData;
	std::vector<sBoneInfo> mBoneInfo;
	unsigned int mNumBones;
	glm::mat4 scaleMatrix;

	void CalcInterpolatedRotation(float AnimationTime, const aiNodeAnim* pNodeAnim, aiQuaternion& out);
	void CalcInterpolatedPosition(float AnimationTime, const aiNodeAnim* pNodeAnim, aiVector3D& out);
	void CalcInterpolatedScaling(float AnimationTime, const aiNodeAnim* pNodeAnim, aiVector3D& out);

	void CalcGLMInterpolatedRotation(float AnimationTime, const aiNodeAnim* pNodeAnim, glm::quat& out);
	void CalcGLMInterpolatedPosition(float AnimationTime, const aiNodeAnim* pNodeAnim, glm::vec3& out);
	void CalcGLMInterpolatedScaling(float AnimationTime, const aiNodeAnim* pNodeAnim, glm::vec3& out);

	unsigned FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim);
	unsigned FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim);
	unsigned FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim);

	void LoadBones(const aiMesh* Mesh, std::vector<sVertexBoneData>& Bones);

	void ReadNodeHeirarchy(float AnimationTime,
		std::string animationName,		// Now with more "choose animation"
		const aiNode* pNode,
		const glm::mat4& parentTransformMatrix);

	// Looks in the animation map and returns the total time
	float FindAnimationTotalTime(std::string animationName);

	const aiNodeAnim* FindNodeAnimationChannel(const aiAnimation* pAnimation, aiString nodeOrBoneName);

public:
	cAnimationComponent(void);
	virtual ~cAnimationComponent(void);

	const aiScene* GetScene() { return pScene; }
	void SetScene(const aiScene* newScene) { pScene = newScene; }

	glm::mat4 GetGlobalInverseTransformation() { return mGlobalInverseTransformation; }
	void SetGlobalInverseTransformation(glm::mat4 InvTrans) { mGlobalInverseTransformation = InvTrans; }

	std::vector<sModelDrawInfo*>& GetMeshes() { return _vec_meshes; }

	const int GetNumBones() { return mNumBones; }

	const std::string GetFileName() { return fileName; }
	void SetFileName(std::string name) { fileName = name; }

	const float GetScale() { return scaleMatrix[0][0]; }
	const glm::mat4 GetScaleMatrix() { return scaleMatrix; }
	void SetScale(float scale) { scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(scale)); }

	// Added to assist with the forward kinematics (id and bone names)
	void GetListOfBoneIDandNames(std::vector<std::string>& vecBoneNames);

	// Returns an empty vector if there is no mesh at that index
	sModelDrawInfo* CreateModelDrawInfoObjectFromCurrentModel(size_t meshNum);
	bool LoadModel_Assimp(unsigned meshNum, sModelDrawInfo *theMesh);

	void BoneTransform(std::vector<glm::mat4>& FinalTransformation, std::vector<glm::mat4>& Globals, std::vector<glm::mat4>& Offsets);

	unsigned GetNums(void) const { return this->mNumBones; }	
	float GetDuration(void);

	float GetTime(void) { return this->AnimaTime; }
	void SetTime(float newTime) { this->AnimaTime = newTime; }

	//cant call without pScene - loader needs to handle
	bool Initialize(void);

	//Keep here, since animations are bound to this object?
	bool LoadMeshFromFile(const std::string& friendlyName, const std::string& filename);
	bool LoadMeshAnimation(const std::string& friendlyName, const std::string& filename);	// Only want animations

	bool QueueAnimation(std::string AnimaName);
	void PopAnimation() { this->AnimaQueue.pop(); }
	bool QueueEmpty() { return AnimaQueue.empty(); }
	std::string GetCurrentAnimation() { return this->AnimaQueue.front(); }

	glm::vec3 getBonePositionByBoneNameModelSpace(std::string name);
	std::string GetPrevAnimation() { return this->prevAnimName; }
	void SetPrevAnimation(std::string name) { this->prevAnimName = name; }

	// Inherited via iGameObject
	virtual unsigned int getUniqueID(void) override;
	virtual glm::vec3 getPosition(void) override;

	iGameObject* GetParent() { return this->parentObject; }
	iGameObject* SetParent(iGameObject* EntityPointer) { this->parentObject = EntityPointer; }
};