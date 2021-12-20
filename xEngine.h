#pragma once
#include <iostream>
#include "Input.h"
#include "Shader.h"
#include <vector>
#include <string>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

using namespace glm;
class GameObject;
template<typename T> struct Tuple
{
	std::string key;
	T value;
	Tuple(std::string s, T i)
	{
		key = s;
		value = i;
	}
};
class Component
{
	protected:
		GameObject* gameObject = NULL;
	public:
		GameObject* GetGameObject();
		bool SetGameObject(GameObject* object);
};
class Behaviour : public Component
{
public:
	virtual void Update() = 0;
};
class Transform : public Component
{
	quat rotation = quat(vec3(0));
	vec3 position = vec3(0);
	vec3 scale = vec3(1);
public:
	glm::mat4 GetModelMat()
	{
		glm::mat4 trans = glm::mat4(1);
		trans = glm::translate(trans, position);
		trans = trans * mat4_cast(rotation);
		trans = glm::scale(trans, scale);
		return trans;
	}
	void SetRotation(quat newRotation);
	void SetPosition(vec3 newPosition);
	void SetScale(vec3 newScale);
	vec3 Forward();
	vec3 Right();
	vec3 Up();
	vec3 EulerAngles();
	void Rotate(vec3 euler);
	quat GetRotation() {
		return rotation;
	}
	vec3 GetPosition()
	{
		return position;
	}
	vec3 GetScale()
	{
		return scale;
	}
};
struct Camera : public Component
{
	float fov = 45;
	float aspect = 1;
	float nearPlane = 0.1f;
	float farPlane = 1000;
};
struct Light : public Component
{
	int Id;
	float radius;
	vec3 color;
	static void CreateLight(GameObject* lightSource, float radius, float R, float G, float B);
	void RemoveMe();
};
class Helper
{
	static Helper* instance;
	GLfloat deltaTime = 0;
	Camera* mainCamera = NULL;
	GLfloat lastFrameTime;
	std::vector<Light*> lights;
	std::vector<Shader*> shaders;
	GLfloat globalIntencity = 100;
	void UpdateAllLights()
	{
		UpdateGlobalIntensity();
		UpdateLightCount();
		for (int i = 0; i < instance->lights.size(); i++)
			UpdateParticularLight(i);
	}
	void UpdateLightCount()
	{
		for (int i = 0; i < instance->shaders.size(); i++)
		{
			Shader& shader = *(instance->shaders[i]);
			shader.SetInt("lightCount", instance->lights.size());
		}
	}
	void UpdateGlobalIntensity()
	{
		for (int i = 0; i < instance->shaders.size(); i++)
		{
			Shader& shader = *(instance->shaders[i]);
			shader.SetFloat("globalLightIntensity", instance->globalIntencity);
		}
	}
public:
	Helper();
	static void SetMainCamera(Camera* cam);
	static Camera* GetMainCamera();
	static GLfloat DeltaTime();
	static void SetNewTime();
	static GLfloat Time();
	static void AddLight(Light* newLight)
	{
		newLight->Id = instance->lights.size();
		instance->lights.push_back(newLight);
		instance->UpdateLightCount();
		UpdateParticularLight(instance->lights.size() - 1);
	}
	static void UpdateParticularLight(int lightId);
	static void RemoveLight(Light* light)
	{
		if (!instance)
		{
			delete light;
			return;
		}
		auto old = instance->lights;
		instance->lights.clear();
		for (int i = 0; i < old.size(); i++)
			if (old[i] != light)
			{
				old[i]->Id = instance->lights.size();
				instance->lights.push_back(old[i]);
			}
		delete light;
		instance->UpdateAllLights();
	}
	static void SetGlobalIntensity(GLfloat intencity)
	{
		instance->globalIntencity = intencity;
		instance->UpdateGlobalIntensity();
	}
	static Shader* CreateShader(const GLchar* vertexPath, const GLchar* fragmentPath)
	{
		auto shader = new Shader(vertexPath, fragmentPath);
		instance->shaders.push_back(shader);
		instance->UpdateAllLights();
		return shader;
	}
	~Helper()
	{
		for (int i = 0; i < shaders.size(); i++)
			delete shaders[i];
		for (int i = 0; i < lights.size(); i++)
			delete lights[i];
		instance = NULL;
	}
};
class Player : public Behaviour
{
	float speed = 10;
	float sensetivity = 0.01f;
public:
	void Update();
};

