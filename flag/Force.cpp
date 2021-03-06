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
** DRAG
*/
glm::vec3 Drag::apply(float mass, const glm::vec3 &pos, const glm::vec3 &vel)						// NOT WORKING
{
	float pho = 1.225f; // density of medium
	float cd = 0.05f; // coefficient of drag
	float a = 1.0f; // cross sectional area of object
	glm::vec3 e = -vel / glm::length(vel);

	glm::vec3 drag = 0.5f * pho * glm::length(vel * vel) * cd * a * e;

	return drag + glm::vec3(0.0f, 0.0f, 0.0f);
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

/*
** SURFACE DRAG
*/
glm::vec3 SurfaceDrag::apply(float mass, const glm::vec3 &pos, const glm::vec3 &vel)
{
	//std::cout << "Surface drag called" << std::endl;

	float pho = 1.225f; // density of medium (air)
	float cd = 0.05f; // coefficient of drag

	// velocity
	glm::vec3 v = (m_b1->getVel() + m_b2->getVel() + m_b3->getVel()) / 3.0f;
	v -= m_wind;
	// normal
	glm::vec3 n = (m_b2->getPos() - m_b1->getPos()) * (m_b3->getPos() - m_b1->getPos()) / glm::length((m_b2->getPos() - m_b1->getPos()) * (m_b3->getPos() - m_b1->getPos()));
	// cross sectional area of object
	float a0 = 0.5f * glm::length((m_b2->getPos() - m_b1->getPos()) * (m_b3->getPos() - m_b1->getPos()));
	float a = a0 * glm::dot(v, n) / glm::length(v);
	

	glm::vec3 drag = 0.5f * pho * glm::length(v) * glm::length(v) * cd * a * n;

	return -drag / 3.0f / m_connectedTriangles;


	//return -v * n * 0.4f / 3.0f / m_connectedTriangles;
}