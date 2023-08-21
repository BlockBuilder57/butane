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

	glClearColor(0.05f, 0.05f, 0.05f, 1.f);
	glViewport(0, 0, 800, 600);

	// init stuff

	auto& theScene = core::scene::Scene::The();
	auto theCam = new core::scene::Camera();

	theScene.SetCamera(theCam);

	theCam->transform.SetPosRot({}, glm::identity<glm::quat>());
	theCam->SetFovNearFar(45.f, 0.1f, 1000.f);

	float vertices[] = {
		// position           // normal             // uvs
		-0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,   0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,   0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,   0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,   0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,   0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,   0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		 0.5f,  0.5f,  0.5f,   1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,   1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,   1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,   1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,   1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,   1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,   0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,   0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
	 	 0.5f, -0.5f,  0.5f,   0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
	 	 0.5f, -0.5f,  0.5f,   0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,   0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,   0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,   0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
	 	 0.5f,  0.5f, -0.5f,   0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
	 	 0.5f,  0.5f,  0.5f,   0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,   0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,   0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,   0.0f,  1.0f,  0.0f,  0.0f, 1.0f
	};


	// model

	u32 VAO, VBO, EBO;
	u32 lightVAO;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	//glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	// bind buffers
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// texture coord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// light
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);
	// we only need to bind to the VBO, the container's VBO's data already contains the data.
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// set the vertex attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// vertex/fragment shaders
	gl::Shader vertexShader(gl::Shader::Kind::Vertex);
	gl::Shader fragmentShader(gl::Shader::Kind::Fragment);
	gl::Shader lightFragmentShader(gl::Shader::Kind::Fragment);
	vertexShader.SetPath(core::filesystem::Filesystem::The().GetAbsolutePathFor("shaders/default.vert"));
	fragmentShader.SetPath(core::filesystem::Filesystem::The().GetAbsolutePathFor("shaders/default.frag"));
	lightFragmentShader.SetPath(core::filesystem::Filesystem::The().GetAbsolutePathFor("shaders/light.frag"));

	if(!vertexShader.Compile()) {
		core::LogInfo("Vertex shader compilation failure: {}", vertexShader.GetInfoLog());
	}
	if(!fragmentShader.Compile()) {
		core::LogInfo("Fragment shader compilation failure: {}", fragmentShader.GetInfoLog());
	}
	if(!lightFragmentShader.Compile()) {
		core::LogInfo("Light fragment shader compilation failure: {}", lightFragmentShader.GetInfoLog());
	}

	gl::ShaderProgram cubeProgram;
	cubeProgram.AttachShader(vertexShader);
	cubeProgram.AttachShader(fragmentShader);
	cubeProgram.Link();

	gl::ShaderProgram lightProgram;
	lightProgram.AttachShader(vertexShader);
	lightProgram.AttachShader(lightFragmentShader);
	lightProgram.Link();

	// textures
	gl::Texture image1, image2;
	image1.WrapModeU = GL_REPEAT;
	image1.WrapModeV = GL_REPEAT;
	image1.LoadTexture(core::filesystem::Filesystem::The().GetAbsolutePathFor("textures/test.png"));
	image2.LoadTexture(core::filesystem::Filesystem::The().GetAbsolutePathFor("textures/test2.png"));

	// loop variables

	bool run = true;

	// This is essentially how many update ticks we run (when we can)
	// This should be made a configurable value later on
	constexpr static float UpdateRate = 1. / 120.;

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

	const glm::vec3 lightPos = {1.2f, 1.4f, 2.0f};

	while(run) {
		// Fixed timestep updates.
		//
		// Note that this loop is not "greedy"; it only executes
		// updates for the times it can, and does not otherwise.
		while(deltaTime >= 1.) {
			//core::LogInfo("Update {}", deltaTime);
			core::SystemManager::The().Tick();
			deltaTime--;

			theCam->transform.SetPosRot({cos(nowTime * 1.2f) * 3.f, 0.2f, sin(nowTime) * 3.f}, glm::identity<glm::quat>());
			theCam->transform.LookAtTarget({});
			theCam->transform.SetPos(theCam->transform.metaPos + glm::vec3(0, 0.5, 0));
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

		cubeProgram.Bind();
		glUniform1i(glGetUniformLocation(cubeProgram.GetID(), "texture1"), 0);
		glUniform1i(glGetUniformLocation(cubeProgram.GetID(), "texture2"), 1);
		cubeProgram.SetUniform("viewPos", theCam->transform.metaPos);
		cubeProgram.SetUniform("lightPos", lightPos);
		cubeProgram.SetUniform("objectColor", {1.0f, 0.5f, 0.31f});
		cubeProgram.SetUniform("lightColor",  {1.0f, 1.0f, 1.0f});
		cubeProgram.SetUniform("time", glm::vec2(nowTime, std::chrono::system_clock::now().time_since_epoch().count()));
		cubeProgram.SetUniform("matProjection", theScene.GetCameraProjection());
		cubeProgram.SetUniform("matView", theScene.GetCameraView());
		cubeProgram.SetUniform("matModel", glm::identity<glm::mat4>());

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		lightProgram.Bind();
		lightProgram.SetUniform("time", glm::vec2(nowTime, std::chrono::system_clock::now().time_since_epoch().count()));
		lightProgram.SetUniform("matProjection", theScene.GetCameraProjection());
		lightProgram.SetUniform("matView", theScene.GetCameraView());

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.2f));
		lightProgram.SetUniform("matModel", model);

		glBindVertexArray(lightVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		window.Swap();
		// Run the SDL window event loop last
		window.Poll();
	}

	SDL_Quit();
	return 0;
}
