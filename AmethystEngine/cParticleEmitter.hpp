#pragma once

#include "GLCommon.hpp"
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/gtx/norm.hpp>
#include <vector>
#include <algorithm>    // for std::copy()

class cParticle
{
public:
	cParticle();
	~cParticle() { }
	glm::vec3 location;
	glm::vec3 velocity;
	float lifeTime;
	// glm::vec orientation
	float scale;


	unsigned char r, g, b, a; // Color
	float cameradistance; // *Squared* distance to the camera. if dead : -1.0f

	bool operator<(const cParticle& that) const {
		// Sort in reverse order : far particles drawn first.
		return this->cameradistance > that.cameradistance;
	}
};

class cParticleEmitter
{
public:
	cParticleEmitter();
	~cParticleEmitter();

	glm::vec3 acceleration;
	glm::vec3 location;
	glm::vec3 m_minVelocity;
	glm::vec3 m_maxVelocity;

	void Initialize(float minSize, float maxSize,
		glm::vec3 minVelocity, glm::vec3 maxVelocity,
		glm::vec3 minDeltaPosition, glm::vec3 maxDeltaPosition,
		float minLifeSeconds, float maxLifeSeconds,
		int minParticlesPerFrame, int maxParticlesPerFrame);

	// Called every frame
	void Step(float deltaTime, glm::vec3 CameraPosition);

	// This will be called when we want to draw them
	// Note: returns nothing and passed by reference ("&")
	void getParticles(std::vector<cParticle*>& vec_pParticles);

	int getMaxParticles() { return DEFAULT_NUM_PARTICLES; }
	int getParticlesCount() { return ParticlesCount; }

	glm::mat4* g_particule_mat_model_data;

private:
	// Static consts can be set here, static can't
	static const int DEFAULT_NUM_PARTICLES = 1000;
	int ParticlesCount;

	std::vector<cParticle*> m_vec_pParticles;

	// Taken from line 229 of the DebugRenderer:
	template <class T>
	T randInRange(T min, T max)
	{
		double value =
			min + static_cast <double> (rand())
			/ (static_cast <double> (RAND_MAX / (static_cast<double>(max - min))));
		return static_cast<T>(value);
	};

	// Creates a single particle (called by Initialize and Step)
	void m_CreateNewParticle(float minSize, float maxSize,
		glm::vec3 minVelocity, glm::vec3 maxVelocity,
		glm::vec3 minDeltaPosition, glm::vec3 maxDeltaPosition,
		float minLifeSeconds, float maxLifeSeconds);

	// Saved for when we call Step()
	// These are set when we call Initialize();
	float m_minSize;
	float m_maxSize;
	glm::vec3 m_minDeltaPosition;
	glm::vec3 m_maxDeltaPosition;
	float m_minLifeSeconds;
	float m_maxLifeSeconds;
	int m_minParticlesPerFrame;
	int m_maxParticlesPerFrame;

	void SortParticles() {
		std::sort(m_vec_pParticles.begin(), m_vec_pParticles.end());
	}
};