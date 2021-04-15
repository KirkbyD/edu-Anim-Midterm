#include "cPhysicsFactory.hpp"

nPhysics::cPhysicsFactory::cPhysicsFactory() { }

nPhysics::iPhysicsWorld* nPhysics::cPhysicsFactory::CreateWorld() {
	return new cPhysicsWorld();
}

nPhysics::iBallComponent* nPhysics::cPhysicsFactory::CreateBall(const sBallDef& def, const unsigned& id) {
	return new cBallComponent(def, id);
}

nPhysics::iPlaneComponent* nPhysics::cPhysicsFactory::CreatePlane(const sPlaneDef& def, const unsigned& id) {
	return new cPlaneComponent(def, id);
}