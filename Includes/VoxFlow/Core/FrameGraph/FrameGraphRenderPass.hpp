// Author : snowapril

#ifndef VOXEL_FLOW_FRAME_GRAPH_RENDER_PASS_HPP
#define VOXEL_FLOW_FRAME_GRAPH_RENDER_PASS_HPP

#include <VoxFlow/Core/FrameGraph/Resource.hpp>
#include <VoxFlow/Core/FrameGraph/ResourceHandle.hpp>
#include <VoxFlow/Core/Graphics/RenderPass/RenderPassParams.hpp>
#include <VoxFlow/Core/Resources/Handle.hpp>
#include <VoxFlow/Core/Utils/RendererCommon.hpp>
#include <array>
#include <string>

namespace VoxFlow
{

class RenderResourceAllocator;
class Texture;

namespace RenderGraph
{
struct FrameGraphRenderPass
{
    struct Attachments
    {
        union
        {
            std::array<ResourceHandle, MAX_RENDER_TARGET_COUNTS + 1> _array = {
                INVALID_RESOURCE_HANDLE,
            };
            struct
            {
                std::array<ResourceHandle, MAX_RENDER_TARGET_COUNTS> _colors;
                ResourceHandle _depthStencil;
            };
        };
    };
    struct Descriptor
    {
        Attachments _attachments;
        glm::uvec2 _viewportSize = glm::uvec2(0U, 0U);
        std::array<glm::vec4, MAX_RENDER_TARGET_COUNTS> _clearColors;
        float _clearDepth = 0.0f;
        uint8_t _clearStencil = 0;
        AttachmentMaskFlags _clearFlags = AttachmentMaskFlags::None;
        AttachmentMaskFlags _writableAttachment = AttachmentMaskFlags::All;
        uint8_t _numSamples = 1;
    };

    uint32_t _id = 0;
};
}  // namespace RenderGraph

}  // namespace VoxFlow

#endif