#pragma once
#include "Input.h"
#include <iostream>

Input* Input::instance = NULL;
	Event::Event()
	{
		subscribersCount = 0;
		maxSubscribersCount = 1;
		funcArray = new Action[1];
		funcArray[0] = NULL;
	}
	void Event::Subscribe(Action funcPointer)
	{
		if (subscribersCount == maxSubscribersCount)
		{
			maxSubscribersCount *= 2;
			auto buf = new Action[maxSubscribersCount];
			int i = 0;
			for (; i < subscribersCount; i++)
				buf[i] = funcArray[i];
			for (; i < maxSubscribersCount; i++)
				buf[i] = NULL;
			delete[] funcArray;
			funcArray = buf;
			Subscribe(funcPointer);
		}
		else
		{
			funcArray[subscribersCount] = funcPointer;
			subscribersCount++;
		}
	}
	void Event::Unsubscribe(Action funcPointer)
	{
		for (int i = 0; i < subscribersCount; i++)
		{
			if (funcArray[i] == funcPointer)
				funcArray[i] = NULL;
		}
	}
	void Event::Invoke()
	{
		for (int i = 0; i < subscribersCount; i++)
			if (funcArray[i] != NULL)
				funcArray[i]();
	}
	Event::~Event()
	{
		delete[] funcArray;
	}
	Input::Input(GLFWwindow* window)
	{
		instance = this;
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		glfwSetKeyCallback(window, ProcessKeyEvent);
		glfwSetCursorPosCallback(window, ProcessMouse);
		glfwSetCursorPos(window, 0, 0);
		mousePos = glm::vec2(0);
		for (int i = 0; i < 349; i++)
			pressed[i] = false;
	}
	void Input::ProcessKeyEvent(GLFWwindow* window, int key, int scancode, int action, int mode)
	{
		switch (action)
		{
		case GLFW_PRESS:
			if (instance->pressed[key] == false)
				instance->OnPressed[key].Invoke();
			instance->pressed[key] = true;
			break;
		case GLFW_RELEASE:
			if (instance->pressed[key] == true)
				instance->OnReleased[key].Invoke();
			instance->pressed[key] = false;
			break;
		default:
			break;
		}
	}
	void Input::ProcessMouse(GLFWwindow* window, double xpos, double ypos)
	{
		instance->mousePos = glm::vec2((float)xpos, (float)-ypos);
	}
	bool Input::GetKey(int key)
	{
		return instance->pressed[key];
	}
	void Input::Subscribe(Action funcPointer, int key, int action)
	{
		switch (action)
		{
		case GLFW_PRESS:
			instance->OnPressed[key].Subscribe(funcPointer);
			break;
		case GLFW_RELEASE:
			instance->OnReleased[key].Subscribe(funcPointer);
			break;
		default:
			throw;
		}
	}
	void Input::Unsubscribe(Action funcPointer, int key, int action)
	{
		switch (action)
		{
		case GLFW_PRESS:
			instance->OnPressed[key].Unsubscribe(funcPointer);
			break;
		case GLFW_RELEASE:
			instance->OnReleased[key].Unsubscribe(funcPointer);
			break;
		default:
			throw;
		}
	}