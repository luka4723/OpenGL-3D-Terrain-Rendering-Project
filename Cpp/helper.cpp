#include "helper.hpp"
#include "camera.hpp"
#include <iostream>
#include <random>
#include <vector>

#include "model.hpp"
#include "shader.hpp"
#include "stb_image.hpp"
#include "texture.hpp"
#include "VAO.hpp"
#include "glm/gtc/quaternion.hpp"

int SCR_WIDTH = 800 , SCR_HEIGHT = 600;
GLFWwindow* window = nullptr;
float scale =0.0f;
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
float deltaTime = 0.0f;
float lastFrame = 0.0f;
Camera camera(glm::vec3(0.0f, 50.0f, 5.0f));
bool xray = false;
float is_shift = 1.0f;
bool is_on_ground = false;
std::vector<glm::mat4> *trees = nullptr;
std::vector<glm::mat4> *grass = nullptr;
std::vector<glm::mat4> *prozori = nullptr;
double explodeTime = 0;
bool explode = false;
bool lastKeys[2] = { false };

int NUM_PATCH_PTS = 4;
Shader *tessHeightMapShader;
Shader *waterShader;
Shader *skyShader;
VAO *skyboxVAO;
VAO *waterVAO;
VAO *terrainVAO;
VAO *grassVAO;
Texture *heightmap;
Texture *gradient;
Texture *water;
Texture *Prozor;
unsigned int cubemapTexture;
int rez;
float dy=-5;
Texture *cobble_diffuse;
Texture *cobble_specular;
Texture *grassTex;
Texture *skullTex;
Texture *lisce;
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

Shader *normalShader;
Shader *geomShader;
Shader *skullShader;
Model *Saloon;
Model *Celavi;
Model *Lobanja;
Model *Drvo;

float front[] = {
	-1.0f,  1.0f, -1.0f,  0.0f, 1.0f,
	-1.0f, -1.0f, -1.0f,  0.0f, 0.0f,
	 1.0f, -1.0f, -1.0f,  1.0f, 0.0f,
	 1.0f, -1.0f, -1.0f,  1.0f, 0.0f,
	 1.0f,  1.0f, -1.0f,  1.0f, 1.0f,
	-1.0f,  1.0f, -1.0f,  0.0f, 1.0f,
};
float back[] = {
	-1.0f, -1.0f,  1.0f,  0.0f, 0.0f,
	-1.0f,  1.0f,  1.0f,  0.0f, 1.0f,
	 1.0f,  1.0f,  1.0f,  1.0f, 1.0f,
	 1.0f,  1.0f,  1.0f,  1.0f, 1.0f,
	 1.0f, -1.0f,  1.0f,  1.0f, 0.0f,
	-1.0f, -1.0f,  1.0f,  0.0f, 0.0f,
};

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        {
            if(scale<1.0f) scale+=0.02f;
        }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        {
            if(scale>0.0f) scale-=0.02f;
        }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    	if (is_on_ground) dy = 12.0f;
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    	is_shift = 1.7f;
	else is_shift = 1.0;
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS && !lastKeys[0]) {
		explodeTime = glfwGetTime();
		explode = true;
	}

	lastKeys[0] = glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

int windowinit(int width, int height, const char* title)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    window = glfwCreateWindow(width, height, title, NULL, NULL);
    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	trees = new std::vector<glm::mat4>;
	grass = new std::vector<glm::mat4>;
	prozori = new std::vector<glm::mat4>;

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        glfwTerminate();
        return -1;
    }

    glEnable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    return 0;
}

