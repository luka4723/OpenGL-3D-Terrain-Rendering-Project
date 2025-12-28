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
	Shader waterShader("Glsls/water-vert.glsl","Glsls/water-frag.glsl", nullptr, nullptr, nullptr);
	Shader skyShader("Glsls/sky-vert.glsl","Glsls/sky-frag.glsl",nullptr,nullptr,nullptr);

    Texture heightmap = Texture("Resources/render.png",GL_TEXTURE_2D,GL_TEXTURE0,GL_UNSIGNED_BYTE,GL_CLAMP_TO_EDGE,1);
	Texture gradient = Texture("Resources/gradient.png",GL_TEXTURE_2D,GL_TEXTURE1,GL_UNSIGNED_BYTE,GL_CLAMP_TO_EDGE,1);
	gradient.Bind();
	gradient.texUnit(tessHeightMapShader,"gradient");
	float width = heightmap.getWidth();
	float height = heightmap.getHeight();

	Texture water = Texture("Resources/water-normal2.jpg",GL_TEXTURE_2D,GL_TEXTURE2,GL_UNSIGNED_BYTE,GL_REPEAT,1);
	water.texUnit(waterShader,"normal_map");
	float skyboxVertices[] = {
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};
	float scale = 0.1;
	float waterVerts[] = {
		-width*scale, 3.3f, -height*scale,
		-width*scale, 3.3f, height*scale,
		width*scale, 3.3f, height*scale,

		-width*scale, 3.3f, -height*scale,
		width*scale, 3.3f, -height*scale,
		width*scale, 3.3f, height*scale,
	};
	VAO waterVAO;
	VBO waterVBO(waterVerts,sizeof(waterVerts));
	waterVAO.Bind();
	VAO::LinkAttrib(waterVBO,0,3,GL_FLOAT,3*sizeof(float),nullptr);

	VAO skyboxVAO;
	VBO skyboxVBO(skyboxVertices,sizeof(skyboxVertices));
	skyboxVAO.Bind();
	VAO::LinkAttrib(skyboxVBO,0,3,GL_FLOAT,3*sizeof(float),nullptr);

	std::string facesCubemap[6] =
	{
		"Resources/right.jpg",
		"Resources/left.jpg",
		"Resources/top.jpg",
		"Resources/bottom.jpg",
		"Resources/front.jpg",
		"Resources/back.jpg",

	};

	unsigned int cubemapTexture;
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
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

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
		tessHeightMapShader.setVec3("cameraPos", camera.Position);

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::scale(model,glm::vec3(0.2,0.2,0.2));
		tessHeightMapShader.setMat4("model", model);

		glDepthFunc(GL_LEQUAL);
		glDepthMask(GL_FALSE);
		skyShader.Activate();
		view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
		skyShader.setMat4("view", view);
		skyShader.setMat4("projection", projection);
		skyboxVAO.Bind();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthMask(GL_TRUE);
		glDepthFunc(GL_LESS);

		 view = camera.GetViewMatrix();

		tessHeightMapShader.Activate();
		terrainVAO.Bind();
		heightmap.Bind();

		glDrawArrays(GL_PATCHES, 0, NUM_PATCH_PTS*rez*rez);

		waterShader.Activate();
		waterShader.Activate();
		waterShader.setMat4("uni_VP", projection * view);
		waterShader.setMat4("uni_M", glm::mat4(1.0f));
		waterShader.setVec3("uni_camera_pos", camera.Position);
		waterShader.setFloat("uni_phase", currentFrame*0.1f);

		waterVAO.Bind();
		water.Bind();
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glDisable(GL_BLEND);



		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	terrainVAO.Delete();
	terrainVBO.Delete();

	glfwTerminate();
	return 0;
}

