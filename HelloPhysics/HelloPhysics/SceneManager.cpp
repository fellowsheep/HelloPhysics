#include "SceneManager.h"

//static controllers for mouse and keyboard
static bool keys[1024];
static bool resized;
static GLuint width, height;
static bool mouseDown, mouseUp, mouseMove;
static b2Vec2 pw, ps; //ponto em coords mundo e tela 


SceneManager::SceneManager()
{
}

SceneManager::~SceneManager()
{
}

void SceneManager::initialize(GLuint w, GLuint h)
{
	width = w;
	height = h;

	mouseDown = false;
	mouseUp = false;
	
	//Inicializando o gerenciador da F�sica do mundo
	physics = new PhysicsManager;
	b2Vec2 gravity;
	gravity.Set(0.0f, -9.8f);
	physics->initialize(gravity);
	clone = true;

	// GLFW - GLEW - OPENGL general setup -- TODO: config file
	initializeGraphics();

	

}

void SceneManager::initializeGraphics()
{
	// Init GLFW
	glfwInit();

	// Create a GLFWwindow object that we can use for GLFW's functions
	window = glfwCreateWindow(width, height, "Hello Transform", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	// Set the required callback functions
	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, MouseButtonCallback);
	glfwSetCursorPosCallback(window, MouseMotionCallback);

	//Setando a callback de redimensionamento da janela
	glfwSetWindowSizeCallback(window, resize);
	
	// GLAD: carrega todos os ponteiros d fun��es da OpenGL
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;

	}

	// Build and compile our shader program
	addShader("../shaders/simpleRenderer.vs", "../shaders/simpleRenderer.frag");
	shader->Use();
	
	//Valores iniciais
	shader->setInt("rasterCode", 0); //padr�o preencher
	shader->setVec4("fillColor", 1.0, 0.0, 1.0, 1.0);
	shader->setVec4("contourColor", 0.0, 0.0, 0.0, 1.0);
	shader->setVec4("pointColor", 0.0, 1.0, 1.0, 1.0);

	glPointSize(15);
	glLineWidth(5);

	//setup the scene -- LEMBRANDO QUE A DESCRI��O DE UMA CENA PODE VIR DE ARQUIVO(S) DE 
	// CONFIGURA��O
	setupScene();

	resized = true; //para entrar no setup da c�mera na 1a vez

}

void SceneManager::addShader(string vFilename, string fFilename)
{
	shader = new Shader (vFilename.c_str(), fFilename.c_str());
}


void SceneManager::key_callback(GLFWwindow * window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			keys[key] = true;
		else if (action == GLFW_RELEASE)
			keys[key] = false;
	}
}

void SceneManager::resize(GLFWwindow * window, int w, int h)
{
	width = w;
	height = h;
	resized = true;

	// Define the viewport dimensions
	glViewport(0, 0, width, height);
}


