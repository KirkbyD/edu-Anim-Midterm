#pragma once
#include "iPhysicsComponent.h"

namespace nPhysics {
	class iPhysicsWorld 	{
	public:
		virtual ~iPhysicsWorld() {}
		
		// Moves all nonstatic bodies in the world.
		virtual void Update(float dt) = 0;
		// Add Components to the world for movement and collisions.
		virtual bool AddComponent(iPhysicsComponent* component) = 0;
		// Remove a physics component from the simulation
		virtual bool RemoveComponent(iPhysicsComponent* component) = 0;
		// Set the worlds gravity, defaults to 0, 0, 0
		virtual void SetGravity(glm::vec3 gravity) = 0;
	};
}