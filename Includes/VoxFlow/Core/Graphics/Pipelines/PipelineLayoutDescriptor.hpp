// Author : snowapril

#ifndef VOXEL_FLOW_SHADER_LAYOUT_BINDING_HPP
#define VOXEL_FLOW_SHADER_LAYOUT_BINDING_HPP

#include <volk/volk.h>
#include <VoxFlow/Core/Graphics/Descriptors/DescriptorSet.hpp>
#include <VoxFlow/Core/Graphics/Descriptors/DescriptorSetConfig.hpp>
#include <VoxFlow/Core/Utils/VertexFormat.hpp>
#include <array>
#include <vector>

namespace VoxFlow
{

struct VertexInputLayout
{
    uint32_t _location = 0;
    uint32_t _stride = 0;  //! # bytes
    VertexFormatBaseType _baseType = VertexFormatBaseType::Unknown;

    VkFormat getVkFormat() const;
};

struct FragmentOutputLayout
{
    uint32_t _location = 0;
    VkFormat _format = VK_FORMAT_UNDEFINED;
};

struct PipelineLayoutDescriptor
{
    std::unordered_map<std::string_view, ShaderVariable> _shaderVariablesMap;
    std::array<DescriptorSetLayoutDesc, MAX_NUM_SET_SLOTS> _sets{};
    std::vector<VertexInputLayout> _stageInputs;
    std::vector<FragmentOutputLayout> _stageOutputs;
    uint32_t _pushConstantSize = 0;
};

struct ShaderReflectionDataGroup
{
    std::unordered_map<std::string, ShaderVariable> _descriptors;
    std::vector<VertexInputLayout> _vertexInputLayouts;
    std::vector<FragmentOutputLayout> _fragmentOutputLayouts;
    uint32_t _pushConstantSize = 0;
    VkShaderStageFlagBits _stageFlagBit;

    ShaderReflectionDataGroup() = default;
    ~ShaderReflectionDataGroup() = default;
    ShaderReflectionDataGroup(const ShaderReflectionDataGroup& rhs);
    ShaderReflectionDataGroup(ShaderReflectionDataGroup&& rhs);
    ShaderReflectionDataGroup& operator=(const ShaderReflectionDataGroup& rhs);
    ShaderReflectionDataGroup& operator=(ShaderReflectionDataGroup&& rhs);
};
}  // namespace VoxFlow

template <>
struct std::hash<VoxFlow::VertexInputLayout>
{
    std::size_t operator()(
        VoxFlow::VertexInputLayout const& inputLayout) const noexcept;
};

template <>
struct std::hash<VoxFlow::FragmentOutputLayout>
{
    std::size_t operator()(
        VoxFlow::FragmentOutputLayout const& outputLayout) const noexcept;
};

template <>
struct std::hash<VoxFlow::PipelineLayoutDescriptor>
{
    std::size_t operator()(
        VoxFlow::PipelineLayoutDescriptor const& shaderLayout) const noexcept;
};

#endif