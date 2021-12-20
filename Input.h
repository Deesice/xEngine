#pragma once
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/matrix.hpp>
#include <glm/gtc/quaternion.hpp>
#include "glm/glm.hpp"
#include <glew.h>
#include <glfw3.h>
#include <iostream>

//typedef void (*pfn_MyFuncType)(char* str, int len);
typedef void (*Action)();
class Event
{
	unsigned int subscribersCount;
	unsigned int maxSubscribersCount;
	Action* funcArray;
public:
	Event();
	void Subscribe(Action funcPointer);
	void Unsubscribe(Action funcPointer);
	void Invoke();
	~Event();
};
class Input
{
	static Input* instance;
	glm::vec2 mousePos;
	bool pressed[349];
	Event OnPressed[349];
	Event OnReleased[349];
public:
	Input(GLFWwindow* window);
	static void ProcessKeyEvent(GLFWwindow* window, int key, int scancode, int action, int mode);
	static bool GetKey(int key);
	static void Subscribe(Action funcPointer, int key, int action);
	static void Unsubscribe(Action funcPointer, int key, int action);
	static void ProcessMouse(GLFWwindow* window, double xpos, double ypos);
	static glm::vec2 GetCurrentMousePos()
	{
		return instance->mousePos;
	}
};

