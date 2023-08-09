// Author : snowapril

#ifndef VOXEL_FLOW_FRAME_GRAPH_IMPL_HPP
#define VOXEL_FLOW_FRAME_GRAPH_IMPL_HPP

#include <VoxFlow/Core/FrameGraph/FrameGraph.hpp>
#include <VoxFlow/Core/FrameGraph/FrameGraphPass.hpp>
#include <functional>

namespace VoxFlow
{

namespace FrameGraph
{
constexpr uint32_t EXECUTION_LAMBDA_SIZE_LIMIT = 1024U;

template <ResourceConcept ResourceDataType>
ResourceHandle FrameGraphBuilder::allocate(
    std::string_view&& resourceName,
    typename ResourceDataType::Descriptor&& initArgs)
{
    return _frameGraph->create<ResourceDataType>(std::move(resourceName),
                                                 std::move(initArgs));
}

template <typename PassDataType, typename SetupPhase, typename ExecutePhase>
const PassDataType& FrameGraph::addCallbackPass(std::string_view&& passName,
                                                SetupPhase&& setup,
                                                ExecutePhase&& execute)
{
    static_assert(sizeof(ExecutePhase) < EXECUTION_LAMBDA_SIZE_LIMIT,
                  "ExecutePhase() lambda captures too much data");

    std::unique_ptr<FrameGraphPass<PassDataType, ExecutePhase>> pass =
        std::make_unique<FrameGraphPass<PassDataType, ExecutePhase>>(
            std::forward<ExecutePhase>(execute));

    PassDataType& passData = pass->getPassData();

    _passNodes.emplace_back(
        new RenderPassNode(this, std::move(passName), std::move(pass)));

    FrameGraphBuilder builder(this, _passNodes.back());
    std::invoke(setup, builder, passData);

    return passData;
}

template <typename SetupPhase>
void FrameGraph::addPresentPass(std::string_view&& passName, SetupPhase&& setup,
                                SwapChain* swapChain,
                                const FrameContext& frameContext)
{
    _passNodes.emplace_back(new PresentPassNode(this, std::move(passName),
                                                swapChain, frameContext));

    FrameGraphBuilder builder(this, _passNodes.back());
    std::invoke(setup, builder);
    builder.setSideEffectPass();
}

template <ResourceConcept ResourceDataType>
ResourceHandle FrameGraph::create(
    std::string_view&& resourceName,
    typename ResourceDataType::Descriptor&& resourceDescArgs)
{
    VirtualResource* virtualResource =
        new Resource<ResourceDataType>(std::move(resourceDescArgs));

    ResourceHandle resourceHandle =
        static_cast<ResourceHandle>(_resources.size());

    _resourceSlots.push_back(
        { ._resourceIndex =
              static_cast<ResourceSlot::IndexType>(_resourceNodes.size()),
          ._nodeIndex = static_cast<ResourceSlot::IndexType>(_resources.size()),
          ._version = static_cast<ResourceSlot::VersionType>(0) });
    _resources.push_back(virtualResource);

    ResourceNode* resourceNode = new ResourceNode(
        &_dependencyGraph, std::move(resourceName), resourceHandle);
    _resourceNodes.push_back(resourceNode);

    return resourceHandle;
}

template <ResourceConcept ResourceDataType>
const typename ResourceDataType::Descriptor FrameGraph::getResourceDescriptor(
    ResourceHandle id) const
{
    const ResourceSlot& resourceSlot = getResourceSlot(id);
    auto resource = static_cast<Resource<ResourceDataType>*>(
        _resources[resourceSlot._resourceIndex]);
    return resource->getDescriptor();
}

}  // namespace FrameGraph

}  // namespace VoxFlow

#endif