void SceneManager::update()
{
	
	if (keys[GLFW_KEY_ESCAPE])
		glfwSetWindowShouldClose(window, GL_TRUE);
	if (keys[GLFW_KEY_SPACE])
	{
		//------------------------------------
		//Cria��o do GameObject 1
		GameObject *obj = new GameObject;
		obj->setShader(shader);
		obj->setVAO(objs[0]->getVAO());

		//Geometria, dimens�es (escala), posi��o (transla��o), orienta��o (rota��o)
		//Escala: largura e altura (a sprite tem 1x1)
		obj->setScale(glm::vec3(1.5, 1.5, 1.0));

		//Se � corpo r�gido, fazer a cria��o pelo Physics Manager
		//Definir: forma (box pra sprite), dimensoes (pegar do obj), 
		//atrito, restitui��o e densidade,
		//static ou dynamic
		b2Body *box = physics->createBox(glm::vec2(0.0, 0.0), obj->getDimensions(), 1.0, 0.5, 0.9, true);

		obj->setPhysics(true);
		obj->setBody(box);
		obj->setVAO(objs[0]->getVAO());

		objs.push_back(obj);

	}


	if (objs[0]->getPosition().y <= -5.0 && clone)
	{
		boxTimer.finish();
		v2 = objs[0]->getBody()->GetLinearVelocity();
		GameObject *obj = new GameObject;
		obj->setShader(shader);
		obj->setAsBox();

		//Geometria, dimens�es (escala), posi��o (transla��o), orienta��o (rota��o)
		//Escala: largura e altura (a sprite tem 1x1)
		obj->setScale(glm::vec3(2.0, 3.0, 1.0));

		//Se � corpo r�gido, fazer a cria��o pelo Physics Manager
		//Definir: forma (box pra sprite), dimensoes (pegar do obj), 
		//atrito, restitui��o e densidade,
		//static ou dynamic
		b2Body* box = physics->createBox(glm::vec2(5.0, 0.0), obj->getDimensions(), 1.0f, 0.5f, 0.5f, true);

		obj->setPhysics(true);
		obj->setBody(box);
		obj->setAsBox();
		
		//calculando o impulso do objeto
		double mass = objs[0]->getBody()->GetMass();
		b2Vec2 impulse = mass * v2 - mass * v1;
		impulse *= -1.0;

		obj->getBody()->ApplyLinearImpulse(impulse, box->GetWorldCenter(), true);

		objs.push_back(obj);

		clone = false;
		//objs[0]->getBody();

	}
	// testa as flags que indicaram eventos e modifica os atributos da cena (matrizes de transforman��o)
	//..
	//..
	//if (keys[GLFW_KEY_UP]) { ... } 
	
	//// No caso aqui, apenas o primeiro objeto vai rotacionar e escalar
	//objs[0]->setTranslation(glm::vec3(-0.5, 0.0, 0.0));
	//objs[0]->setRotation((GLfloat)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f),false);
	//objs[0]->setScale(glm::vec3(0.25, 0.25, 1.0),false); //pra acumular as transforms, n�o pode resetar!

	////E o segundo vai sofrer uma transla��o e escalas diferentes em x e  y
	//objs[1]->setTranslation(glm::vec3(0.5, 0.0, 0.0));
	//objs[1]->setScale(glm::vec3(0.3, 1.5, 1.0), false);

	// Dica: se esses comportamentos forem padr�o como os de cima (n�o mudam com o user), poderiam ser 
	// tamb�m estabelecidos dentro do gameobject. Neste caso, ter�amos q armazenar no gameObject
	// atributos que armazenam os deslocamentos, fatores de escala e rota��o inicial de cada objeto


	//Chama o passo da simula��o F�sica
	if (mouseDown)
	{
		physics->MouseDown(pw);
		mouseDown = false;
	}

	if (mouseUp)
	{
		physics->MouseUp(pw);
		mouseUp = false;
	}

	if (mouseMove)
	{
		physics->MouseMove(pw);
		mouseMove = false;
	}

	physics->update();

	//E aqui testamos se houve mudan�a na c�mera 2D (proje��o ortogr�fica)
	if (resized) //se houve redimensionamento na janela, redefine a projection matrix
	{
		setupCamera2D();
		resized = false;
	}
}

void SceneManager::render()
{
	// Clear the colorbuffer
	glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (int i = 0; i < objs.size(); i++)
	{
		objs[i]->update(); //para atualizar os uniforms do objeto
		objs[i]->draw();
	}

	//for (int i = 0; i < nObjs; i++)
	//{
	//	objs[i]->update(); //para atualizar os uniforms do objeto
	//	objs[i]->draw(); //para fazer a drawcall propriamente
	//}
}

void SceneManager::run()
{
	// Render scene
	shader->Use();

	boxTimer.start();
	v1 = objs[0]->getBody()->GetLinearVelocity();

	//GAME LOOP
	while (!glfwWindowShouldClose(window))
	{
		timer.start();
		
		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();

		//Update method(s)
		update();

		//Render scene
		render();
		
		timer.finish();
		double waitingTime = timer.calcWaitingTime(60, timer.getElapsedTimeMs());
		if (waitingTime)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds((int)waitingTime));
		}
		// Swap the screen buffers
		glfwSwapBuffers(window);
	}
}

