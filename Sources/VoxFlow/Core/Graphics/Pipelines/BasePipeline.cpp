// Author : snowapril

#include <spdlog/spdlog.h>

#include <VoxFlow/Core/Devices/LogicalDevice.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/BasePipeline.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/GlslangUtil.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/PipelineLayout.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/ShaderModule.hpp>
#include <VoxFlow/Core/Utils/Logger.hpp>

namespace VoxFlow
{
BasePipeline::BasePipeline(LogicalDevice* logicalDevice,
                           std::vector<const char*>&& shaderFilePaths)
    : _logicalDevice(logicalDevice)
{
    std::vector<ShaderLayoutBinding> combinedLayoutBindings;
    for (const char* shaderPath : shaderFilePaths)
    {
        _shaderModules.push_back(
            std::make_unique<ShaderModule>(_logicalDevice, shaderPath));
        combinedLayoutBindings.push_back(
            _shaderModules.back()->getShaderLayoutBinding());
    }

    _pipelineLayout = std::make_unique<PipelineLayout>(_logicalDevice);

    if (_pipelineLayout->initialize(std::move(combinedLayoutBindings)) == false)
    {
        VOX_ASSERT(false, "Failed to initialize pipeline layout");
    }
}

BasePipeline::~BasePipeline()
{
    release();
}

BasePipeline::BasePipeline(BasePipeline&& other) noexcept
{
    operator=(std::move(other));
}

BasePipeline& BasePipeline::operator=(BasePipeline&& other) noexcept
{
    if (&other != this)
    {
        _logicalDevice = other._logicalDevice;
        _pipelineLayout.swap(other._pipelineLayout);
        _shaderModules.swap(other._shaderModules);
        _pipeline = other._pipeline;
        other._pipeline = VK_NULL_HANDLE;
    }
    return *this;
}

void BasePipeline::release()
{
    _shaderModules.clear();
    _pipelineLayout.reset();
    if (_pipeline != VK_NULL_HANDLE)
    {
        vkDestroyPipeline(_logicalDevice->get(), _pipeline, nullptr);
        _pipeline = VK_NULL_HANDLE;
    }
}

}  // namespace VoxFlow