float getTerrainHeight(float worldX, float worldZ) {
    if (!heightmap || !heightmap->data) return 0.0f;

    unsigned char* data = heightmap->data;
    int w = heightmap->width;
    int h = heightmap->height;
    int ch = heightmap->nrChannels;

    // 1. Izračunaj dimenzije u svetu nakon skaliranja (0.2)
    float worldWidth = w * 0.2f;
    float worldDepth = h * 0.2f;

    // 2. Pomeri world koordinate da počinju od 0,0 (jer je tvoj teren centriran)
    float terrainX = worldX + (worldWidth / 2.0f);
    float terrainZ = worldZ + (worldDepth / 2.0f);

    // 3. Provera granica
    if (terrainX < 0 || terrainX >= worldWidth || terrainZ < 0 || terrainZ >= worldDepth) return 0.0f;

    // 4. Mapiranje na piksele (OVO JE KLJUČNO)
    float gridX = (terrainX / worldWidth) * (w - 1);

    // Često je potrebno obrnuti Z osu jer slike počinju odozgo (0) ka dole (H)
    // Ako ti visina i dalje nije tačna, probaj: float gridZ = (1.0f - (terrainZ / worldDepth)) * (h - 1);
    float gridZ = (terrainZ / worldDepth) * (h - 1);

    int x0 = (int)gridX;
    int z0 = (int)gridZ;
    float fracX = gridX - x0;
    float fracZ = gridZ - z0;

    auto getSample = [&](int x, int z) {
        x = std::max(0, std::min(x, w - 1));
        z = std::max(0, std::min(z, h - 1));
        int index = (z * w + x) * ch;

        // Čitanje Zelenog kanala (Y) kao u shaderu
        float valY = (ch >= 2) ? (data[index + 1] / 255.0f) : (data[index] / 255.0f);

        // Identificna formula iz shadera
        return (valY * 128.0f - 16.0f);
    };

    // Interpolacija
    float h00 = getSample(x0, z0);
    float h10 = getSample(x0 + 1, z0);
    float h01 = getSample(x0, z0 + 1);
    float h11 = getSample(x0 + 1, z0 + 1);

    float topMix = h00 + (h10 - h00) * fracX;
    float botMix = h01 + (h11 - h01) * fracX;

    float unscaledHeight = topMix + (botMix - topMix) * fracZ;

    // 5. FINALNO SKALIRANJE: Pošto si u drawTerrain uradio model = scale(0.2),
    // i tvoja visina se smanjila za toliko!
    return unscaledHeight * 0.2f;
}

