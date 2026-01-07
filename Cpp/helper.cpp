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

int SCR_WIDTH = 1024 , SCR_HEIGHT = 768;
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
Texture *lisce;
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

Shader *normalShader= nullptr;
Shader *geomShader= nullptr;
Shader *skullShader= nullptr;
Model *Saloon= nullptr;
Model *Celavi= nullptr;
Model *Lobanja= nullptr;
Model *Drvo= nullptr;
Model *Heli= nullptr;
Model *Bomba = nullptr;

glm::mat4 modelTess = glm::scale(glm::mat4(1.0f),glm::vec3(0.2,0.2,0.2));
glm::mat4 modelCobble = glm::translate(glm::mat4(1.0f),glm::vec3(-40,-3,20));
glm::mat4 modelStencil = glm::translate(glm::mat4(1.0f),glm::vec3(-37,7,20));
glm::mat4 modelBomba = glm::translate(glm::mat4(1.0f), glm::vec3(25,4,0));

std::vector<std::vector<float>> visine;
int w,h;

bool checkTerrain(glm::vec3 pos) {

	printf("GGGG %f, %f, %f\n",pos.x,pos.y,pos.z);
	if (pos.x > -60.5 && pos.x < -19.5 && pos.z < 40.5 && pos.z > -0.5 && pos.y < 8) {

		return false;
	}
	return true;
}
void handleY(glm::vec3 pos) {
	if (pos.y < 8 && !checkTerrain(pos)) {
		dy=0;
		pos.y = 8;
	}
	else camera.Position.y += dy*deltaTime;
}
void processInput(GLFWwindow *window) {
	float velocity = SPEED * deltaTime * is_shift;

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		glm::vec3 tempPos = camera.Position + camera.Front * velocity;
		if (checkTerrain(tempPos)) camera.Position = tempPos; //camera.ProcessKeyboard(FORWARD, deltaTime);
	}
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
    	glm::vec3 tempPos = camera.Position - camera.Front * velocity;
    	if (checkTerrain(tempPos)) camera.Position = tempPos; //camera.ProcessKeyboard(BACKWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
    	glm::vec3 tempPos = camera.Position - camera.Right * velocity;
    	if (checkTerrain(tempPos)) camera.Position = tempPos; //camera.ProcessKeyboard(LEFT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
    	glm::vec3 tempPos = camera.Position + camera.Right * velocity;
    	if (checkTerrain(tempPos)) camera.Position = tempPos; //camera.ProcessKeyboard(RIGHT, deltaTime);
    }
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
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);



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

void precomputeHeightGrid() {
	if (!heightmap || !heightmap->data) return;

	w = heightmap->width;
	h = heightmap->height;
	int ch = heightmap->nrChannels;
	unsigned char* data = heightmap->data;

	visine.resize(w, std::vector<float>(h));

	for (int z = 0; z < h; z++) {
		for (int x = 0; x < w; x++) {
			int index = (z * w + x) * ch;
			float valY = (ch >= 2) ? (data[index + 1] / 255.0f) : (data[index] / 255.0f); // ako slika nije grayscale uzima druug vrednost (y)
			visine[x][z] = (valY * 128.0f - 16.0f) * 0.2f;
		}
	}
}

float getTerrainHeight(float worldX, float worldZ) {

    float worldWidth = w * 0.2f;
    float worldDepth = h * 0.2f;

    float terrainX = worldX + (worldWidth / 2.0f); // mora da pocinje od 0
    float terrainZ = worldZ + (worldDepth / 2.0f);

    float gridX = (terrainX / worldWidth) * (w - 1);
    float gridZ = (terrainZ / worldDepth) * (h - 1);

    int x0 = (int)gridX;
    int z0 = (int)gridZ;
    float fracX = gridX - x0;
    float fracZ = gridZ - z0;

	float h00 = visine[x0][z0];
	float h10 = visine[x0 + 1][z0];
	float h01 = visine[x0][z0 + 1];
	float h11 = visine[x0 + 1][z0 + 1];

    float topMix = h00 + (h10 - h00) * fracX;
    float botMix = h01 + (h11 - h01) * fracX;

    return topMix + (botMix - topMix) * fracZ;
}

