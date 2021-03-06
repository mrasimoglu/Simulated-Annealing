#include "pch.h"
#include <iostream>

#include <vector>
#include <algorithm>
#include <time.h>

#include <Windows.h>

#include "ogl/GLEW/glew.h"
#include "ogl/GLFW/glfw3.h"

#define numOfLocations 50
#define mapWidth 1000
#define mapHeight 1000

struct Vector2 {
	float x, y;

	Vector2(float x, float y) : x(x), y(y) {}
};

struct Color {
	float r, g, b;

	Color(float r, float g, float b) : r(r), g(g), b(b) {}
};

void Init();
void SimulatedAnnealing();
void Render();

Vector2 *GenerateRandomLocations(unsigned int numberOfCities, int width, int height);
unsigned int *GenerateRandomPath(unsigned int numberOfCities);
unsigned int *GenerateCanditadePath(unsigned int numberOfCities, unsigned int *currentPath, float probability);

void DrawCities();
void DrawPaths();

void DrawPoint(Vector2 pos, Color col);
void DrawLines(Vector2 pos1, Vector2 pos2, Color col);

Vector2 WorldToOpenGL(Vector2 pos);
float CalculateTotalDistance(Vector2 *locations, unsigned int *path);
void CopyPath(unsigned int *dest, unsigned int *source);

Vector2 *locations;
unsigned int *path;
float totalDistance;
float temprature = 50.f;

Vector2 viewport = Vector2(640, 640);

int main()
{
	srand(time(NULL));

	GLFWwindow* window;
	if (!glfwInit())
		return -1;

	Init();

	window = glfwCreateWindow(viewport.x, viewport.y, "Simulated Annealing", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT);
		glClearColor(0.3, 0.3, 0.3, 1);
		glViewport(0, 0, viewport.x, viewport.y);

		SimulatedAnnealing();
		Render();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}

void Init()
{
	locations = GenerateRandomLocations(numOfLocations, mapWidth, mapHeight);
	path = GenerateRandomPath(numOfLocations);

	totalDistance = CalculateTotalDistance(locations, path);
}

void SimulatedAnnealing()
{
	unsigned int *candidatePath = GenerateCanditadePath(numOfLocations, path, 0.5f);
	float candidateTotalDistance = CalculateTotalDistance(locations, candidatePath);

	if (totalDistance < candidateTotalDistance)
	{
		float p = std::exp(-(CalculateTotalDistance(locations, candidatePath) - CalculateTotalDistance(locations, path)) / temprature);
		float r = (rand() % 1000) / 1000;

		if (r < p)
		{
			temprature = (temprature - (CalculateTotalDistance(locations, candidatePath) - CalculateTotalDistance(locations, path))) / std::log(r);
			//c++;
			CopyPath(path, candidatePath);
			std::cout << "switch\n";
		}
	}
	else
	{
		CopyPath(path, candidatePath);
	}

	totalDistance = CalculateTotalDistance(locations, path);

	std::cout << "Total Distance : " << totalDistance << std::endl;
}

void CopyPath(unsigned int *dest, unsigned int *source)
{
	for (int i = 0; i < numOfLocations; i++)
		dest[i] = source[i];
}

float CalculateTotalDistance(Vector2 *locations, unsigned int *path)
{
	float dist = 0.f;
	for (int i = 1; i < numOfLocations; i++)
	{
		dist += std::sqrt(std::pow((double)(locations[path[i]].x - locations[path[i - 1]].x), 2) + std::pow((double)(locations[path[i]].y - locations[path[i - 1]].y), 2));
	}

	return dist;
}

void Render()
{
	DrawCities();
	DrawPaths();
}

void DrawCities()
{
	glPointSize(5.f);
	glBegin(GL_POINTS);
	for (int i = 0; i < numOfLocations; i++)
		DrawPoint(locations[i], Color(0, 0, 1));
	glEnd();
}

void DrawPaths()
{
	glBegin(GL_LINES);
	for (int i = 1; i < numOfLocations; i++)
		DrawLines(locations[path[i]], locations[path[i - 1]], Color(0, 0, 0));
	glEnd();
}

void DrawLines(Vector2 pos1, Vector2 pos2, Color col)
{
	glColor3f(col.r, col.g, col.b);


	Vector2 GLpos = WorldToOpenGL(pos1);
	glVertex2f(GLpos.x, GLpos.y);
	
	GLpos = WorldToOpenGL(pos2);
	glVertex2f(GLpos.x, GLpos.y);
}

void DrawPoint(Vector2 pos, Color col)
{
	glColor3f(col.r, col.g, col.b);

	Vector2 GLpos = WorldToOpenGL(pos);
	glVertex2f(GLpos.x, GLpos.y);
}

Vector2 WorldToOpenGL(Vector2 pos)
{
	float w = 2.f / mapWidth;
	float h = 2.f / mapHeight;
	
	float x = ((-2.f + ((pos.x) * w + w / 2) * 2.f) / 2) * 0.75f;
	float y = ((2.f - ((pos.y) * h + h / 2) * 2.f) / 2) * 0.75f;
	
	return Vector2(x, y);
}

Vector2 *GenerateRandomLocations(unsigned int numberOfCities, int width, int height)
{
	Vector2 *locations = (Vector2*)malloc(sizeof(Vector2) * numberOfCities);

	for (int i = 0; i < numberOfCities; i++)
	{
		int x = rand() % width;
		int y = rand() % height;

		locations[i].x = x;
		locations[i].y = y;
	}

	return locations;
}

unsigned int *GenerateRandomPath(unsigned int numberOfCities)
{
	unsigned int *path = (unsigned int*)malloc(sizeof(unsigned int) * numberOfCities);

	for (int i = 0; i < numberOfCities; i++)
		path[i] = i;

	for (int i = 0; i < numberOfCities; i++)
		std::swap(path[i], path[rand() % numberOfCities]);

	return path;
}

unsigned int *GenerateCanditadePath(unsigned int numberOfCities, unsigned int *currentPath, float probability)
{
	unsigned int *path = (unsigned int*)malloc(sizeof(unsigned int) * numberOfCities);

	for (int i = 0; i < numberOfCities; i++)
		path[i] = currentPath[i];

	for (int i = 0; i < numberOfCities; i++)
		if(rand() % 100 < probability)
			std::swap(path[i], path[rand() % numberOfCities]);

	return path;
}