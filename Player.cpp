#pragma once
#include "xEngine.h"
#include <iostream>
void Player::Update()
{
    vec2 keyboardInput(0);

    auto f = speed * Helper::DeltaTime();
    if (Input::GetKey(GLFW_KEY_W))
        keyboardInput.y += 1;
    if (Input::GetKey(GLFW_KEY_S))
        keyboardInput.y -= 1;
    if (Input::GetKey(GLFW_KEY_D))
        keyboardInput.x += 1;
    else if (Input::GetKey(GLFW_KEY_A))
        keyboardInput.x -= 1;

    keyboardInput /= keyboardInput.length();
    keyboardInput *= speed * Helper::DeltaTime();

    auto mousePos = Input::GetCurrentMousePos() * sensetivity;

    //std::cout << "MousePos: " << mousePos.x << " " << mousePos.y << std::endl;

    vec3 lookDir(sinf(mousePos.x) * abs(cosf(mousePos.y)),
        sinf(mousePos.y) * sign(cosf(mousePos.y)),
        -cosf(mousePos.x) * abs(cosf(mousePos.y)));

    //std::cout << "LookDir: " << lookDir.x << " " << lookDir.y << " " << lookDir.z << std::endl;

    auto myTransform = gameObject->transform;
    //auto buf = glm::lookAt(lookDir, vec3(0), vec3(0, 1, 0));
    //myTransform->localRotation = quat(buf);
    myTransform->SetRotation(quatLookAt(lookDir, cross(cross(lookDir, vec3(0, 1, 0)), lookDir)));
    if (keyboardInput.y != 0 || keyboardInput.x != 0)
        myTransform->SetPosition(myTransform->GetPosition() + myTransform->Forward() * keyboardInput.y + myTransform->Right() * keyboardInput.x);
}