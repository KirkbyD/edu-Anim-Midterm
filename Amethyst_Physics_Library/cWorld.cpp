#include "cWorld.hpp"
#include "nCollide.hpp"
#include <glm\gtx\projection.hpp>

phys::cWorld::cWorld() 
	: mDt(0.f)
	, mGravity(glm::vec3(0.f, 0.f, 0.f)) {

}

void phys::cWorld::GetGravity(glm::vec3& gravityOut) {
	gravityOut = mGravity;
}

void phys::cWorld::SetGravity(glm::vec3& gravityIn) {
	mGravity = gravityIn;
}

void phys::cWorld::Update(float dt) {
	size_t numBodies = mBodies.size();
	if (numBodies == 0) return;
	
	// Step 1: Integrate
	mDt = dt;
	for (size_t i = 0; i < numBodies; i++) {
		IntegrateRigidBody(mBodies[i], mDt);
	}
	// Step 2: Handle Collisions
	//SUPER SIMPLE, NOT GREAT
	for (size_t idxA = 0; idxA < numBodies - 1; idxA++)	{
		for (size_t idxB = idxA+1; idxB < numBodies; idxB++) {
			//this structure hits all unique pairs once!
			Collide(mBodies[idxA], mBodies[idxB]);
		}
	}

	// Step 3: Clear the accelerations for all rigid bodies.
	for (size_t i = 0; i < numBodies; i++) {
		mBodies[i]->mAcceleration = glm::vec3(0.f, 0.f, 0.f);
		
		//Check Terminal Velocity
		//if (mBodies[i]->mMaxVelocity.y != 0.f 
		//	&& mBodies[i]->mMaxVelocity.y < fabs(mBodies[i]->mVelocity.y)) {
		//	//exceeding terminal velocity
		//	mBodies[i]->mVelocity.y = mBodies[i]->mMaxVelocity.y * glm::sign(mBodies[i]->mVelocity.y);
		//}

		//XZ velocity max
		/*if (mBodies[i]->mVelocity.x != 0 && mBodies[i]->mVelocity.z != 0) {
			float xzVel = glm::length(glm::vec2(mBodies[i]->mVelocity.x, mBodies[i]->mVelocity.z));
			float xzMax = glm::length(glm::vec2(mBodies[i]->mMaxVelocity.x, mBodies[i]->mMaxVelocity.z));
			if (xzVel > xzMax) {
				float multiplier = xzMax / xzVel;
				mBodies[i]->mVelocity.x *= multiplier;
				mBodies[i]->mVelocity.z *= multiplier;
			}
		}*/
	}

	// Step 4: Tell everyone about the collisions
}

bool phys::cWorld::AddRigidBody(cRigidBody* rigidBody) {
	if (!rigidBody) return false; //can't add nothing
	std::vector<cRigidBody*>::iterator itBody = std::find(mBodies.begin(), mBodies.end(), rigidBody);
	if (itBody == mBodies.end()) {
		//not already there, so add it
		mBodies.push_back(rigidBody);
		return true;
	}
	return false;
}

bool phys::cWorld::RemoveRigidBody(cRigidBody* rigidBody) {
	if (!rigidBody) return false; //can't remove nothing
	std::vector<cRigidBody*>::iterator itBody = std::find(mBodies.begin(), mBodies.end(), rigidBody);
	if (itBody == mBodies.end()) return false; //wasn't here, wasn't removed
	mBodies.erase(itBody);
	return true;
}

void phys::cWorld::IntegrateRigidBody(cRigidBody* body, float dt) {
	if (body->IsStatic()) return;
	
	body->mPreviousPosition = body->mPosition;
	body->mAcceleration *= body->mInvMass;
	mIntegrator.RK4(body->mPosition, body->mVelocity, body->mAcceleration, mGravity, dt);
	body->mVelocity *= glm::pow(0.95, dt);
}

bool phys::cWorld::Collide(cRigidBody* bodyA, cRigidBody* bodyB) {
	eShapeType shapeTypeA = bodyA->GetShapeType();
	eShapeType shapeTypeB = bodyB->GetShapeType();

	if (shapeTypeA == eShapeType::plane) {
		if (shapeTypeB == eShapeType::plane) {
			return false;
		}
		if (shapeTypeB == eShapeType::sphere) {
			return CollideSpherePlane(bodyB, (cSphere*)(bodyB->GetShape()),
									  bodyA, (cPlane*)(bodyA->GetShape()));
		}
	}
	if (shapeTypeA == eShapeType::sphere) {
		if (shapeTypeB == eShapeType::plane) {
			return CollideSpherePlane(bodyA, (cSphere*)(bodyA->GetShape()),
									  bodyB, (cPlane*)(bodyB->GetShape()));
		}
		if (shapeTypeB == eShapeType::sphere) {
			return CollideSphereSphere(bodyA, (cSphere*)(bodyA->GetShape()),
									   bodyB, (cSphere*)(bodyB->GetShape()));
		}
	}
	return false;
}

