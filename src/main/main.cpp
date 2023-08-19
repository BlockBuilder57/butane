// Created by block on 8/14/23.

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <core/filesystem/Filesystem.hpp>
#include <core/filesystem/WatchSystem.hpp>
#include <core/gl/GLHeaders.hpp>
#include <core/gl/Shader.hpp>
#include <core/gl/Texture.hpp>
#include <core/Logger.hpp>
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

void TestThing(const std::filesystem::path& path, core::filesystem::Watch::Event ev) {
	core::LogInfo("Got event for \"{}\" event {}", path.string(), static_cast<i32>(ev));
}

int main(int argc, char** argv) {
	static_cast<void>(argc);
	static_cast<void>(argv);

	core::LoggerAttachStdout();

	if(SDL_Init(SDL_INIT_EVERYTHING) > 0) {
		core::LogFatal("Failed to initialize SDL; giving up");
		return 1;
	}

	auto window = sdl::Window { "engine", 800, 600 };

	// By this point the Window class has setup OpenGL and made the context it created current,
	// so now we can load OpenGL APIs.
	gladLoadGLLoader(SDL_GL_GetProcAddress);

	DumpOglInfo();

	glClearColor(0.f, 0.f, 0.f, 1.f);
	glViewport(0, 0, 800, 600);

	// init stuff

	float vertices[] = {
		// positions          // colors           // texture coords
		 0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
		 0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
		-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left
	};
	unsigned int indices[] = {  // note that we start from 0!
		0, 1, 3,   // first triangle
		1, 2, 3    // second triangle
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

	u32 VAO, VBO, EBO;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	// bind buffers
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// uv attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// textures
	gl::Texture image1, image2;
	image1.LoadTexture(core::filesystem::Filesystem::The().GetDataDir() / "textures" / "test.png");
	image2.LoadTexture(core::filesystem::Filesystem::The().GetDataDir() / "textures" / "test2.png");

	// set up uniforms
	program.Bind();
	glUniform1i(glGetUniformLocation(program.GetID(), "texture1"), 0);
	glUniform1i(glGetUniformLocation(program.GetID(), "texture2"), 1);


	auto system = new core::filesystem::WatchSystem;
	auto watch = new core::filesystem::Watch(  core::filesystem::Filesystem::The().GetDataDir() / "shaders"  );

	watch->SetCallback(TestThing);

	core::SystemManager::The().Add(static_cast<engine::core::PerTickSystem*>(system));

	system->AddWatch(watch);

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

	core::SystemManager::The().Init();

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
		program.SetUniform("time", nowTime, std::chrono::system_clock::now().time_since_epoch().count());

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::rotate(model, nowTime * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));

		glm::mat4 view = glm::mat4(1.0f);
		// note that we're translating the scene in the reverse direction of where we want to move
		view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

		glm::mat4 projection;
		projection = glm::perspective(glm::radians(45.0f), 800.f / 600.f, 0.1f, 100.0f);

		unsigned int modelLoc = glGetUniformLocation(program.GetID(), "matModel");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		unsigned int viewLoc = glGetUniformLocation(program.GetID(), "matView");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		unsigned int projLoc = glGetUniformLocation(program.GetID(), "matProjection");
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		window.Swap();

		// Run the SDL window event loop last
		window.Poll();
	}

	SDL_Quit();
	return 0;
}
