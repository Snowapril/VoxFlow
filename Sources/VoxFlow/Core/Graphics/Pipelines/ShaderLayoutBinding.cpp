// Author : snowapril

#include <VoxFlow/Core/Graphics/Pipelines/ShaderLayoutBinding.hpp>

namespace VoxFlow
{
}  // namespace VoxFlow

std::size_t std::hash<VoxFlow::ShaderLayoutBinding>::operator()(
    VoxFlow::ShaderLayoutBinding const& shaderLayout) const noexcept
{
    uint32_t seed = 0;

    for (const VoxFlow::DescriptorSetLayoutDesc& desc : shaderLayout._sets)
    {
        VoxFlow::hash_combine(seed, desc);
    }

    for (const VoxFlow::ShaderLayoutBinding::VertexInputLayout& inputLayout :
         shaderLayout._stageInputs)
    {
        VoxFlow::hash_combine(seed, static_cast<uint32_t>(inputLayout._format));
        VoxFlow::hash_combine(seed, inputLayout._size);
    }

    for (const VoxFlow::ShaderLayoutBinding::VertexInputLayout& outputLayout :
         shaderLayout._stageOutputs)
    {
        VoxFlow::hash_combine(seed,
                              static_cast<uint32_t>(outputLayout._format));
        VoxFlow::hash_combine(seed, outputLayout._size);
    }

    VoxFlow::hash_combine(seed, shaderLayout._pushConstantSize);
    return seed;
}