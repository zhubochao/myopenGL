#include<GL/glew.h>
#include <GLFW/glfw3.h>
#include<iostream>
#include<fstream>
#include<string>
#include<sstream>
#include<Windows.h>


#include"Renderer.h"
#include"VertexBuffer.h"
#include"IndexBuffer.h"
#include"VertexArray.h"


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

int main(void)
{
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(800, 680, "Hello World", NULL, NULL);
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
		-0.5f, -0.5f,
		 0.5f, -0.5f,
		 0.5f,  0.5f,
		-0.5f,  0.5f,
		};
		unsigned int indices[] = {
			0, 1, 2,
			2, 3, 0
		};

		//unsigned int vao;
		//GLCall(glGenVertexArrays(1, &vao));
		//GLCall(glBindVertexArray(vao));

		VertexArray va;
		VertexBuffer vb(positions, 4 * 2 * sizeof(float));
		//va.AddBuffer(vb);

		VertexBufferLayout layout;
		layout.Push<float>(2);
		va.AddBuffer(vb, layout);


		IndexBuffer ib(indices, 6);

		//unsigned int ibo;
		//glGenBuffers(1, &ibo);
		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		//glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW);

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


		//double a = 0.0, b = 0.5, c = -0.5;

		float r = 0.0f;
		float increment = 0.05f;
		/* Loop until the user closes the window */
		while (!glfwWindowShouldClose(window))
		{
			/* Render here */
			glClear(GL_COLOR_BUFFER_BIT);


			GLCall(glUseProgram(shader));
			GLCall(glUniform4f(location, r, 0.3f, 0.8f, 1.0f));

			//GLCall(glBindVertexArray(vao));
			va.Bind();
			ib.Bind();
			
			

			GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));
			if (r > 1.0f)
				increment = -0.05f;
			if (r < 0.0f)
				increment = 0.05f;
			r += increment;
			//GLClearError();
			

			//ASSERT(GLLogCall());
			//glDrawArrays(GL_TRIANGLES, 0, 6);

			/* Swap front and back buffers */
			glfwSwapBuffers(window);
			//Sleep(1000);
			/* Poll for and process events */
			glfwPollEvents();
		}

		glDeleteProgram(shader);
	}
	glfwTerminate();
	return 0;
}