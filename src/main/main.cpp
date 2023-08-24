// Created by block on 8/14/23.

#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_sdl2.h>
#include <imgui.h>
#include <SDL2/SDL.h>

#include <core/filesystem/Filesystem.hpp>
#include <core/filesystem/WatchSystem.hpp>
#include <core/gl/GLHeaders.hpp>
#include <core/gl/Shader.hpp>
#include <core/gl/Texture.hpp>
#include <core/InputSystem.hpp>
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

	// Create file watch system
	core::filesystem::watchSystem = new core::filesystem::WatchSystem;
	core::SystemManager::The().Add(static_cast<core::PerTickSystem*>(core::filesystem::watchSystem));

	// Create input system
	core::SystemManager::The().Add(static_cast<core::PerTickSystem*>(&core::InputSystem::The()));

	auto window = sdl::Window { "engine", 800, 600 };
	sdl::Window::CurrentWindow = &window;

	// By this point the Window class has setup OpenGL and made the context it created current,
	// so now we can load OpenGL APIs.
	gladLoadGLLoader(SDL_GL_GetProcAddress);

	DumpOglInfo();

	// imgui init
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.IniFilename = "data/imgui.ini";
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	ImGui::StyleColorsDark();

	ImGui_ImplSDL2_InitForOpenGL(window.Raw(), window.GlContext());
	ImGui_ImplOpenGL3_Init("#version 330");

	// init stuff

	auto& theScene = core::scene::Scene::The();
	auto theCam = new core::scene::Camera();

	theScene.SetCamera(theCam);

	theCam->transform.SetPosRot({0.f, 0.7f, 0.f}, glm::identity<glm::quat>());
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
	gl::Texture blank, image1, image2;
	const unsigned char colorWhite[4] = { 255, 255, 255, 255 };
	blank.LoadTexture(1, 1, (void*)&colorWhite[0]);
	image1.WrapModeU = GL_REPEAT;
	image1.WrapModeV = GL_REPEAT;
	image1.LoadTexture(core::filesystem::Filesystem::The().GetAbsolutePathFor("textures/test.png"));
	image2.LoadTexture(core::filesystem::Filesystem::The().GetAbsolutePathFor("textures/test2.png"));

	// loop variables

	bool run = true;

	// This is essentially how many update ticks we run (when we can)
	// This should be made a configurable value later on
	constexpr static float UpdateRate = 1. / 100.;

	float deltaTime = 0.f;
	float lastTime = SDL_GetTicks64() / 1000.f;
	float nowTime;

	// Assign window event handlers
	window.On(SDL_QUIT, [&](SDL_Event& ev) {
		static_cast<void>(ev);
		run = false;
	});

	window.On(SDL_KEYDOWN, [&](SDL_Event& ev) {
		core::InputSystem::The().KeyEvent(ev.key);
	});
	window.On(SDL_KEYUP, [&](SDL_Event& ev) {
		core::InputSystem::The().KeyEvent(ev.key);
	});

	//SDL_Surface* windowSurface = SDL_GetWindowSurface(window.Raw());

	glClearColor(0.05f, 0.05f, 0.05f, 1.f);
	glViewport(0, 0, 800, 600);
	glEnable(GL_DEPTH_TEST);

	bool animateCam = false;
	bool lookAtTarget = true;
	glm::vec3 camPos = {-2.f, 1.f, -2.5f};
	glm::quat camRot = glm::identity<glm::quat>();
	float camSpeed = 5.f;
	glm::vec3 lightPos = {1.2f, 1.4f, 2.0f};

	while(run) {
		// Fixed timestep updates.
		//
		// Note that this loop is not "greedy"; it only executes
		// updates for the times it can, and does not otherwise.
		while(deltaTime >= UpdateRate) {
			//core::LogInfo("Update {}", deltaTime);
			core::SystemManager::The().StartTick();

			if(core::InputSystem::The().ButtonDown(SDL_Scancode::SDL_SCANCODE_L, SDL_Keymod::KMOD_CTRL)) {
				core::InputSystem::The().SetMouseLock(!core::InputSystem::The().IsMouseLocked());
			}

			if(core::InputSystem::The().IsMouseLocked()) {
				glm::vec2 look = core::InputSystem::The().GetMouseDelta();
				if (look.length() > 0) {
					look = {-look.y, -look.x}; // fix axies
					look *= deltaTime * 8.f; // sensitivity

					// yaw is in world space
					float yawDeg = glm::radians(look.y);
					glm::quat yawRot = glm::angleAxis(yawDeg, glm::vec3(0, 1, 0));

					// pitch is in local space
					float pitchDeg = glm::radians(look.x);
					glm::quat pitchRot = glm::angleAxis(pitchDeg, camRot * glm::vec3(1, 0, 0));

					camRot = yawRot * pitchRot * camRot;
				}

				bool forward = core::InputSystem::The().ButtonHeld(SDL_Scancode::SDL_SCANCODE_W);
				bool back = core::InputSystem::The().ButtonHeld(SDL_Scancode::SDL_SCANCODE_S);
				bool left = core::InputSystem::The().ButtonHeld(SDL_Scancode::SDL_SCANCODE_A);
				bool right = core::InputSystem::The().ButtonHeld(SDL_Scancode::SDL_SCANCODE_D);

				bool up = core::InputSystem::The().ButtonHeld(SDL_Scancode::SDL_SCANCODE_SPACE);
				bool down = core::InputSystem::The().ButtonHeld(SDL_Scancode::SDL_SCANCODE_C);

				glm::vec3 move = {};

				if (forward)
					move += core::scene::Transform::Forward;
				else if (back)
					move += core::scene::Transform::Back;

				if (left)
					move += core::scene::Transform::Left;
				else if (right)
					move += core::scene::Transform::Right;

				if (up)
					move += core::scene::Transform::Up;
				else if (down)
					move += core::scene::Transform::Down;


				move = camRot * move;
				move *= camSpeed * deltaTime;

				camPos += move;
			}

			core::SystemManager::The().EndTick();
			deltaTime = 0;
		}

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();

		nowTime = SDL_GetTicks64() / 1000.f;
		deltaTime += (nowTime - lastTime);
		lastTime = nowTime;

		//core::LogInfo("delta time: {}", 1.f/(nowTime - lastTime));

		if (animateCam) {
			theCam->transform.SetPos({cos(nowTime * 1.2f) * 3.f, theCam->transform.metaPos.y, sin(nowTime) * 3.f});
			if (lookAtTarget)
				theCam->transform.LookAtTarget({});
			camPos = theCam->transform.metaPos;
			camRot = theCam->transform.metaRot;
		}
		else {
			theCam->transform.SetPosRot(camPos, camRot);
		}

		// imgui drawing

		{
			ImGui::Begin("Blah");

			ImGui::Text("Frame time: %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
			ImGui::Checkbox("animate cam", &animateCam);
			ImGui::Checkbox("look at target", &lookAtTarget);
			ImGui::DragFloat3("cam pos", &camPos.x, 0.1f);
			ImGui::DragFloat("cam speed", &camSpeed, 0.1f);
			ImGui::DragFloat3("light", &lightPos.x, 0.1f);



			ImGui::End();
		}

		ImGui::Render();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// do actual drawing now

		glActiveTexture(GL_TEXTURE0);
		blank.Bind();
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


		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		window.Swap();
		// Run the SDL window event loop last
		window.Poll();
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	SDL_Quit();
	return 0;
}
