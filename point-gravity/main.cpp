#pragma once

#include <vector>

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
#include <Windows.h>

// project includes
#include "Application.h"
#include "Shader.h"
#include "Mesh.h"
#include "Particle.h"
#include "Force.h"


// time
GLfloat deltaTime = 0.007f;
GLfloat simulationTime = 0.0f;


// collision with plane
void planeCollision(GLfloat yCoordinate, Particle &particle)
{
	// Check if it collides with plane
	if (particle.getPos()[1] < yCoordinate)
	{
		// Put particle back above the plane
		particle.setPos(1, yCoordinate);
		// Invert velocity on this axis
		particle.setVel(1, particle.getVel()[1] *= -0.8f);
	}
}



// main function
int main()
{
	// create application
	Application app = Application::Application();
	app.initRender();
	Application::camera.setCameraPosition(glm::vec3(0.0f, 5.0f, 20.0f));
			
	// create ground plane
	Mesh plane = Mesh::Mesh();
	// scale it up x5
	plane.scale(glm::vec3(50.0f, 50.0f, 50.0f));
	plane.setShader(Shader("resources/shaders/core.vert", "resources/shaders/core.frag"));

	// Collection of particles
	std::vector<Particle> particles;
	int noOfParticles = 300;

	// DEBUG
	std::vector<Particle> debugParticles;

	float minInitialVelocity = -1.5f;
	float maxInitialVelocity = 1.5f;

	for (int i = 0; i < noOfParticles; i++)
	{
		// create particle
		Particle newParticle = Particle::Particle();
		newParticle.setMass(0.09f);
		newParticle.scale(glm::vec3(2.0f));
		newParticle.getMesh().setShader(Shader("resources/shaders/core.vert", "resources/shaders/core_red.frag"));
		// Set position
		newParticle.setPos(glm::vec3(-11.0f, 5.0f, 0.0f));
		// Set random velocity (bug: cannot be 0)
		newParticle.setVel(glm::vec3(minInitialVelocity + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (maxInitialVelocity - minInitialVelocity))),
			minInitialVelocity + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (maxInitialVelocity - minInitialVelocity))),
			/*minInitialVelocity + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (maxInitialVelocity - minInitialVelocity)))*/ 0.0f));

		particles.push_back(newParticle);
	}

	// Forces
	Gravity g = Gravity();
	Drag d = Drag();
	float pointGravity1Power = 3.5f;
	glm::vec3 pointGravity1Origin = glm::vec3(-5.0f, 5.0f, -0.0f);
	float pointGravity1Radius = 7.0f;
	float pointGravity2Power = 3.5f;
	glm::vec3 pointGravity2Origin = glm::vec3(5.0f, 5.0f, 0.0f);
	float pointGravity2Radius = 7.0f;

	// DEBUG
	Particle d1 = Particle::Particle();
	d1.getMesh().setShader(Shader("resources/shaders/core.vert", "resources/shaders/core.frag"));
	d1.scale(glm::vec3(2.0f));
	d1.setPos(pointGravity1Origin);
	debugParticles.push_back(d1);
	Particle d2 = Particle::Particle();
	d2.getMesh().setShader(Shader("resources/shaders/core.vert", "resources/shaders/core.frag"));
	d2.scale(glm::vec3(2.0f));
	d2.setPos(pointGravity2Origin);
	debugParticles.push_back(d2);

	// apply forces
	for (int i = 0; i < noOfParticles; i++)
	{
		//particles[i].addForce(&g);

		PointGravity* p1 = new PointGravity(pointGravity1Power, pointGravity1Origin, pointGravity1Radius, &particles[i]);
		particles[i].addForce(p1);

		PointGravity* p2 = new PointGravity(pointGravity2Power, pointGravity2Origin, pointGravity2Radius, &particles[i]);
		particles[i].addForce(p2);

		particles[i].addForce(&d);
	}

	// time
	GLfloat currentTime = (GLfloat) glfwGetTime();
	GLfloat timeAccumulator = 0.0f;

	// Cube
	glm::vec3 cubeOrigin = glm::vec3(-2.5f, 0.0f, -2.5f);
	glm::vec3 cubeDimentions = glm::vec3(5.0f, 5.0f, 5.0f);

	// Game loop
	while (!glfwWindowShouldClose(app.getWindow()))
	{
		if (GetKeyState(VK_LBUTTON) < 0)
		{
			
		}

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


			for (int i = 0; i < noOfParticles; i++)
			{
				particles[i].setAcc(particles[i].applyForces(particles[i].getPos(), particles[i].getVel(), currentTime, deltaTime));
				particles[i].setVel(particles[i].getVel() + deltaTime * particles[i].getAcc()); // eq1
				glm::vec3 displacement = deltaTime * particles[i].getVel(); // eq2
				particles[i].translate(displacement); // eq2
				// Collision with the plane
				//planeCollision(plane.getPos()[1], particles[i]);
			}

			//// Time step
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
		// draw particles
		for (int i = 0; i < particles.size(); i++) {
			app.draw(particles[i].getMesh());
		}
		for (int i = 0; i < debugParticles.size(); i++) {
			app.draw(debugParticles[i].getMesh());
		}

		app.display();
	}

	app.terminate();

	return EXIT_SUCCESS;
}

