#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>
#include <memory>

#ifdef _DEBUG
#define DEBUG_NEW new (_NORMAL_BLOCK , __FILE__ , __LINE__)
#else
#define DBG_NEW
#endif

#include "cAIManager.hpp"
#include "nAI.hpp"
#include <iostream>

#pragma region SINGLETON
cAIManager cAIManager::stonAIMngr;
cAIManager* cAIManager::GetAIManager() { return &(cAIManager::stonAIMngr); }
cAIManager::cAIManager()
	: ai_behaviours(), pMediator(nullptr)
{ std::cout << "AI Manager Created" << std::endl; }
#pragma endregion



#pragma region MEDIATOR SETUP
void cAIManager::setMediatorPointer(iMediatorInterface* pMediator) { this->pMediator = pMediator; }
#pragma endregion



#pragma region MEDIATOR COMMUNICATIONS
sData cAIManager::RecieveMessage(sData& data) {
	data.setResult(OK);
	return data;
}
#pragma endregion



#pragma region AI BEHAVIOURS UPDATE
void cAIManager::update(sData& data) {
	// change this such that it passes through only what is needed to the AI Behaviours
	// before this switch sends data to the AI Behaviours it needs to determine if the behaviour in question can be enacted...
	switch (data.getSourceGameObj()->GetActor()->getActiveBehaviour()) {
	case nAI::eAIBehviours::IDLE:
		// check physics to make sure the AI can be enacted...
		ai_behaviours.idle(data);
		break;

	case nAI::eAIBehviours::SEEK:
		ai_behaviours.seek(data);
		break;

	case nAI::eAIBehviours::APPROACH:
		ai_behaviours.approach(data);
		break;

	case nAI::eAIBehviours::PURSURE:
		ai_behaviours.pursure(data);
		break;

	case nAI::eAIBehviours::FLEE:
		ai_behaviours.flee(data);
		break;

	case nAI::eAIBehviours::EVADE:
		ai_behaviours.evade(data);
		break;

	case nAI::eAIBehviours::WANDER:
		ai_behaviours.wander(data);
		break;

	case nAI::eAIBehviours::FOLLOW_PATH:
		// TODO: Implement for second project
		break;

	default:
		break;
	}

	// update any active formations after the steering behaviours
	switch (data.getSourceGameObj()->GetActor()->getActiveFormation()) {
	case nAI::eFormationBehaviours::NONE:
		break;

	case nAI::eFormationBehaviours::CIRCLE:
		if (data.getSourceGameObj()->GetActor()->isSelected()) {
			for (auto a : g_vec_pActorObjects) {
				if (a->_is_anchor == true) {
					data.setSourceGameObj((cComplexObject*)a);
					break;
				}
			}
			ai_behaviours.formCircle(data);
		}
		break;

	case nAI::eFormationBehaviours::COLUMN:
		if (data.getSourceGameObj()->GetActor()->isSelected()) {
			for (auto a : g_vec_pActorObjects) {
				if (a->_is_anchor == true) {
					data.setSourceGameObj((cComplexObject*)a);
					break;
				}
			}
			ai_behaviours.formColumn(data);
		}
		break;

	case nAI::eFormationBehaviours::LINE:
		if (data.getSourceGameObj()->GetActor()->isSelected()) {
			for (auto a : g_vec_pActorObjects) {
				if (a->_is_anchor == true) {
					data.setSourceGameObj((cComplexObject*)a);
					break;
				}
			}
			ai_behaviours.formLine(data);
		}
		break;

	case nAI::eFormationBehaviours::SQUARE:
		if (data.getSourceGameObj()->GetActor()->isSelected()) {
			for (auto a : g_vec_pActorObjects) {
				if (a->_is_anchor == true) {
					data.setSourceGameObj((cComplexObject*)a);
					break;
				}
			}
			ai_behaviours.formBox(data);
		}
		break;

	case nAI::eFormationBehaviours::WEDGE:
		if (data.getSourceGameObj()->GetActor()->isSelected()) {
			for (auto a : g_vec_pActorObjects) {
				if (a->_is_anchor == true) {
					data.setSourceGameObj((cComplexObject*)a);
					break;
				}
			}
			ai_behaviours.formWedge(data);
		}
		break;

	default:
		break;
	}

}
#pragma endregion
