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



//
/*int testOBBOBB(RigidBody &a, RigidBody &b)
{
	float ra, rb;
	glm::mat3 R, AbsR;

	float EPSILON = 0.01f;

	int smallestOverlapAxis = 0;
	float smallestOverlap = 50000.0f; // change this

	// compute rotation matrix expressing b in a's coordinate frame
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			R[i][j] = glm::dot(a.getRotate()[i], b.getRotate()[j]);
		}
	}

	// compute translation vector t
	glm::vec3 t = b.getPos() - a.getPos();
	// Bring translation into a's coordinate frame
	t = glm::vec3(glm::dot(t, a.getRotate()[0]), glm::dot(t, a.getRotate()[1]), glm::dot(t, a.getRotate()[2]));

	// compute common subexpression. add in an epsilon term to
	// counteract arithmetic errors when two edges are parallel and
	// their cross product is (near) null
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			AbsR[i][j] = abs(R[i][j]) + EPSILON;
		}
	}

	// test axis L = A0, L = A1, L = A2
	for (int i = 0; i < 3; i++)
	{
		ra = a.getScale()[i][i];
		rb = b.getScale()[0][0] * AbsR[i][0] + b.getScale()[1][1] * AbsR[i][1] + b.getScale()[2][2] * AbsR[i][2];
		//if (abs(t[i]) > ra + rb) return false;
		if (abs(t[i]) < ra + rb)
		{
			float currentOverlap = (ra + rb) - abs(t[i]);
			if (smallestOverlap > currentOverlap)
			{
				smallestOverlapAxis = i + 1;
				smallestOverlap = currentOverlap;
			}
		}
		else return 0;
	}

	// test axis L = B0, L = B1, L = B2
	for (int i = 0; i < 3; i++)
	{
		ra = a.getScale()[0][0] * AbsR[0][i] + a.getScale()[1][1] * AbsR[1][i] + a.getScale()[2][2] * AbsR[2][1];
		rb = b.getScale()[i][i];
		//if (abs(t[0] * R[0][i] + t[1] * R[1][i] + t[2] * R[2][i]) > ra + rb) return false;
		if (abs(t[0] * R[0][i] + t[1] * R[1][i] + t[2] * R[2][i]) < ra + rb)
		{
			float currentOverlap = (ra + rb) - abs(t[0] * R[0][i] + t[1] * R[1][i] + t[2] * R[2][i]);
			if (smallestOverlap > currentOverlap)
			{
				smallestOverlapAxis = i + 4;
				smallestOverlap = currentOverlap;
			}
		}
		else return 0;
	}

	// test axis L = A0 x B0
	ra = a.getScale()[1][1] * AbsR[2][0] + a.getScale()[2][2] * AbsR[1][0];
	rb = b.getScale()[1][1] * AbsR[0][2] + b.getScale()[2][2] * AbsR[0][1];
	//if (abs(t[2] * R[1][0] - t[1] * R[2][0]) > ra + rb) return false;
	if (abs(t[2] * R[1][0] - t[1] * R[2][0]) < ra + rb)
	{
		float currentOverlap = (ra + rb) - abs(t[2] * R[1][0] - t[1] * R[2][0]);
		if (smallestOverlap > currentOverlap)
		{
			smallestOverlapAxis = 7;
			smallestOverlap = currentOverlap;
		}
	}
	else return 0;

	// test axis L = A0 x B1
	ra = a.getScale()[1][1] * AbsR[2][1] + a.getScale()[2][2] * AbsR[1][1];
	rb = b.getScale()[0][0] * AbsR[0][2] + b.getScale()[2][2] * AbsR[0][0];
	//if (abs(t[2] * R[1][0] - t[1] * R[2][0]) > ra + rb) return false;
	if (abs(t[2] * R[1][0] - t[1] * R[2][0]) < ra + rb)
	{
		float currentOverlap = (ra + rb) - abs(t[2] * R[1][0] - t[1] * R[2][0]);
		if (smallestOverlap > currentOverlap)
		{
			smallestOverlapAxis = 8;
			smallestOverlap = currentOverlap;
		}
	}
	else return 0;

	// test axis L = A0 x B2
	ra = a.getScale()[1][1] * AbsR[2][2] + a.getScale()[2][2] * AbsR[1][2];
	rb = b.getScale()[0][0] * AbsR[0][1] + b.getScale()[1][1] * AbsR[0][0];
	//if (abs(t[2] * R[1][2] - t[1] * R[2][2]) > ra + rb) return false;
	if (abs(t[2] * R[1][2] - t[1] * R[2][2]) < ra + rb)
	{
		float currentOverlap = (ra + rb) - abs(t[2] * R[1][2] - t[1] * R[2][2]);
		if (smallestOverlap > currentOverlap)
		{
			smallestOverlapAxis = 9;
			smallestOverlap = currentOverlap;
		}
	}
	else return 0;

	// test axis L = A1 x B0
	ra = a.getScale()[0][0] * AbsR[2][0] + a.getScale()[2][2] * AbsR[0][0];
	rb = b.getScale()[1][1] * AbsR[1][2] + b.getScale()[2][2] * AbsR[1][1];
	//if (abs(t[0] * R[2][0] - t[2] * R[0][0]) > ra + rb) return false;
	if (abs(t[0] * R[2][0] - t[2] * R[0][0]) < ra + rb)
	{
		float currentOverlap = (ra + rb) - abs(t[0] * R[2][0] - t[2] * R[0][0]);
		if (smallestOverlap > currentOverlap)
		{
			smallestOverlapAxis = 10;
			smallestOverlap = currentOverlap;
		}
	}
	else return 0;

	// test axis L = A1 x B1
	ra = a.getScale()[0][0] * AbsR[2][1] + a.getScale()[2][2] * AbsR[0][1];
	rb = b.getScale()[0][0] * AbsR[0][2] + b.getScale()[2][2] * AbsR[0][0];
	//if (abs(t[0] * R[2][1] - t[2] * R[0][1]) > ra + rb) return false;
	if (abs(t[0] * R[2][1] - t[2] * R[0][1]) < ra + rb)
	{
		float currentOverlap = (ra + rb) - abs(t[0] * R[2][1] - t[2] * R[0][1]);
		if (smallestOverlap > currentOverlap)
		{
			smallestOverlapAxis = 11;
			smallestOverlap = currentOverlap;
		}
	}
	else return 0;

	// test axis L = A1 x B2
	ra = a.getScale()[0][0] * AbsR[2][2] + a.getScale()[2][2] * AbsR[0][2];
	rb = b.getScale()[0][0] * AbsR[1][1] + b.getScale()[1][1] * AbsR[1][0];
	//if (abs(t[0] * R[2][2] - t[2] * R[0][2]) > ra + rb) return false;
	if (abs(t[0] * R[2][2] - t[2] * R[0][2]) < ra + rb)
	{
		float currentOverlap = (ra + rb) - abs(t[0] * R[2][2] - t[2] * R[0][2]);
		if (smallestOverlap > currentOverlap)
		{
			smallestOverlapAxis = 12;
			smallestOverlap = currentOverlap;
		}
	}
	else return 0;

	// test axis L = A2 x B0
	ra = a.getScale()[0][0] * AbsR[1][0] + a.getScale()[1][1] * AbsR[0][0];
	rb = b.getScale()[1][1] * AbsR[2][2] + b.getScale()[2][2] * AbsR[2][1];
	//if (abs(t[1] * R[0][0] - t[0] * R[1][0]) > ra + rb) return false;
	if (abs(t[1] * R[0][0] - t[0] * R[1][0]) < ra + rb)
	{
		float currentOverlap = (ra + rb) - abs(t[1] * R[0][0] - t[0] * R[1][0]);
		if (smallestOverlap > currentOverlap)
		{
			smallestOverlapAxis = 13;
			smallestOverlap = currentOverlap;
		}
	}
	else return 0;

	// test axis L = A2 x B1
	ra = a.getScale()[0][0] * AbsR[1][1] + a.getScale()[1][1] * AbsR[0][1];
	rb = b.getScale()[0][0] * AbsR[2][2] + b.getScale()[2][2] * AbsR[2][0];
	//if (abs(t[1] * R[0][1] - t[0] * R[1][1]) > ra + rb) return false;
	if (abs(t[1] * R[0][1] - t[0] * R[1][1]) < ra + rb)
	{
		float currentOverlap = (ra + rb) - abs(t[1] * R[0][1] - t[0] * R[1][1]);
		if (smallestOverlap > currentOverlap)
		{
			smallestOverlapAxis = 14;
			smallestOverlap = currentOverlap;
		}
	}
	else return 0;

	// test axis L = A2 x B2
	ra = a.getScale()[0][0] * AbsR[1][2] + a.getScale()[1][1] * AbsR[0][2];
	rb = b.getScale()[0][0] * AbsR[2][1] + b.getScale()[1][1] * AbsR[2][0];
	//if (abs(t[1] * R[0][2] - t[0] * R[1][2]) > ra + rb) return false;
	if (abs(t[1] * R[0][2] - t[0] * R[1][2]) < ra + rb)
	{
		float currentOverlap = (ra + rb) - abs(t[1] * R[0][2] - t[0] * R[1][2]);
		if (smallestOverlap > currentOverlap)
		{
			smallestOverlapAxis = 15;
			smallestOverlap = currentOverlap;
		}	
	}
	else return 0;

	// since no separating axis is found, the ))Bs must be interseting
	if (smallestOverlapAxis > 0)
	{
		//std::cout << smallestOverlapAxis << " " << smallestOverlap << std::endl;
	}
	return smallestOverlapAxis;

}*/



