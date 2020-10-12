#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include <iostream>

#include <shader.hpp>
#include <rc_spline.h>

struct Orientation {
	// Front
	glm::vec3 Front;
	// Up
	glm::vec3 Up;
	// Right
	glm::vec3 Right;
	// origin
	glm::vec3 origin;
};


class Track
{
public:

	// VAO
	unsigned int VAO;

	// Control Points Loading Class for loading from File
	rc_Spline g_Track;

	// Vector of control points
	std::vector<glm::vec3> controlPoints;

	// Track data
	std::vector<Vertex> vertices;
	// indices for EBO
	std::vector<unsigned int> indices;
	std::vector<glm::vec3> points;

	// hmax for camera
	float hmax = 0.0f;

	//for keeping track of the local orientation of the track
	struct Orientation localOrientation;


	// constructor, just use same VBO as before, 
	Track(const char* trackPath)
	{
		//setting local basis
		localOrientation = { glm::vec3(1, 0, 0), glm::vec3(0, 1, 0), glm::vec3(0, 0, 1), glm::vec3(0, 0, 0) };

		// load Track data
		load_track(trackPath);

		create_track();

		setup_track();
	}

	// render the mesh
	void Draw(Shader shader, unsigned int textureID)
	{
		// Set the shader properties
		shader.use();
		glm::mat4 model;
		model = glm::scale(model, glm::vec3(20.0f, 10.0f, 20.0f));
		model = glm::scale(model, glm::vec3(200.0f, 100.0f, 200.0f));
		shader.setMat4("model", model);


		// Set material properties
		shader.setVec3("material.specular", 0.3f, 0.3f, 0.3f);
		shader.setFloat("material.shininess", 64.0f);


		// active proper texture unit before binding
		glActiveTexture(GL_TEXTURE0);
		// and finally bind the textures
		glBindTexture(GL_TEXTURE_2D, textureID);

		// draw mesh
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, vertices.size());
		glBindVertexArray(0);

		// always good practice to set everything back to defaults once configured.
		glActiveTexture(GL_TEXTURE0);
	}

	// give a positive float s, find the point by interpolation
	// determine pA, pB, pC, pD based on the integer of s
	// determine u based on the decimal of s
	// E.g. s=1.5 is the at the halfway point between the 1st and 2nd control point,
	//		the 4 control points are:[0,1,2,3], with u=0.5
	glm::vec3 get_point(float s)
	{
		int pA, pB, pC, pD;
		float u;

		//mod with the number of control points because the track is a loop,
		//so we need to get back to the front
		pA = glm::floor(s) - 1;
		pB = glm::floor(s) - 0;
		pC = glm::floor(s) + 1;
		pD = glm::floor(s) + 2;

		//could've done above but wouldve required a cast since glm::floor() returns a float
		pA %= controlPoints.size();
		pB %= controlPoints.size();
		pC %= controlPoints.size();
		pD %= controlPoints.size();
		u = s - floor(s);

		return interpolate(controlPoints[pA], controlPoints[pB], controlPoints[pC], controlPoints[pD], 0.5f, u);
	}


	void delete_buffers()
	{
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &EBO);
	}

