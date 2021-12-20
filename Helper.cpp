#pragma once
#include <string>
#include "xEngine.h"
Helper* Helper::instance = NULL;
Helper::Helper()
{
	instance = this;
	lastFrameTime = (GLfloat)glfwGetTime();
}
void Helper::SetMainCamera(Camera* cam)
{
	instance->mainCamera = cam;
}
Camera* Helper::GetMainCamera()
{
	return instance->mainCamera;
}
GLfloat Helper::DeltaTime()
{
	return instance->deltaTime;
}
void Helper::SetNewTime()
{
	instance->deltaTime = (GLfloat)glfwGetTime() - instance->lastFrameTime;
	instance->lastFrameTime = (GLfloat)glfwGetTime();
}
GLfloat Helper::Time()
{
	return (GLfloat)glfwGetTime();
}
void Helper::UpdateParticularLight(int lightId)
{
	if (lightId >= instance->lights.size())
		return;

	std::string s;
	char buf[10];
	for (int i = 0; i < instance->shaders.size(); i++)
	{
		Shader& shader = *(instance->shaders[i]);
		s.clear();
		s += "pointLights[";
		_itoa_s(lightId, buf, 10);
		s += string(buf);
		s += "].position";
		shader.SetVector3(s.c_str(), instance->lights[lightId]->GetGameObject()->transform->GetPosition());
		//cout << "Send position" << endl;
		s.clear();
		s += "pointLights[";
		_itoa_s(lightId, buf, 10);
		s += string(buf);
		s += "].color";
		shader.SetVector3(s.c_str(), instance->lights[lightId]->color);
		//cout << "Send color" << endl;
		s.clear();
		s += "pointLights[";
		_itoa_s(lightId, buf, 10);
		s += string(buf);
		s += "].radius";
		shader.SetFloat(s.c_str(), instance->lights[lightId]->radius);
		//cout << "Send radius" << endl;
	}
}