using namespace std;

struct Vertex {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
};
struct Mesh
{
	/*  Mesh Data  */
	vector<glm::mat4> meshMatrices;
	vector<Vertex> vertices;
	vector<unsigned int> indices;
	unsigned textureId; //индекс теустуры в Graphic:: буфере
	/*  Functions  */
	Mesh(vector<Vertex> vertices, vector<unsigned int> indices, unsigned textureId);
	void Draw(Shader* shader);
	friend bool operator== (const Mesh& m1, const Mesh& m2);
	void setupMesh();
	unsigned int VAO, VBO, EBO, IBO;
	unsigned int AddInstance(glm::mat4 initMatrix)
	{
		meshMatrices.push_back(initMatrix);
		return meshMatrices.size() - 1;
	}
	void RemoveInstance(unsigned deletedId)
	{
		auto old = meshMatrices;
		meshMatrices.clear();
		for (unsigned i = 0; i < old.size(); i++)
			if (i != deletedId)
				meshMatrices.push_back(old[i]);
		UpdateIBO();
	}
	void UpdateInstance(unsigned id, glm::mat4 newModelMat)
	{
		meshMatrices[id] = newModelMat;
		UpdateIBO();
	}
	void UpdateIBO()
	{
		glBindBuffer(GL_ARRAY_BUFFER, IBO);
		glBufferData(GL_ARRAY_BUFFER, meshMatrices.size() * sizeof(glm::mat4), &meshMatrices[0], GL_STATIC_DRAW);
	}
};
class Model
{
public:
	Model(const char* path, GameObject* host)
	{
		this->host = host;
		loadModel(path);
	}
	vector<unsigned> sharedMeshes; //индексы мешей в Graphic:: буфере
	vector<unsigned> instanceIds;
	void UpdateModelMat(glm::mat4 mat);
	static void AddModel(GameObject* source, const char* path);
	void RemoveMe();
private:
	GameObject* host;
	string directory;
	void loadModel(string path);
	void processNode(aiNode* node, const aiScene* scene);
	void processMesh(aiMesh* mesh, const aiScene* scene);
	unsigned loadMaterialTexture(aiMaterial* mat, aiTextureType type);
};
class GameObject
{
Behaviour** behaviours;
Component** components;
int componentCount;
int behaviourCount;
public:
	Transform* transform;
	Light* light = NULL;
	Model* model = NULL;
	char* Name;
	GameObject(const char* s = "", vec3 pos = vec3(0), quat rot = quat(vec3(0)), vec3 scale = vec3(1));
	void UpdateAll();
	~GameObject();
	template<class T> T* GetBehaviour();
	void AddBehaviour(Behaviour* component);
	template<class T> T* GetComponent();
	void AddComponent(Component* component);
};
class Graphic {
	std::vector<Mesh> meshes;
	std::vector<Tuple<GLuint>> textures;
	static Graphic* instance;
public:
	~Graphic() { instance = NULL; }
	static bool InstanceExist()
	{
		return instance;
	}
	static void DrawAll(Shader* shader)
	{
		for (int i = 0; i < instance->meshes.size(); i++)
			instance->meshes[i].Draw(shader);
	}
	static GLuint GetTexture(unsigned id)
	{
		return instance->textures[id].value;
	}
	Graphic() { instance = this; }
	static unsigned AddInstance(unsigned meshId, glm::mat4 initMat)
	{
		return instance->meshes[meshId].AddInstance(initMat);
	}
	static void RemoveInstance(unsigned meshId, unsigned instanceId)
	{
		instance->meshes[meshId].RemoveInstance(instanceId);
	}
	static void UpdateInstance(unsigned meshId, unsigned instanceId, glm::mat4 modelMat)
	{
		instance->meshes[meshId].UpdateInstance(instanceId, modelMat);
	}
	static unsigned AddMesh(Mesh newMesh)
	{
		unsigned i = 0;
		for (; i < instance->meshes.size(); i++)
			if (instance->meshes[i] == newMesh)
				return i;

		newMesh.setupMesh();

		instance->meshes.push_back(newMesh);

		return i;
	}
	static void UpdateAllIBO()
	{
		for (int i = 0; i < instance->meshes.size(); i++)
			instance->meshes[i].UpdateIBO();
	}
	static unsigned AddTexture(string key) 
	{
		for (unsigned i = 0; i < instance->textures.size(); i++)
			if (instance->textures[i].key == key)
				return i;

		auto path = key.c_str();
		// prepare some variables
		unsigned int offset = 0;
		unsigned int size = 0;
		unsigned char* header;

		unsigned int width;
		unsigned int height;
		unsigned int mipMapCount;

		unsigned int blockSize;
		unsigned int format;

		unsigned int w;
		unsigned int h;

		unsigned char* buffer = 0;

		GLuint tid = 0;

		// open the DDS file for binary reading and get file size
		FILE* f;
		fopen_s(&f, path, "rb");
		if (f == 0)
			return 0;
		fseek(f, 0, SEEK_END);
		long file_size = ftell(f);
		fseek(f, 0, SEEK_SET);

		// allocate new unsigned char space with 4 (file code) + 124 (header size) bytes
		// read in 128 bytes from the file
		header = (unsigned char*)malloc(128);
		fread(header, 1, 128, f);

		// compare the `DDS ` signature
		if (memcmp(header, "DDS ", 4) != 0)
			goto exit;

		// extract height, width, and amount of mipmaps - yes it is stored height then width
		height = (header[12]) | (header[13] << 8) | (header[14] << 16) | (header[15] << 24);
		width = (header[16]) | (header[17] << 8) | (header[18] << 16) | (header[19] << 24);
		mipMapCount = (header[28]) | (header[29] << 8) | (header[30] << 16) | (header[31] << 24);

		// figure out what format to use for what fourCC file type it is
		// block size is about physical chunk storage of compressed data in file (important)
		if (header[84] == 'D') {
			switch (header[87]) {
			case '1': // DXT1
				format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
				blockSize = 8;
				break;
			case '3': // DXT3
				format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
				blockSize = 16;
				break;
			case '5': // DXT5
				format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
				blockSize = 16;
				break;
			case '0': // DX10
				// unsupported, else will error
				// as it adds sizeof(struct DDS_HEADER_DXT10) between pixels
				// so, buffer = malloc((file_size - 128) - sizeof(struct DDS_HEADER_DXT10));
			default: goto exit;
			}
		}
		else // BC4U/BC4S/ATI2/BC55/R8G8_B8G8/G8R8_G8B8/UYVY-packed/YUY2-packed unsupported
			goto exit;

		// allocate new unsigned char space with file_size - (file_code + header_size) magnitude
		// read rest of file
		buffer = (unsigned char*)malloc(file_size - 128);
		if (buffer == 0)
			goto exit;
		fread(buffer, 1, file_size, f);

		// prepare new incomplete texture
		glGenTextures(1, &tid);
		if (tid == 0)
			goto exit;

		// bind the texture
		// make it complete by specifying all needed parameters and ensuring all mipmaps are filled
		glBindTexture(GL_TEXTURE_2D, tid);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, mipMapCount - 1); // opengl likes array length of mipmaps
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // don't forget to enable mipmaping
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		w = width;
		h = height;

