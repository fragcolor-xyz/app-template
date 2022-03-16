#include "gfx/context.hpp"
#include "gfx/drawable.hpp"
#include "gfx/features/base_color.hpp"
#include "gfx/features/debug_color.hpp"
#include "gfx/features/transform.hpp"
#include "gfx/geom.hpp"
#include "gfx/imgui/imgui.hpp"
#include "gfx/loop.hpp"
#include "gfx/platform.hpp"
#include "gfx/renderer.hpp"
#include "gfx/view.hpp"
#include "gfx/window.hpp"
#include <SDL_events.h>
#include <SDL_video.h>
#include <boost/algorithm/string/case_conv.hpp>
#include <spdlog/spdlog.h>

using namespace gfx;

static void setupSpdLog();
#if GFX_ANDROID
#include <spdlog/sinks/android_sink.h>
void setupSpdLog() {
  auto android_sink =
      std::make_shared<spdlog::sinks::android_sink_mt>("android");
  spdlog::default_logger()->sinks().push_back(android_sink);
  spdlog::set_pattern("%v");
  spdlog::set_level(spdlog::level::debug);
}
#else
void setupSpdLog() {}
#endif

#if GFX_EMSCRIPTEN
#include <emscripten/html5.h>
void osYield() { emscripten_sleep(0); }
#else
void osYield() {}
#endif

template <typename T>
static MeshPtr
createMesh(const std::vector<T> &verts,
           const std::vector<geom::GeneratorBase::index_t> &indices) {
  MeshPtr mesh = std::make_shared<Mesh>();
  MeshFormat format{
      .vertexAttributes = T::getAttributes(),
  };
  mesh->update(format, verts.data(), verts.size() * sizeof(T), indices.data(),
               indices.size() * sizeof(geom::GeneratorBase::index_t));
  return mesh;
}

static MeshPtr createPlaneMesh() {
  geom::PlaneGenerator gen;
  gen.generate();
  return createMesh(gen.vertices, gen.indices);
}

struct App {
  Window window;
  Loop loop;
  Context context;
  ViewPtr view;
  MeshPtr plane;
  PipelineSteps steps;
  DrawQueue queue;
  DrawablePtr drawable;
  std::shared_ptr<ImGuiRenderer> imgui;
  std::shared_ptr<Renderer> renderer;
  std::vector<SDL_Event> events;

  void init() {
    spdlog::debug("sandbox Init");
    window.init(WindowCreationOptions{.width = 1280, .height = 720});

    gfx::ContextCreationOptions contextOptions = {};
    contextOptions.debug = false;
    context.init(window, contextOptions);

    renderer = std::make_shared<Renderer>(context);
    imgui = std::make_shared<ImGuiRenderer>(context);

    plane = createPlaneMesh();
    view = std::make_shared<View>();
    view->proj = ViewOrthographicProjection{
        .size = 2.0f,
        .near = -10.0f,
        .far = 10.0f,
    };

    steps = {
        makeDrawablePipelineStep(RenderDrawablesStep{
            .features =
                {
                    features::Transform::create(),
                    features::BaseColor::create(),
                    features::DebugColor::create(
                        "texCoord0", gfx::ProgrammableGraphicsStage::Fragment),
                },
        }),
    };

    drawable = std::make_shared<Drawable>(plane, linalg::identity);
  }

  float rotation = 0.0f;
  void updateDrawable(float deltaTime) {
    rotation += deltaTime * degToRad(-180.0f);

    float4 rotQuat = linalg::rotation_quat(float3(0, 0, 1), rotation);
    float4x4 r = linalg::rotation_matrix(rotQuat);
    float4x4 t = linalg::translation_matrix(float3(0.0f, 0.0f, 0.0f));
    float4x4 transform = linalg::mul(r, t);
    drawable->transform = transform;
  }

  void renderUI(float deltaTime) {
    using namespace ImGui;
    imgui->beginFrame(events);

    auto &io = GetIO();
    io.FontGlobalScale = 1.0f;

    ImGuiViewport *viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);

    if (ImGui::Begin("Demo", nullptr,
                     ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize |
                         ImGuiWindowFlags_NoMove |
                         ImGuiWindowFlags_NoBackground)) {
      ImGui::Text("Hello world");
      ImGui::TextColored(ImVec4(1, 0, 1, 1), "Magenta text :O");
      if (ImGui::Button("Button")) {
        spdlog::info("The button was pressed");
      }
      ImGui::End();
    }

    imgui->endFrame();
  }

  void runMainLoop() {
    bool quit = false;
    while (!quit) {
      window.pollEvents(events);

      bool displayChanged = false;

      for (auto &event : events) {
        if (event.type == SDL_APP_DIDENTERBACKGROUND) {
          spdlog::info("App Suspend");
          context.suspend();
        } else if (event.type == SDL_APP_DIDENTERFOREGROUND) {
          spdlog::info("App Resume");
          context.resume();
        } else if (event.type == SDL_DISPLAYEVENT) {
          if (event.display.type == SDL_DISPLAYEVENT_ORIENTATION) {
            displayChanged = true;
          }
        }

        if (event.type == SDL_WINDOWEVENT) {
          if (event.window.type == SDL_WINDOWEVENT_SIZE_CHANGED) {
          }
          spdlog::info("SDL Window Event {}", event.window.type);
        } else {
          spdlog::info("SDL Event {}", event.type);
        }
        if (event.type == SDL_QUIT)
          quit = true;
      }

      if (displayChanged) {
        spdlog::info("Display changed");
        context.sync();
      }

      context.resizeMainOutputConditional(window.getDrawableSize());

      float deltaTime;
      if (loop.beginFrame(1.0f / 120.0f, deltaTime)) {
        updateDrawable(deltaTime);

        if (context.beginFrame()) {
          renderer->beginFrame();

          queue.clear();
          queue.add(drawable);

          renderer->render(queue, view, steps);
          renderer->endFrame();

          renderUI(deltaTime);

          context.endFrame();
        } else {
          spdlog::debug("Skipping render");
        }
      } else {
        osYield();
      }
    }
  }
};

int main() {
  setupSpdLog();
  spdlog::info("HI =)");
  App instance;
  instance.init();
  instance.runMainLoop();
  return 0;
}