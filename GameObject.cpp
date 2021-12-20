#pragma once
#include "xEngine.h"
	GameObject::GameObject(const char* s, vec3 pos, quat rot, vec3 scale)
	{
		components = new Component * [10];
		componentCount = 0;
		behaviours = new Behaviour * [10];
		behaviourCount = 0;
		Name = new char[256];
		int i = 0;
		while (s[i] != 0)
		{
			Name[i] = s[i];
			i++;
		}
		Name[i] = 0;
		transform = new Transform;
		transform->SetGameObject(this);
		transform->SetPosition(pos);
		transform->SetRotation(rot);
		transform->SetScale(scale);
	}
	void GameObject::UpdateAll()
	{
		for (int i = 0; i < behaviourCount; i++)
			behaviours[i]->Update();
	}
	GameObject::~GameObject()
	{
		delete transform;
		delete[] Name;
		if (light)
			Helper::RemoveLight(light);
		if (model)
		{
			if (Graphic::InstanceExist())
				model->RemoveMe();
			delete model;
		}

		for (int i = 0; i < componentCount; i++)
			delete components[i];
		delete[] components;
		delete[] behaviours;
	}
	template<class T> T* GameObject::GetBehaviour()
	{
		static_assert(std::is_base_of<Behaviour, T>::value, "type parameter of this class must derive from Component");
		T* behaviour = NULL;
		for (int i = 0; i < behaviourCount; i++)
		{
			behaviour = dynamic_cast<T>(behaviours[i]);
			if (behaviour)
				return behaviour;
		}
		return behaviour;
	}
	void GameObject::AddBehaviour(Behaviour* behaviour)
	{
		if (!behaviour)
			return;

		AddComponent(behaviour);
		behaviours[behaviourCount] = behaviour;
		behaviourCount++;
	}
	template<class T> T* GameObject::GetComponent()
	{
		static_assert(std::is_base_of<Component, T>::value, "type parameter of this class must derive from Component");
		T* component = NULL;
		for (int i = 0; i < componentCount; i++)
		{
			component = dynamic_cast<T>(components[i]);
			if (component)
				return component;
		}
		return component;
	}
	void GameObject::AddComponent(Component* component)
	{
		if (!component)
			return;

		component->SetGameObject(this);
		components[componentCount] = component;
		componentCount++;
	}