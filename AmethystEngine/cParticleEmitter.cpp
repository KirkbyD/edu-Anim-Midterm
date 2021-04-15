#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>
#include <memory>

#ifdef _DEBUG
#define DEBUG_NEW new (_NORMAL_BLOCK , __FILE__ , __LINE__)
#else
#define DBG_NEW
#endif

#include "cParticleEmitter.hpp"

cParticle::cParticle()
{
	// Note: we aren't really using these default values...
	this->location = glm::vec3(0.0f, 0.0f, 0.0f);
	this->velocity = glm::vec3(0.0f, 0.0f, 0.0f);
	this->lifeTime = 0.0f;

	this->scale = 1.0f;

	this->r = 0.0f;
	this->g = 0.0f;
	this->b = 0.0f;
	this->a = 1.0f;

	this->cameradistance = -1.0;

	return;
}

cParticleEmitter::cParticleEmitter()
{
	// NOTE: I'm NOT setting up the particles here.
	// That will happen in initialize()
	this->location = glm::vec3(0.0f, 0.0f, 0.0f);
	this->acceleration = glm::vec3(0.0f, 0.0f, 0.0f);
	return;
}

cParticleEmitter::~cParticleEmitter() {
	for (auto a : m_vec_pParticles) {
		a->~cParticle();
		delete a;
	}
	delete g_particule_mat_model_data;
}