private:
	

	/*  Render data  */
	unsigned int VBO, EBO;

	void load_track(const char* trackPath)
	{
		// Set folder path for our projects (easier than repeatedly defining it)
		g_Track.folder = "../Project_2/Media/";

		// Load the control points
		g_Track.loadSplineFrom(trackPath);

	}

	// Implement the Catmull-Rom Spline here
	//	Given 4 points, a tau and the u value 
	//	u in range of [0,1]  
	//	Since you can just use linear algebra from glm, just make the vectors and matrices and multiply them.  
	//	This should not be a very complicated function
	glm::vec3 interpolate(glm::vec3 pointA, glm::vec3 pointB, glm::vec3 pointC, glm::vec3 pointD, float tau, float u)
	{
		return glm::vec4(1, u, u * u, u * u * u) *
			glm::mat4x4(
				0,   -tau,   2 * tau,       -tau,
				1,   0,      tau - 3,       2 - tau,
				0,   tau,    3 - 2 * tau,   tau - 2,
				0,   0,      -tau,          tau) *
			glm::mat3x4(
				pointA.x, pointB.x, pointC.x, pointD.x,
				pointA.y, pointB.y, pointC.y, pointD.y,
				pointA.z, pointB.z, pointC.z, pointD.z);
	}

	// Here is the class where you will make the vertices or positions of the necessary objects of the track (calling subfunctions)
	//  For example, to make a basic roller coster:
	//    First, make the vertices for each rail here (and indices for the EBO if you do it that way).  
	//        You need the XYZ world coordinates, the Normal Coordinates, and the texture coordinates.
	//        The normal coordinates are necessary for the lighting to work.  
	//    Second, make vector of transformations for the planks across the rails
	void create_track()
	{
		// Create the vertices and indices (optional) for the rails
		//    One trick in creating these is to move along the spline and 
		//    shift left and right (from the forward direction of the spline) 
		//     to find the 3D coordinates of the rails.



		// Create the plank transformations or just creating the planks vertices
		//   Remember, you have to make planks be on the rails and in the same rotational direction 
		//       (look at the pictures from the project description to give you ideas).  


		//getting all of the control points
		glm::vec3 currentpos = glm::vec3(-2.0f, 0.0f, -2.0f);

		//iterate throught  the points	g_Track.points() returns the vector containing all the control points
		for (pointVectorIter ptsiter = g_Track.points().begin(); ptsiter != g_Track.points().end(); ptsiter++)
		{
			/* get the next point from the iterator */
			glm::vec3 pt(*ptsiter);

			// Print the Box
			std::cout << pt.x << "  " << pt.y << "  " << pt.z << std::endl;


			/* now just the uninteresting code that is no use at all for this project */
			currentpos += pt;
			//  Mutliplying by two and translating (in initialization) just to move the boxes further apart.  
			controlPoints.push_back(currentpos*2.0f);
		}


		//go through all of the control points and make four vertices. Send them to make triangle to push them to the buffer
		for (float s = 0; s < controlPoints.size(); s += .2)
		{
			//interpolated points
			glm::vec3 pointA, pointB, pointC, pointD;

			//make two triangles using the current point, the point .2 forward, and the point to the left a bit
			pointA = get_point(s);
			pointB = get_point(s + .2f);
			pointC = get_point(s + .2f) + (-2.0f * localOrientation.Right);
			pointD = get_point(s)       + (-2.0f * localOrientation.Right);

			//this method might make gaps; like you'll have one square, miss the next one, and so on --- nvm we inc by .2 so the next should start where last left off
			make_triangle(pointA, pointB, pointC, false);
			make_triangle(pointA, pointC, pointD, false);

			//as we go along use the catmull rom spline to get the different vectors for local orientation
			localOrientation.Front = glm::normalize(pointA - pointB);
		}
		
		//print out the list of vertices
		std::cout << "vertices list: \n";
		for (int i = 0; i < vertices.size(); i++)
		{
			printf("(%f, %f, %f)\n", vertices.at(i).Position.x, vertices.at(i).Position.y, vertices.at(i).Position.z);
		}
		std::cout << "vertices list ended.\n";
	}


	// Given 3 Points, create a triangle and push it into vertices (and EBO if you are using one)
		// Optional boolean to flip the normal if you need to
	void make_triangle(glm::vec3 pointA, glm::vec3 pointB, glm::vec3 pointC,bool flipNormal)
	{
		//not doing anything with flip normals just yet
		Vertex v1, v2, v3;
		v1.Position = pointA;
		v2.Position = pointB;
		v3.Position = pointC;
		set_normals(v1, v2, v3);
		v1.TexCoords = glm::vec2(pointA.x, pointA.y);
		v2.TexCoords = glm::vec2(pointB.x, pointB.y);
		v3.TexCoords = glm::vec2(pointC.x, pointC.y);
		vertices.push_back(v1);
		vertices.push_back(v2);
		vertices.push_back(v3);
	}

	// Given two orintations, create the rail between them.  Offset can be useful if you want to call this for more than for multiple rails
	void makeRailPart(Orientation ori_prev, Orientation ori_cur, glm::vec2 offset)
	{

	}

	// Find the normal for each triangle uisng the cross product and then add it to all three vertices of the triangle.  
	//   The normalization of all the triangles happens in the shader which averages all norms of adjacent triangles.   
	//   Order of the triangles matters here since you want to normal facing out of the object.  
	void set_normals(Vertex &p1, Vertex &p2, Vertex &p3)
	{
		glm::vec3 normal = glm::cross(p2.Position - p1.Position, p3.Position - p1.Position);
		p1.Normal += normal;
		p2.Normal += normal;
		p3.Normal += normal;
	}

	//just copied as of now, will update when I have a better idea of how to do the track
	void setup_track()
	{
		// create buffers/arrays
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

		//position coords
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		
		
		// vertex normal coords
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

		// vertex texture coords
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
		
		glBindVertexArray(0);
	}

};
