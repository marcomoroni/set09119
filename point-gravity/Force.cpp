#include <iostream>
#include <cmath>
#include "Force.h"
#include "Body.h"
#include "glm/ext.hpp"

Force::Force()
{
}


Force::~Force()
{
}

glm::vec3 Force::apply(float mass, const glm::vec3 &pos, const glm::vec3 &vel) {
	return glm::vec3(0.0f);
}

/*
** GRAVITY
*/
glm::vec3 Gravity::apply(float mass, const glm::vec3 &pos, const glm::vec3 &vel)
{
	return mass * m_gravity;
}

/*
** POINT GRAVITY
*/
glm::vec3 PointGravity::apply(float mass, const glm::vec3 &pos, const glm::vec3 &vel)
{
	float distance = glm::distance(m_origin, m_body->getPos());

	// if the body is not in the gravitational field do not calculate the gratity
	if (distance > m_radius) return glm::vec3(0.0f);

	// get gravity "power"
	float multiplier = 1 - (distance / m_radius);
	float gravityPower = m_gravity * multiplier;
	// get gravity direction
	glm::vec3 gravityDirection = glm::normalize(m_origin - m_body->getPos());

	// DEBUG
	//if (multiplier > 0.5) m_body->getMesh().setShader(Shader("resources/shaders/core.vert", "resources/shaders/core_red.frag"));
	//else m_body->getMesh().setShader(Shader("resources/shaders/core.vert", "resources/shaders/core_blue.frag"));

	return gravityDirection * gravityPower;
}

/*
** DRAG
*/
glm::vec3 Drag::apply(float mass, const glm::vec3 &pos, const glm::vec3 &vel)
{
	float pho = 1.225f; // density of medium
	float cd = 0.01f; // coefficient of drag
	float a = 0.001f; // cross sectional area of object
	glm::vec3 e = -vel / glm::length(vel);

	glm::vec3 drag = 0.5f * pho * glm::length(vel * vel) * cd * a * e;

	return drag;
}

/*
** HOOKE
*/
glm::vec3 Hooke::apply(float mass, const glm::vec3 &pos, const glm::vec3 &vel)
{

	float particleDistance = glm::distance(m_b1->getPos(), m_b2->getPos());

	float fs = -m_ks * (m_rest - particleDistance);
	glm::vec3 e = (m_b2->getPos() - m_b1->getPos()) / particleDistance;
	float fd = -m_kd * (glm::dot(m_b1->getVel(), e) - glm::dot(m_b2->getVel(), e));

	float fsd = fs + fd;

	return fsd * e;
}