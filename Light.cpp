#pragma once
#include "xEngine.h"

void Light::CreateLight(GameObject* lightSource, float radius, float R, float G, float B)
{
	bool alreadyExist = false;
	if (lightSource->light)
	{
		alreadyExist = true;
	}
	else
	{
		lightSource->light = new Light;
		lightSource->light->SetGameObject(lightSource);
	}
	lightSource->light->radius = radius;
	lightSource->light->color = vec3(R, G, B);
	if (!alreadyExist)
		Helper::AddLight(lightSource->light);
}
void Light::RemoveMe()
{
	Helper::RemoveLight(this);
}