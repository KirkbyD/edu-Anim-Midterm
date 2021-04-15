#pragma once
#include <physics/interfaces/iPhysicsWorld.h>
#include <cWorld.hpp>

namespace nPhysics {
	class cPhysicsWorld : public iPhysicsWorld {
	private:
		phys::cWorld* mWorld;

	public:
		cPhysicsWorld();
		virtual ~cPhysicsWorld() {}

		virtual void Update(float dt);
		virtual bool AddComponent(iPhysicsComponent* component);
		virtual bool RemoveComponent(iPhysicsComponent* component);
		virtual void SetGravity(glm::vec3 gravity);
	};
}