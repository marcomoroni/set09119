#pragma once
#include <glm/glm.hpp>
#include <iostream>

class Body; // forward declaration to avoid circular dependencies

class Force
{
public:
	Force();
	~Force();

	virtual glm::vec3 apply(float mass, const glm::vec3 &pos, const glm::vec3 &vel);
};


/*
** GRAVITY CLASS
*/
class Gravity : public Force {

public:
	// constructor
	Gravity() {}
	Gravity(const glm::vec3 &gravity) { m_gravity = gravity; }

	// get and set methods
	glm::vec3 getGravity() const { return m_gravity; }
	void setGravity(glm::vec3 gravity) { m_gravity = gravity; }

	// physics
	glm::vec3 apply(float mass, const glm::vec3 &pos, const glm::vec3 &vel);

private:
	glm::vec3 m_gravity = glm::vec3(0.0f, -9.8f, 0.0f);
};

/*
** POINT GRAVITY CLASS
*/
class PointGravity : public Force {

public:
	// constructor
	PointGravity() {}
	PointGravity(float gravity, glm::vec3 origin, float radius, Body* body) { m_gravity = gravity; m_origin = origin; m_radius = radius; m_body = body; }

	// get and set methods
	float getGravity() const { return m_gravity; }
	void setGravity(float gravity) { m_gravity = gravity; }
	glm::vec3 getOrigin() const { return m_origin; }
	void setOrigin(glm::vec3 origin) { m_origin = origin; }
	float getRadius() const { return m_radius; }
	void setRadius(float radius) { m_radius = radius; }
	Body* getBody() const { return m_body; }
	void setBody(Body* body) { m_body = body; }

	// physics
	glm::vec3 apply(float mass, const glm::vec3 &pos, const glm::vec3 &vel);

private:
	float m_gravity;
	glm::vec3 m_origin;
	float m_radius;
	Body* m_body;
};

/*
** DRAG CLASS
*/
class Drag : public Force {
public:
	Drag() {}

	// physics
	glm::vec3 apply(float mass, const glm::vec3 &pos, const glm::vec3 &vel);

private:

};

/*
** HOOKE CLASS
*/
class Hooke : public Force {
public:
	Hooke() {}
	Hooke(Body* b1, Body* b2, float ks, float kd, float rest) {
		m_ks = ks; m_kd = kd; m_rest = rest; m_b1 = b1; m_b2 = b2;
	}

	// get and set methods
	float getSpringStiffness() const { return m_ks; }
	void setSpringStiffness(float ks) { m_ks = ks; }
	float getDampingCoefficient() const { return m_kd; }
	void setDampingCoefficient(float kd) { m_kd = kd; }
	float getRestLength() const { return m_rest; }
	void setRestLength(float rest) { m_rest = rest; }
	Body* getBody1() const { return m_b1; }
	void setBody1(Body* b1) { m_b1 = b1; }
	Body* getBody2() const { return m_b2; }
	void setBody2(Body* b2) { m_b2 = b2; }

	// physics
	glm::vec3 apply(float mass, const glm::vec3 &pos, const glm::vec3 &vel);

private:
	float m_ks; // spring stiffness
	float m_kd; // damping coefficient
	float m_rest; // spring rest length

	Body* m_b1; // pointer to the body connected to one extremity of the spring
	Body* m_b2; // pointer to the body connected to the other extremity
};