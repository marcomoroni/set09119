#include "RigidBody.h"



RigidBody::RigidBody()
{
}


RigidBody::~RigidBody()
{
}


void RigidBody::updateInvInertia()
{
	float h = getMesh().getScale()[1][1] * 2.0f;
	float d = getMesh().getScale()[2][2] * 2.0f;
	float w = getMesh().getScale()[0][0] * 2.0f;

	glm::mat3 inertia = glm::mat3(0.0f);
	inertia[0][0] = getMass() / 12.0f * (h * h + d * d);
	inertia[1][1] = getMass() / 12.0f * (w * w + d * d);
	inertia[2][2] = getMass() / 12.0f * (w * w + h * h);

	m_invInertia = inverse(inertia);
}
