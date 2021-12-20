#define GLEW_STATIC
#include <glew.h>
#include <glfw3.h>
#include <iostream>
#include "Shader.h"
#include "SOIL.h"
#include "xEngine.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

using namespace std;
GLFWwindow* window;

void CloseApplication()
{
	glfwSetWindowShouldClose(window, GL_TRUE);
}
int main()
{
	//Инициализация GLFW
	glfwInit();
	//Настройка GLFW
	//Задается минимальная требуемая версия OpenGL. 
	//Мажорная 
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	//Минорная
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//Установка профайла для которого создается контекст
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//Выключение возможности изменения размера окна
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	window = glfwCreateWindow(800, 600, "LearnOpenGL", nullptr, nullptr);
	Helper helper;
	Graphic graphic;
	Input inputManager(window);
	Input::Subscribe(CloseApplication, GLFW_KEY_ESCAPE, GLFW_PRESS);
	if (window == nullptr)
	{
		cout << "Failed to create GLFW window" << endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return -1;
	}

	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	auto mainShader = Helper::CreateShader("simple.vert", "simple.frag");

	Level level("00_rainy_hall.map");	

	glEnable(GL_DEPTH_TEST);
	while (!glfwWindowShouldClose(window))
	{
		Helper::SetNewTime();
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glfwPollEvents();

		level.Update();

		mainShader->Use();
		glm::mat4 view = glm::lookAt(Helper::GetMainCamera()->GetGameObject()->transform->GetPosition(),
			Helper::GetMainCamera()->GetGameObject()->transform->GetPosition()
			+ Helper::GetMainCamera()->GetGameObject()->transform->Forward(),
			vec3(0, 1, 0));
		//view = glm::mat4(1);
		glm::mat4 projection = glm::perspective(Helper::GetMainCamera()->fov,
			Helper::GetMainCamera()->aspect,
			Helper::GetMainCamera()->nearPlane,
			Helper::GetMainCamera()->farPlane);

		GLint viewLoc = glGetUniformLocation(mainShader->Program, "view");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		GLint projectionLoc = glGetUniformLocation(mainShader->Program, "projection");
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

		Graphic::DrawAll(mainShader);
		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}