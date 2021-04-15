#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>
#include <memory>

#ifdef _DEBUG
#define DEBUG_NEW new (_NORMAL_BLOCK , __FILE__ , __LINE__)
#else
#define DBG_NEW
#endif

#include <iostream>`
#include "cAnimationManager.hpp"
#include "nConvert.hpp"
#include <glm\gtc\type_ptr.hpp>

extern bool MIDTERM_FALLING;

#pragma region SINGLETON
cAnimationManager cAnimationManager::stonAnimaMngr;
cAnimationManager* cAnimationManager::GetAnimationManager() { return &(cAnimationManager::stonAnimaMngr); }
cAnimationManager::cAnimationManager() {
	std::cout << "Animation Manager Created" << std::endl;
	pMediator = nullptr;
}
#pragma endregion


void cAnimationManager::DeconstructAnimationComponents() {
	for (std::pair<std::string, cAnimationComponent*> it : mpAnima) {
		delete it.second;
	}
	mpAnima.clear();
}

cAnimationComponent* cAnimationManager::LoadMeshFromFile(const std::string& friendlyName, const std::string& filename) {
	cAnimationComponent* AnimaObj = dynamic_cast<cAnimationComponent*>(this->_fact_game_obj.CreateGameObject("[ANIMATION]"));

	if (AnimaObj->LoadMeshFromFile(friendlyName, filename)) {
		AnimaObj->friendlyIDNumber = Module_Hex_Value | System_Hex_Value | next_UniqueComponentID++;
		mpAnima[friendlyName] = AnimaObj;
		return AnimaObj;
	}
	return nullptr;
}

void cAnimationManager::IterateComponent(cAnimationComponent* component, float dt) {
	float TicksPerSecond = static_cast<float>(component->GetScene()->mAnimations[0]->mTicksPerSecond != 0 ?
		component->GetScene()->mAnimations[0]->mTicksPerSecond : 25.0);

	float TimeInTicks = dt * TicksPerSecond;
	//float AnimationTime = fmod(TimeInTicks, (float)component->GetScene()->mAnimations[0]->mDuration);

	//check prev anim
	std::string prev = component->GetPrevAnimation();
	bool wasIdle = (prev == "Idle") ? true : false;

	float AnimaTime = component->GetTime() + TimeInTicks;
	cComplexObject* pParent = dynamic_cast<cComplexObject*>(component->GetParent());

	std::string prevAnim = pParent->getCurrentAnimationName();

	if (AnimaTime > (float)component->GetScene()->mAnimations[0]->mDuration /*|| wasIdle*/) {
		component->SetTime(0.f);
		AnimaTime = 0.f;

		//Resolve previous animation
		if (prevAnim == "Fall") {
			glm::vec3 currPos = pParent->getPosition();
			currPos.y -= 7.f * dt;
			pParent->SetPosition(currPos);

			if (!component->QueueEmpty())
				component->PopAnimation();

			if (MIDTERM_FALLING) {
				if (component->QueueEmpty())
					component->QueueAnimation("Fall");
			}
		}

		else if (!wasIdle) {
			if (prevAnim == "Death") {
				if (!component->QueueEmpty())
					component->PopAnimation();
			}

			else if (prevAnim != "Punch") {
				glm::vec3 currPos = pParent->getPosition();
				glm::vec3 LFPos = pParent->getBonePositionByBoneName("B_L_Foot");
				glm::vec3 RFPos = pParent->getBonePositionByBoneName("B_R_Foot");
				glm::vec3 newPos(0.f);
				newPos.y = currPos.y;
				newPos.z = (LFPos.z + RFPos.z) * 0.5f;
				pParent->SetPosition(newPos);
				component->PopAnimation();
			}

			else
				component->PopAnimation();
			//Clear animations and dispatch idle if queue empty
		}

		if (component->QueueEmpty()) {
			component->QueueAnimation("Idle");
		}

		//Grab new animation name
		std::string currAnim = pParent->getCurrentAnimationName();
		//Dispatch physics Impulses
		float accelSpeed = 10.f;
		if (currAnim == "Idle") {
			pParent->VelocityZero();
		}
		else {
			pParent->VelocityZero();
			if (currAnim == "WalkF") {
				pParent->SetOrientation(glm::quatLookAt(glm::vec3(0.f, 0.f, -1.f), glm::vec3(0.f, 1.f, 0.f)));
			}
			else if (currAnim == "WalkB") {
				pParent->SetOrientation(glm::quatLookAt(glm::vec3(0.f, 0.f, 1.f), glm::vec3(0.f, 1.f, 0.f)));
			}
			else if (currAnim == "RunF") {
				pParent->SetOrientation(glm::quatLookAt(glm::vec3(0.f, 0.f, -1.f), glm::vec3(0.f, 1.f, 0.f)));
			}
			else if (currAnim == "RunB") {
				pParent->SetOrientation(glm::quatLookAt(glm::vec3(0.f, 0.f, 1.f), glm::vec3(0.f, 1.f, 0.f)));
			}
		}

		component->SetPrevAnimation(currAnim);
	}

	else {
		std::string anima = component->GetPrevAnimation();
		if (anima == "Jump") {
			glm::vec3 currPos = pParent->getPosition();
			glm::vec3 LFPos = pParent->getBonePositionByBoneName("B_L_Foot");
			glm::vec3 RFPos = pParent->getBonePositionByBoneName("B_R_Foot");
			glm::vec3 newPos(0.f);
			//adjust for previous animation here (z movement)?
			newPos.x = currPos.x;

			newPos.y = currPos.y + (LFPos.y + RFPos.y - currPos.y * 2.f) * 0.11f;
			
			if(prevAnim == "WalkF")
				newPos.z = currPos.z + 0.1f;
			else if(prevAnim == "RunF")
				newPos.z = currPos.z + 0.3f;
			else if(prevAnim == "WalkB")
				newPos.z = currPos.z + 0.1f;
			else if(prevAnim == "RunB")
				newPos.z = currPos.z + 0.3f;
			else
				newPos.z = currPos.z;

			pParent->SetPosition(newPos);
		}
		else if (anima == "Fall") {
			if (MIDTERM_FALLING) {
				glm::vec3 currPos = pParent->getPosition();
				currPos.y -= 7.f * dt;
				pParent->SetPosition(currPos);
			}
			else {
				component->PopAnimation();
				if (component->QueueEmpty()) {
					component->QueueAnimation("Idle");
				}
			}
		}
		component->SetTime(AnimaTime);
	}
	return;
}

