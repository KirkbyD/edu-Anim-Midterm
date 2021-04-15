#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>
#include <memory>

#ifdef _DEBUG
#define DEBUG_NEW new (_NORMAL_BLOCK , __FILE__ , __LINE__)
#else
#define DBG_NEW
#endif

#include "cCamera.hpp"

cCamera::cCamera(unsigned id)
	: friendlyID(id)
	, type(CameraType::freeCam)
	, yaw(0.f), pitch(0.f)
	, position(glm::vec3(0.f)), previousPosition(position)
	, front(glm::vec3(0.f, 0.f, 1.f)), previousFront(front)
	, upVector(glm::vec3(0.f,1.f,0.f))
	, viewMatrix(glm::mat4(1.0f))
	, lookAtObject(nullptr)
	//Object Camera
	, followDistance(1.f), followOffset(glm::vec3(0.f))
	, followObject(nullptr)
	//Cinema Camera
	, innerSpeedRadius(1.f), outerSpeedRadius(5.f)
	, maxSpeed(5.f), targetPosition(glm::vec3(0.f))
	//FMOD
	, fmod_Pos(FMOD_VECTOR{ 0.0f, 0.0f, 0.0f }), fmod_For(FMOD_VECTOR{ 0.0f, 0.0f, 0.0f })
	, fmod_Up(FMOD_VECTOR{ 0.0f, 0.0f, 0.0f }), fmod_Vel(FMOD_VECTOR{ 0.0f, 0.0f, 0.0f })
{
	//More Complicated Sets
	this->right = glm::normalize(glm::cross(this->upVector, this->front));
	this->up = glm::cross(this->front, this->right);
}

cCamera::cCamera(unsigned id, sCameraDef def)
	: friendlyID(id)
	, type(def.type)
	, yaw(def.yaw), pitch(def.pitch)
	, position(def.position), previousPosition(position)
	, front(def.front), previousFront(front)
	, upVector(def.upVector)
	, viewMatrix(def.viewMatrix)
	, lookAtObject(def.lookAtObject)
	//Object Camera
	, followDistance(def.followDistance), followOffset(def.followOffset)
	, followObject(def.followObject)
	//Cinema Camera
	, innerSpeedRadius(def.innerSpeedRadius), outerSpeedRadius(def.outerSpeedRadius)
	, maxSpeed(def.maxSpeed), targetPosition(def.targetPosition)
	//FMOD
	, fmod_Pos(FMOD_VECTOR{ 0.0f, 0.0f, 0.0f }), fmod_For(FMOD_VECTOR{ 0.0f, 0.0f, 0.0f })
	, fmod_Up(FMOD_VECTOR{ 0.0f, 0.0f, 0.0f }), fmod_Vel(FMOD_VECTOR{ 0.0f, 0.0f, 0.0f })
{
	//More Complicated Sets
	this->right = glm::normalize(glm::cross(this->upVector, this->front));
	this->up = glm::cross(this->front, this->right);
}

glm::mat4 cCamera::GetViewMatrix() {
	this->viewMatrix = glm::lookAt(this->position,
		this->position + this->front,
		this->upVector);

	return this->viewMatrix;
}

void cCamera::MutateFollowDistance(float offset) {
	this->followDistance += 0.1 * offset;
	if (this->followDistance < 0.1f)
		this->followDistance = 0.1f;
	else if (this->followDistance > 5.f)
		this->followDistance = 5.f;
}

	void cCamera::SetFrontVector(glm::vec3 FrontIn) {
		glm::vec3 direction = glm::normalize(FrontIn - position);
		pitch = asin(direction.y);
		yaw = atan2(direction.x, direction.z);
	}