void terrainDataInit() {
     tessHeightMapShader = new Shader("Glsls/height.vs.glsl","Glsls/height.fs.glsl", nullptr, "Glsls/height.tcs.glsl", "Glsls/height.tes.glsl");
	 waterShader =new Shader("Glsls/water-vert.glsl","Glsls/water-frag.glsl", nullptr, nullptr, nullptr);
	 skyShader=new Shader("Glsls/sky-vert.glsl","Glsls/sky-frag.glsl",nullptr,nullptr,nullptr);

	heightmap = new Texture("Resources/render.png",GL_TEXTURE_2D,GL_TEXTURE0,GL_UNSIGNED_BYTE,GL_CLAMP_TO_EDGE,1);
	gradient =  new Texture("Resources/gradient.png",GL_TEXTURE_2D,GL_TEXTURE1,GL_UNSIGNED_BYTE,GL_CLAMP_TO_EDGE,1);
	Prozor =  new Texture("Resources/prozor.png",GL_TEXTURE_2D,GL_TEXTURE2,GL_UNSIGNED_BYTE,GL_CLAMP_TO_EDGE,1);
	skullTex =  new Texture("Resources/Skull.jpg",GL_TEXTURE_2D,GL_TEXTURE3,GL_UNSIGNED_BYTE,GL_CLAMP_TO_EDGE,1);
	lisce = new Texture("Resources/lisce.jpg",GL_TEXTURE_2D,GL_TEXTURE0,GL_UNSIGNED_BYTE,GL_CLAMP_TO_EDGE,1);

	gradient->Bind();
	gradient->texUnit(*tessHeightMapShader,"gradient");
	float width = heightmap->getWidth();
	float height = heightmap->getHeight();

	 water = new Texture("Resources/water-normal2.jpg",GL_TEXTURE_2D,GL_TEXTURE2,GL_UNSIGNED_BYTE,GL_REPEAT,1);
	water->texUnit(*waterShader,"normal_map");

	normalShader = new Shader("Glsls/vShader.glsl","Glsls/fShader.glsl",nullptr,nullptr,nullptr);
	geomShader = new Shader("Glsls/grayVShader.glsl","Glsls/grayFShader.glsl","Glsls/geomShader.glsl",nullptr,nullptr);
	skullShader = new Shader("Glsls/grayVShader.glsl","Glsls/grayFShader.glsl",nullptr,nullptr,nullptr);
	Saloon = new Model("Resources/saloon.obj");
	Celavi  = new Model("Resources/celavi.obj");
	Lobanja = new Model("Resources/12140_Skull_v3_L2.obj");
	Drvo = new Model("Resources/drvo.obj");


	grassTex = new Texture("Resources/grass.png",GL_TEXTURE_2D,GL_TEXTURE0,GL_UNSIGNED_BYTE,GL_CLAMP_TO_EDGE,1);

	float scale = 0.1;
	float waterVerts[] = {
		-width*scale, 3.3f, -height*scale,
		-width*scale, 3.3f, height*scale,
		width*scale, 3.3f, height*scale,

		-width*scale, 3.3f, -height*scale,
		width*scale, 3.3f, -height*scale,
		width*scale, 3.3f, height*scale,
	};
	waterVAO = new VAO();
	VBO waterVBO(waterVerts,sizeof(waterVerts));
	waterVAO->Bind();
	VAO::LinkAttrib(waterVBO,0,3,GL_FLOAT,3*sizeof(float),nullptr);

	float skyboxVertices[] = {
		// positions          // uvs
		-1.0f,  1.0f, -1.0f,  0.0f, 1.0f, //
		-1.0f, -1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f, -1.0f,  1.0f, 0.0f,
		 1.0f, -1.0f, -1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f, -1.0f,  1.0f, 1.0f,
		-1.0f,  1.0f, -1.0f,  0.0f, 1.0f,

		-1.0f, -1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f, -1.0f,  0.0f, 0.0f,
		-1.0f,  1.0f, -1.0f,  1.0f, 0.0f,
		-1.0f,  1.0f, -1.0f,  1.0f, 0.0f,
		-1.0f,  1.0f,  1.0f,  1.0f, 1.0f,
		-1.0f, -1.0f,  1.0f,  0.0f, 1.0f,

		 1.0f, -1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f,  1.0f, 1.0f,
		 1.0f,  1.0f,  1.0f,  1.0f, 1.0f,
		 1.0f,  1.0f, -1.0f,  0.0f, 1.0f,
		 1.0f, -1.0f, -1.0f,  0.0f, 0.0f,

		-1.0f, -1.0f,  1.0f,  0.0f, 0.0f, //
		-1.0f,  1.0f,  1.0f,  0.0f, 1.0f,
		 1.0f,  1.0f,  1.0f,  1.0f, 1.0f,
		 1.0f,  1.0f,  1.0f,  1.0f, 1.0f,
		 1.0f, -1.0f,  1.0f,  1.0f, 0.0f,
		-1.0f, -1.0f,  1.0f,  0.0f, 0.0f,

		-1.0f,  1.0f, -1.0f,  0.0f, 1.0f,
		 1.0f,  1.0f, -1.0f,  1.0f, 1.0f,
		 1.0f,  1.0f,  1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f,  1.0f, 0.0f,
		-1.0f,  1.0f,  1.0f,  0.0f, 0.0f,
		-1.0f,  1.0f, -1.0f,  0.0f, 1.0f,

		-1.0f, -1.0f, -1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f, -1.0f,  1.0f, 1.0f,
		 1.0f, -1.0f, -1.0f,  1.0f, 1.0f,
		-1.0f, -1.0f,  1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f,  1.0f, 0.0f
	};

	skyboxVAO = new VAO();
	VBO skyboxVBO(skyboxVertices,sizeof(skyboxVertices));
	skyboxVAO->Bind();
	VAO::LinkAttrib(skyboxVBO,0,3,GL_FLOAT,5*sizeof(float),nullptr);
	VAO::LinkAttrib(skyboxVBO,1,2,GL_FLOAT,5*sizeof(float),reinterpret_cast<void *>(3 * sizeof(float)));

	glGenTextures(1, &cubemapTexture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	//glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	for (unsigned int i = 0; i < 6; i++)
	{
		std::string facesCubemap[6] =
		{
			"Resources/right.jpg",
			"Resources/left.jpg",
			"Resources/top.jpg",
			"Resources/bottom.jpg",
			"Resources/front.jpg",
			"Resources/back.jpg",

		};
		int width, height, nrChannels;
		unsigned char* data = stbi_load(facesCubemap[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			stbi_set_flip_vertically_on_load(false);
			glTexImage2D
			(
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0,
				GL_RGB,
				width,
				height,
				0,
				GL_RGB,
				GL_UNSIGNED_BYTE,
				data
			);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Failed to load texture: " << facesCubemap[i] << std::endl;
			stbi_image_free(data);
		}
	}

	std::vector<float> terrain_vertices;

	rez = 20;
	for(unsigned i = 0; i <= rez-1; i++)
	{
		for(unsigned j = 0; j <= rez-1; j++)
		{
			terrain_vertices.push_back(-width/2.0f + width*i/(float)rez); // v.x
			terrain_vertices.push_back(0.0f); // v.y
			terrain_vertices.push_back(-height/2.0f + height*j/(float)rez); // v.z
			terrain_vertices.push_back(i / (float)rez); // u
			terrain_vertices.push_back(j / (float)rez); // v

			terrain_vertices.push_back(-width/2.0f + width*(i+1)/(float)rez); // v.x
			terrain_vertices.push_back(0.0f); // v.y
			terrain_vertices.push_back(-height/2.0f + height*j/(float)rez); // v.z
			terrain_vertices.push_back((i+1) / (float)rez); // u
			terrain_vertices.push_back(j / (float)rez); // v

			terrain_vertices.push_back(-width/2.0f + width*i/(float)rez); // v.x
			terrain_vertices.push_back(0.0f); // v.y
			terrain_vertices.push_back(-height/2.0f + height*(j+1)/(float)rez); // v.z
			terrain_vertices.push_back(i / (float)rez); // u
			terrain_vertices.push_back((j+1) / (float)rez); // v

			terrain_vertices.push_back(-width/2.0f + width*(i+1)/(float)rez); // v.x
			terrain_vertices.push_back(0.0f); // v.y
			terrain_vertices.push_back(-height/2.0f + height*(j+1)/(float)rez); // v.z
			terrain_vertices.push_back((i+1) / (float)rez); // u
			terrain_vertices.push_back((j+1) / (float)rez); // v
		}
	}
	std::cout << "Loaded " << rez*rez << " patches of 4 control points each" << std::endl;
	std::cout << "Processing " << rez*rez*4 << " vertices in vertex shader" << std::endl;

	terrainVAO = new VAO();
	VBO terrainVBO(terrain_vertices.data(), terrain_vertices.size() * sizeof(float));
    terrainVAO->Bind();
	VAO::LinkAttrib(terrainVBO,0,3,GL_FLOAT,5*sizeof(float),nullptr);
	VAO::LinkAttrib(terrainVBO,1,2,GL_FLOAT,5*sizeof(float),reinterpret_cast<void *>(3 * sizeof(float)));

	cobble_diffuse = new Texture("Resources/cobble-diffuse.jpg",GL_TEXTURE_2D,GL_TEXTURE0,GL_UNSIGNED_BYTE,GL_REPEAT,1);
	//cobble_specular = new Texture("Resources/cobble-specular.jpg",GL_TEXTURE_2D,GL_TEXTURE1,GL_UNSIGNED_BYTE,GL_REPEAT,1);

	normalShader->Activate();
	cobble_diffuse->texUnit(*normalShader,"texture_diffuse1");
	//cobble_specular->texUnit(*normalShader,"material.specular");

	std::random_device rd;
	std::mt19937 mt(rd());
	std::uniform_real_distribution<float> distributionW(-1.0f*heightmap->getWidth()*0.1f, heightmap->getWidth()*0.1f);
	std::uniform_real_distribution<float> distributionH(-1.0f*heightmap->getHeight()*0.1f, heightmap->getHeight()*0.1f);
	std::uniform_int_distribution<int> distributionA(0,360);
	std::uniform_real_distribution<float> distributionS(0,10);
	for (int i =0;i<500;i++) {
		 float w = distributionW(mt);
		 float h = distributionH(mt);
		if (getTerrainHeight(w,h) > 4.0f) {

			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(w,getTerrainHeight(w,h),h));
			model = glm::scale(model, glm::vec3(0.3f,0.3f,0.3f));
			trees->push_back(model);
		}
	}
	for (int i =0;i<20000;i++) {
		float w = distributionW(mt);
		float h = distributionH(mt);
		if (getTerrainHeight(w,h) > 4.0f) {

			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(w,getTerrainHeight(w,h),h));
			model = glm::rotate(model,glm::radians(static_cast<float>(distributionA(mt))) ,glm::vec3(0.0f,1.0f,0.0f));
			grass->push_back(model);
		}
	}

	for (int i =0;i<200;i++) {
		float w = distributionW(mt);
		float h = distributionH(mt);
		if (getTerrainHeight(w,h) > 4.0f) {

			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(w,getTerrainHeight(w,h),h));
			float s = distributionS(mt);
			model = glm::rotate(model,glm::radians(static_cast<float>(distributionA(mt))) ,glm::vec3(0.0f,1.0f,0.0f));
			model = glm::scale(model, glm::vec3(s,s,0.3f));
			prozori->push_back(model);
		}
	}


	float grassVerts[] = {
		0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
		0.0f, 0.0f,  0.0f,  0.0f,  0.0f,
		1.0f, 0.0f,  0.0f,  1.0f,  0.0f,

		0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
		1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		1.0f,  1.0f,  0.0f,  1.0f,  1.0f
	};

	grassVAO = new VAO();
	VBO grassVBO(grassVerts, sizeof(grassVerts));
	grassVAO->Bind();
	VAO::LinkAttrib(grassVBO,0,3,GL_FLOAT,5*sizeof(float),nullptr);
	VAO::LinkAttrib(grassVBO,1,2,GL_FLOAT,5*sizeof(float),reinterpret_cast<void *>(3 * sizeof(float)));
}

