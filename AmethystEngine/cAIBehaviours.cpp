#pragma region MEM LEAK CHECK
#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>
#include <memory>

#ifdef _DEBUG
#define DEBUG_NEW new (_NORMAL_BLOCK , __FILE__ , __LINE__)
#else
#define DBG_NEW
#endif
#pragma endregion

#include "cAIBehaviours.hpp"
#include <glm/gtc/constants.hpp>
#include <random>

void cAIBehaviours::seek(sData& data) {
	// calculates the desired velocity
	glm::vec3 desiredVelocity = data.getTargetGameObj()->getPosition() - data.getSourceGameObj()->getPosition();

	// get the distance from target
	float distance = glm::length(desiredVelocity);

	desiredVelocity = glm::normalize(desiredVelocity);
	desiredVelocity *= data.GetMaxVelocity();

	// calculate the steering force
	glm::vec3 steer = desiredVelocity - data.getSourceGameObj()->getVelocity();

	// add steering force to current velocity
	data.getSourceGameObj()->ApplyImpulse(steer * data.GetDeltaTime());
}

void cAIBehaviours::pursure(sData& data) {
	// calculate the number of frames we are looking ahead
	glm::vec3 distance = data.getTargetGameObj()->getPosition() - data.getSourceGameObj()->getPosition();
	int T = glm::length(distance) / glm::length(data.GetMaxVelocity());

	//the future target point the vehicle will pursue towards
	glm::vec3 futurePosition = data.getTargetGameObj()->getPosition() + data.getTargetGameObj()->getVelocity() * (float)T;

	// calculates the desired velocity
	glm::vec3 desiredVelocity = futurePosition - data.getSourceGameObj()->getPosition();

	desiredVelocity = glm::normalize(desiredVelocity);
	desiredVelocity *= data.GetMaxVelocity();

	// calculate the steering force
	glm::vec3 steer = desiredVelocity - data.getSourceGameObj()->getVelocity();

	// add steering force to current velocity
	data.getSourceGameObj()->ApplyImpulse(data.getSourceGameObj()->getVelocity() + (steer * data.GetDeltaTime()));
}

void cAIBehaviours::flee(sData& data) {
	// calculates the desired velocity
	glm::vec3 desiredVelocity = data.getSourceGameObj()->getPosition() - data.getTargetGameObj()->getPosition();

	// get the distance from target
	float distance = glm::length(desiredVelocity);

	desiredVelocity = glm::normalize(desiredVelocity);
	desiredVelocity *= data.GetMaxVelocity();

	// calculate the steering force
	glm::vec3 steer = desiredVelocity - data.getSourceGameObj()->getVelocity();

	// add steering force to current velocity
	data.getSourceGameObj()->ApplyImpulse(data.getSourceGameObj()->getVelocity() + (steer * data.GetDeltaTime()));
}

void cAIBehaviours::approach(sData& data) {
	// calculates the desired velocity
	glm::vec3 desiredVelocity = data.getTargetGameObj()->getPosition() - data.getSourceGameObj()->getPosition();

	// get the distance from target
	float distance = glm::length(desiredVelocity);
	desiredVelocity = glm::normalize(desiredVelocity);

	// is the game object within the radius around the target
	if (distance < data.GetSlowingRadius())
		// game object is approaching the target and slows down
		desiredVelocity = desiredVelocity * data.GetMaxVelocity() * (distance / data.GetSlowingRadius());
	else
		// target is far away from game object
		desiredVelocity *= data.GetMaxVelocity();

	// calculate the steering force
	glm::vec3 steer = desiredVelocity - data.getSourceGameObj()->getVelocity();

	// add steering force to current velocity
	data.getSourceGameObj()->ApplyImpulse(data.getSourceGameObj()->getVelocity() + (steer * data.GetDeltaTime()));
}

void cAIBehaviours::evade(sData& data) {
	// calculate the number of frames we are looking ahead
	glm::vec3 distance = data.getTargetPos() - data.getSourceGameObj()->getPosition();
	int T = glm::length(distance) / glm::length(data.GetMaxVelocity());

	//the future target point the vehicle will pursue towards
	glm::vec3 futurePosition = data.getTargetPos() + data.getTargetVel() * (float)T;

	// calculates the desired velocity
	glm::vec3 desiredVelocity = data.getSourceGameObj()->getPosition() - futurePosition;

	desiredVelocity = glm::normalize(desiredVelocity);

	desiredVelocity *= data.GetMaxVelocity();

	// calculate the steering force
	glm::vec3 steer = desiredVelocity - data.getSourceGameObj()->getVelocity();

	// add steering force to current velocity
	data.getSourceGameObj()->ApplyImpulse(data.getSourceGameObj()->getVelocity() + (steer * data.GetDeltaTime()));
}

void cAIBehaviours::wander(sData& data) {
	float vehicleOrientation = glm::radians(data.getSourceGameObj()->getEulerAngle().y);

	//calculate the circle's center point
	glm::vec3 circlePoint = data.getSourceGameObj()->getPosition() + glm::vec3(glm::cos(-vehicleOrientation), 0, glm::sin(-vehicleOrientation)) * data.GetDistanceToCircle();

	//calculate a random spot on the circle's circumference
	std::random_device rd;
	std::uniform_real_distribution<float> dist(0.0f, glm::pi<float>() * 2);
	float angle = dist(rd);
	
	float x = glm::sin(angle) * data.GetCircleRadius();
	float z = glm::cos(angle) * data.GetCircleRadius();

	//the target point the wandering vehicle will seek towards
	glm::vec3 targetPosition = glm::vec3(circlePoint.x + x, 0.5f, circlePoint.z + z);

	/*calculates the desired velocity */
	glm::vec3 desiredVelocity = targetPosition - data.getSourceGameObj()->getPosition();

	desiredVelocity = glm::normalize(desiredVelocity);

	desiredVelocity *= data.GetMaxVelocity();

	/*calculate the steering force */
	glm::vec3 steer = desiredVelocity - data.getSourceGameObj()->getVelocity();

	/* add steering force to current velocity*/
	data.getSourceGameObj()->ApplyImpulse(data.getSourceGameObj()->getVelocity() + (steer * data.GetDeltaTime()));
}

void cAIBehaviours::idle(sData& data) {
	glm::vec3 invVel(0.f);
	invVel = data.getSourceGameObj()->getVelocity();
	invVel *= -1.f;
	data.getSourceGameObj()->ApplyImpulse(invVel);
}

void cAIBehaviours::followPath() {
	// TODO: IMPLEMENT FOLLOW PATH BEHAVIOUR
}

void cAIBehaviours::formCircle(sData& data) {

}

void cAIBehaviours::formBox(sData& data) {

}

void cAIBehaviours::formLine(sData& data) {

}

void cAIBehaviours::formWedge(sData& data) {

}

void cAIBehaviours::formColumn(sData& data) {

}