void terrainDataInit() {
     tessHeightMapShader = new Shader("Glsls/height.vs.glsl","Glsls/height.fs.glsl", nullptr, "Glsls/height.tcs.glsl", "Glsls/height.tes.glsl");
	 waterShader =new Shader("Glsls/water-vert.glsl","Glsls/water-frag.glsl", nullptr, nullptr, nullptr);
	 skyShader=new Shader("Glsls/sky-vert.glsl","Glsls/sky-frag.glsl",nullptr,nullptr,nullptr);
	normalShader = new Shader("Glsls/vShader.glsl","Glsls/fShader.glsl",nullptr,nullptr,nullptr);
	geomShader = new Shader("Glsls/geomVShader.glsl","Glsls/fShader.glsl","Glsls/geomShader.glsl",nullptr,nullptr);
	skullShader = new Shader("Glsls/grayVShader.glsl","Glsls/grayFShader.glsl",nullptr,nullptr,nullptr);

	heightmap = new Texture("Resources/render.png",GL_TEXTURE_2D,GL_TEXTURE0,GL_UNSIGNED_BYTE,GL_CLAMP_TO_EDGE,1);
	gradient =  new Texture("Resources/gradient.png",GL_TEXTURE_2D,GL_TEXTURE1,GL_UNSIGNED_BYTE,GL_CLAMP_TO_EDGE,1);
	Prozor =  new Texture("Resources/prozor.png",GL_TEXTURE_2D,GL_TEXTURE0,GL_UNSIGNED_BYTE,GL_CLAMP_TO_EDGE,1);
	lisce = new Texture("Resources/lisce.jpg",GL_TEXTURE_2D,GL_TEXTURE0,GL_UNSIGNED_BYTE,GL_CLAMP_TO_EDGE,1);
	water = new Texture("Resources/water-normal2.jpg",GL_TEXTURE_2D,GL_TEXTURE0,GL_UNSIGNED_BYTE,GL_REPEAT,1);
	grassTex = new Texture("Resources/grass.png",GL_TEXTURE_2D,GL_TEXTURE0,GL_UNSIGNED_BYTE,GL_CLAMP_TO_EDGE,1);
	cobble_diffuse = new Texture("Resources/cobble-diffuse.jpg",GL_TEXTURE_2D,GL_TEXTURE0,GL_UNSIGNED_BYTE,GL_REPEAT,1);
	//cobble_specular = new Texture("Resources/cobble-specular.jpg",GL_TEXTURE_2D,GL_TEXTURE1,GL_UNSIGNED_BYTE,GL_REPEAT,1);

	Saloon = new Model("Resources/kuca/cottage_obj.obj");
	Celavi  = new Model("Resources/celavi.obj");
	Lobanja = new Model("Resources/skull/12140_Skull_v3_L2.obj");
	Drvo = new Model("Resources/tree/drvo.obj");
	Heli  = new Model("Resources/heli/chopper.obj");
	Bomba = new Model("Resources/bomba/Bomb.obj");

	modelCobble = glm::scale(modelCobble,glm::vec3(20.0,10.0,20.0));

	gradient->texUnit(*tessHeightMapShader,"gradient");
	water->texUnit(*waterShader,"normal_map");

	precomputeHeightGrid();

	float width = heightmap->getWidth();
	float height = heightmap->getHeight();

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
		-1.0f,  1.0f, -1.0f,  0.0f, 1.0f,
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

		-1.0f, -1.0f,  1.0f,  0.0f, 0.0f,
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
	VAO::LinkAttrib(skyboxVBO,2,2,GL_FLOAT,5*sizeof(float),reinterpret_cast<void *>(3 * sizeof(float)));

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
			"Resources/skybox/right.jpg",
			"Resources/skybox/left.jpg",
			"Resources/skybox/top.jpg",
			"Resources/skybox/bottom.jpg",
			"Resources/skybox/front.jpg",
			"Resources/skybox/back.jpg",

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
			terrain_vertices.push_back(-width/2.0f + width*i/(float)rez);
			terrain_vertices.push_back(0.0f);
			terrain_vertices.push_back(-height/2.0f + height*j/(float)rez);
			terrain_vertices.push_back(i / (float)rez);
			terrain_vertices.push_back(j / (float)rez);

			terrain_vertices.push_back(-width/2.0f + width*(i+1)/(float)rez);
			terrain_vertices.push_back(0.0f);
			terrain_vertices.push_back(-height/2.0f + height*j/(float)rez);
			terrain_vertices.push_back((i+1) / (float)rez);
			terrain_vertices.push_back(j / (float)rez);

			terrain_vertices.push_back(-width/2.0f + width*i/(float)rez);
			terrain_vertices.push_back(0.0f);
			terrain_vertices.push_back(-height/2.0f + height*(j+1)/(float)rez);
			terrain_vertices.push_back(i / (float)rez);
			terrain_vertices.push_back((j+1) / (float)rez);

			terrain_vertices.push_back(-width/2.0f + width*(i+1)/(float)rez);
			terrain_vertices.push_back(0.0f);
			terrain_vertices.push_back(-height/2.0f + height*(j+1)/(float)rez);
			terrain_vertices.push_back((i+1) / (float)rez);
			terrain_vertices.push_back((j+1) / (float)rez);
		}
	}

	terrainVAO = new VAO();
	VBO terrainVBO(terrain_vertices.data(), terrain_vertices.size() * sizeof(float));
    terrainVAO->Bind();
	VAO::LinkAttrib(terrainVBO,0,3,GL_FLOAT,5*sizeof(float),nullptr);
	VAO::LinkAttrib(terrainVBO,2,2,GL_FLOAT,5*sizeof(float),reinterpret_cast<void *>(3 * sizeof(float)));

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
	VAO::LinkAttrib(grassVBO,2,2,GL_FLOAT,5*sizeof(float),reinterpret_cast<void *>(3 * sizeof(float)));
}

