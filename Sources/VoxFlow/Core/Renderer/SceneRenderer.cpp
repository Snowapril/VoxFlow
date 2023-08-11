// Author : snowapril

#include <VoxFlow/Core/Devices/LogicalDevice.hpp>
#include <VoxFlow/Core/Devices/SwapChain.hpp>
#include <VoxFlow/Core/Graphics/Commands/CommandJobSystem.hpp>
#include <VoxFlow/Core/Renderer/SceneRenderPass.hpp>
#include <VoxFlow/Core/Renderer/SceneRenderer.hpp>
#include <VoxFlow/Core/Resources/Texture.hpp>
#include <VoxFlow/Core/Utils/ChromeTracer.hpp>

namespace VoxFlow
{
SceneRenderer::SceneRenderer(LogicalDevice* mainLogicalDevice,
                             FrameGraph::FrameGraph* frameGraph)
    : _mainLogicalDevice(mainLogicalDevice),
      _frameGraph(frameGraph)
{
    _commandJobSystem = _mainLogicalDevice->getCommandJobSystem();

    _renderCmdStreamKey =
        CommandStreamKey{ ._cmdStreamName = MAIN_GRAPHICS_STREAM_NAME,
                          ._cmdStreamUsage = CommandStreamUsage::Graphics };
}

SceneRenderer::~SceneRenderer()
{
}

bool SceneRenderer::initializePasses()
{
    for (auto& [passName, pass] : _sceneRenderPasses)
    {
        if (pass->initialize() == false)
        {
            VOX_ASSERT(false, "Failed to initialize scene render pass {}",
                       passName);
            return false;
        }
    }
    return true;
}

void SceneRenderer::updateRender(ResourceUploadContext* uploadContext)
{
    for (auto& [_, pass] : _sceneRenderPasses)
    {
        pass->updateRender(uploadContext);
    }
}

void SceneRenderer::beginFrameGraph(const FrameContext& frameContext)
{
    SCOPED_CHROME_TRACING("SceneRenderer::beginFrameGraph");

    _currentFrameContext = frameContext;

    _frameGraph->reset(_commandJobSystem->getCommandStream(_renderCmdStreamKey),
                       nullptr);

    SwapChain* swapChain =
        _mainLogicalDevice->getSwapChain(_currentFrameContext._swapChainIndex)
            .get();
    Texture* currentBackBuffer =
        swapChain->getSwapChainImage(_currentFrameContext._backBufferIndex)
            .get();

    const TextureInfo& backBufferInfo = currentBackBuffer->getTextureInfo();

    auto backBufferDescriptor = FrameGraph::FrameGraphTexture::Descriptor{
        ._width = backBufferInfo._extent.x,
        ._height = backBufferInfo._extent.y,
        ._depth = backBufferInfo._extent.z,
        ._level = 0,
        ._sampleCounts = 1,
        ._format = backBufferInfo._format,
    };

    FrameGraph::BlackBoard& blackBoard = _frameGraph->getBlackBoard();

    FrameGraph::ResourceHandle backBufferHandle =
        _frameGraph->importRenderTarget(
            "BackBuffer", std::move(backBufferDescriptor), currentBackBuffer);
    blackBoard["BackBuffer"] = backBufferHandle;
}

tf::Future<void> SceneRenderer::resolveSceneRenderPasses(SwapChain* swapChain)
{
    SCOPED_CHROME_TRACING("SceneRenderer::resolveSceneRenderPasses");

    tf::Taskflow taskflow;
    std::unordered_map<std::string, tf::Task> tasks;

    // Prepare tasks from registered scene render passes
    for (const auto& [passName, pass] : _sceneRenderPasses)
    {
        tf::Task fgTask =
            taskflow
                .emplace([this, &pass]() { pass->renderScene(_frameGraph); })
                .name(passName);

        tasks.emplace(passName, std::move(fgTask));
    }

    // Resolve dependency between tasks
    for (const auto& [passName, pass] : _sceneRenderPasses)
    {
        const std::vector<std::string>* dependentPasses =
            pass->getDepenentPasses();
        tf::Task& fgTask = tasks.find(passName)->second;

        for (const std::string& dependentPassName : *dependentPasses)
        {
            fgTask.succeed(tasks.find(dependentPassName)->second);
        }
    }

    // Add present pass which followed by all other passes
    FrameGraph::ResourceHandle backBufferHandle =
        _frameGraph->getBlackBoard().getHandle("BackBuffer");

    tf::Task presentTask =
        taskflow
            .emplace([&]() {
                _frameGraph->addPresentPass(
                    "PresentPass",
                    std::move([&](FrameGraph::FrameGraphBuilder& builder) {
                        builder.read(backBufferHandle);
                    }),
                    swapChain, _currentFrameContext);
            })
            .name("PresentPass");

    for (const auto& [passName, pass] : _sceneRenderPasses)
    {
        tf::Task& fgTask = tasks.find(passName)->second;

        fgTask.precede(presentTask);
    }

    // Add frame graph compilation task
    taskflow.emplace([&]() { _frameGraph->compile(); })
        .name("Compilation")
        .succeed(presentTask);

    tf::Executor executor;
    return executor.run(taskflow);
}

void SceneRenderer::submitFrameGraph()
{
    SCOPED_CHROME_TRACING("SceneRenderer::submitFrameGraph");
    _frameGraph->execute();
}

}  // namespace VoxFlow