void cParticleEmitter::Step(float deltaTime, glm::vec3 CameraPosition)
{
	// Loop through the particles:
	// - Decrease the life by "deltaTime"
	// - If "still alive" (lifeTime > 0.0):
	//    - Move the particle

	ParticlesCount = 0;

	for (std::vector<cParticle*>::iterator itPart = this->m_vec_pParticles.begin();
		itPart != this->m_vec_pParticles.end();  itPart++)
	{
		cParticle* pCurParticle = *itPart;	// Dereference iterator, giving us the original type

		pCurParticle->lifeTime -= deltaTime;

		// Is it dead, Jim? (Star Trek reference)
		if (pCurParticle->lifeTime > 0.0f)
		{
			// Update velocity from acceler
			pCurParticle->velocity += (this->acceleration * deltaTime);

			// Update position from velocity
			pCurParticle->location += (pCurParticle->velocity * deltaTime);


			pCurParticle->cameradistance = glm::length2(pCurParticle->location - CameraPosition);



			glm::mat4 matWorld = glm::mat4(1.0f);
			// 1. translation: displace along circle with 'radius' in range [-offset, offset]
			matWorld = glm::translate(matWorld, glm::vec3(pCurParticle->location.x, pCurParticle->location.y, pCurParticle->location.z));

			// 2. scale: Scale between 0.05 and 0.25f
			matWorld = glm::scale(matWorld, glm::vec3(pCurParticle->scale));

			// 3. rotation: add random rotation around a (semi)randomly picked rotation axis vector
			float rotAngle = (rand() % 360);
			matWorld = glm::rotate(matWorld, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

			this->g_particule_mat_model_data[ParticlesCount] = matWorld;
		}
		else {
			// Particles that just died will be put at the end of the buffer in SortParticles();
			pCurParticle->cameradistance = -1.0f;
		}
		ParticlesCount++;
	}

	SortParticles();

	// How many new particles should we create this frame? 
	int numParticleToCreate = randInRange<int>(this->m_minParticlesPerFrame, this->m_maxParticlesPerFrame);

	for (int count = 0; count != numParticleToCreate; count++)
	{
		this->m_CreateNewParticle(this->m_minSize, this->m_maxSize, this->m_minVelocity, this->m_maxVelocity,
			this->m_minDeltaPosition, this->m_maxDeltaPosition,
			this->m_minLifeSeconds, this->m_maxLifeSeconds);
	}//for (int count = 0;

	return;
}

void cParticleEmitter::Initialize(float minSize, float maxSize,
	glm::vec3 minVelocity, glm::vec3 maxVelocity,
	glm::vec3 minDeltaPosition, glm::vec3 maxDeltaPosition,
	float minLifeSeconds, float maxLifeSeconds,
	int minParticlesPerFrame, int maxParticlesPerFrame)
{
	// In case we are calling Initialize() again... 
	// Clear all the existing particles


	// TODO: 
	// - If the new size is LARGER than the current vector, 
	//   allocate the extra space and call new
	// - If the new size is SMALLER than the current vector, 
	//   go though and call delete on all the "extra" particles. 


	//	this->m_vec_pParticles.clear();

	// Load the vector with the maximum number of particles
	// Reserve doesn't create the objects, it just sets asside space in 1 step
	this->m_vec_pParticles.reserve(cParticleEmitter::DEFAULT_NUM_PARTICLES);

	for (int count = 0; count != cParticleEmitter::DEFAULT_NUM_PARTICLES; count++)
	{
		// TODO: If we are calling Initialize() AGAIN, 
		//       then we DON'T want to call new
		cParticle* pNewParticle = new cParticle();

		// Set the lifetime to 0.0 for all of them
		pNewParticle->lifeTime = 0.0f;

		// Set the cameradistance to -1.0 for all of them
		pNewParticle->cameradistance = -1.0f;

		this->m_vec_pParticles.push_back(pNewParticle);

	}

	//// Or you could do it a silly way... 
	//for ( std::vector<cParticle*>::iterator itParticle = this->m_vec_pParticles.begin();
	//	  itParticle != this->m_vec_pParticles.end(); itParticle++ )
	//{
	//	(*itParticle)->lifeTime = 0.0f;
	//}

	this->m_minVelocity = minVelocity;
	this->m_maxVelocity = maxVelocity;
	this->m_minDeltaPosition = minDeltaPosition;
	this->m_maxDeltaPosition = maxDeltaPosition;
	this->m_minLifeSeconds = minLifeSeconds;
	this->m_maxLifeSeconds = maxLifeSeconds;
	this->m_minParticlesPerFrame = minParticlesPerFrame;
	this->m_maxParticlesPerFrame = maxParticlesPerFrame;
	this->m_minSize = minSize;
	this->m_maxSize = maxSize;

	this->g_particule_mat_model_data = new glm::mat4[DEFAULT_NUM_PARTICLES];

	return;
}



void cParticleEmitter::m_CreateNewParticle(
	float minSize, float maxSize,
	glm::vec3 minVelocity, glm::vec3 maxVelocity,
	glm::vec3 minDeltaPosition, glm::vec3 maxDeltaPosition,
	float minLifeSeconds, float maxLifeSeconds)
{
	// Find the 1st particle that has life <= 0.0f
	for (std::vector<cParticle*>::iterator itPart = this->m_vec_pParticles.begin();
		itPart != this->m_vec_pParticles.end();  itPart++)
	{
		// Is this particle "dead"? 
		cParticle* pCurParticle = *itPart;

		if (pCurParticle->lifeTime <= 0.0f)
		{
			// Make it "alive" and set to some values
			pCurParticle->lifeTime = randInRange<float>(minLifeSeconds, maxLifeSeconds);

			// Set the initial velocity
			pCurParticle->velocity.x = randInRange<float>(minVelocity.x, maxVelocity.x);
			pCurParticle->velocity.y = randInRange<float>(minVelocity.y, maxVelocity.y);
			pCurParticle->velocity.z = randInRange<float>(minVelocity.z, maxVelocity.z);
			//pCurParticle->velocity = randInRange<glm::vec3>(minVelocity, maxVelocity);

			/*************************/
			// Very bad way to generate a random color
			pCurParticle->r = rand() % 256;
			pCurParticle->g = rand() % 256;
			pCurParticle->b = rand() % 256;
			pCurParticle->a = (rand() % 256) / 3;

			pCurParticle->scale = randInRange<float>(minSize, maxSize);
			/*************************/

			// TODO: Adjust position based on Max and Min delta postiion
			// (For now, just make the particle the same as the emitter)
			pCurParticle->location = this->location;
		}
	}
	return;
}


void cParticleEmitter::getParticles(std::vector<cParticle*>& vec_pParticles)
{
	// Copy the internal vector of particles to the one that is passed.

	vec_pParticles.clear();
	vec_pParticles.reserve(this->m_vec_pParticles.size());
	//	// Sets the pointer values to zero (NULL)
	//	vec_pParticles.resize( this->m_vec_pParticles.size(), 0 );

		// NOTE: There has to be space there, 
		// AND the objects have to be initialized already
	//	std::copy( this->m_vec_pParticles.begin(), 
	//			   this->m_vec_pParticles.end(), 
	//			   vec_pParticles.begin() );

	for (std::vector<cParticle*>::iterator itPart = this->m_vec_pParticles.begin();
		itPart != this->m_vec_pParticles.end(); itPart++)
	{
		// If alive, copy over
		cParticle* pCurPart = *itPart;

		if (pCurPart->lifeTime > 0.0f)
		{
			vec_pParticles.push_back(pCurPart);
		}
	}

	return;
}