void drawTerrain(double currentFrame) {
    tessHeightMapShader->Activate();
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100000.0f);
    glm::mat4 view = camera.GetViewMatrix();
    tessHeightMapShader->setMat4("projection", projection);
    tessHeightMapShader->setMat4("view", view);
    tessHeightMapShader->setVec3("cameraPos", camera.Position);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::scale(model,glm::vec3(0.2,0.2,0.2));
    tessHeightMapShader->setMat4("model", model);

    glDepthFunc(GL_LEQUAL);
    glDepthMask(GL_FALSE);
    skyShader->Activate();
    view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
    skyShader->setMat4("view", view);
    skyShader->setMat4("projection", projection);
    skyboxVAO->Bind();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);

    view = camera.GetViewMatrix();

    tessHeightMapShader->Activate();
    terrainVAO->Bind();
    heightmap->Bind();
	gradient->Bind();

    glDrawArrays(GL_PATCHES, 0, NUM_PATCH_PTS*rez*rez);


	normalShader->Activate();
	skyboxVAO->Bind();
	cobble_diffuse->Bind();
	cobble_diffuse->texUnit(*normalShader,"texture_diffuse1");
	model = glm::mat4(1.0f);
    model = glm::translate(model,glm::vec3(-40,-3,20));
    model = glm::scale(model,glm::vec3(20.0,10.0,20.0));
    normalShader->setMat4("model", model);
	normalShader->setMat4("view", view);
	normalShader->setMat4("projection", projection);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// skullShader->Activate();
	// skullShader->setMat4("projection", projection);
	// skullShader->setMat4("view", view);
	// for (int i=0; i< trees->size();i++) {
	// 	skullShader->setMat4("model", trees->at(i));
	// 	Drvo->Draw(*skullShader);
	// }

	lisce->Bind();
	lisce->texUnit(*normalShader,"texture_diffuse1");
	for (int i=0; i< trees->size();i++) {
		normalShader->setMat4("model", trees->at(i));
		Drvo->Draw(*normalShader);
	}

	normalShader->Activate();
	grassTex->Bind();
	grassTex->texUnit(*normalShader,"texture_diffuse1");
	grassVAO->Bind();
	normalShader->setMat4("projection", projection);
	normalShader->setMat4("view", view);
	for (int i=0; i< grass->size();i++) {

		normalShader->setMat4("model", grass->at(i));
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}

    skyboxVAO->Bind();
	projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100000.0f);
	view = camera.GetViewMatrix();
	skullShader->Activate();
	skullShader->setMat4("projection", projection);
	skullShader->setMat4("view", view);

	for (int i=0;i<prozori->size();i++) {
		skullShader->Activate();
		skullShader->setMat4("model", prozori->at(i));
		glDrawArrays(GL_TRIANGLES, 6, 6); // crta samo okvir, staklo mora na kraju svega
		glDrawArrays(GL_TRIANGLES, 12, 6);
		glDrawArrays(GL_TRIANGLES, 24, 6);
		glDrawArrays(GL_TRIANGLES, 30, 6);

		// normalShader->Activate();
		// Prozor->Bind();
		// Prozor->texUnit(*normalShader,"tex");
		// normalShader->setMat4("projection", projection);
		// normalShader->setMat4("view", view);
		// normalShader->setMat4("model", prozori->at(i));
		// glEnable(GL_BLEND);
		// glDrawArrays(GL_TRIANGLES, 0, 6);
		// glDrawArrays(GL_TRIANGLES, 18, 6);
		// glDisable(GL_BLEND);
	}



}

