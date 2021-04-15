#pragma once
#include <physics/interfaces/iPhysicsFactory.h>
#include "cPhysicsWorld.hpp"
#include "cBallComponent.hpp"
#include "cPlaneComponent.hpp"

namespace nPhysics {
	class cPhysicsFactory : public iPhysicsFactory {
	public:
		cPhysicsFactory();
		virtual ~cPhysicsFactory() {}

		// Inherited via iPhysicsFactory
		virtual iPhysicsWorld* CreateWorld() override;
		virtual iBallComponent* CreateBall(const sBallDef& def, const unsigned& id) override;
		virtual iPlaneComponent* CreatePlane(const sPlaneDef& def, const unsigned& id) override;
	};
}