#pragma once
#include "cRigidBody.hpp"
#include "Shapes/cSphere.hpp"
#include "Shapes/cPlane.hpp"
#include "cIntegrator.hpp"
#include <vector>

namespace phys {
	// cWorld
	// A rigid body simulator.
	// 
	// Pointers to cRigidBody instances can be added and removed.
	// 
	// cWorld will operate on the contained cRigidBody's during timesteps
	// triggered by calls to cWorld::Update(dt)
	// 
	// cWorld does not own cRigidBody pointers, and will not
	// delete them when it is deleted.
	class cWorld {
	private:
		// Acceleration due to gravity applied to each rigid body
		// during each timestep.
		glm::vec3 mGravity;
		// All the rigid bodies currently in the world.
		// Not owned by cWorld, will not be deleted in the destructor.
		std::vector<cRigidBody*> mBodies;
		// The delta time input cached during each Update(dt) call.
		float mDt;

		// Our handy dandy integrator.
		cIntegrator mIntegrator;

		// Constructors not to be used.
		cWorld(const cWorld& other) = delete;
		cWorld& operator=(const cWorld& other) = delete;

	protected:
		// IntegrateRigidBody
		// Performs a single numerical integration step.
		// Safe for null pointers.
		// Safe for static rigid bodies.
		void IntegrateRigidBody(cRigidBody* body, float dt);

		// Collide
		// Entry point for collision detection.
		// Returns the result of specific Collide[shapeA][ShapeB] methods.
		bool Collide(cRigidBody* bodyA, cRigidBody* bodyB);

		// CollideSpherePlane
		// Handles collision detection and reaction between a sphere and a plane.
		// Returns true if a collision occured.
		// Returns false if no collision occured.
		bool CollideSpherePlane(cRigidBody* sphereBody, cSphere* sphereShape, cRigidBody* planeBody, cPlane* planeShape);

		// CollideSphereSphere
		// Handles collision detection and reaction between two spheres.
		// Returns true if a collision occured.
		// Returns false if no collision occured.
		bool CollideSphereSphere(cRigidBody* sphereBodyA, cSphere* sphereShapeA, cRigidBody* sphereBodyB, cSphere* sphereShapeB);

	public:
		// Create a default cWorld
		// The world is initially empty, containing no rigid bodies.
		// The world has no gravity.
		cWorld();

		~cWorld() {}

		//Gravity get/set
		void GetGravity(glm::vec3& gravityOut);
		void SetGravity(glm::vec3& gravityIn);

		void Update(float dt);

		// helpful comment like what does the return bool mean?
		bool AddRigidBody(cRigidBody* rigidBody);
		bool RemoveRigidBody(cRigidBody* rigidBody);
	};
}