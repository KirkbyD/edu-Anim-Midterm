#pragma once
#include "iBallComponent.h"
#include "iPlaneComponent.h"
#include "iPhysicsWorld.h"

namespace nPhysics {
	//iPhysicsFactoru
	//
	// Abstract factory to allow our game to hook up to wrapper factories of different libraries.
	class iPhysicsFactory {
	public:
		virtual ~iPhysicsFactory() {}
		virtual iPhysicsWorld* CreateWorld() = 0;
		// components
		virtual iBallComponent* CreateBall(const sBallDef& def, const unsigned& id) = 0;

		virtual iPlaneComponent* CreatePlane(const sPlaneDef& def, const unsigned& id) = 0;
	};
}