void SceneManager::finish()
{
	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();
}


void SceneManager::setupScene()
{

//------------------------------------
	//Cria��o do GameObject 1 - caixa
	GameObject *obj = new GameObject;
	obj->setShader(shader);
	obj->setAsBox();

	//Geometria, dimens�es (escala), posi��o (transla��o), orienta��o (rota��o)
	//Escala: largura e altura (a sprite tem 1x1)
	obj->setScale(glm::vec3(2.0, 3.0, 1.0));

	//Se � corpo r�gido, fazer a cria��o pelo Physics Manager
	//Definir: forma (box pra sprite), dimensoes (pegar do obj), 
	//atrito, restitui��o e densidade,
	//static ou dynamic
	b2Body *box = physics->createBox(glm::vec2(-5.0, 5.0), obj->getDimensions(), 1.0f, 0.5f, 0.5f, true);

	obj->setPhysics(true);
	obj->setBody(box);
	obj->setAsBox();

	objs.push_back(obj);

	//-------------- trigger
	obj = new GameObject;
	obj->setShader(shader);
	obj->setAsBox();

	//Geometria, dimens�es (escala), posi��o (transla��o), orienta��o (rota��o)
	//Escala: largura e altura (a sprite tem 1x1)
	obj->setScale(glm::vec3(2.0, 1.0, 1.0));

	//Se � corpo r�gido, fazer a cria��o pelo Physics Manager
	//Definir: forma (box pra sprite), dimensoes (pegar do obj), 
	//atrito, restitui��o e densidade,
	//static ou dynamic
	box = physics->createBox(glm::vec2(-5.0, -5.0), obj->getDimensions(), 1.0f, 0.5f, 0.5f, false);

	obj->setPhysics(true);
	obj->setBody(box);
	obj->setAsBox();
	obj->getBody()->GetFixtureList()->SetSensor(true);

	objs.push_back(obj);
	

	//------------------------------------------------
	//Cria��o do GameObject 2 - ch�o
	obj = new GameObject;
	obj->setShader(shader);
	obj->setAsBox();

	//Geometria, dimens�es (escala), posi��o (transla��o), orienta��o (rota��o)
	//Escala: largura e altura (a sprite tem 1x1)
	obj->setScale(glm::vec3(20.0, 0.5, 1.0));

	//Se � corpo r�gido, fazer a cria��o pelo Physics Manager
	//Definir: forma (box pra sprite), dimensoes (pegar do obj), 
	//atrito, restitui��o e densidade,
	//static ou dynamic
	box = physics->createBox(glm::vec2(0.0, -9.0), obj->getDimensions(), 1.0, 0.5, 0.5, false);

	obj->setPhysics(true);
	obj->setBody(box);
	obj->setAsBox();

	physics->setGround(box);

	objs.push_back(obj);
	//------------------------------------------------
	//Cria��o do GameObject 3 - parede esquerda
	obj = new GameObject;
	obj->setShader(shader);
	obj->setAsBox();

	//Geometria, dimens�es (escala), posi��o (transla��o), orienta��o (rota��o)
	//Escala: largura e altura (a sprite tem 1x1)
	obj->setScale(glm::vec3(0.5, 20.0, 1.0));

	//Se � corpo r�gido, fazer a cria��o pelo Physics Manager
	//Definir: forma (box pra sprite), dimensoes (pegar do obj), 
	//atrito, restitui��o e densidade,
	//static ou dynamic
	box = physics->createBox(glm::vec2(-9.0, 0.0), obj->getDimensions(), 1.0, 0.5, 0.5, false);

	obj->setPhysics(true);
	obj->setBody(box);
	obj->setAsBox();

	objs.push_back(obj);
	//------------------------------------------------
	//Cria��o do GameObject 4 - parede direita
	obj = new GameObject;
	obj->setShader(shader);
	obj->setAsBox();

	//Geometria, dimens�es (escala), posi��o (transla��o), orienta��o (rota��o)
	//Escala: largura e altura (a sprite tem 1x1)
	obj->setScale(glm::vec3(0.5, 20.0, 1.0));

	//Se � corpo r�gido, fazer a cria��o pelo Physics Manager
	//Definir: forma (box pra sprite), dimensoes (pegar do obj), 
	//atrito, restitui��o e densidade,
	//static ou dynamic
	box = physics->createBox(glm::vec2(9.0, 0.0), obj->getDimensions(), 1.0, 0.5, 0.5, false);

	obj->setPhysics(true);
	obj->setBody(box);
	obj->setAsBox();

	objs.push_back(obj);

	//------------------------------------------------
	//Cria��o do GameObject 2 - ch�o
	obj = new GameObject;
	obj->setShader(shader);
	obj->setAsBox();

	//Geometria, dimens�es (escala), posi��o (transla��o), orienta��o (rota��o)
	//Escala: largura e altura (a sprite tem 1x1)
	obj->setScale(glm::vec3(20.0, 0.5, 1.0));

	//Se � corpo r�gido, fazer a cria��o pelo Physics Manager
	//Definir: forma (box pra sprite), dimensoes (pegar do obj), 
	//atrito, restitui��o e densidade,
	//static ou dynamic
	box = physics->createBox(glm::vec2(0.0, 9.0), obj->getDimensions(), 1.0, 0.5, 0.5, false);

	obj->setPhysics(true);
	obj->setBody(box);
	obj->setAsBox();

	objs.push_back(obj);

	//-----------------------



	//Se tiver textura, inicializar o objeto de Sprite

	//nObjs = 2;
	//objs = new GameObject*[nObjs];

	////Setando (no bra�o) o primeiro objeto
	//objs[0] = new GameObject;
	//objs[0]->setShader(shader);
	//objs[0]->setVAO(VAO);
	//
	////Setando (no bra�o) o segundo objeto
	//objs[1] = new GameObject;
	//objs[1]->setShader(shader);
	//objs[1]->setVAO(VAO);
	

}

