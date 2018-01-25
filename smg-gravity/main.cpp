#pragma once

#include <vector>
#include <map>

// Math constants
#define _USE_MATH_DEFINES
#include <cmath>  
#include <random>

// Std. Includes
#include <string>
#include <time.h>
#include <list>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_operation.hpp>
#include "glm/ext.hpp"


// Other Libs
#include "SOIL2/SOIL2.h"

// project includes
#include "Application.h"
#include "Shader.h"
#include "Mesh.h"
#include "Particle.h"
#include "Force.h"
#include "RigidBody.h"


// time
GLfloat deltaTime = 0.01f;//0.003f;
GLfloat simulationTime = 0.0f;


// collision with plane
std::vector<Vertex> cuboidVerticesCollidingWithPlane(GLfloat yCoordinate, RigidBody &rb)
{
	std::vector<Vertex> collidingVertices;

	// Check if it collides with plane
	for (Vertex v : rb.getMesh().getVertices())
	{
		// check the y coord of: localCoord * sclae + worldCoord
		// note: with opengl you need to use mat * vec
		glm::vec3 transformedVertex = glm::mat3(rb.getMesh().getModel()) * v.getCoord() + rb.getPos();
		if (transformedVertex[1] < yCoordinate)
		{
			collidingVertices.push_back(transformedVertex);
		}
	}

	return collidingVertices;
}



// collision with any plane
bool testBoxPlane(RigidBody box, glm::vec3 planeNormal, glm::vec3 planePos)
{
	float radius =
		box.getScale()[0][0] * abs(glm::dot(planeNormal, box.getRotate()[0])) +
		box.getScale()[1][1] * abs(glm::dot(planeNormal, box.getRotate()[1])) +
		box.getScale()[2][2] * abs(glm::dot(planeNormal, box.getRotate()[2]));

	float distance = glm::dot(planeNormal, box.getPos()) - glm::distance(glm::vec3(0.0f), planePos);

	return abs(distance) <= radius;
}



// Given point p, return point q on (or in) OBB b, closest to p
void ClosestPtPointOBB(Vertex p, RigidBody b, Vertex &q)
{
	glm::vec3 d = p.getCoord() - b.getPos();
	// Start result at center of box; make steps from there
	q = b.getPos();
	// For each OBB axis...
	for (int i = 0; i < 3; i++)
	{
		// ...project d onto that axis to get the distance
		// along the axis of d from the box center
		float dist = glm::dot(d, b.getRotate()[i]);
		// If distance farther that the box extents, clamp to the box
		if (dist > b.getScale()[i][i]) dist = b.getScale()[i][i];
		if (dist < -b.getScale()[i][i]) dist = -b.getScale()[i][i];
		// Step that distance along the axis to get world coordinate
		//q += dist * b.getRotate()[i];
		q = Vertex(q.getCoord() + (dist * b.getRotate()[i]));
	}
}



// Computes the square distance between point p and OBB b
float SqDistPointOBB(Vertex p, RigidBody b)
{
	Vertex closest;
	ClosestPtPointOBB(p, b, closest);
	float sqDist = glm::dot(closest.getCoord() - p.getCoord(), closest.getCoord() - p.getCoord());
	return sqDist;
}