void drawTerrain(double currentFrame) {
	glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100000.0f);

    glDepthFunc(GL_LEQUAL);
    glDepthMask(GL_FALSE);
    skyShader->Activate();
    glm::mat4 view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
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
	tessHeightMapShader->setMat4("projection", projection);
	tessHeightMapShader->setMat4("view", view);
	tessHeightMapShader->setVec3("cameraPos", camera.Position);
	tessHeightMapShader->setMat4("model", modelTess);
    terrainVAO->Bind();
    heightmap->Bind();
	gradient->Bind();
    glDrawArrays(GL_PATCHES, 0, NUM_PATCH_PTS*rez*rez);


	normalShader->Activate();
	skyboxVAO->Bind();
	cobble_diffuse->Bind();
	cobble_diffuse->texUnit(*normalShader,"texture_diffuse1");
    normalShader->setMat4("model", modelCobble);
	normalShader->setMat4("view", view);
	normalShader->setMat4("projection", projection);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	for (int i=0; i< trees->size();i++) {
		normalShader->setMat4("model", trees->at(i));
		Drvo->Draw(*normalShader);
	}

	grassTex->Bind();
	grassTex->texUnit(*normalShader,"texture_diffuse1");
	grassVAO->Bind();
	normalShader->setMat4("projection", projection);
	normalShader->setMat4("view", view);
	for (int i=0; i< grass->size();i++) {
		normalShader->setMat4("model", grass->at(i));
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}

	skullShader->Activate();
	skyboxVAO->Bind();
	skullShader->setMat4("projection", projection);
	skullShader->setMat4("view", view);

	for (int i=0;i<prozori->size();i++) {
		skullShader->setMat4("model", prozori->at(i));
		glDrawArrays(GL_TRIANGLES, 6, 6); // crta samo okvir, staklo mora na kraju svega
		glDrawArrays(GL_TRIANGLES, 12, 6);
		glDrawArrays(GL_TRIANGLES, 24, 6);
		glDrawArrays(GL_TRIANGLES, 30, 6);

	}
}

void drawWater(double currentFrame) {
	glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100000.0f);
	glm::mat4 view = camera.GetViewMatrix();
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
	normalShader->setMat4("model", modelStencil);
	Saloon->Draw(*normalShader);
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilMask(0x00);
	skullShader->Activate();
	skullShader->setMat4("projection", projection);
	skullShader->setMat4("view", view);
	skullShader->setMat4("model", glm::scale(modelStencil, glm::vec3(1.03f)));
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
	geomShader->setMat4("model", modelBomba);
	Bomba->Draw(*geomShader);

	normalShader->Activate();

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
	glm::mat4 model;
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

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(cos(currentFrame*0.15)*150,50,sin(currentFrame*0.15)*150));
	glm::mat4 model3 = glm::rotate(model, glm::radians(180.0f),glm::vec3(0.0f,1.0f,0.0f));
	model3 = glm::rotate(model3, glm::radians(-((float)currentFrame/42)*360),glm::vec3(0.0f,1.0f,0.0f));
	normalShader->setMat4("model", model3);
	Heli->meshes[2].Draw(*normalShader);
	glm::mat4 model2 = glm::rotate(model,glm::radians((float)currentFrame*500),glm::vec3(0.0,1.0,0.0));
	normalShader->setMat4("model", model2);
	Heli->meshes[1].Draw(*normalShader);
}

void drawWindows() {
	skyboxVAO->Bind();
	glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100000.0f);
	glm::mat4 view = camera.GetViewMatrix();

	std::vector<std::pair<float, int>> prozoriSorted; //sadrzi sortirane pozicije svakog lica
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