void SceneManager::setupCamera2D()
{
	//corrigindo o aspecto
	float ratio;
	float xMin = -10.0, xMax = 10.0, yMin = -10.0, yMax = 10.0, zNear = -1.0, zFar = 1.0;
	if (width >= height)
	{
		ratio = width / (float)height;
		//ratio = 1.0;
		projection = glm::ortho(xMin*ratio, xMax*ratio, yMin, yMax, zNear, zFar);
	}
	else
	{
		ratio = height / (float)width;
		projection = glm::ortho(xMin, xMax, yMin*ratio, yMax*ratio, zNear, zFar);
	}

	// Get their uniform location
	GLint projLoc = glGetUniformLocation(shader->Program, "projection");
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
}

void SceneManager::MouseButtonCallback(GLFWwindow* window, int32 button, int32 action, int32 mods)
{

	double xd, yd;
	glfwGetCursorPos(window, &xd, &yd);
	b2Vec2 ps((float)xd, (float)yd);

	// Use the mouse to move things around.
	if (button == GLFW_MOUSE_BUTTON_1)
	{
		pw = convertScreenToWorld(ps);
		if (action == GLFW_PRESS)
		{
			
			mouseDown = true;
			
		}

		if (action == GLFW_RELEASE)
		{
			mouseUp = true;
		}
	}
	
}

void SceneManager::MouseMotionCallback(GLFWwindow*, double xd, double yd)
{
	ps.x = (float)xd;
	ps.y = (float)yd;

	pw = convertScreenToWorld(ps);
	
	mouseMove = true;

}

b2Vec2 SceneManager::convertScreenToWorld(const b2Vec2& ps)
{
	float w = float(width);
	float h = float(height);
	float u = ps.x / w;
	float v = (h - ps.y) / h;

	float ratio = w / h;
	b2Vec2 extents(ratio * 10.0f, 10.0f); //xMax e yMax
	//extents *= m_zoom;

	b2Vec2 lower = - extents;
	b2Vec2 upper = extents;

	b2Vec2 pw;
	pw.x = (1.0f - u) * lower.x + u * upper.x;
	pw.y = (1.0f - v) * lower.y + v * upper.y;
	return pw;
}