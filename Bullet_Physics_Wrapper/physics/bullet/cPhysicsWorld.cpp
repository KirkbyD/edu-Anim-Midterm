#include "cPhysicsWorld.hpp"
#include <physics/interfaces/eComponentType.h>
#include "cBallComponent.hpp"
#include "cPlaneComponent.hpp"
#include "nConvert.hpp"

nPhysics::cPhysicsWorld::cPhysicsWorld() {
	///collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
	mCollisionConfiguration = new btDefaultCollisionConfiguration();

	///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
	mDispatcher = new btCollisionDispatcher(mCollisionConfiguration);

	///btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
	mOverlappingPairCache = new btDbvtBroadphase();

	///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
	mSolver = new btSequentialImpulseConstraintSolver;

	mDynamicsWorld = new btDiscreteDynamicsWorld(mDispatcher, mOverlappingPairCache, mSolver, mCollisionConfiguration);
	
	//Initialise gravity to 0
	mDynamicsWorld->setGravity(btVector3(0, 0, 0));
}

nPhysics::cPhysicsWorld::~cPhysicsWorld() {
	//Remove the rigidbodies from the dynamics world and delete them
	for (int i = mDynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)	{
		btCollisionObject* obj = mDynamicsWorld->getCollisionObjectArray()[i];
		btRigidBody* body = btRigidBody::upcast(obj);
		if (body && body->getMotionState()) {
			delete body->getMotionState();
		}
		mDynamicsWorld->removeCollisionObject(obj);
		delete obj;
	}

	//Delete order very specific.
	//taken from the Bullet's hello world example
	delete mDynamicsWorld;
	delete mSolver;
	delete mOverlappingPairCache;
	delete mDispatcher;
	delete mCollisionConfiguration;
}

void nPhysics::cPhysicsWorld::Update(float dt) {
	mDynamicsWorld->stepSimulation(dt, 10);
	//TODO - collision listening stuff
		//
}

//re SetCollisionListener
//if you have one, delete it
//

bool nPhysics::cPhysicsWorld::AddComponent(iPhysicsComponent* component) {
	if (!component) {
		return false;
	}

	switch (component->GetComponentType()) {
	case eComponentType::ball:
		mDynamicsWorld->addRigidBody(((cBallComponent*)component)->mBody);
		mShapeVector.push_back(((cBallComponent*)component)->mBody->getCollisionShape());
		return true;
	case eComponentType::plane:
		mDynamicsWorld->addRigidBody(((cPlaneComponent*)component)->mBody);
		mShapeVector.push_back(((cPlaneComponent*)component)->mBody->getCollisionShape());
		return true;
	default:
		return false;
	}
}

bool nPhysics::cPhysicsWorld::RemoveComponent(iPhysicsComponent* component) {
	switch (component->GetComponentType()) {
	case eComponentType::ball:
		mDynamicsWorld->removeRigidBody(((cBallComponent*)component)->mBody);
		return true;
	case eComponentType::plane:
		mDynamicsWorld->removeRigidBody(((cPlaneComponent*)component)->mBody);
		return true;
	default:
		return false;
	}
}

void nPhysics::cPhysicsWorld::SetGravity(glm::vec3 gravity) {
	mDynamicsWorld->setGravity(nConvert::ToBullet(gravity));
}
