#pragma once
#include <btBulletDynamicsCommon.h>
#include <glm/gtx/matrix_decompose.hpp>

namespace nConvert {
	// glm::vec3 -> btVector3
	inline btVector3 ToBullet(const glm::vec3& vec) {
		return btVector3(vec.x, vec.y, vec.z);
	}
	inline glm::vec3 ToSimple(const btVector3 vec) {
		return glm::vec3(vec.x(), vec.y(), vec.z());
	}

	inline btQuaternion ToBullet(const glm::quat quat) {
		btScalar x = quat.x;
		btScalar y = quat.y;
		btScalar z = quat.z;
		btScalar w = quat.w;
		return btQuaternion(x, y, z, w);
	}

	inline void ToSimple(const btTransform& transformIn, glm::mat4& transformOut) {
		transformIn.getOpenGLMatrix(&transformOut[0][0]);
	}
	inline void ToBullet(const glm::mat4& transformIn, btTransform& transformOut) {
		glm::vec3 scale;
		glm::quat rotation;
		glm::vec3 translation;
		glm::vec3 skew;
		glm::vec4 perspective;
		glm::decompose(transformIn, scale, rotation, translation, skew, perspective);

		transformOut = btTransform(ToBullet(rotation), ToBullet(translation));
	}
}