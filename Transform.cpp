#pragma once
#include "xEngine.h"
vec3 Transform::Forward()
{
    return vec3(0.0, 0.0, -1.0) * inverse(rotation);
}
vec3 Transform::Right()
{
    return vec3(1.0, 0.0, 0.0) * inverse(rotation);
}
vec3 Transform::Up()
{
    return vec3(0.0, 1.0, 0.0) * inverse(rotation);
}
vec3 Transform::EulerAngles()
{
    auto rot = eulerAngles(rotation);
    return degrees(rot);
}
void Transform::Rotate(vec3 euler)
{
    SetRotation(rotation * quat(euler));
}
void Transform::SetPosition(vec3 newPosition)
{
    position = newPosition;
    if (gameObject->light)
        Helper::UpdateParticularLight(gameObject->light->Id);
    if (gameObject->model)
        gameObject->model->UpdateModelMat(GetModelMat());
}
void Transform::SetScale(vec3 newScale)
{
    scale = newScale;
    if (gameObject->model)
        gameObject->model->UpdateModelMat(GetModelMat());
}
void Transform::SetRotation(quat newRotation)
{
    rotation = newRotation;
    if (gameObject->model)
        gameObject->model->UpdateModelMat(GetModelMat());
}