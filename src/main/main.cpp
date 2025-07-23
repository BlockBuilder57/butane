// Created by block on 8/14/23.

#include <imgui.h>
#include <SDL2/SDL.h>
#include <libfasstv/libfasstv.hpp>
#include <libfasstv/ExportUtils.hpp>

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

#include "wrap/Wrap.hpp"

namespace core = butane::core;
namespace wrap = butane::wrap;
namespace sdl = core::sdl;
namespace gfx = core::gfx;

int main(int argc, char** argv) {
	static_cast<void>(argc);
	static_cast<void>(argv);

	core::LoggerAttachStdout();

	int res = wrap::sdl::Init();
	if (res != 0)
		return res;

	sdl::Window* window = wrap::sdl::CreateWindow();

	wrap::systems::RegisterSystemsAndInit();

	// imgui init
	wrap::imgui::Init();
	ImGuiIO& io = ImGui::GetIO();

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
	wrap::sdl::SetupEventHandlers();

	//SDL_Surface* windowSurface = SDL_GetWindowSurface(window->Raw());
	SDL_Rect windowRect = window->GetRect();

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

	auto& sstvenc = fasstv::SSTVEncode::The();
	sstvenc.SetSampleRate(44100);
	sstvenc.SetPixelProvider(&butane::wrap::fasstv::GetSampleFromSurface);
	std::vector<float> samples;

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
				sstvenc.RunAllInstructions(samples, {0, 0, windowRect.w, windowRect.h});
				std::ofstream file(core::filesystem::Filesystem::GetDataDir() / "sstv" / "sstv.wav", std::ios::binary);
				fasstv::SamplesToWAV(samples, 44100, file);
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

		wrap::imgui::NewFrame();

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
				auto& sstv = fasstv::SSTV::The();

				if (ImGui::CollapsingHeader("Modes")) {
					for(auto i = 0; i < sstv.MODES.size(); i++) {
						fasstv::SSTV::Mode& mode = sstv.MODES[i];
						if(ImGui::Button(mode.name.c_str())) {
							sstvenc.SetMode(&mode);
							sstvenc.SetLetterbox(fasstv::Rect::CreateLetterbox(mode.width, mode.lines, { 0, 0, windowRect.w, windowRect.h }));
						}

						if(i % 6 != 5 && i != sstv.MODES.size() - 1)
							ImGui::SameLine();
					}
				}

				ImGui::LabelText("Current SSTV Mode", "%s", sstvenc.GetMode() ? sstvenc.GetMode()->name.c_str() : "<none>");
				if (ImGui::Button(bind_sstv->AsString().c_str())) {
					sstvenc.RunAllInstructions(samples, {0, 0, windowRect.w, windowRect.h});
					std::ofstream file(core::filesystem::Filesystem::GetDataDir() / "sstv" / "sstv.wav", std::ios::binary);
					fasstv::SamplesToWAV(samples, 44100, file);
				}

				ImGui::EndMenu();
			}

			ImGui::MenuItem("ImGui Demo", "", &show_demo);
			if (show_demo)
				ImGui::ShowDemoWindow(&show_demo);

			ImGui::TextDisabled("%.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);

			ImGui::EndMainMenuBar();
		}

		wrap::imgui::Render();

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


		wrap::imgui::RenderDrawData();
		window->Swap();
		// Run the SDL window event loop last
		window->Poll();
	}

	wrap::imgui::Shutdown();
	wrap::sdl::Shutdown();
	return 0;
}
