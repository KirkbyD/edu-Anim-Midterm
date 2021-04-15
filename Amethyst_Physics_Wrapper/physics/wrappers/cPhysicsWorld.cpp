#include "cPhysicsWorld.hpp"
#include <physics/interfaces/eComponentType.h>
#include "cBallComponent.hpp"
#include "cPlaneComponent.hpp"

nPhysics::cPhysicsWorld::cPhysicsWorld() {
	mWorld = new phys::cWorld();
}

void nPhysics::cPhysicsWorld::Update(float dt) {
	mWorld->Update(dt);
}

bool nPhysics::cPhysicsWorld::AddComponent(iPhysicsComponent* component) {
	switch (component->GetComponentType()) {
	case eComponentType::ball:
		return mWorld->AddRigidBody(((cBallComponent*)component)->mRigidBody);
	case eComponentType::plane:
		return mWorld->AddRigidBody(((cPlaneComponent*)component)->mRigidBody);
	default:
		return false;
	}
}

bool nPhysics::cPhysicsWorld::RemoveComponent(iPhysicsComponent* component) {
	switch (component->GetComponentType()) {
	case eComponentType::ball:
		return mWorld->RemoveRigidBody(((cBallComponent*)component)->mRigidBody);
	case eComponentType::plane:
		return mWorld->RemoveRigidBody(((cPlaneComponent*)component)->mRigidBody);
	default:
		return false;
	}
}

void nPhysics::cPhysicsWorld::SetGravity(glm::vec3 gravity) {
	mWorld->SetGravity(gravity);
}
