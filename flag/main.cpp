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
GLfloat deltaTime = 0.01f;//0.003f;
GLfloat simulationTime = 0.0f;


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


// check whether the particle is stationary
bool particleIsStationary(int clothDimentionPerSide, int row, int col)
{
	bool isStationary = false;

	/*if (row == 0 && (col == 0 || col == clothDimentionPerSide - 1))
	{
		isStationary = true;
	}*/
	if (row == 0)
	{
		isStationary = true;
	}

	return isStationary;
}


// get connected triagles
int getConnectedTriangles(int row, int col, int noOfParticlesPerSide)
{
	int trianglesAccumulator = 0;
	if (col - 1 > 0 && row - 1 > 0) // top left corner
	{
		trianglesAccumulator += 2;
	}
	if (col + 1 < noOfParticlesPerSide && row - 1 > 0) // top right corner
	{
		trianglesAccumulator += 1;
	}
	if (col - 1 > 0 && row + 1 < noOfParticlesPerSide) // bottom left corner
	{
		trianglesAccumulator += 1;
	}
	if (col + 1 < noOfParticlesPerSide && row + 1 < noOfParticlesPerSide) // bottom right corner
	{
		trianglesAccumulator += 2;
	}
	return trianglesAccumulator;
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
	std::vector<std::vector<Particle>> particles;
	int noOfParticlesPerSide = 12;

	// resize 2d vector
	particles.resize(noOfParticlesPerSide);
	for (int i = 0; i < noOfParticlesPerSide; i++)
	{
		particles[i].resize(noOfParticlesPerSide);
	}

	float minInitialVelocity = 0.0f;
	float maxInitialVelocity = 2.0f;

	// Initial position of particles
	float distanceBetweenParticles = 1.0f;
	float distanceFromGround = 12.0f;
	glm::vec3 startPosition(glm::vec3(-distanceBetweenParticles * noOfParticlesPerSide / 2, plane.getPos()[1] + distanceFromGround, -distanceBetweenParticles * noOfParticlesPerSide / 2));

	for (int i = 0; i < noOfParticlesPerSide; i++)
	{
		for (int j = 0; j < noOfParticlesPerSide; j++)
		{
			// create particle
			Particle newParticle = Particle::Particle();
			newParticle.setMass(0.1f);
			newParticle.scale(glm::vec3(2.0f));
			newParticle.getMesh().setShader(Shader("resources/shaders/core.vert", "resources/shaders/core_green.frag"));
			// Set position
			newParticle.setPos(startPosition + glm::vec3(distanceBetweenParticles * i, 0.0f, distanceBetweenParticles * j));
			// Set velocity
			// note: if moving it cannot be 0.0 because Hooke has a division by it
			newParticle.setVel(glm::vec3(0.00f));

			particles[i][j] = newParticle;
		}
	}

	// Forces
	Gravity g = Gravity();
	//Drag d = Drag();
	// loop though them ignoring side particles
	for (int i = 0; i < noOfParticlesPerSide; i++)
	{
		for (int j = 0; j < noOfParticlesPerSide; j++)
		{
			// ignore stationary particles
			if (!particleIsStationary(noOfParticlesPerSide, j, i))
			{
				particles[i][j].addForce(&g);

				//particles[i][j].addForce(&d);

				float ks = 9.0f; // spring stiffness
				float kd = 2.0f; // damping coefficient
				float rest = 0.8f; // spring rest length
				if (i - 1 > -1)
				{
					Hooke* hLeft = new Hooke(&particles[i][j], &particles[i - 1][j], ks, kd, rest);
					particles[i][j].addForce(hLeft);
				}
				if (j + 1 < noOfParticlesPerSide)
				{
					Hooke* hDown = new Hooke(&particles[i][j], &particles[i][j + 1], ks, kd, rest);
					particles[i][j].addForce(hDown);
				}
				if (i + 1 < noOfParticlesPerSide)
				{
					Hooke* hRight = new Hooke(&particles[i][j], &particles[i + 1][j], ks, kd, rest);
					particles[i][j].addForce(hRight);
				}
				if (j - 1 > -1)
				{
					Hooke* hUp = new Hooke(&particles[i][j], &particles[i][j - 1], ks, kd, rest);
					particles[i][j].addForce(hUp);
				}

				// aerodynamic drag + wind (apply to triangles)
				int connectedTriangles = getConnectedTriangles(j, i, noOfParticlesPerSide);
				//glm::vec3 wind_direction = glm::vec3(8.0f, 3.0f, -5.0f);
				glm::vec3 wind_direction = glm::vec3(10.0f, 2.0f, -8.0f);
				if (i - 1 > 0 && j - 1 > 0) // top left corner
				{
					SurfaceDrag* d1 = new SurfaceDrag(&particles[i - 1][j - 1], &particles[i - 1][j], &particles[i][j], wind_direction, connectedTriangles);
					particles[i][j].addForce(d1);
					SurfaceDrag* d2 = new SurfaceDrag(&particles[i - 1][j - 1], &particles[i][j], &particles[i][j - 1], wind_direction, connectedTriangles);
					particles[i][j].addForce(d2);
				}
				if (i + 1 < noOfParticlesPerSide && j - 1 > 0) // top right corner
				{
					SurfaceDrag* d3 = new SurfaceDrag(&particles[i][j - 1], &particles[i][j], &particles[i + 1][j], wind_direction, connectedTriangles);
					particles[i][j].addForce(d3);
				}
				if (i - 1 > 0 && j + 1 < noOfParticlesPerSide) // bottom left corner
				{
					SurfaceDrag* d4 = new SurfaceDrag(&particles[i - 1][j], &particles[i][j + 1], &particles[i][j], wind_direction, connectedTriangles);
					particles[i][j].addForce(d4);
				}
				if (i + 1 < noOfParticlesPerSide && j + 1 < noOfParticlesPerSide) // bottom right corner
				{
					SurfaceDrag* d5 = new SurfaceDrag(&particles[i][j], &particles[i][j + 1], &particles[i + 1][j + 1], wind_direction, connectedTriangles);
					particles[i][j].addForce(d5);
					SurfaceDrag* d6 = new SurfaceDrag(&particles[i][j], &particles[i + 1][j + 1], &particles[i + 1][j], wind_direction, connectedTriangles);
					particles[i][j].addForce(d6);
				}
				
			}

			// aerodynamic drag + wind (apply to triangles)
			// note: the particle is the top left corner of a quad
			// ignore bottom and right side
			/*if (i < noOfParticlesPerSide - 1 && j < noOfParticlesPerSide - 1)
			{
				glm::vec3 wind_direction = glm::vec3(0.0f, 1.0f, -3.0f);
				SurfaceDrag* dLowerTriangle = new SurfaceDrag(&particles[i][j], &particles[i][j + 1], &particles[i + 1][j + 1], wind_direction);
				SurfaceDrag* dUpperTriangle = new SurfaceDrag(&particles[i][j], &particles[i + 1][j + 1], &particles[i + 1][j], wind_direction);

				// apply the drag to all the non stationary vertices of the 2 triangles
				if (!particleIsStationary(noOfParticlesPerSide, j, i))
				{
					//particles[i][j].getMesh().setShader(Shader("resources/shaders/core.vert", "resources/shaders/core_red.frag"));
					particles[i][j].addForce(dLowerTriangle);
					particles[i][j].addForce(dUpperTriangle);
				}
				if (!particleIsStationary(noOfParticlesPerSide, j + 1, i))
				{
					particles[i][j + 1].addForce(dLowerTriangle);
				}
				if (!particleIsStationary(noOfParticlesPerSide, j + 1, i + 1))
				{
					particles[i + 1][j + 1].addForce(dLowerTriangle);
					particles[i + 1][j + 1].addForce(dUpperTriangle);
				}
				if (!particleIsStationary(noOfParticlesPerSide, j, i + 1))
				{
					particles[i + 1][j].addForce(dUpperTriangle);
				}
			}*/

		}
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
		// Time step
		GLfloat newTime = (GLfloat)glfwGetTime();
		GLfloat frameTime = newTime - currentTime;
		frameTime *= 3.0f; // velocity
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


			for (int i = 0; i < noOfParticlesPerSide; i++)
			{
				for (int j = 0; j < noOfParticlesPerSide; j++)
				{
					particles[i][j].setAcc(particles[i][j].applyForces(particles[i][j].getPos(), particles[i][j].getVel(), currentTime, deltaTime));
				}
			}
			for (int i = 0; i < noOfParticlesPerSide; i++)
			{
				for (int j = 0; j < noOfParticlesPerSide; j++)
				{
					particles[i][j].setVel(particles[i][j].getVel() + deltaTime * particles[i][j].getAcc()); // eq1
					glm::vec3 displacement = deltaTime * particles[i][j].getVel(); // eq2
					particles[i][j].translate(displacement); // eq2
					// Collision with the plane
					//planeCollision(plane.getPos()[1], particles[i][j]);
				}
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
		for (int i = 0; i < noOfParticlesPerSide; i++)
		{
			for (int j = 0; j < noOfParticlesPerSide; j++)
			{
				app.draw(particles[i][j].getMesh());
			}
		}

		app.display();
	}

	app.terminate();

	return EXIT_SUCCESS;
}

