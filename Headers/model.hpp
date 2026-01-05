#ifndef MODEL_HPP
#define MODEL_HPP

#include <string>
#include <vector>


#include "Assimp/scene.h"
#include "Assimp/Importer.hpp"
#include "Assimp/postprocess.h"

#include "mesh.hpp"
#include "Shader.hpp"

// helper funkcija (deklaracija)
unsigned int TextureFromFile(
    const char* path,
    const std::string& directory,
    bool gamma = false
);

class Model {
public:
    // data
    std::vector<TextureM> textures_loaded;
    std::vector<Mesh>    meshes;
    std::string          directory;
    bool                 gammaCorrection;

    // ctor
    Model(const std::string& path, bool gamma = false);

    // draw
    void Draw(Shader& shader);

private:
    // internal loading
    void loadModel(const std::string& path);
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);

    std::vector<TextureM> loadMaterialTextures(
        aiMaterial* mat,
        aiTextureType type,
        const std::string& typeName
    );
};

#endif
