#include<GL\glew.h>
#include<GLFW\glfw3.h>
#include<iostream>
#include<fstream>
#include<string>
#include<sstream>
#include<Windows.h>


#include"Renderer.h"
#include"VertexBufferLayout.h"
#include"VertexBuffer.h"
#include"IndexBuffer.h"
#include"VertexArray.h"
#include"Shader.h"
#include"Texture.h"

#include"glm/glm.hpp"
#include"glm/gtc/matrix_transform.hpp"

#include"imgui/imgui.h"
#include"imgui/imgui_impl_glfw.h"
#include"imgui/imgui_impl_opengl3.h"


int main(void)
{
	GLFWwindow* window;
	
	/* Initialize the library */
	if (!glfwInit())
		return -1;
	const char* glsl_version = "#version 130";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(960, 540, "Hello World", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	glfwSwapInterval(1);

	if (glewInit() != GLEW_OK) {
		std::cout << "error " << std::endl;
	}

	std::cout << glGetString(GL_VERSION) << std::endl;
	{

		float positions[] = {
		 -50.0f, -50.0f, 0.0f, 0.0f,	//0  位置 位置 纹理坐标 纹理坐标
		  50.0f, -50.0f, 1.0f, 0.0f,	//1
		  50.0f,  50.0f, 1.0f, 1.0f,	//2
		 -50.0f,  50.0f, 0.0f, 1.0f,	//3
		};
		unsigned int indices[] = {
			0, 1, 2,
			2, 3, 0
		};

		GLCall(glEnable(GL_BLEND));
		GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
		
		VertexArray va;
		VertexBuffer vb(positions, 4 * 4 * sizeof(float));
		VertexBufferLayout layout;
		layout.Push<float>(2);
		layout.Push<float>(2);
		va.AddBuffer(vb, layout);


		IndexBuffer ib(indices, 6);

		glm::mat4 proj = glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 1.0f);
		
		glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
		glm::mat4 rotate = glm::rotate(glm::mat4(1.0f), glm::radians(-10.0f), glm::vec3(0.0, 0.0, 1.0));
		glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(1.5, 1.5,1.5));
		//glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(200, 200, 0));


		//glm::mat4 mvp = proj * view *model;


		Shader shader("res/shaders/basic.shader");
		shader.Bind();
		shader.SetUniform4f("u_Color", 0.8f, 0.3f, 0.8f, 0.5f);
		


		Texture texture("res/texture/teslalogo.png");
		texture.Bind(0);
		shader.SetUniform1i("u_Texture", 0);

		va.Unbind();
		shader.Unbind();
		vb.Unbind();
		ib.Unbind();

		glm::vec3 translation(200, 200, 0);

		bool show_demo_window = true;
		bool show_another_window = false;
		ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);


		float r = 0.0f, g = 0.5f, b = 1.0f;
		float increment = 0.05f;
		Renderer renderer;

		ImGui::CreateContext();
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init(glsl_version);
		ImGui::StyleColorsDark();

		/* Loop until the user closes the window */
		while (!glfwWindowShouldClose(window))
		{
			/* Render here */
			//glClear(GL_COLOR_BUFFER_BIT);
			renderer.Clear();

			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			glm::mat4 model = glm::translate(glm::mat4(1.0f), translation);
			glm::mat4 mvp = proj  * view * model;
			
			shader.Bind();
			shader.SetUniform4f("u_Color", r, g, 0.0f, 1.0f);
			shader.SetUniformMat4f("u_MVP", mvp);

			renderer.Draw(va, ib, shader);


			if (r > 1.0f)
				increment = -0.05f;
			if (r < 0.0f)
				increment = 0.05f;

			r += 0.1f * increment;

			{
				static float f = 0.0f;
				ImGui::SliderFloat("Translation", &translation.x, 0.0f, 960.0f);
				ImGui::SliderFloat("Translation", &translation.y, 0.0f, 540.0f);
				ImGui::SliderFloat("Translation", &translation.z, 0.0f, 540.0f);
				
				ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
				ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

			}

			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

			/* Swap front and back buffers */
			glfwSwapBuffers(window);
			//Sleep(1000);
			/* Poll for and process events */
			glfwPollEvents();
		}

		//glDeleteProgram(shader);
	}
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}



/*

struct ShaderProgramSource {
	std::string VertexSource, FragmentSource;
};
static ShaderProgramSource ParseShader(const std::string& filepath) {
	std::ifstream stream(filepath);
	enum class ShaderType {
		NONE = -1, vertex = 0, fragment = 1

	};
	std::string line;
	std::stringstream ss[2];
	ShaderType type = ShaderType::NONE;
	while (getline(stream, line)) {
		if (line.find("#shader") != std::string::npos) {
			if (line.find("vertex") != std::string::npos) {
				type = ShaderType::vertex;

			}
			else if (line.find("fragment") != std::string::npos) {
				type = ShaderType::fragment;

			}
		}
		else {
				ss[(int)type] << line << '\n';
		}
	}
	return{ ss[0].str(), ss[1].str() };

}

static unsigned int CompileShader(unsigned int type, const std::string& source) {
	unsigned int id = glCreateShader(type);
	const char *src = source.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);
	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE) {
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char *message = (char*)alloca(length * sizeof(char));
		glGetShaderInfoLog(id, length, &length, message);
		std::cout << "Fail to compile "<< (type == GL_VERTEX_SHADER? "vertex":"fragement")  <<"shader! " << std::endl;
		std::cout << message << std::endl;
		glDeleteShader(id);
		return 0;

	}
	return id;
}
static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader) {
	unsigned int program = glCreateProgram();
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glValidateProgram(program);

	glDeleteShader(vs);
	glDeleteShader(fs);

	return program;
}
*/

//unsigned int ibo;
//glGenBuffers(1, &ibo);
//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
//glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW);
/*
ShaderProgramSource source = ParseShader("res/shaders/basic.shader");
std::cout << "Vertex" << std::endl;
std::cout << source.VertexSource << std::endl;
std::cout << "Fragment" << std::endl;
std::cout << source.FragmentSource << std::endl;
unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
GLCall(glUseProgram(shader));



int location = glGetUniformLocation(shader, "u_Color");
ASSERT(location != -1);
GLCall(glUniform4f(location, 0.8f, 0.3f, 0.8f, 1.0f));

GLCall(glUseProgram(0));
GLCall(glBindVertexArray(0));
GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
*/