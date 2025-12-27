#ifndef HELPER_H
#define HELPER_H


#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "camera.hpp"

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

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
int windowinit(int width, int height, const char* title);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);



#endif
