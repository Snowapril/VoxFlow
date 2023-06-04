// Author : snowapril

#include <VoxFlow/Core/Devices/LogicalDevice.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/ComputePipeline.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/PipelineLayout.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/ShaderModule.hpp>
#include <VoxFlow/Core/Utils/Logger.hpp>

namespace VoxFlow
{
ComputePipeline::ComputePipeline(LogicalDevice* logicalDevice,
                                 const char* shaderPath)
    : BasePipeline(logicalDevice, { shaderPath })
{
}

ComputePipeline::~ComputePipeline()
{
    // Do nothing
}

ComputePipeline::ComputePipeline(ComputePipeline&& other) noexcept
    : BasePipeline(std::move(other))
{
    // Do nothing
}

ComputePipeline& ComputePipeline::operator=(ComputePipeline&& other) noexcept
{
    if (&other != this)
    {
        BasePipeline::operator=(std::move(other));
    }
    return *this;
}

bool ComputePipeline::initialize()
{
    ShaderModule* computeShaderModule = _shaderModules.front().get();
    const VkPipelineShaderStageCreateInfo stageCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .stage = VK_SHADER_STAGE_COMPUTE_BIT,
        .module = computeShaderModule->get(),
        .pName = "main",
        .pSpecializationInfo = 0,
    };

    [[maybe_unused]] const VkComputePipelineCreateInfo pipelineInfo = {
        .sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .stage = stageCreateInfo,
        .layout = _pipelineLayout->get(),
        .basePipelineHandle = VK_NULL_HANDLE,
        .basePipelineIndex = -1
    };

    VK_ASSERT(vkCreateComputePipelines(_logicalDevice->get(), VK_NULL_HANDLE, 1,
                                       &pipelineInfo, nullptr, &_pipeline));

    return true;
}

}  // namespace VoxFlow