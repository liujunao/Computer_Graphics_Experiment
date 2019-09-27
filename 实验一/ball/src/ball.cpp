#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include "shader/shader.h"
#include <direct.h>
#include <string>
#include <iostream>
using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// 初始参数
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
const unsigned int Y_SEGMENTS = 100;
const unsigned int X_SEGMENTS = 100;
const float PI = 3.1415926;

template<typename T> T* vec2arr(vector<T>& vec) {
	T* arr = new T[vec.size()];
	if (!vec.empty()) {
		memcpy(arr, &vec[0], vec.size() * sizeof(T));
	}
	return arr;
}

int main()
{
	// glfw: initialize and configure
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

	// glfw window 创建
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// glad: 加载 OpenGL 函数
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// build and compile shader 程序
	Shader ourShader("res/shader/3.3.shader.vs", "res/shader/3.3.shader.fs"); // 自己命名 shader files

	// 生成球的顶点 
	vector<float> sphereVertices;
	for (int y = 0; y <= Y_SEGMENTS; y++) {
		for (int x = 0; x <= X_SEGMENTS; x++) {
			float xSegment = (float)x / (float)X_SEGMENTS;
			float ySegment = (float)y / (float)Y_SEGMENTS;
			float xPos = cos(xSegment * 2.0f * PI) * sin(ySegment * PI);
			float yPos = cos(ySegment * PI);
			float zPos = sin(xSegment * 2.0f * PI) * sin(ySegment * PI);
			sphereVertices.push_back(xPos);
			sphereVertices.push_back(yPos);
			sphereVertices.push_back(zPos);
		}
	}
	float* vertices = vec2arr(sphereVertices);

	//根据球面上每一点的坐标，去构造一个一个三角形顶点数组 
	vector<int> sphereIndices;
	for (int i = 0; i < Y_SEGMENTS; i++) { 
		for (int j = 0; j < X_SEGMENTS; j++) { 
			sphereIndices.push_back(i * (X_SEGMENTS+1) + j); 
			sphereIndices.push_back((i + 1) * (X_SEGMENTS + 1) + j); 
			sphereIndices.push_back((i + 1) * (X_SEGMENTS + 1) + j + 1); 
			sphereIndices.push_back(i * (X_SEGMENTS + 1) + j); 
			sphereIndices.push_back((i + 1) * (X_SEGMENTS + 1) + j + 1); 
			sphereIndices.push_back(i * (X_SEGMENTS + 1) + j + 1); 
		} 
	}
	int* indices = vec2arr(sphereIndices);
	unsigned int VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sphereVertices.size() * sizeof(float), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphereIndices.size() * sizeof(int), indices, GL_STATIC_DRAW);
	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);//使用线框模式绘制
	//开启面剔除(只需要展示一个面，否则会有重合) 
	glEnable(GL_CULL_FACE); 
	glCullFace(GL_BACK);
	// render loop
	while (!glfwWindowShouldClose(window))
	{
		// input 
		processInput(window);
		// render
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		// render the triangle
		ourShader.use();
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, sphereIndices.size(), GL_UNSIGNED_INT, 0);
		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	// optional: de-allocate all resources once they've outlived their purpose:
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	// glfw: terminate, clearing all previously allocated GLFW resources.
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

// glfw: 当 windows 尺寸改变时，该回调函数运行
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}