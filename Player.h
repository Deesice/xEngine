#pragma once
#include "Component.h"
#include "Input.h"
#include "Helper.h"
#include "glm/glm.hpp"

class Player : Component
{
public:
    Player(GameObject* g) : Component(g)
    {
    }
    void Update()
    {
        vec2 input;
        input.y = 0;
        if (Input::GetKey(GLFW_KEY_W))
            input.y = 1;
        else if (Input::GetKey(GLFW_KEY_S))
            input.y = -1;
        input.x = 0;
        if (Input::GetKey(GLFW_KEY_D))
            input.x = 1;
        else if (Input::GetKey(GLFW_KEY_A))
            input.x = -1;

        vec3 euler;
        euler.y = input.x * Helper::DeltaTime();
        euler.x = input.y * Helper::DeltaTime();
        gameObject->transform->Rotate(euler);
    }
};