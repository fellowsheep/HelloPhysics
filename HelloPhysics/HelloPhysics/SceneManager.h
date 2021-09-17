#pragma once

#include "Shader.h"
#include "GameObject.h"
#include "Timer.h"

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include "PhysicsManager.h"

using namespace std;

class SceneManager
{
public:
	SceneManager();
	~SceneManager();
	void initialize(GLuint width, GLuint height);
	void initializeGraphics();

	void addShader(string vFilename, string fFilename);


	static b2Vec2 convertScreenToWorld(const b2Vec2& ps);
	//GLFW callbacks
	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
	static void resize(GLFWwindow* window, int width, int height);
	static void MouseButtonCallback(GLFWwindow* window, int32 button, int32 action, int32 mods);
	static void MouseMotionCallback(GLFWwindow*, double xd, double yd);

	void update();
	void render();
	void run();
	void finish();


	// scene
	void setupScene();
	void setupCamera2D();


private:
	
	//GFLW window
	GLFWwindow *window;

	//our shader program
	Shader *shader;
	
	//2D Camera - Projection matrix
	glm::mat4 projection;

	//Scene object(s)
	GameObject *gameObj;

	//A ideia � poder ter mais de um GameObject 
	vector <GameObject *> objs;

	//Gerenciador da simula��o f�sica
	PhysicsManager *physics;

	bool clone;

	Timer timer;
	Timer boxTimer;
	b2Vec2 v1, v2;
};

