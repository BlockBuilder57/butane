// Created by block on 8/14/23.

#include <SDL2/SDL.h>
#include <core/gl/GLHeaders.hpp>
#include <core/gl/Shader.hpp>
#include <core/Logger.hpp>
#include <core/StdoutSink.hpp>
#include <core/Types.hpp>

#include <core/sdl/Window.hpp>

#include <core/filesystem/Filesystem.hpp>

namespace core = engine::core;
namespace sdl = core::sdl;
namespace gl = core::gl;

void DumpOglInfo() {
	int maj;
	int min;

	core::LogInfo("OpenGL Information:");
	core::LogInfo("Vendor   : {}", reinterpret_cast<const char*>(glGetString(GL_VENDOR)));
	core::LogInfo("Renderer : {}", reinterpret_cast<const char*>(glGetString(GL_RENDERER)));
	core::LogInfo("Version  : {}", reinterpret_cast<const char*>(glGetString(GL_VERSION)));
	core::LogInfo("GLSL     : {}", reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION)));

	SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &maj);
	SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &min);
	core::LogInfo("Context  : {}.{}", maj, min);

	glGetIntegerv(GL_MAJOR_VERSION, &maj);
	glGetIntegerv(GL_MINOR_VERSION, &min);
	core::LogInfo("OpenGL   : {}.{}", maj, min);
}


int main(int argc, char** argv) {
	static_cast<void>(argc);
	static_cast<void>(argv);

	core::LoggerAttachStdout();

	if(SDL_Init(SDL_INIT_EVERYTHING) > 0) {
		core::LogFatal("Failed to initialize SDL; giving up");
		return 1;
	}

	auto window = sdl::Window{"engine", 800, 600};

	// By this point the Window class has setup OpenGL and made the context it created current,
	// so now we can load OpenGL APIs.
	gladLoadGLLoader(SDL_GL_GetProcAddress);

	DumpOglInfo();

	glClearColor(0.f, 0.f, 0.f, 1.f);
	glViewport(0, 0, 800, 600);

	// init stuff

	float vertices[] = {
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		0.0f,  0.5f, 0.0f
	};

	// vertex/fragment shaders

	auto vsFile = core::filesystem::Filesystem::The().OpenReadFile(core::filesystem::stdfs::path("shaders") / "demo.vert");
	auto fsFile = core::filesystem::Filesystem::The().OpenReadFile(core::filesystem::stdfs::path("shaders") / "demo.frag");

	if(!vsFile || !fsFile) {
		core::LogFatal("could not open files");
		return 1;
	}

	gl::Shader vertexShader(gl::Shader::Kind::Vertex);
	gl::Shader fragmentShader(gl::Shader::Kind::Fragment);
	vertexShader.SetSource(vsFile->ReadString());
	fragmentShader.SetSource(fsFile->ReadString());

	if(!vertexShader.Compile()) {
		core::LogInfo("Vertex shader compilation failure: {}", vertexShader.GetInfoLog());
	}

	if(!fragmentShader.Compile()) {
		core::LogInfo("fragment shader compilation failure: {}", fragmentShader.GetInfoLog());
	}

	gl::ShaderProgram program;
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

	// loop variables

	bool run = true;
	
	// This is essentially how many update ticks we run (when we can)
	// This should be made a configurable value later on
	constexpr static float UpdateRate = 1. / 66.;

	float deltaTime = 0.f;
	float lastTime = SDL_GetTicks64() / 1000.f;
	float nowTime;

	// Assign window event handlers
	window.On(SDL_QUIT, [&](auto& ev) {
		static_cast<void>(ev);
		run = false;
	});

	while(run) {
		// Fixed timestep updates.
		//
		// Note that this loop is not "greedy"; it only executes
		// updates for the times it can, and does not otherwise.
		while(deltaTime >= 1.) {
			core::LogInfo("Update {}", deltaTime);
			deltaTime--;
		}

		nowTime = SDL_GetTicks64() / 1000.f;
		deltaTime += (nowTime - lastTime) / UpdateRate;
		lastTime = nowTime;

		//core::LogInfo("delta time: {}", 1.f/(nowTime - lastTime));

		// do actual drawing now
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		program.SetUniform("time", nowTime, std::chrono::system_clock::now().time_since_epoch().count());
		program.Bind();
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		window.Swap();

		// Run the SDL window event loop last
		window.Poll();
	}

	SDL_Quit();
    return 0;
}
