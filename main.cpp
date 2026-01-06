#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <random>
#include <vector>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "shader.hpp"
#include "VAO.hpp"
#include "VBO.hpp"
#include "EBO.hpp"
#include "helper.hpp"
#include "stb_image.hpp"
#include "texture.hpp"
#include "camera.hpp"
#include "model.hpp"



int main()
{
    if (windowinit(SCR_WIDTH, SCR_HEIGHT, "Moj OpenGL Prozor") != 0) return -1;

	terrainDataInit();

	glPatchParameteri(GL_PATCH_VERTICES, NUM_PATCH_PTS);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		float groundHeight = getTerrainHeight(camera.Position.x, camera.Position.z);

		camera.Position.y += dy*deltaTime;
		//dy=0;
		 if (camera.Position.y<groundHeight+1.0f) {
		 	camera.Position.y = groundHeight+1.0f;
		 	if (camera.Position.y < 4.0f) dy = 0.0f;
		 	is_on_ground = true;
		 }
		 else {
		 	is_on_ground = false;
		 	if (camera.Position.y > 4.0f) dy-=0.5f;
		 }
		//printf("%f\n", dy);
		//printf("%f,%f,%f\n", camera.Position.x, camera.Position.y, camera.Position.z);

		drawTerrain(currentFrame);
		drawModels(currentFrame);

		if (camera.Position.y > 3.3f) {
			drawWater(currentFrame);
			drawWindows();
		}
		else {
			drawWindows();
			drawWater(currentFrame);
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

