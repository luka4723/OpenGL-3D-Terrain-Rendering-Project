#include <iostream>
#include <cstring>

#include "model.hpp"

#include "stb_image.hpp"

Model::Model(const std::string& path, bool gamma)
    : gammaCorrection(gamma)
{
    loadModel(path);
}

void Model::Draw(Shader& shader)
{
    for (auto& mesh : meshes)
        mesh.Draw(shader);
}

void Model::loadModel(const std::string& path)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(
        path,
        aiProcess_Triangulate |
        aiProcess_GenSmoothNormals |
        //aiProcess_FlipUVs |
        aiProcess_CalcTangentSpace
    );

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cerr << "ERROR::ASSIMP:: "
                  << importer.GetErrorString() << std::endl;
        return;
    }

    directory = path.substr(0, path.find_last_of('/'));
    processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene)
{
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++)
        processNode(node->mChildren[i], scene);
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<TextureM> textures;

    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex{};
        glm::vec3 v;

        // position
        v.x = mesh->mVertices[i].x;
        v.y = mesh->mVertices[i].y;
        v.z = mesh->mVertices[i].z;
        vertex.Position = v;

        // normal
        if (mesh->HasNormals())
        {
            v.x = mesh->mNormals[i].x;
            v.y = mesh->mNormals[i].y;
            v.z = mesh->mNormals[i].z;
            vertex.Normal = v;
        }

        // texcoords / tangent / bitangent
        if (mesh->mTextureCoords[0])
        {
            vertex.TexCoords = {
                mesh->mTextureCoords[0][i].x,
                mesh->mTextureCoords[0][i].y
            };

            v.x = mesh->mTangents[i].x;
            v.y = mesh->mTangents[i].y;
            v.z = mesh->mTangents[i].z;
            vertex.Tangent = v;

            v.x = mesh->mBitangents[i].x;
            v.y = mesh->mBitangents[i].y;
            v.z = mesh->mBitangents[i].z;
            vertex.Bitangent = v;
        }
        else
        {
            vertex.TexCoords = glm::vec2(0.0f);
        }

        vertices.push_back(vertex);
    }

    // indices
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    // materials
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

    auto diffuseMaps =
        loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

    auto specularMaps =
        loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

    auto normalMaps =
        loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
    textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

    auto heightMaps =
        loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
    textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

    return Mesh(vertices, indices, textures);
}

std::vector<TextureM> Model::loadMaterialTextures(
    aiMaterial* mat,
    aiTextureType type,
    const std::string& typeName
)
{
    std::vector<TextureM> textures;

    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString str;
        mat->GetTexture(type, i, &str);

        bool skip = false;
        for (auto& loaded : textures_loaded)
        {
            if (std::strcmp(loaded.path.c_str(), str.C_Str()) == 0)
            {
                textures.push_back(loaded);
                skip = true;
                break;
            }
        }

        if (!skip)
        {
            TextureM texture;
            texture.id   = TextureFromFile(str.C_Str(), directory);
            texture.type = typeName;
            texture.path = str.C_Str();

            textures.push_back(texture);
            textures_loaded.push_back(texture);
        }
    }

    return textures;
}

// ==================== Texture helper ====================

unsigned int TextureFromFile(
    const char* path,
    const std::string& directory,
    bool /*gamma*/
)
{
    std::string filename = directory + '/' + path;

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;

    unsigned char* data =
        stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);

    if (data)
    {
        GLenum format = GL_RGB;
        if (nrComponents == 1) format = GL_RED;
        else if (nrComponents == 4) format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(
            GL_TEXTURE_2D, 0, format,
            width, height, 0,
            format, GL_UNSIGNED_BYTE, data
        );
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cerr << "Texture failed to load: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}
