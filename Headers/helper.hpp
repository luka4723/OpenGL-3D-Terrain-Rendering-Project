#ifndef HELPER_H
#define HELPER_H


#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "camera.hpp"
#include "texture.hpp"

extern GLFWwindow* window;
extern float scale;
extern int SCR_WIDTH;
extern int SCR_HEIGHT;
extern float lastX;
extern float lastY;
extern bool firstMouse;
extern float deltaTime;
extern float lastFrame;
extern Camera camera;
extern glm::vec3 lightPos;
extern bool xray;
extern int NUM_PATCH_PTS;
extern Texture *heightmap;
extern float dy;
extern float is_shift;
extern bool is_on_ground;
extern std::vector<glm::mat4> *trees;
extern bool explode;
extern double explodeTime;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
int windowinit(int width, int height, const char* title);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
void drawTerrain(double currentFrame);
void terrainDataInit();
void drawWater(double currentFrame);
void drawModels(double currentFrame);
float getTerrainHeight(float worldX, float worldZ);
void drawWindows();
void handleY(glm::vec3 pos);
bool checkTerrain(glm::vec3& pos, glm::vec3 dir);


#endif
