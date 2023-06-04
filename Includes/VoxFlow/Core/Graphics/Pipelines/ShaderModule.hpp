// Author : snowapril

#ifndef VOXEL_FLOW_SHADER_MODULE_HPP
#define VOXEL_FLOW_SHADER_MODULE_HPP

#include <volk/volk.h>
#include <VoxFlow/Core/Graphics/Commands/CommandBuffer.hpp>
#include <VoxFlow/Core/Graphics/Descriptors/DescriptorSet.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/ShaderLayoutBinding.hpp>
#include <VoxFlow/Core/Utils/NonCopyable.hpp>
#include <array>
#include <memory>

namespace VoxFlow
{
class LogicalDevice;

class ShaderModule : private NonCopyable
{
 public:
    explicit ShaderModule(LogicalDevice* logicalDevice,
                          const char* shaderFilePath);
    ~ShaderModule() override;
    ShaderModule(ShaderModule&& other) noexcept;
    ShaderModule& operator=(ShaderModule&& other) noexcept;

    [[nodiscard]] inline VkShaderModule get() const noexcept
    {
        return _shaderModule;
    }

    [[nodiscard]] inline VkShaderStageFlagBits getStageFlagBits() const
    {
        return _stageFlagBits;
    }

    // Get reflected shader layout binding of thie module
    [[nodiscard]] inline const ShaderLayoutBinding& getShaderLayoutBinding()
        const
    {
        return _shaderLayoutBinding;
    }

    void release();

 private:
    // Reflect shader layout bindings from spirv binary data
    static bool reflectShaderLayoutBindings(
        ShaderLayoutBinding* shaderLayoutBinding,
        std::vector<uint32_t>&& spirvCodes, VkShaderStageFlagBits stageBits);

 protected:
    LogicalDevice* _logicalDevice;
    VkShaderModule _shaderModule = VK_NULL_HANDLE;
    ShaderLayoutBinding _shaderLayoutBinding;
    const char* _shaderFilePath = nullptr;
    VkShaderStageFlagBits _stageFlagBits;
};
}  // namespace VoxFlow

#endif