		// loop through sending block at a time with the magic formula
		// upload to opengl properly, note the offset transverses the pointer
		// assumes each mipmap is 1/2 the size of the previous mipmap
		for (unsigned int i = 0; i < mipMapCount; i++) {
			if (w == 0 || h == 0) { // discard any odd mipmaps 0x1 0x2 resolutions
				mipMapCount--;
				continue;
			}
			size = ((w + 3) / 4) * ((h + 3) / 4) * blockSize;
			glCompressedTexImage2D(GL_TEXTURE_2D, i, format, w, h, 0, size, buffer + offset);
			offset += size;
			w /= 2;
			h /= 2;
		}
		// discard any odd mipmaps, ensure a complete texture
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, mipMapCount - 1);
		// unbind
		glBindTexture(GL_TEXTURE_2D, 0);

		// easy macro to get out quick and uniform (minus like 15 lines of bulk)
	exit:
		free(buffer);
		free(header);
		fclose(f);
		instance->textures.push_back(Tuple<GLuint>(key, tid));
		return instance->textures.size() - 1;
	}
};
class Level
{
	vector<GameObject*> objects;
public:
	Level(const char* path);
	void AddObject(GameObject* object)
	{
		objects.push_back(object);
	}
	void Update()
	{
		for (unsigned i = 0; i < objects.size(); i++)
			objects[i]->UpdateAll();
	}
	~Level()
	{

		for (unsigned i = 0; i < objects.size(); i++)
			delete objects[i];
	}
};