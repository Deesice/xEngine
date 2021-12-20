#pragma once
#include "xEngine.h"
GameObject* Component::GetGameObject()
{
	return gameObject;
}
bool Component::SetGameObject(GameObject* object)
{
	if (gameObject == NULL)
	{
		gameObject = object;
		return true;
	}
	return false;
}