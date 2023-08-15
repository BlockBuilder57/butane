// Created by block on 8/14/23.

#include <SDL2/SDL.h>
#include <core/gl/GLHeaders.hpp>
#include <core/gl/Shader.hpp>
#include <core/Logger.hpp>
#include <core/StdoutSink.hpp>
#include <core/Types.hpp>


int main(int argc, char** argv) {
	engine::core::LoggerAttachStdout();

	if(SDL_Init(SDL_INIT_EVERYTHING) > 0) {
		engine::core::LogFatal("brugh");
		return 1;
	}

	engine::core::LogInfo("no brugh");


	auto window = SDL_CreateWindow("engine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_OPENGL);
	auto glc = SDL_GL_CreateContext(window);

	// open gl contxxt and of ?
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
	gladLoadGLLoader(SDL_GL_GetProcAddress);


	int maj;
	int min;

	engine::core::LogInfo("Vendor   : {}", reinterpret_cast<const char*>(glGetString(GL_VENDOR)));
	engine::core::LogInfo("Renderer : {}", reinterpret_cast<const char*>(glGetString(GL_RENDERER)));
	engine::core::LogInfo("Version  : {}", reinterpret_cast<const char*>(glGetString(GL_VERSION)));
	engine::core::LogInfo("GLSL     : {}", reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION)));

	SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &maj);
	SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &min);
	engine::core::LogInfo("Context  : {}.{}", maj, min);

	glGetIntegerv(GL_MAJOR_VERSION, &maj);
	glGetIntegerv(GL_MINOR_VERSION, &min);
	engine::core::LogInfo("OpenGL   : {}.{}", maj, min);

	bool run = true;

	glClearColor(0.f, 0.f, 0.f, 1.f);
	glViewport(0, 0, 800, 600);

	// init stuff

	float vertices[] = {
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		0.0f,  0.5f, 0.0f
	};

	const char *vertexShaderSource = R"(
	#version 330 core
	layout (location = 0) in vec3 aPos;

	uniform vec2 time;

	void main() {
   		gl_Position = vec4(aPos.x, aPos.y + (sin(time.x) * 0.2f) + cos((aPos.x+0.5)+time.x)*0.2f, aPos.z, 1.0);
	})";

	const char* fragmentShaderSource = R"(
	#version 330 core
	out vec4 FragColor;

	uniform vec2 time;

	void main() {
		FragColor = vec4(mod(time.x, 1.0f), 0.5f, 0.2f, 1.0f);
	})";

	// compile errors
	char infoLog[512];


	engine::core::gl::Shader vertexShader(engine::core::gl::Shader::Kind::Vertex);
	engine::core::gl::Shader fragmentShader(engine::core::gl::Shader::Kind::Fragment);
	vertexShader.SetSource(vertexShaderSource);
	fragmentShader.SetSource(fragmentShaderSource);

	if(!vertexShader.Compile()) {
		glGetShaderInfoLog(vertexShader.Get(), 512, nullptr, infoLog);
		engine::core::LogInfo("Vertex shader compilation failure: {}", infoLog);
	}

	if(!fragmentShader.Compile()) {
		glGetShaderInfoLog(fragmentShader.Get(), 512, nullptr, infoLog);
		engine::core::LogInfo("fragment shader compilation failure: {}", infoLog);
	}

	engine::core::gl::Program program;
	program.AttachShader(vertexShader);
	program.AttachShader(fragmentShader);
	program.Link();

	program.Bind();

	u32 VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	//
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
	glEnableVertexAttribArray(0);

	constexpr static float UpdateRate = 1. / 60.;

	float deltaTime = 0.f;
	float lastTime = SDL_GetTicks64() / 1000.f;
	float nowTime;

	while(run) {
		SDL_Event ev;

		// run update loop
		lastTime = nowTime;

		// run SDL event loop
		while(SDL_PollEvent(&ev) > 0) {
			switch(ev.type) {
				case SDL_EventType::SDL_QUIT:
					run = false;
					break;

				default:
					break;
			}
		}

		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

		nowTime = SDL_GetTicks64() / 1000.f;

		//deltaTime += (nowTime - lastTime) / UpdateRate;

		engine::core::LogInfo("delta time: {}", 1.f/(nowTime - lastTime));

		program.SetUniform("time", nowTime, std::chrono::system_clock::now().time_since_epoch().count());

		// do actual drawing now
		program.Bind();
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		SDL_GL_SwapWindow(window);
	}

	SDL_GL_DeleteContext(glc);
	SDL_DestroyWindow(window);
	SDL_Quit();
    return 0;
}
