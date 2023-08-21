// Created by block on 8/14/23.

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <core/filesystem/Filesystem.hpp>
#include <core/filesystem/WatchSystem.hpp>
#include <core/gl/GLHeaders.hpp>
#include <core/gl/Shader.hpp>
#include <core/gl/Texture.hpp>
#include <core/Logger.hpp>
#include <core/scene/Scene.hpp>
#include <core/sdl/Window.hpp>
#include <core/StdoutSink.hpp>
#include <core/Types.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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

	core::SystemManager::The().Init();

	// Create watch system once
	core::filesystem::watchSystem = new core::filesystem::WatchSystem;
	core::SystemManager::The().Add(static_cast<core::PerTickSystem*>(core::filesystem::watchSystem));

	auto window = sdl::Window { "engine", 800, 600 };

	// By this point the Window class has setup OpenGL and made the context it created current,
	// so now we can load OpenGL APIs.
	gladLoadGLLoader(SDL_GL_GetProcAddress);

	DumpOglInfo();

	glClearColor(0.f, 0.f, 0.f, 1.f);
	glViewport(0, 0, 800, 600);

	// init stuff

	auto& theScene = core::scene::Scene::The();
	auto theCam = new core::scene::Camera();

	theScene.SetCamera(theCam);

	theCam->SetPosRot({0, 0, 3}, glm::identity<glm::quat>());
	theCam->SetFovNearFar(60.f, 0.1f, 1000.f);


	float vertices[] = {
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};


	glm::vec3 cubePositions[] = {
		glm::vec3( 0.0f,  0.0f,  0.0f),
		glm::vec3( 2.0f,  5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3( 2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3( 1.3f, -2.0f, -2.5f),
		glm::vec3( 1.5f,  2.0f, -2.5f),
		glm::vec3( 1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f)
	};

	// model

	u32 VAO, VBO, EBO;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	// bind buffers
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// vertex/fragment shaders
	gl::Shader vertexShader(gl::Shader::Kind::Vertex);
	gl::Shader fragmentShader(gl::Shader::Kind::Fragment);
	vertexShader.SetPath(core::filesystem::Filesystem::The().GetAbsolutePathFor("shaders/demo.vert"));
	fragmentShader.SetPath(core::filesystem::Filesystem::The().GetAbsolutePathFor("shaders/demo.frag"));

	if(!vertexShader.Compile()) {
		core::LogInfo("Vertex shader compilation failure: {}", vertexShader.GetInfoLog());
	}

	if(!fragmentShader.Compile()) {
		core::LogInfo("Fragment shader compilation failure: {}", fragmentShader.GetInfoLog());
	}

	gl::ShaderProgram program;
	program.AttachShader(vertexShader);
	program.AttachShader(fragmentShader);
	program.Link();
	program.Bind();

	// textures
	gl::Texture image1, image2;
	image1.LoadTexture(core::filesystem::Filesystem::The().GetAbsolutePathFor("textures/test.png"));
	image2.LoadTexture(core::filesystem::Filesystem::The().GetAbsolutePathFor("textures/test2.png"));

	// set up uniforms
	program.Bind();
	glUniform1i(glGetUniformLocation(program.GetID(), "texture1"), 0);
	glUniform1i(glGetUniformLocation(program.GetID(), "texture2"), 1);

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

	//SDL_Surface* windowSurface = SDL_GetWindowSurface(window.Raw());

	glEnable(GL_DEPTH_TEST);

	while(run) {
		// Fixed timestep updates.
		//
		// Note that this loop is not "greedy"; it only executes
		// updates for the times it can, and does not otherwise.
		while(deltaTime >= 1.) {
			//core::LogInfo("Update {}", deltaTime);
			core::SystemManager::The().Tick();
			deltaTime--;
		}

		nowTime = SDL_GetTicks64() / 1000.f;
		deltaTime += (nowTime - lastTime) / UpdateRate;
		lastTime = nowTime;

		//core::LogInfo("delta time: {}", 1.f/(nowTime - lastTime));

		// do actual drawing now
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glActiveTexture(GL_TEXTURE0);
		image1.Bind();
		glActiveTexture(GL_TEXTURE1);
		image2.Bind();

		program.Bind();
		program.SetUniform("time", glm::vec2(nowTime, std::chrono::system_clock::now().time_since_epoch().count()));

		theCam->SetPos({sin(nowTime), 0, 3.f + cos(nowTime)});
		theCam->LookAtTarget({0,0,0}, sin(nowTime) * 25.f);

		program.SetUniform("matProjection", theScene.GetCameraProjection());
		program.SetUniform("matView", theScene.GetCameraView());

		glBindVertexArray(VAO);
		for(unsigned int i = 0; i < 10; i++)
		{
			auto model = glm::mat4(1.0f);
			//model = glm::translate(model, cubePositions[i] + glm::vec3(0, sin(nowTime * tan(i)), 0));
			model = glm::translate(model, cubePositions[i]);
			float angle = 20.0f * i;
			//angle += tan(nowTime) * 40.f;
			model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			program.SetUniform("matModel", model);

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}


		window.Swap();
		// Run the SDL window event loop last
		window.Poll();
	}

	SDL_Quit();
	return 0;
}