void cAnimationManager::Update(float dt) {
	for (auto it : mpAnima) {
		auto pCurrentObject = it.second;

		IterateComponent(pCurrentObject, dt);

		// Set to all identity
		const int NUMBEROFBONES = pCurrentObject->GetNumBones();

		// Taken from "Skinned Mesh 2 - todo.docx"
		std::vector< glm::mat4x4 > vecFinalTransformation;
		std::vector< glm::mat4x4 > vecOffsets;
		std::vector< glm::mat4x4 > vecObjectBoneTransformation;

		// This loads the bone transforms from the animation model
		pCurrentObject->BoneTransform(mpFinalTransformation[it.first],
									  mpOffsets[it.first],
									  mpObjectBoneTransforms[it.first]);

		// Wait until all threads are done updating.
	}
	return;
}

void cAnimationManager::Render(cRenderer* pRenderer, GLint shaderProgID, cVAOManager* pVAOManager) {
	for (auto it : mpAnima) {
		auto pCurrentObject = it.second;

		GLint numBonesUsed = (GLint)mpFinalTransformation[it.first].size();

		std::vector<sModelDrawInfo*> vecDrawInfo = it.second->GetMeshes();

		for (size_t i = 0; i < vecDrawInfo.size(); i++) {
			pRenderer->RenderAnimaObject(pCurrentObject, vecDrawInfo[i], shaderProgID, pVAOManager, numBonesUsed, glm::value_ptr(mpFinalTransformation[it.first][0]));
		}
	}
	return;
}



#pragma region MEDIATOR_COMMUNICATION
void cAnimationManager::setMediatorPointer(iMediatorInterface* pMediator) {
	this->pMediator = pMediator;
	return;
}

sData cAnimationManager::RecieveMessage(sData& data) {
	data.setResult(OK);

	return data;
}
#pragma endregion
