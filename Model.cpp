#pragma once
#include "xEngine.h"

Mesh::Mesh(vector<Vertex> vertices, vector<unsigned int> indices, unsigned textureId)
{
    this->vertices = vertices;
    this->indices = indices;
    this->textureId = textureId;
}
void Mesh::setupMesh()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenBuffers(1, &IBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
        &indices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    // vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    // vertex texture coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

    glBindBuffer(GL_ARRAY_BUFFER, IBO);

    GLsizei vec4Size = sizeof(glm::vec4);
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)0);
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)16);
    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)32);
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)48);

    glVertexAttribDivisor(3, 1);
    glVertexAttribDivisor(4, 1);
    glVertexAttribDivisor(5, 1);
    glVertexAttribDivisor(6, 1);

    glBindVertexArray(0);
}
void Mesh::Draw(Shader* shader)
{
    glBindTexture(GL_TEXTURE_2D, Graphic::GetTexture(textureId));
    shader->Use();
    glBindVertexArray(VAO);
    glDrawElementsInstanced(
        GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0, meshMatrices.size()
    );
    glBindVertexArray(0);
}
void Model::loadModel(string path)
{
    Assimp::Importer import;
    const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        cout << "ERROR::ASSIMP::" << import.GetErrorString() << endl;
        return;
    }
    auto lastSlashPos = path.find_last_of('/');
    if (lastSlashPos != 18446744073709551615)
        directory = path.substr(0, path.find_last_of('/') + 1);

    processNode(scene->mRootNode, scene);
}
void Model::processNode(aiNode* node, const aiScene* scene)
{
    string name = node->mName.C_Str();
    if (name.find("collider") < name.length())
        return;
    // обработать все полигональные сетки в узле(если есть)
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        processMesh(mesh, scene);
    }
    // выполнить ту же обработку и дл€ каждого потомка узла
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }
}
void Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
    // data to fill
    vector<Vertex> vertices;
    vector<unsigned int> indices;

    // walk through each of the mesh's vertices
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;
        glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
        // positions
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.Position = vector;
        // normals
        if (mesh->HasNormals())
        {
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.Normal = vector;
        }
        // texture coordinates
        if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
        {
            glm::vec2 vec;
            // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
            // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.TexCoords = vec;
        }
        else
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);

        vertices.push_back(vertex);
    }
    // now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        // retrieve all indices of the face and store them in the indices vector
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }
    // process materials
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
    // return a mesh object created from the extracted mesh data
    auto meshId = Graphic::AddMesh(Mesh(vertices, indices, loadMaterialTexture(material, aiTextureType_DIFFUSE)));
    auto instanceId = Graphic::AddInstance(meshId, host->transform->GetModelMat());
    sharedMeshes.push_back(meshId);
    instanceIds.push_back(instanceId);
}
unsigned Model::loadMaterialTexture(aiMaterial* mat, aiTextureType type)
{
    if (mat->GetTextureCount(type) == 0)
        return 0;

    aiString str;
    mat->GetTexture(type, 0, &str);

    string filename = string(str.C_Str());
    filename = directory + filename.substr(filename.find_last_of('/') + 1, filename.length() - filename.find_last_of('/') - 1);

    //cout << "Requested texture: " << filename << endl;

    //GLuint texture;
    //glGenTextures(1, &texture);
    //glBindTexture(GL_TEXTURE_2D, texture);
    //// ”станавливаем настройки фильтрации и преобразований (на текущей текстуре)
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //// «агружаем и генерируем текстуру
    //int width, height;
    //unsigned char* image = SOIL_load_image(filename.c_str(), &width, &height, 0, SOIL_LOAD_RGB);
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    //glGenerateMipmap(GL_TEXTURE_2D);
    //SOIL_free_image_data(image);
    //glBindTexture(GL_TEXTURE_2D, 0);

    return Graphic::AddTexture(filename);
}
void Model::AddModel(GameObject* source, const char* path)
{
    if (source->model)
        return;

    source->model = new Model(path, source);
}
void Model::RemoveMe()
{
    for (int i = 0; i < sharedMeshes.size(); i++)
        Graphic::RemoveInstance(sharedMeshes[i], instanceIds[i]);
}
void Model::UpdateModelMat(glm::mat4 mat)
{
    for (unsigned i = 0; i < sharedMeshes.size(); i++)
        Graphic::UpdateInstance(sharedMeshes[i], instanceIds[i], mat);
}
bool operator== (const Mesh& m1, const Mesh& m2)
{
    if (m1.vertices.size() != m2.vertices.size())
        return false;
    if (m1.indices.size() != m2.vertices.size())
        return false;
    if (m1.textureId != m2.textureId)
        return false;

    for (int i = 0; i < m1.vertices.size(); i++)
    {
        if (m1.vertices[i].Position != m2.vertices[i].Position)
            return false;
        if (m1.vertices[i].Normal != m2.vertices[i].Normal)
            return false;
        if (m1.vertices[i].TexCoords != m2.vertices[i].TexCoords)
            return false;
    }

    for (int i = 0; i < m1.indices.size(); i++)
        if (m1.indices[i] != m2.indices[i])
            return false;

    return true;
}