void drawWater(double currentFrame) {
	glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100000.0f);
	glm::mat4 view = camera.GetViewMatrix();
	waterShader->Activate();
	waterShader->Activate();
	waterShader->setMat4("uni_VP", projection * view);
	waterShader->setMat4("uni_M", glm::mat4(1.0f));
	waterShader->setVec3("uni_camera_pos", camera.Position);
	waterShader->setFloat("uni_phase", currentFrame*0.1f);

	waterVAO->Bind();
	water->Bind();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glDisable(GL_BLEND);
}

void drawModels(double currentFrame) {

		normalShader->Activate();
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
		glm::mat4 view = camera.GetViewMatrix();
		normalShader->setMat4("projection", projection);
		normalShader->setMat4("view", view);

		glEnable(GL_STENCIL_TEST);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilMask(0xFF);
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-40,12,20));
		model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
		normalShader->setMat4("model", model);
		cobble_diffuse->Bind();
		cobble_diffuse->texUnit(*normalShader,"texture_diffuse1");
		Saloon->Draw(*normalShader);
		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		glStencilMask(0x00);
		skullShader->Activate();
		skullShader->setMat4("projection", projection);
		skullShader->setMat4("view", view);
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-40,12,20));
		model = glm::scale(model, glm::vec3(4.2f, 4.2f, 4.2f));
		skullShader->setMat4("model", model);
		Saloon->Draw(*skullShader);
		glStencilMask(0xFF);
		glStencilFunc(GL_ALWAYS, 0, 0xFF);
		glDisable(GL_STENCIL_TEST);

		geomShader->Activate();
		if (explode) {
			geomShader->setFloat("time", glm::radians(-90.0f)+currentFrame-explodeTime);
		}
		else geomShader->setFloat("time", glm::radians(-90.0f));
		geomShader->setMat4("projection", projection);
		geomShader->setMat4("view", view);
		model = glm::translate(model, glm::vec3(5,0,0));
		model = glm::scale(model, glm::vec3(0.007f, 0.007f, 0.007f));
		model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		geomShader->setMat4("model", model);
		Celavi->Draw(*geomShader);

		normalShader->Activate();
		// skullTex->Bind();
		// skullTex->texUnit(*normalShader,"texture_diffuse1");
		normalShader->setMat4("projection", projection);
		normalShader->setMat4("view", view);
		std::vector<glm::vec3> skullPos = {
			glm::vec3(-450.0f, 80.0f, -350.0f),
			glm::vec3(420.0f, 90.0f, -320.0f),
			glm::vec3(-420.0f, 70.0f, 320.0f),
			glm::vec3(450.0f, 85.0f, 310.0f),
			glm::vec3(500.0f, 95.0f, 0.0f),
			glm::vec3(0.0f, 75.0f, -350.0f),
			glm::vec3(-480.0f, 100.0f, 150.0f)
		};

		for (int i = 0; i < skullPos.size(); i++) {
			model = glm::mat4(1.0f);
			model = glm::translate(model, skullPos.at(i));

			glm::vec3 modelViewDir = glm::normalize( glm::vec3(model[3])-camera.Position);
			glm::quat kvaternion = glm::quatLookAt(modelViewDir, glm::vec3(0.0f, 1.0f, 0.0f));
			model *= glm::mat4_cast(kvaternion);
			model = glm::rotate(model, glm::radians(-90.0f),glm::vec3(1.0f,0.0f,0.0f));
			model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
			normalShader->setMat4("model", model);
			Lobanja->Draw(*normalShader);
		}
}

void drawWindows() {
	skyboxVAO->Bind();
	glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100000.0f);
	glm::mat4 view = camera.GetViewMatrix();

	std::vector<std::pair<float, int>> prozoriSorted; // sadrzi sortirane pozicije svakog lica
	for (int i = 0; i < prozori->size(); i++) {
		auto windowPos = glm::vec3(prozori->at(i)[3]);
		float distance = glm::length(camera.Position - windowPos);
		prozoriSorted.push_back(std::make_pair(distance, i));
	}

	std::sort(prozoriSorted.begin(), prozoriSorted.end(),
			  [](const std::pair<float, int>& a, const std::pair<float, int>& b) {
				  return a.first > b.first;
			  });

	normalShader->Activate();
	Prozor->Bind();
	skyboxVAO->Bind();
	Prozor->texUnit(*normalShader,"texture_diffuse1");
	normalShader->setMat4("projection", projection);
	normalShader->setMat4("view", view);
	glEnable(GL_BLEND);
	for (auto& pair : prozoriSorted) {
		normalShader->setMat4("model", prozori->at(pair.second));
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glDrawArrays(GL_TRIANGLES, 18, 6);
	}
	glDisable(GL_BLEND);
}