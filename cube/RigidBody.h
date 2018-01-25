#pragma once
#include "Body.h"
class RigidBody :
	public Body
{
public:
	RigidBody();
	~RigidBody();

	// set and get methods
	void setAngVel(const glm::vec3 &omega) { m_angVel = omega; }
	void setAngAcc(const glm::vec3 &alpha) { m_angAcc = alpha; }
	void setInvInertia(const glm::mat3 &invInertia) { m_invInertia = invInertia; }
	void setMass(float mass) { m_mass = mass; updateInvInertia(); }

	glm::vec3 getAngVel() { return m_angVel; }
	glm::vec3 getAngAcc() { return m_angAcc; }
	glm::mat3 getInvInertia() { return m_invInertia; }
	void scale(glm::vec3 vect) { m_mesh.scale(vect); updateInvInertia(); }

	void updateInvInertia();
	
private:
	float m_density;
	glm::mat3 m_invInertia; // inverse inertia
	glm::vec3 m_angVel; // angular velocity
	glm::vec3 m_angAcc; // angular accelleration
};

