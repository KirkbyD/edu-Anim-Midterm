#pragma once
#include "cError.hpp"
#include "iMediatorInterface.hpp"
#include "cActorObject.hpp"
#include "nAI.hpp"

extern std::vector<cActorObject*> g_vec_pActorObjects;

class cActorManager {
	cActorManager();
	static cActorManager stonActMngr;
	iMediatorInterface* pMediator;

	cError error;

public:
	~cActorManager() { }

	// SINGLETON FUNCTIONS
	static cActorManager* GetActorManager();

	// MEDIATOR FUNCTIONS
	void setMediatorPointer(iMediatorInterface* pMediator);
	virtual sData RecieveMessage(sData& data);

	// MEDIATOR COMMAND FUCNTIONS
	bool UnselectActors();
	bool SelectActors();
	bool SetFormations(sData& data);

	void AssignFormationPositions();

	void update(sData& data);
};