// main function
int main()
{
	// create application
	Application app = Application::Application();
	app.initRender();
	Application::camera.setCameraPosition(glm::vec3(0.0f, 0.0f, 35.0f));
			
	// create ground plane
	Mesh plane = Mesh::Mesh();
	// scale it up x5
	plane.scale(glm::vec3(50.0f, 0.0f, 50.0f));
	plane.setShader(Shader("resources/shaders/core.vert", "resources/shaders/core.frag"));

	// set up a vector of rigidbody
	std::vector<RigidBody> planetoids;
	int planetoidQuantity = 1;

	// Collection of particles
	std::vector<Particle> particles;
	int noOfParticles = 10;
	float maxInitialVelocity = 10.0f;
	float minInitialVelocity = -maxInitialVelocity;

	// set up cubic rigid body
	for (int i = 0; i < planetoidQuantity; i++)
	{
		RigidBody newRb = RigidBody();
		Mesh m = Mesh::Mesh(Mesh::CUBE);
		newRb.setMesh(m);
		newRb.getMesh().setShader(Shader("resources/shaders/core.vert", "resources/shaders/core.frag"));

		// rigid body motion values
		newRb.setMass(5.0f);
		newRb.scale(glm::vec3(8.0f, 1.5f, 2.0f));
		newRb.setPos(glm::vec3(0.0f, 0.0f, 0.0f));
		newRb.setVel(glm::vec3(0.0f));
		newRb.setAngVel(glm::vec3(0.0f));

		//newRb.rotate(10.0f, glm::vec3(10.0f));

		planetoids.push_back(newRb);
	}

	// set up particles
	for (int i = 0; i < noOfParticles; i++)
	{
		// create particle
		Particle newParticle = Particle::Particle();
		newParticle.setMass(0.09f);
		newParticle.scale(glm::vec3(4.0f));
		newParticle.getMesh().setShader(Shader("resources/shaders/core.vert", "resources/shaders/core_blue.frag"));
		// Set position
		newParticle.setPos(glm::vec3(0, 10.0f, 0.0f));
		// Set random velocity (bug: cannot be 0)
		newParticle.setVel(glm::vec3(minInitialVelocity + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (maxInitialVelocity - minInitialVelocity))),
			minInitialVelocity + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (maxInitialVelocity - minInitialVelocity))),
			/*minInitialVelocity + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (maxInitialVelocity - minInitialVelocity)))*/ 0.0f));

		//newParticle.setVel(glm::vec3(20.0f, 0.0f, 0.0f));

		particles.push_back(newParticle);
	}

	// set positions
	//particles[0].setPos(glm::vec3(1.0f, 10.0f, 0.0f));
	//particles[1].setPos(glm::vec3(10.0f, 0.0f, 0.0f));
	//particles[2].setPos(glm::vec3(-1.0f, -10.0f, 0.0f));
	//particles[3].setPos(glm::vec3(-10.0f, 0.0f, 0.0f));
	//particles[0].setPos(glm::vec3(0.0f, 10.0f, 0.0f));
	//particles[0].setVel(glm::vec3(maxInitialVelocity, 10.0f, 0.0f));
	//particles[0].getMesh().setShader(Shader("resources/shaders/core.vert", "resources/shaders/core_red.frag"));

	// Forces
	Drag d = Drag();
	for (int i = 0; i < particles.size(); i++) {

		//particles[i].addForce(&d);
		for (int n = 0; n < planetoids.size(); n++)
		{
			PlanetoidGravity* g = new PlanetoidGravity(5.8f, &planetoids[n]);
			particles[i].addForce(g);
		}

	}

	// DEBUG
	Particle qParticle = Particle::Particle();
	qParticle.scale(glm::vec3(8.0f));
	qParticle.getMesh().setShader(Shader("resources/shaders/core.vert", "resources/shaders/core_red.frag"));

	// time
	GLfloat currentTime = (GLfloat) glfwGetTime(); 
	GLfloat timeAccumulator = 0.0f;

	// Game loop
	while (!glfwWindowShouldClose(app.getWindow()))
	{
		// Time step
		GLfloat newTime = (GLfloat)glfwGetTime();
		GLfloat frameTime = newTime - currentTime;
		//frameTime *= 3.0f; // velocity
		currentTime = newTime;
		timeAccumulator += frameTime;

		/*
		**	INTERACTION
		*/
		// Manage interaction
		app.doMovement(deltaTime);


		while (timeAccumulator >= deltaTime)
		{
			/*
			**	SIMULATION
			*/

			for (int i = 0; i < particles.size(); i++)
			{
				particles[i].setAcc(particles[i].applyForces(particles[i].getPos(), particles[i].getVel(), currentTime, deltaTime));
				particles[i].setVel(particles[i].getVel() + deltaTime * particles[i].getAcc()); // eq1
				glm::vec3 displacement = deltaTime * particles[i].getVel(); // eq2
				particles[i].translate(displacement); // eq2
				
				// Collision with the plane
				//planeCollision(plane.getPos()[1], particles[i]);
			}


			// DEBUG
			/*Vertex tempQ;
			ClosestPtPointOBB(Vertex(particles[0].getPos()), planetoids[0], tempQ);
			qParticle.setPos(tempQ.getCoord());*/


			// COLLISIONS

			for (int i = 0; i < planetoids.size(); i++)
			{
				for (int j = 0; j < particles.size(); j++)
				{
					if (SqDistPointOBB(Vertex(particles[j].getPos()), planetoids[i]) <= 0.01f)
					{
						//particles[j].getMesh().setShader(Shader("resources/shaders/core.vert", "resources/shaders/core_red.frag"));

						/*float e = 1.0f;
						//glm::vec3 n = glm::normalize(qParticle.getPos() - particles[0].getPos());
						glm::vec3 n = glm::normalize(particles[0].getPos() - qParticle.getPos());
						glm::vec3 impulse = -(1 + e) * particles[i].getVel() / pow(particles[0].getMass(), -1);
						particles[j].setVel(particles[i].getVel() + (impulse / particles[i].getMass()));*/
					}
				}
			}

			//// TIME STEP
			timeAccumulator -= deltaTime;
			simulationTime += deltaTime;
		}
		


		/*
		**	RENDER 
		*/		
		// clear buffer
		app.clear();
		// draw groud plane
		//app.draw(plane);
		// draw rigid body
		for (int i = 0; i < planetoids.size(); i++)
		{
			app.draw(planetoids[i].getMesh());
		}
		// draw particles
		for (Particle p : particles)
		{
			app.draw(p.getMesh());
		}
		app.draw(qParticle.getMesh());

		app.display();
	}

	app.terminate();

	return EXIT_SUCCESS;
}

