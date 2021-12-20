#pragma once
#include "xEngine.h"
#include "pugixml.hpp"
#include <string>

using namespace pugi;

vec3 ParseVec3(string input)
{
    vec3 output;
    output.x = atof(input.substr(0, input.find_first_of(' ')).c_str());
    input = input.substr(input.find_first_of(' ') + 1, input.length() - input.find_first_of(' ') - 1);
    output.y = atof(input.substr(0, input.find_first_of(' ')).c_str());
    input = input.substr(input.find_first_of(' ') + 1, input.length() - input.find_first_of(' ') - 1);
    output.z = atof(input.c_str());
    return output;
}
vec4 ParseVec4(string input)
{
    vec4 output;
    output.x = atof(input.substr(0, input.find_first_of(' ')).c_str());
    input = input.substr(input.find_first_of(' ') + 1, input.length() - input.find_first_of(' ') - 1);
    output.y = atof(input.substr(0, input.find_first_of(' ')).c_str());
    input = input.substr(input.find_first_of(' ') + 1, input.length() - input.find_first_of(' ') - 1);
    output.z = atof(input.substr(0, input.find_first_of(' ')).c_str());
    input = input.substr(input.find_first_of(' ') + 1, input.length() - input.find_first_of(' ') - 1);
    output.w = atof(input.c_str());
    return output;
}

Level::Level(const char* path)
{
    vector<string> files;
	xml_document doc;
	xml_parse_result result = doc.load_file(path);

	pugi::xml_node staticFiles = doc.first_child().child("MapData").child("MapContents").child("FileIndex_StaticObjects");
    for (pugi::xml_node file = staticFiles.first_child(); file; file = file.next_sibling())
        //for (pugi::xml_attribute attr = file.first_attribute(); attr; attr = attr.next_attribute())
            files.push_back(file.attribute("Path").value());

    pugi::xml_node staticObjects = doc.first_child().child("MapData").child("MapContents").child("StaticObjects");
    for (pugi::xml_node instanceInfo = staticObjects.first_child(); instanceInfo; instanceInfo = instanceInfo.next_sibling())
    {
        unsigned index = atoi(instanceInfo.attribute("FileIndex").value());
        string name = instanceInfo.attribute("Name").value();
        vec3 pos = ParseVec3(instanceInfo.attribute("WorldPos").value());
        vec3 scale = ParseVec3(instanceInfo.attribute("Scale").value());
        vec3 eulers = ParseVec3(instanceInfo.attribute("Rotation").value());
        quat rot = quat(eulers);

        //cout << "Name: " << name << "; Position: (" << pos.x << " " << pos.y << " " << pos.z << "); Scale: )"
        //    << scale.x << " " << scale.y << " " << scale.z << "); Rotation: ("
        //    << rot.x << " " << rot.y << " " << rot.z << " " << rot.w << ");" << endl;

        auto g = new GameObject(name.c_str(), pos, rot, scale);
        Model::AddModel(g, files[index].c_str());
        AddObject(g);
    }

    pugi::xml_node entities = doc.first_child().child("MapData").child("MapContents").child("Entities");
    for (pugi::xml_node entityInfo = entities.first_child(); entityInfo; entityInfo = entityInfo.next_sibling())
    {
        string name = string(entityInfo.name());
        if (name != string("PointLight"))
            continue;

        vec4 color = ParseVec4(entityInfo.attribute("DiffuseColor").value());
        float radius = atof(entityInfo.attribute("Radius").value());
        vec3 pos = ParseVec3(entityInfo.attribute("WorldPos").value());

        auto g = new GameObject("light(clone)", pos);
        Light::CreateLight(g, radius, color.x, color.y, color.z);
        AddObject(g);
    }

    GameObject* camera = new GameObject("Main camera");
    auto camComponent = new Camera;
    camera->AddComponent(camComponent);
    camera->AddBehaviour(new Player);
    Helper::SetMainCamera(camComponent);
    AddObject(camera);
    Graphic::UpdateAllIBO();
}