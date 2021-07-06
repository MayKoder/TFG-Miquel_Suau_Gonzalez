#pragma once

#include"DEResource.h"

typedef unsigned int uint;

class ResourceShader;
class C_Camera;

class DE_Cubemap
{

public:
    DE_Cubemap();
    ~DE_Cubemap();

    //LoadCubeMap();
    void CreateGLData();
    void ClearMemory();

    void DrawAsSkybox(C_Camera* _transform);

    ResourceShader* shaderRes;
    unsigned int textureID;

    //unsigned int vboId;

    uint VBO;
    uint VAO;
    uint EBO;
};

static const float skyboxVertices[] =
{
    -0.5f,  0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
    -0.5f,  0.5f, -0.5f,

    -0.5f, -0.5f,  0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f,  0.5f, -0.5f,
    -0.5f,  0.5f, -0.5f,
    -0.5f,  0.5f,  0.5f,
    -0.5f, -0.5f,  0.5f,

    0.5f, -0.5f, -0.5f,
    0.5f, -0.5f,  0.5f,
    0.5f,  0.5f,  0.5f,
    0.5f,  0.5f,  0.5f,
    0.5f,  0.5f, -0.5f,
    0.5f, -0.5f, -0.5f,

    -0.5f, -0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
    0.5f,  0.5f,  0.5f,
    0.5f,  0.5f,  0.5f,
    0.5f, -0.5f,  0.5f,
    -0.5f, -0.5f,  0.5f,

    -0.5f,  0.5f, -0.5f,
    0.5f,  0.5f, -0.5f,
    0.5f,  0.5f,  0.5f,
    0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f, -0.5f,

    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f,  0.5f,
    0.5f, -0.5f, -0.5f,
    0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f,  0.5f,
    0.5f, -0.5f,  0.5f,
};

//TODO: Works, but with seams, maybe use a gradient cubemap texture? IDK
static const float skyboxIndexVertices[] =
{
    //Vertex                //Color
    -0.5f,  0.5f, -0.5f,    0.223, 0.992, 0.925,
    -0.5f, -0.5f, -0.5f,    0, 0.560, 1,
     0.5f, -0.5f, -0.5f,    0, 0.560, 1,
     0.5f,  0.5f, -0.5f,    0.223, 0.992, 0.925,
    -0.5f,  0.5f, 0.5f,     0.223, 0.992, 0.925,
    -0.5f, -0.5f, 0.5f,     0, 0.560, 1,
     0.5f, -0.5f, 0.5f,     0, 0.560, 1,
     0.5f,  0.5f, 0.5f,     0.223, 0.992, 0.925,
};

static const int skyboxIndices[] =
{
    0, 1, 2, 2, 3, 0,
    5, 1, 0, 0, 4, 5,
    2, 6, 7, 7, 3, 2, 
    5, 4, 7, 7, 6, 5, 
    0, 3, 7, 7, 4, 0, 
    1, 5, 2, 2, 5, 6, 
};