bool testOBBOBB(RigidBody &a, RigidBody &b)
{
	float ra, rb;
	glm::mat3 R, AbsR;

	float EPSILON = 0.01f;

	// compute rotation matrix expressing b in a's coordinate frame
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			R[i][j] = glm::dot(a.getRotate()[i], b.getRotate()[j]);
		}
	}

	// compute translation vector t
	glm::vec3 t = b.getPos() - a.getPos();
	// Bring translation into a's coordinate frame
	t = glm::vec3(glm::dot(t, a.getRotate()[0]), glm::dot(t, a.getRotate()[1]), glm::dot(t, a.getRotate()[2]));

	// compute common subexpression. add in an epsilon term to
	// counteract arithmetic errors when two edges are parallel and
	// their cross product is (near) null
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			AbsR[i][j] = abs(R[i][j]) + EPSILON;
		}
	}

	// test axis L = A0, L = A1, L = A2
	for (int i = 0; i < 3; i++)
	{
		ra = a.getScale()[i][i];
		rb = b.getScale()[0][0] * AbsR[i][0] + b.getScale()[1][1] * AbsR[i][1] + b.getScale()[2][2] * AbsR[i][2];
		if (abs(t[i]) > ra + rb) return false;
	}

	// test axis L = B0, L = B1, L = B2
	for (int i = 0; i < 3; i++)
	{
		ra = a.getScale()[0][0] * AbsR[0][i] + a.getScale()[1][1] * AbsR[1][i] + a.getScale()[2][2] * AbsR[2][1];
		rb = b.getScale()[i][i];
		if (abs(t[0] * R[0][i] + t[1] * R[1][i] + t[2] * R[2][i]) > ra + rb) return false;
	}

	// test axis L = A0 x B0
	ra = a.getScale()[1][1] * AbsR[2][0] + a.getScale()[2][2] * AbsR[1][0];
	rb = b.getScale()[1][1] * AbsR[0][2] + b.getScale()[2][2] * AbsR[0][1];
	if (abs(t[2] * R[1][0] - t[1] * R[2][0]) > ra + rb) return false;

	// test axis L = A0 x B1
	ra = a.getScale()[1][1] * AbsR[2][1] + a.getScale()[2][2] * AbsR[1][1];
	rb = b.getScale()[0][0] * AbsR[0][2] + b.getScale()[2][2] * AbsR[0][0];
	if (abs(t[2] * R[1][0] - t[1] * R[2][0]) > ra + rb) return false;

	// test axis L = A0 x B2
	ra = a.getScale()[1][1] * AbsR[2][2] + a.getScale()[2][2] * AbsR[1][2];
	rb = b.getScale()[0][0] * AbsR[0][1] + b.getScale()[1][1] * AbsR[0][0];
	if (abs(t[2] * R[1][2] - t[1] * R[2][2]) > ra + rb) return false;

	// test axis L = A1 x B0
	ra = a.getScale()[0][0] * AbsR[2][0] + a.getScale()[2][2] * AbsR[0][0];
	rb = b.getScale()[1][1] * AbsR[1][2] + b.getScale()[2][2] * AbsR[1][1];
	if (abs(t[0] * R[2][0] - t[2] * R[0][0]) > ra + rb) return false;

	// test axis L = A1 x B1
	ra = a.getScale()[0][0] * AbsR[2][1] + a.getScale()[2][2] * AbsR[0][1];
	rb = b.getScale()[0][0] * AbsR[0][2] + b.getScale()[2][2] * AbsR[0][0];
	if (abs(t[0] * R[2][1] - t[2] * R[0][1]) > ra + rb) return false;

	// test axis L = A1 x B2
	ra = a.getScale()[0][0] * AbsR[2][2] + a.getScale()[2][2] * AbsR[0][2];
	rb = b.getScale()[0][0] * AbsR[1][1] + b.getScale()[1][1] * AbsR[1][0];
	if (abs(t[0] * R[2][2] - t[2] * R[0][2]) > ra + rb) return false;

	// test axis L = A2 x B0
	ra = a.getScale()[0][0] * AbsR[1][0] + a.getScale()[1][1] * AbsR[0][0];
	rb = b.getScale()[1][1] * AbsR[2][2] + b.getScale()[2][2] * AbsR[2][1];
	if (abs(t[1] * R[0][0] - t[0] * R[1][0]) > ra + rb) return false;

	// test axis L = A2 x B1
	ra = a.getScale()[0][0] * AbsR[1][1] + a.getScale()[1][1] * AbsR[0][1];
	rb = b.getScale()[0][0] * AbsR[2][2] + b.getScale()[2][2] * AbsR[2][0];
	if (abs(t[1] * R[0][1] - t[0] * R[1][1]) > ra + rb) return false;

	// test axis L = A2 x B2
	ra = a.getScale()[0][0] * AbsR[1][2] + a.getScale()[1][1] * AbsR[0][2];
	rb = b.getScale()[0][0] * AbsR[2][1] + b.getScale()[1][1] * AbsR[2][0];
	if (abs(t[1] * R[0][2] - t[0] * R[1][2]) > ra + rb) return false;

	return true;

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
	Application::camera.setCameraPosition(glm::vec3(0.0f, 5.0f, 20.0f));
			
	// create ground plane
	Mesh plane = Mesh::Mesh();
	// scale it up x5
	plane.scale(glm::vec3(30.0f, 0.0f, 30.0f));
	plane.setShader(Shader("resources/shaders/core.vert", "resources/shaders/core.frag"));

	// set up a vector of rigidbody
	std::vector<RigidBody> dominoPieces;
	int dominoPiecesQuantity = 2;

	// set up values
	float distanceBetweenPieces = 4.0f;

	// set up cubic rigid body
	for (int i = 0; i < dominoPiecesQuantity; i++)
	{
		RigidBody newRb = RigidBody();
		Mesh m = Mesh::Mesh(Mesh::CUBE);
		newRb.setMesh(m);
		newRb.getMesh().setShader(Shader("resources/shaders/core.vert", "resources/shaders/core_blue.frag"));

		// rigid body motion values
		newRb.setMass(5.0f);
		newRb.scale(glm::vec3(0.5f, 3.0f, 1.0f));
		float xInitialPos = -distanceBetweenPieces * float(dominoPiecesQuantity - 1) / 2.0f + float(distanceBetweenPieces) * float(i);
		float yInitialPos = newRb.getScale()[1][1] / 2.0f;
		float zInitialPos = 0.0f;
		//float zInitialPos = 0.0f + float(i) * -0.4f;
		newRb.setPos(glm::vec3(xInitialPos, yInitialPos, zInitialPos));
		newRb.setVel(glm::vec3(0.0f));
		newRb.setAngVel(glm::vec3(0.0f));

		//newRb.rotate(0.09f * float(i), glm::vec3(0.0f, 1.0f, 0.0f));

		dominoPieces.push_back(newRb);
	}

	// Forces
	Gravity g = Gravity();
	for (int i = 0; i < dominoPiecesQuantity; i++)
	{
		dominoPieces[i].addForce(&g);
	}

	// DEBUG - particles
	std::vector<Particle> particles;

	// time
	GLfloat currentTime = (GLfloat) glfwGetTime(); 
	GLfloat timeAccumulator = 0.0f;

	// DEBUG
	bool invInertiaShowed = false;
	int collisionsToShow = 1;
	int collisionsShowed = 0;

	// inital impulse
	RigidBody &firstPiece = dominoPieces[0];
	glm::vec3 initalImpulse = glm::vec3(6.0f, 0.0f, 0.0f);
	bool initalImpulseApplied = false;
	float initalImpulseApplicationTime = 2.0f;

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


			// RIGID BODY

			for (int i = 0; i < dominoPiecesQuantity; i++)
			{

				// Stop flickering [?]
				/*if (dominoPieces[i].getAngVel()[0] < 0.01f && dominoPieces[i].getAngVel()[1] < 0.01f && dominoPieces[i].getAngVel()[2] < 0.01f)
				{
					dominoPieces[i].setAngVel(glm::vec3(0.0f));
				}*/

				// position integration
				dominoPieces[i].setAcc(dominoPieces[i].applyForces(dominoPieces[i].getPos(), dominoPieces[i].getVel(), currentTime, deltaTime));
				dominoPieces[i].setVel(dominoPieces[i].getVel() + deltaTime * dominoPieces[i].getAcc());
				dominoPieces[i].translate(deltaTime * dominoPieces[i].getVel());

				// rotational integration - 3D
				dominoPieces[i].setAngVel(dominoPieces[i].getAngVel() + deltaTime * dominoPieces[i].getAngAcc());
				glm::mat3 angVelSkew = glm::matrixCross3(dominoPieces[i].getAngVel());
				glm::mat3 R = glm::mat3(dominoPieces[i].getRotate());
				R += deltaTime * angVelSkew * R;
				R = glm::orthonormalize(R);
				dominoPieces[i].getMesh().setRotate(glm::mat4(R));

			}

			// collisions and impulses

			// INITIAL IMPULSE
			if (currentTime > initalImpulseApplicationTime && !initalImpulseApplied)
			{
				Vertex initalImpulseApplicationPoint = Vertex(firstPiece.getPos() + glm::vec3(-0.25f, 2.0f, 0.0f)); // to code better
				std::cout << "initalImpulseApplicationPoint: " << glm::to_string(initalImpulseApplicationPoint.getCoord()) << std::endl;
				firstPiece.setVel(firstPiece.getVel() + (initalImpulse / firstPiece.getMass()));
				firstPiece.setAngVel(firstPiece.getAngVel() + (firstPiece.getInvInertia() * glm::cross(initalImpulseApplicationPoint.getCoord() - firstPiece.getPos(), initalImpulse)));
				initalImpulseApplied = true;

				Particle p;
				p.setPos(initalImpulseApplicationPoint.getCoord());
				p.getMesh().setShader(Shader("resources/shaders/core.vert", "resources/shaders/core_blue.frag"));
				//p.scale(glm::vec3(4.0f));
				particles.push_back(p);
			}

			for (int i = 0; i < dominoPiecesQuantity; i++)
			{
				//particles.clear();

				// DOMINO PIECES COLLISION
				// DEBUG - collide everything and turn it red
				for (int otherDominoPieceNumber = 0; otherDominoPieceNumber < dominoPiecesQuantity; otherDominoPieceNumber++)
				{
					if (i != otherDominoPieceNumber)
					{
						/*// if the collision axis is id = 0 than there is no collision
						int collisionAxis = testOBBOBB(dominoPieces[i], dominoPieces[otherDominoPieceNumber]);
						if (collisionAxis > 0)
						{
							dominoPieces[i].getMesh().setShader(Shader("resources/shaders/core.vert", "resources/shaders/core_red.frag"));
							dominoPieces[otherDominoPieceNumber].getMesh().setShader(Shader("resources/shaders/core.vert", "resources/shaders/core_red.frag"));

							// COLLISION RESPONSE

							// Point to face
							if (collisionAxis <= 6)
							{
								//// get the rigid body which axis are going to be used
								//RigidBody *rigidBodysAxis;
								//if (collisionAxis <= 3) rigidBodysAxis = &dominoPieces[i];
								//else rigidBodysAxis = &dominoPieces[otherDominoPieceNumber];

								//// plane normal
								//glm::vec3 planeNormal;
								//if (collisionAxis == 1 || collisionAxis == 4) planeNormal = glm::vec3(1.0f, 0.0f, 0.0f) * glm::mat3(rigidBodysAxis->getRotate());
								//if (collisionAxis == 2 || collisionAxis == 5) planeNormal = glm::vec3(0.0f, 1.0f, 0.0f) * glm::mat3(rigidBodysAxis->getRotate());
								//if (collisionAxis == 3 || collisionAxis == 6) planeNormal = glm::vec3(0.0f, 0.0f, 1.0f) * glm::mat3(rigidBodysAxis->getRotate());  

								Vertex impulseApplicationPoint;
								float smallestDistance = 50000.0f;
								if (collisionAxis <= 3)
								{
									for (Vertex v : dominoPieces[otherDominoPieceNumber].getMesh().getVertices())
									{
										float currentPointOBBDistance = SqDistPointOBB(v, dominoPieces[i]);
										if (currentPointOBBDistance < smallestDistance)
										{
											smallestDistance = currentPointOBBDistance;
											impulseApplicationPoint = v;
										}
									}
								}
								else
								{
									for (Vertex v : dominoPieces[i].getMesh().getVertices())
									{
										float currentPointOBBDistance = SqDistPointOBB(v, dominoPieces[otherDominoPieceNumber]);
										if (currentPointOBBDistance < smallestDistance)
										{
											smallestDistance = currentPointOBBDistance;
											impulseApplicationPoint = v;
										}
									}
								}
								dominoPieces[i].setAngVel(dominoPieces[i].getAngVel() + (dominoPieces[i].getInvInertia() * glm::cross(impulseApplicationPoint.getCoord() - dominoPieces[i].getPos(), glm::vec3(-4.0f, 0.0f, 5.0f))));
								dominoPieces[otherDominoPieceNumber].setAngVel(dominoPieces[i].getAngVel() + (dominoPieces[i].getInvInertia() * glm::cross(impulseApplicationPoint.getCoord() - dominoPieces[i].getPos(), glm::vec3(4.0f, 0.0f, 0.0f))));
							}
						}*/
						if (testOBBOBB(dominoPieces[i], dominoPieces[otherDominoPieceNumber]))
						{
							//dominoPieces[i].getMesh().setShader(Shader("resources/shaders/core.vert", "resources/shaders/core_red.frag"));

							/*// Get impulse application point
							Vertex impulseApplicationPoint;
							float smallestDistance = 50000.0f;
							for (Vertex v : dominoPieces[otherDominoPieceNumber].getMesh().getVertices())
							{
								float currentPointOBBDistance = SqDistPointOBB(v, dominoPieces[i]);
								if (currentPointOBBDistance < smallestDistance)
								{
									smallestDistance = currentPointOBBDistance;
									impulseApplicationPoint = v;
								}
							}
							for (Vertex v : dominoPieces[i].getMesh().getVertices())
							{
								float currentPointOBBDistance = SqDistPointOBB(v, dominoPieces[otherDominoPieceNumber]);
								if (currentPointOBBDistance < smallestDistance)
								{
									smallestDistance = currentPointOBBDistance;
									impulseApplicationPoint = v;
								}
							}*/

							// Get impulse application point
							std::vector<Vertex> closestVertices;
							float smallestDistance = 500000.0f;
							for (Vertex v : dominoPieces[otherDominoPieceNumber].getMesh().getVertices())
							{
								v = Vertex(glm::mat3(dominoPieces[otherDominoPieceNumber].getMesh().getModel()) * v.getCoord() + dominoPieces[otherDominoPieceNumber].getPos());
								float currentPointOBBDistance = SqDistPointOBB(v, dominoPieces[i]);
								if (currentPointOBBDistance < smallestDistance)
								{
									closestVertices.clear();
									smallestDistance = currentPointOBBDistance;
								}
								if (currentPointOBBDistance == smallestDistance)
								{
									closestVertices.push_back(v);
								}
							}
							for (Vertex v : dominoPieces[i].getMesh().getVertices())
							{
								v = Vertex(glm::mat3(dominoPieces[i].getMesh().getModel()) * v.getCoord() + dominoPieces[i].getPos());
								float currentPointOBBDistance = SqDistPointOBB(v, dominoPieces[otherDominoPieceNumber]);
								if (currentPointOBBDistance < smallestDistance)
								{
									closestVertices.clear();
									smallestDistance = currentPointOBBDistance;
								}
								if (currentPointOBBDistance == smallestDistance)
								{
									closestVertices.push_back(v);
								}
							}
							glm::vec3 sumOfVertices = glm::vec3(0.0f);
							for (Vertex v : closestVertices)
							{
								sumOfVertices += v.getCoord();
							}
							Vertex impulseApplicationPoint = Vertex(sumOfVertices / closestVertices.size());

							// DEBUG - draw impulse application point
							if (particles.size() < 500) {
								Particle p;
								//p.setPos(impulseApplicationPoint.getCoord() + glm::vec3(0.0f, 0.0f, 0.01f + float (particles.size())));
								p.setPos(impulseApplicationPoint.getCoord() + glm::vec3(0.0f, 0.0f, 0.0f));
								p.getMesh().setShader(Shader("resources/shaders/core.vert", "resources/shaders/core_red.frag"));
								//p.scale(glm::vec3(4.0f));
								particles.push_back(p);
							}
							
							// Calculate values for impulse
							glm::vec3 r = impulseApplicationPoint.getCoord() - dominoPieces[i].getPos();
							glm::vec3 j = dominoPieces[i].getVel() + glm::cross(dominoPieces[i].getAngVel(), r);

							// TEST - transalte a bit
							dominoPieces[i].translate(glm::vec3(-0.05f, 0.05f, 0.0f));
							dominoPieces[otherDominoPieceNumber].translate(glm::vec3(0.05f, 0.0f, 0.0f));

							// apply impulse to the two colliding  pieces
							dominoPieces[i].setVel(dominoPieces[i].getVel() - (j / dominoPieces[i].getMass()));
							dominoPieces[i].setAngVel(dominoPieces[i].getAngVel() - (dominoPieces[i].getInvInertia() * glm::cross(r, j)));
							dominoPieces[otherDominoPieceNumber].setVel(dominoPieces[otherDominoPieceNumber].getVel() + (j / dominoPieces[otherDominoPieceNumber].getMass()));
							dominoPieces[otherDominoPieceNumber].setAngVel(dominoPieces[otherDominoPieceNumber].getAngVel() + (dominoPieces[otherDominoPieceNumber].getInvInertia() * glm::cross(r, j)));
						}
					}
				}

				// PLANE COLLISION
				std::vector<Vertex> collidingVertices = cuboidVerticesCollidingWithPlane(plane.getPos()[1], dominoPieces[i]);
				bool collisionOccurs = collidingVertices.size() > 0;
				if (collisionOccurs)
				{
					// translate up on y axis by the lowest vertex on the y axis
					Vertex lowestVertex = collidingVertices[0].getCoord();
					for (Vertex v : collidingVertices)
					{
						if (v.getCoord().y < lowestVertex.getCoord().y)
						{
							lowestVertex = v;
						}
					}
					glm::vec3 displacement = glm::vec3(0.0f);
					displacement.y = glm::abs(lowestVertex.getCoord().y);
					dominoPieces[i].translate(displacement);

					// DEBUG
					if (collisionsShowed < collisionsToShow)
					{
						for (Vertex v : collidingVertices)
						{
							std::cout << "Collision detected in: " << glm::to_string(v.getCoord()) << std::endl;
						}
						std::cout << std::endl;
					}

					// get average collision point
					glm::vec3 sumOfVertices = glm::vec3(0.0f);
					for (Vertex v : collidingVertices)
					{
						sumOfVertices += v.getCoord();
					}
					Vertex averageCollidingPoint = Vertex(sumOfVertices / collidingVertices.size());

					// calculate impulse j
					glm::vec3 r = averageCollidingPoint.getCoord() - dominoPieces[i].getPos();
					glm::vec3 vr = dominoPieces[i].getVel() + glm::cross(dominoPieces[i].getAngVel(), r);
					glm::vec3 n = glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f));
					float e = 0.4f;
					glm::vec3 jn = (-(1 + e) * vr * n) / (pow(dominoPieces[i].getMass(), -1) + n * glm::cross((dominoPieces[i].getInvInertia() * (glm::cross(r, n))), r));

					// calculate friction
					glm::vec3 vt = vr - glm::dot(vr, n) * n;
					float mu = 0.09f;
					glm::vec3 jFriction;
					if (vt != glm::vec3(0.0f))
					{
						jFriction = -mu * glm::length(jn) * glm::normalize(vt);
					}
					else
					{
						jFriction = glm::vec3(0.0f);
					}

					// DEBUG
					if (collisionsShowed < collisionsToShow)
					{
						std::cout << "Apply j in: " << glm::to_string(averageCollidingPoint.getCoord()) << std::endl;
						std::cout << "jn = " << glm::to_string(jn) << std::endl;
						std::cout << "jFriction = " << glm::to_string(jFriction) << std::endl;
						std::cout << std::endl;
					}
					collisionsShowed++;

					// apply j
					dominoPieces[i].setVel(dominoPieces[i].getVel() + (jn / dominoPieces[i].getMass()));
					dominoPieces[i].setAngVel(dominoPieces[i].getAngVel() + (dominoPieces[i].getInvInertia() * glm::cross(r, jn)));

					// apply friction
					dominoPieces[i].setVel(dominoPieces[i].getVel() + (jFriction / dominoPieces[i].getMass()));
					dominoPieces[i].setAngVel(dominoPieces[i].getAngVel() + (dominoPieces[i].getInvInertia() * glm::cross(r, jFriction)));

					// DEBUG
					collisionsShowed++;
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
		app.draw(plane);
		// draw rigid body
		for (int i = 0; i < dominoPiecesQuantity; i++)
		{
			app.draw(dominoPieces[i].getMesh());
		}
		// draw particles
		for (Particle p : particles)
		{
			//app.draw(p.getMesh());
		}

		app.display();
	}

	app.terminate();

	return EXIT_SUCCESS;
}

