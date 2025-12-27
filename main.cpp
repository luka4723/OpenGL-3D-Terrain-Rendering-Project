#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
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

int main()
{
    if (windowinit(SCR_WIDTH, SCR_HEIGHT, "Moj OpenGL Prozor") != 0) return -1;

	Shader tessHeightMapShader("Glsls/height.vs.glsl","Glsls/height.fs.glsl", nullptr, "Glsls/height.tcs.glsl", "Glsls/height.tes.glsl");

    Texture heightmap = Texture("Resources/render.png",GL_TEXTURE_2D,GL_TEXTURE0,GL_UNSIGNED_BYTE,GL_REPEAT,1);
	int width = heightmap.getWidth();
	int height = heightmap.getHeight();

	std::vector<float> terrain_vertices;

	unsigned rez = 20;
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

	VAO terrainVAO;
	VBO terrainVBO(terrain_vertices.data(), terrain_vertices.size() * sizeof(float));
    terrainVAO.Bind();
	VAO::LinkAttrib(terrainVBO,0,3,GL_FLOAT,5*sizeof(float),nullptr);
	VAO::LinkAttrib(terrainVBO,1,2,GL_FLOAT,5*sizeof(float),reinterpret_cast<void *>(3 * sizeof(float)));

	glPatchParameteri(GL_PATCH_VERTICES, NUM_PATCH_PTS);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		tessHeightMapShader.Activate();

		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100000.0f);
		glm::mat4 view = camera.GetViewMatrix();
		tessHeightMapShader.setMat4("projection", projection);
		tessHeightMapShader.setMat4("view", view);

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::scale(model,glm::vec3(0.05,0.05,0.05));
		tessHeightMapShader.setMat4("model", model);

		terrainVAO.Bind();
		heightmap.Bind();
		glDrawArrays(GL_PATCHES, 0, NUM_PATCH_PTS*rez*rez);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	terrainVAO.Delete();
	terrainVBO.Delete();

	glfwTerminate();
	return 0;
}

