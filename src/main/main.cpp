// Created by block on 8/14/23.

#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_sdl2.h>
#include <imgui.h>
#include <SDL2/SDL.h>

#include <core/experiments/SSTV.hpp>
#include <core/filesystem/Filesystem.hpp>
#include <core/filesystem/WatchSystem.hpp>
#include <core/rendering/GLHeaders.hpp>
#include <core/rendering/MaterialSystem.hpp>
#include <core/rendering/TextureSystem.hpp>
#include <core/rendering/ShaderSystem.hpp>
#include <core/rendering/Model.hpp>
#include <core/InputSystem.hpp>
#include <core/TimeSystem.hpp>
#include <core/Logger.hpp>
#include <core/scene/Scene.hpp>
#include <core/scene/Light.hpp>
#include <core/sdl/Window.hpp>
#include <core/StdoutSink.hpp>
#include <core/Types.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace core = butane::core;
namespace sdl = core::sdl;
namespace gfx = core::gfx;

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

	// Create time system
	core::SystemManager::The().Add(static_cast<core::System*>(&core::TimeSystem::The()));

	// Create input system
	core::SystemManager::The().Add(static_cast<core::PerTickSystem*>(&core::InputSystem::The()));

	// Create file watch system
	core::filesystem::watchSystem = new core::filesystem::WatchSystem;
	core::SystemManager::The().Add(static_cast<core::PerTickSystem*>(core::filesystem::watchSystem));

	// Create shader, texture, and material systems
	core::SystemManager::The().Add(static_cast<core::System*>(&core::gfx::ShaderSystem::The()));
	core::SystemManager::The().Add(static_cast<core::System*>(&core::gfx::TextureSystem::The()));
	core::SystemManager::The().Add(static_cast<core::System*>(&core::gfx::MaterialSystem::The()));

	// Create systems for the scene
	core::SystemManager::The().Add(static_cast<core::PerTickSystem*>(&core::scene::LightManager::The()));

	auto window = sdl::Window { "butane", 1280, 720 };
	sdl::Window::CurrentWindow = &window;

	// By this point the Window class has setup OpenGL and made the context it created current,
	// so now we can load OpenGL APIs.
	gladLoadGLLoader(SDL_GL_GetProcAddress);

	// Init all registered systems
	core::SystemManager::The().Init();

	DumpOglInfo();

	// imgui init
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.IniFilename = "data/imgui.ini";
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	//ImGui::StyleColorsDark();

	ImGuiStyle& style = ImGui::GetStyle();

	style.Colors[ImGuiCol_Text]                   = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	style.Colors[ImGuiCol_TextDisabled]           = ImVec4(0.86f, 0.81f, 0.75f, 0.60f);
	style.Colors[ImGuiCol_WindowBg]               = ImVec4(0.24f, 0.22f, 0.17f, 0.90f);
	style.Colors[ImGuiCol_ChildBg]                = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style.Colors[ImGuiCol_PopupBg]                = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
	style.Colors[ImGuiCol_Border]                 = ImVec4(0.57f, 0.51f, 0.45f, 0.50f);
	style.Colors[ImGuiCol_BorderShadow]           = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style.Colors[ImGuiCol_FrameBg]                = ImVec4(0.29f, 0.27f, 0.25f, 1.00f);
	style.Colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.56f, 0.53f, 0.46f, 0.40f);
	style.Colors[ImGuiCol_FrameBgActive]          = ImVec4(0.79f, 0.76f, 0.72f, 0.67f);
	style.Colors[ImGuiCol_TitleBg]                = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
	style.Colors[ImGuiCol_TitleBgActive]          = ImVec4(0.42f, 0.28f, 0.22f, 1.00f);
	style.Colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
	style.Colors[ImGuiCol_MenuBarBg]              = ImVec4(0.13f, 0.13f, 0.12f, 0.78f);
	style.Colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
	style.Colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
	style.Colors[ImGuiCol_CheckMark]              = ImVec4(0.90f, 0.94f, 0.30f, 0.78f);
	style.Colors[ImGuiCol_SliderGrab]             = ImVec4(0.60f, 0.57f, 0.52f, 1.00f);
	style.Colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.79f, 0.76f, 0.72f, 1.00f);
	style.Colors[ImGuiCol_Button]                 = ImVec4(0.32f, 0.30f, 0.28f, 1.00f);
	style.Colors[ImGuiCol_ButtonHovered]          = ImVec4(0.69f, 0.64f, 0.12f, 1.00f);
	style.Colors[ImGuiCol_ButtonActive]           = ImVec4(0.91f, 0.97f, 0.50f, 1.00f);
	style.Colors[ImGuiCol_Header]                 = ImVec4(0.80f, 0.24f, 0.24f, 0.31f);
	style.Colors[ImGuiCol_HeaderHovered]          = ImVec4(0.80f, 0.24f, 0.24f, 0.66f);
	style.Colors[ImGuiCol_HeaderActive]           = ImVec4(0.69f, 0.64f, 0.12f, 0.76f);
	style.Colors[ImGuiCol_Separator]              = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
	style.Colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
	style.Colors[ImGuiCol_SeparatorActive]        = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
	style.Colors[ImGuiCol_ResizeGrip]             = ImVec4(0.55f, 0.27f, 0.15f, 0.20f);
	style.Colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.55f, 0.27f, 0.15f, 0.67f);
	style.Colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.69f, 0.64f, 0.12f, 0.76f);
	style.Colors[ImGuiCol_Tab]                    = ImVec4(0.29f, 0.27f, 0.24f, 0.46f);
	style.Colors[ImGuiCol_TabHovered]             = ImVec4(0.75f, 0.87f, 0.31f, 0.80f);
	style.Colors[ImGuiCol_TabActive]              = ImVec4(0.55f, 0.27f, 0.15f, 1.00f);
	style.Colors[ImGuiCol_TabUnfocused]           = ImVec4(0.07f, 0.10f, 0.15f, 0.97f);
	style.Colors[ImGuiCol_TabUnfocusedActive]     = ImVec4(0.13f, 0.26f, 0.42f, 1.00f);
	style.Colors[ImGuiCol_DockingPreview]         = ImVec4(0.26f, 0.59f, 0.98f, 0.70f);
	style.Colors[ImGuiCol_DockingEmptyBg]         = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
	style.Colors[ImGuiCol_PlotLines]              = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
	style.Colors[ImGuiCol_PlotLinesHovered]       = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogram]          = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_TableHeaderBg]          = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
	style.Colors[ImGuiCol_TableBorderStrong]      = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);
	style.Colors[ImGuiCol_TableBorderLight]       = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);
	style.Colors[ImGuiCol_TableRowBg]             = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style.Colors[ImGuiCol_TableRowBgAlt]          = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
	style.Colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
	style.Colors[ImGuiCol_DragDropTarget]         = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
	style.Colors[ImGuiCol_NavHighlight]           = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
	style.Colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
	style.Colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);

	ImGui_ImplSDL2_InitForOpenGL(window.Raw(), window.GlContext());
	ImGui_ImplOpenGL3_Init("#version 330 core");

	// init stuff

	core::TimeSystem& timeSystem = core::TimeSystem::The();

	auto& theScene = core::scene::Scene::The();
	auto theCam = new core::scene::Camera();

	theScene.SetCamera(theCam);

	theCam->transform.SetPosRot({0.f, 0.7f, 0.f}, glm::identity<glm::quat>());
	theCam->SetFovNearFar(70.f, 0.1f, 1000.f);

	// materials
	gfx::Material* materialCube = gfx::MaterialSystem::The().GetMaterial("materials/container.material");
	gfx::Material* materialLight = gfx::MaterialSystem::The().GetMaterial("materials/light.material");

	// models
	gfx::Model* modelCube = new gfx::Model(core::filesystem::Filesystem::GetDataDir() / "models/defaults/cube.obj");
	gfx::Model* modelBackpack = new gfx::Model(core::filesystem::Filesystem::GetDataDir() / "models/backpack/backpack.obj");

	// loop variables
	bool run = true;

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
	SDL_Rect windowRect = window.GetRect();

	glClearColor(0.05f, 0.05f, 0.05f, 1.f);
	glViewport(0, 0, windowRect.w, windowRect.h);
	glEnable(GL_DEPTH_TEST);

	auto bind_forward = core::InputSystem::The().RegisterBind("forward", {SDL_Scancode::SDL_SCANCODE_W}, SDL_Keymod::KMOD_NONE);
	auto bind_back = core::InputSystem::The().RegisterBind("back", {SDL_Scancode::SDL_SCANCODE_S}, SDL_Keymod::KMOD_NONE);
	auto bind_left = core::InputSystem::The().RegisterBind("left", {SDL_Scancode::SDL_SCANCODE_A}, SDL_Keymod::KMOD_NONE);
	auto bind_right = core::InputSystem::The().RegisterBind("right", {SDL_Scancode::SDL_SCANCODE_D}, SDL_Keymod::KMOD_NONE);

	auto bind_up = core::InputSystem::The().RegisterBind("up", {SDL_Scancode::SDL_SCANCODE_SPACE}, SDL_Keymod::KMOD_NONE);
	auto bind_down = core::InputSystem::The().RegisterBind("down", {SDL_Scancode::SDL_SCANCODE_C}, SDL_Keymod::KMOD_NONE);

	auto bind_lock = core::InputSystem::The().RegisterBind("lock", {SDL_Scancode::SDL_SCANCODE_L}, SDL_Keymod::KMOD_CTRL);
	auto debug_menu = core::InputSystem::The().RegisterBind("debug_menu", {SDL_Scancode::SDL_SCANCODE_M}, SDL_Keymod::KMOD_CTRL);

	auto bind_sstv = core::InputSystem::The().RegisterBind("sstv", {SDL_Scancode::SDL_SCANCODE_S}, SDL_Keymod::KMOD_ALT);

	bool animateCam = false;
	bool lookAtTarget = false;
	bool updateSpotlight = true;
	bool debugMenuFlag = true;
	glm::vec3 camPos = {};
	glm::quat camRot = glm::identity<glm::quat>();
	float camSpeed = 5.f;

	const glm::vec3 cubePositions[] = {
			glm::vec3( 0.0f,  0.0f,  8.0f),
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

	while(run) {
		timeSystem.SystemsUpdate();

		// Fixed timestep updates.
		//
		// Note that this loop is not "greedy"; it only executes
		// updates for the times it can, and does not otherwise.
		while(timeSystem.TickNextReady()) {
			//core::LogInfo("New tick!! {} >= {}", timeSystem.TickConsumingTime(), timeSystem.UpdateRate());
			timeSystem.StartTick();
			core::SystemManager::The().StartTick();

			if(bind_lock->Down()) {
				core::InputSystem::The().SetMouseLock(!core::InputSystem::The().IsMouseLocked());
				//animateCam = lookAtTarget = !core::InputSystem::The().IsMouseLocked();
			}

			if (debug_menu->Down()) {
				debugMenuFlag = !debugMenuFlag;
			}

			if (bind_sstv->Down()) {
				auto sstv = core::experiments::SSTV::The();
				sstv.DoTheThing(windowRect);
			}

			if(core::InputSystem::The().IsMouseLocked()) {
				glm::vec2 look = core::InputSystem::The().GetMouseDelta();
				if (look.length() > 0) {
					look = {-look.y, -look.x}; // fix axies
					look *= timeSystem.TickDeltaTime() * 8.f; // sensitivity

					// yaw is in world space
					float yawDeg = glm::radians(look.y);
					glm::quat yawRot = glm::angleAxis(yawDeg, glm::vec3(0, 1, 0));

					// pitch is in local space
					float pitchDeg = glm::radians(look.x);
					glm::quat pitchRot = glm::angleAxis(pitchDeg, camRot * glm::vec3(1, 0, 0));

					camRot = yawRot * pitchRot * camRot;
				}

				bool forward = bind_forward->Held();
				bool back = bind_back->Held();
				bool left = bind_left->Held();
				bool right = bind_right->Held();

				bool up = bind_up->Held();
				bool down = bind_down->Held();

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
				move *= camSpeed * timeSystem.TickDeltaTime();

				camPos += move;
			}

			core::SystemManager::The().EndTick();
			timeSystem.EndTick();
		}

		core::debug::ImGuiExtensions::IdIndex = 0;
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();

		//core::LogDebug("Render delta time: {}", timeSystem.DeltaTime());

		if (animateCam) {
			theCam->transform.SetPos({cos(timeSystem.NowTime() * 1.2f) * 3.f, theCam->transform.metaPos.y, sin(timeSystem.NowTime()) * 3.f});
			if (lookAtTarget)
				theCam->transform.LookAtTarget({});
			camPos = theCam->transform.metaPos;
			camRot = theCam->transform.metaRot;
		}
		else {
			theCam->transform.SetPosRot(camPos, camRot);
		}

		// imgui drawing
		static bool show_demo = false;
		if(debugMenuFlag && ImGui::BeginMainMenuBar()) {
			butane::core::SystemManager::The().ImGuiDebug();

			if (ImGui::BeginMenu("Camera")) {
				ImGui::Checkbox("animate cam", &animateCam);
				ImGui::Checkbox("look at target", &lookAtTarget);
				ImGui::Checkbox("update spotlight", &updateSpotlight);
				ImGui::DragFloat3("cam pos", &camPos.x, 0.1f);
				ImGui::DragFloat("cam speed", &camSpeed, 0.1f);

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("SSTV")) {
				auto sstv = core::experiments::SSTV::The();
				for (size_t i = 0; i < sstv.MODES.size(); i++) {
					core::experiments::SSTV::Mode* mode = &sstv.MODES[i];
					if (ImGui::Button(mode->name.c_str())) {
						sstv.SetMode(mode);
						sstv.DoTheThing(windowRect);
					}
				}

				ImGui::EndMenu();
			}

			ImGui::MenuItem("ImGui Demo", "", &show_demo);
			if (show_demo)
				ImGui::ShowDemoWindow(&show_demo);

			ImGui::TextDisabled("%.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);

			ImGui::EndMainMenuBar();
		}

		ImGui::Render();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// do actual drawing now

		if (updateSpotlight)
			core::scene::LightManager::The().spot.transform = theCam->transform;

		// manual models
		glm::mat4 matModel = glm::identity<glm::mat4>();
		matModel = glm::translate(matModel, {0.f, sin(timeSystem.NowTime()), -3.f});
		matModel = glm::scale(matModel, {0.4f, 0.4f, 0.4f});
		matModel = glm::rotate(matModel, timeSystem.NowTime() * (3.1415926f/2.f), core::scene::Transform::Up);
		//matModel = matModel * glm::mat4_cast(camRot);

		modelBackpack->Draw(gfx::MaterialSystem::The().GetMaterial("models/backpack/backpack.material"), matModel);

		// cubes
		materialCube->shaderProgram->Bind();
		materialCube->shaderProgram->SetUniform("m_viewPos", theCam->transform.metaPos);

		for(int i = 0; i < 10; i++) {
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, cubePositions[i]);
			float angle = 20.0f * i;
			model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));

			modelCube->Draw(materialCube, model);
		}

		// lights
		materialLight->shaderProgram->Bind();

		for(int i = 0; i < 4; i++) {
			auto* light = &core::scene::LightManager::The().points[i];

			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, light->transform.metaPos);
			model = glm::scale(model, glm::vec3(0.1f));

			materialLight->shaderProgram->SetUniform("lightColor", light->GetColor());

			modelCube->Draw(materialLight, model);
		}


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
