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


// collision with cube
void cubeCollision(glm::vec3 cubeOrigin, glm::vec3 cubeDimentions, Particle &particle)
{
	for (int i = 0; i < 3; i++) // loop through the 3 dimentions
	{
		// Check if it collides with one of the two sides of the cube
		if (particle.getPos()[i] < cubeOrigin[i])
		{
			// Put particle back inside the cube
			particle.setPos(i, cubeOrigin[i]);
			// Invert velocity on this i axis
			particle.setVel(i, particle.getVel()[i] *= -1.0f);
		}
		if (particle.getPos()[i] > cubeOrigin[i] + cubeDimentions[i])
		{
			// Put particle back inside the cube
			particle.setPos(i, cubeOrigin[i] + cubeDimentions[i]);
			// Invert velocity on this i axis
			particle.setVel(i, particle.getVel()[i] *= -1.0f);
		}
	}
}



// main function
int main()
{
	// create application
	Application app = Application::Application();
	app.initRender();
	Application::camera.setCameraPosition(glm::vec3(0.0f, 20.0f, 60.0f));
			
	// create ground plane
	Mesh plane = Mesh::Mesh();
	// scale it up
	plane.scale(glm::vec3(40.0f, 0.0f, 40.0f));
	plane.setShader(Shader("resources/shaders/core.vert", "resources/shaders/core.frag"));

	// Collection of particles
	std::vector<Particle> particles;
	int noOfParticles = 30;

	// DEBUG
	std::vector<Particle> debugParticles;

	float maxInitialVelocity = 5.5f;
	float minInitialVelocity = -maxInitialVelocity;

	// Initial position of particles
	float distanceBetweenParticles = 2.0f;
	glm::vec3 startPosition(glm::vec3(-distanceBetweenParticles * (noOfParticles - 1) / 2.0f, 3.0f, 0.0f));

	for (int i = 0; i < noOfParticles; i++)
	{
		// create particle
		Particle newParticle = Particle::Particle();
		//newParticle.setMass(5.0f + 1.5f * i);
		newParticle.setMass(15.0f);
		newParticle.scale(glm::vec3(5.0f));
		//newParticle.scale(glm::vec3(2.0f + newParticle.getMass() / 2.0f));
		newParticle.getMesh().setShader(Shader("resources/shaders/core.vert", "resources/shaders/core_blue.frag"));
		// Set position
		newParticle.setPos(startPosition + glm::vec3(distanceBetweenParticles * i, 2.0f * i, 0.0f));
		// Set random velocity (bug: cannot be 0 because of drag)
		newParticle.setVel(glm::vec3(minInitialVelocity + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (maxInitialVelocity - minInitialVelocity))),
			minInitialVelocity + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (maxInitialVelocity - minInitialVelocity))),
			minInitialVelocity + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (maxInitialVelocity - minInitialVelocity)))));

		particles.push_back(newParticle);
	}

	particles[15].setMass(500.0f);
	//particles[15].scale(glm::vec3(3.0f));
	particles[15].getMesh().setShader(Shader("resources/shaders/core.vert", "resources/shaders/core_red.frag"));

	// Forces
	Gravity g = Gravity();
	Drag d = Drag();

	// apply forces
	for (int i = 0; i < noOfParticles; i++)
	{
		//particles[i].addForce(&g);

		for (int j = 0; j < noOfParticles; j++) {
			if (i != j) {
				NewtonGravity* n = new NewtonGravity(&particles[i], &particles[j]);
				particles[i].addForce(n);
			}
		}

		particles[i].addForce(&d);
	}

	// time
	GLfloat currentTime = (GLfloat) glfwGetTime();
	GLfloat timeAccumulator = 0.0f;

	// Cube
	glm::vec3 cubeOrigin = glm::vec3(-20.0f, 0.0f, -20.0f);
	glm::vec3 cubeDimentions = glm::vec3(40.0f);

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


			for (int i = 0; i < noOfParticles; i++)
			{
				particles[i].setAcc(particles[i].applyForces(particles[i].getPos(), particles[i].getVel(), currentTime, deltaTime));
				particles[i].setVel(particles[i].getVel() + deltaTime * particles[i].getAcc()); // eq1
				glm::vec3 displacement = deltaTime * particles[i].getVel(); // eq2
				particles[i].translate(displacement); // eq2

				// Collision with the plane
				//planeCollision(plane.getPos()[1], particles[i]);

				// Collision with the cube
				cubeCollision(cubeOrigin, cubeDimentions, particles[i]);

				// TEST - almost but not really a collision between spheres
				/*for (int j = 0; j < noOfParticles; j++) {
					if (j != i && glm::distance(particles[i].getPos(), particles[j].getPos()) < 2.5f) // (radius)
					{
						particles[i].getMesh().setShader(Shader("resources/shaders/core.vert", "resources/shaders/core_red.frag"));

						glm::vec3 j = -particles[i].getVel() * 1.5f;
						particles[i].setVel(particles[i].getVel() + (j / particles[i].getMass()));
					}
				}*/

				// Clamp velocity
				if (particles[i].getVel()[0] > 10.0f) particles[i].setVel(glm::vec3(10.0f, particles[i].getVel()[1], particles[i].getVel()[2]));
				if (particles[i].getVel()[1] > 10.0f) particles[i].setVel(glm::vec3(particles[i].getVel()[0], 10.0f, particles[i].getVel()[2]));
				if (particles[i].getVel()[2] > 10.0f) particles[i].setVel(glm::vec3(particles[i].getVel()[0], particles[i].getVel()[1], 10.0f));
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
		app.draw(plane);
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

