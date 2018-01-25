#pragma once
// Math constants
#define _USE_MATH_DEFINES
#include <cmath>  
#include <random>

// Std. Includes
#include <string>
#include <time.h>

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


// time
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;


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
	Application::camera.setCameraPosition(glm::vec3(0.0f, 5.0f, 20.0f));
			
	// create ground plane
	Mesh plane = Mesh::Mesh();
	// scale it up x5
	plane.scale(glm::vec3(5.0f, 5.0f, 5.0f));
	plane.setShader(Shader("resources/shaders/core.vert", "resources/shaders/core.frag"));

	// create list of particles
	//List<Particle> Particles = new List<Particle>();																// TO DO

	// create particle (Forward Euler integration)
	Particle particle2 = Particle::Particle();
	particle2.getMesh().setShader(Shader("resources/shaders/core.vert", "resources/shaders/core_blue.frag"));

	// create particle (Semi-implicit Euler integration)
	Particle particle3 = Particle::Particle();
	particle3.getMesh().setShader(Shader("resources/shaders/core.vert", "resources/shaders/core_red.frag"));

	// time
	GLfloat firstFrame = (GLfloat) glfwGetTime();

	// OTHER THINGS
	// For particles
	particle2.setVel(glm::vec3(5.0f, 8.0f, 1.0f));
	particle2.setPos(glm::vec3(0.0f, 2.0f, 2.0f));
	particle3.setVel(glm::vec3(5.0f, 8.0f, 1.0f));
	particle3.setPos(glm::vec3(0.0f, 2.0f, 2.0f));
	// Gravity
	glm::vec3 gravity = glm::vec3(0.0f, -9.8f, 0.0f);	
	// Cube
	glm::vec3 cubeOrigin = glm::vec3(-2.5f, 0.0f, -2.5f);
	glm::vec3 cubeDimentions = glm::vec3(5.0f, 5.0f, 5.0f);
	// Aerodinamic drag																								// CHANGE THIS!
	float aerodynamicDragCoefficient = 0.3;
	// Wind
	glm::vec3 isotropicWind = glm::vec3(-0.5f, 0.0f, -0.9f);

	// Game loop
	while (!glfwWindowShouldClose(app.getWindow()))
	{
		// Set frame time
		GLfloat currentFrame = (GLfloat)  glfwGetTime() - firstFrame;
		// the animation can be sped up or slowed down by multiplying currentFrame by a factor.
		currentFrame *= 1.5f;
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		/*
		**	INTERACTION
		*/
		// Manage interaction
		app.doMovement(deltaTime);


		/*
		**	SIMULATION
		*/


		///// particle2
		glm::vec3 aerodynamicDrag = -particle2.getVel() * aerodynamicDragCoefficient;								// CHANGE THIS!
		particle2.setAcc(gravity + aerodynamicDrag + isotropicWind);
		particle2.setVel(particle2.getVel() + deltaTime * particle2.getAcc()); // eq1
		glm::vec3 displacement = deltaTime * particle2.getVel(); // eq2
		particle2.translate(displacement);

		// Collision with a cube
		cubeCollision(cubeOrigin, cubeDimentions, particle2);


		///// particle3
		aerodynamicDrag = -particle3.getVel() * aerodynamicDragCoefficient;											// CHANGE THIS!
		particle3.setAcc(gravity + aerodynamicDrag + isotropicWind);
		displacement = deltaTime * particle3.getVel();
		particle3.setVel(particle3.getVel() + deltaTime * particle3.getAcc());
		particle3.translate(displacement);

		// Collision with a cube
		cubeCollision(cubeOrigin, cubeDimentions, particle3);


		/*
		**	RENDER 
		*/		
		// clear buffer
		app.clear();
		// draw groud plane
		app.draw(plane);
		// draw particles
		app.draw(particle2.getMesh());
		app.draw(particle3.getMesh());

		app.display();
	}

	app.terminate();

	return EXIT_SUCCESS;
}