void cCamera::Move() {
	if (this->followObject == nullptr)
		return;

	this->previousPosition = this->position;
	this->targetPosition = this->followObject->getPosition();
	this->targetPosition += (followDistance * followOffset) * -this->front;

	//find distance between position and target
	glm::vec3 direction = this->targetPosition - this->position;
	float distance = glm::length(direction);

	//move position based on distance
	float move = smootherstep(this->innerSpeedRadius, this->outerSpeedRadius, distance);
	this->position += glm::normalize(direction) * move * this->maxSpeed;
}

void cCamera::Move(glm::vec3 moveVec) {
	this->position += moveVec.x * this->right;
	this->position += moveVec.y * this->up;
	this->position += moveVec.z * this->front;
}

//WIKIPEDIA - https://en.wikipedia.org/wiki/Smoothstep
float cCamera::smootherstep(float edge0, float edge1, float x) {
	// Scale, and clamp x to 0..1 range
	x = clamp((x - edge0) / (edge1 - edge0), 0.0, 1.0);
	// Evaluate polynomial
	return x * x * x * (x * (x * 6 - 15) + 10);
}

float cCamera::clamp(float x, float lowerlimit, float upperlimit) {
	if (x < lowerlimit)
		x = lowerlimit;
	if (x > upperlimit)
		x = upperlimit;
	return x;
}

void cCamera::MoveFront() {
	// Always look at 0 for Physics project
	/*glm::vec3 tmp = this->lookAtObject->getPosition();
	tmp.y = 0.f;
	this->front.y = glm::normalize(tmp - this->position).y;*/

	this->previousFront = this->front;

	if (this->lookAtObject != NULL) {
		this->front = glm::normalize(this->lookAtObject->getPosition() - this->position);
	}
	else {
		this->front.x = cos(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
		this->front.y = sin(glm::radians(this->pitch));
		this->front.z = sin(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
	}
	this->front = glm::normalize(this->front);
	this->right = glm::normalize(glm::cross(this->upVector, this->front));
	this->up = glm::cross(this->front, this->right);

	if (this->followObject == nullptr)
		RotatePosition();
	return;
}

// Used to rotate the camera around view object without invoking smootherstep.
void cCamera::RotatePosition() {

	//TODO

	/*	if (this->followObject == nullptr)
		return;

	this->previousPosition = this->position;
	glm::vec3 newPos = this->position - this->followObject->getPosition();
	this->position = newPos * -this->front;*/
}

void cCamera::OffsetYaw(float offset) {
	this->yaw = glm::mod(this->yaw + offset, 360.0f);
}

void cCamera::OffsetPitch(float offset) {
	this->pitch += offset;
	if (this->pitch > 89.0f)
		this->pitch = 89.0f;
	if (this->pitch < -89.0f)
		this->pitch = -89.0f;
}

#pragma region FMOD
FMOD_VECTOR cCamera::AudLisGetPosition() {
	glm::vec3 targetPos;
	if (this->followObject == nullptr) 
		targetPos = this->position;
	else 
		targetPos = this->followObject->getPosition();
	this->fmod_Pos.x = targetPos.x;
	this->fmod_Pos.y = targetPos.y;
	this->fmod_Pos.z = targetPos.z;
	return this->fmod_Pos;
}

FMOD_VECTOR cCamera::AudLisGetVelocity() {
	glm::vec3 targetVel = this->position - this->previousPosition;
	this->fmod_Vel.x = targetVel.x;
	this->fmod_Vel.y = targetVel.y;
	this->fmod_Vel.z = targetVel.z;
	return this->fmod_Vel;
}

FMOD_VECTOR cCamera::AudLisGetForward() {
	this->fmod_For.x = -this->front.x;
	this->fmod_For.y = -this->front.y;
	this->fmod_For.z = -this->front.z;
	return this->fmod_For;
}

FMOD_VECTOR cCamera::AudLisGetUp() {
	this->fmod_Up.x = this->up.x;
	this->fmod_Up.y = this->up.y;
	this->fmod_Up.z = this->up.z;
	return this->fmod_Up;
}
#pragma endregion