bool phys::cWorld::CollideSpherePlane(cRigidBody* sphereBody, cSphere* sphereShape, cRigidBody* planeBody, cPlane* planeShape) {
	glm::vec3 c = sphereBody->mPreviousPosition;
	float r = sphereShape->GetRadius();
	glm::vec3 v = sphereBody->mPosition - sphereBody->mPreviousPosition;
	glm::vec3 n = planeShape->GetNormal();
	float d = planeShape->GetConstant();
	float t(1.f);
	glm::vec3 q;	
	int result = nCollide::intersect_moving_sphere_plane(c, r, v, n, d, t, q);

	if (result == 0) { //No collision
		return false;
	}
	if (result == -1) { //Already colliding at start of time step
		glm::vec3 pointOnPlane = nCollide::closest_point_on_plane(sphereBody->mPreviousPosition,
			planeShape->GetNormal(), planeShape->GetConstant());
		//Find Corrective Impulse to escape plane
		float distance = glm::distance(sphereBody->mPreviousPosition, pointOnPlane);
		float targetDistance = r;
		glm::vec3 impulse = n * (targetDistance - distance) / mDt;
		sphereBody->mVelocity += impulse;
		//Move Back
		sphereBody->mPosition = sphereBody->mPreviousPosition;
		//Integrate
		IntegrateRigidBody(sphereBody, mDt);
		return true;
	}
	// Collision
	// Reflect via plane normal
	sphereBody->mVelocity = glm::reflect(sphereBody->mVelocity, planeShape->GetNormal());
	// Energy Loss
	glm::vec3 nComponent = glm::proj(sphereBody->mVelocity, planeShape->GetNormal());
	sphereBody->mVelocity -= nComponent * 0.2f;
	// Rewind
	sphereBody->mPosition = (c + v * t);
	// Integrate
	IntegrateRigidBody(sphereBody, mDt * (1.f - t));
	return true;
}

bool phys::cWorld::CollideSphereSphere(cRigidBody* bodyA, cSphere* shapeA, cRigidBody* bodyB, cSphere* shapeB) {
	glm::vec3 cA = bodyA->mPreviousPosition;
	glm::vec3 cB = bodyB->mPreviousPosition;
	glm::vec3 vA = bodyA->mPosition - bodyA->mPreviousPosition;
	glm::vec3 vB = bodyB->mPosition - bodyB->mPreviousPosition;
	float rA = shapeA->GetRadius();
	float rB = shapeB->GetRadius();
	float t(1.f);
	int result = nCollide::intersect_moving_sphere_sphere(cA, rA, vA, cB, rB, vB, t);

	if (result == 0) { // No collision
		return false;
	}
	//Get masses to make collisions look good
	float ma = bodyA->mMass;
	float mb = bodyB->mMass;
	float mt = ma + mb;
	if (result == -1) { //Already colliding at start of time step
		//Find Corrective Impulse for both spheres
		float initialDistance = glm::distance(bodyA->mPreviousPosition, bodyB->mPreviousPosition);
		float targetDistance = rA + rB;
		glm::vec3 impulseToA = glm::normalize(bodyA->mPreviousPosition - bodyB->mPreviousPosition);
		impulseToA *= (targetDistance - initialDistance);
		bodyA->mPosition = bodyA->mPreviousPosition;
		bodyB->mPosition = bodyB->mPreviousPosition;
		// Impulse scaled by relative mass
		bodyA->mVelocity += impulseToA * (mb/mt);
		bodyB->mVelocity -= impulseToA * (ma/mt);
		//Integrate
		IntegrateRigidBody(bodyA, mDt);
		IntegrateRigidBody(bodyB, mDt);
		return true;
	}
	// Collision
	// Move to prev pos
	bodyA->mPosition = bodyA->mPreviousPosition + vA * t;
	bodyB->mPosition = bodyB->mPreviousPosition + vB * t;
	// Change velocities to actual instead of or step motion.
	vA = bodyA->mVelocity;
	vB = bodyB->mVelocity;
	// Find post collision velocity
	// Formula at https://en.wikipedia.org/wiki/Inelastic_collision
	// CoefficientOfRestitution - lower is less elastic, higher is more.
	float c = 0.2f; //	todo, bind to rigid body. Multiply each bodies together || take average for this calc. Its a material based value.
	bodyA->mVelocity = (c * mb * (vB - vA) + ma * vA + mb * vB) / mt;
	bodyA->mVelocity = (c * ma * (vA - vB) + ma * vA + mb * vB) / mt;
	// Integrate
	IntegrateRigidBody(bodyA, mDt * (1.f - t));
	IntegrateRigidBody(bodyB, mDt * (1.f - t));
	return true;
}