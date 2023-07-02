// Author : snowapril

#ifndef VOXEL_FLOW_LOGICAL_DEVICE_HPP
#define VOXEL_FLOW_LOGICAL_DEVICE_HPP

#include <volk/volk.h>
#include <VoxFlow/Core/Devices/Context.hpp>
#include <VoxFlow/Core/Devices/Queue.hpp>
#include <VoxFlow/Core/Utils/NonCopyable.hpp>
#include <VoxFlow/Core/Utils/RendererCommon.hpp>
#include <functional>
#include <glm/vec2.hpp>
#include <memory>
#include <unordered_map>

namespace VoxFlow
{
class SwapChain;
class RenderResourceMemoryPool;
class Buffer;
class Texture;
class PhysicalDevice;
class Instance;
class RenderPassCollector;
class FrameBufferCollector;
class DescriptorSetAllocatorPool;

class LogicalDevice : NonCopyable
{
 public:
    LogicalDevice(const Context& ctx, PhysicalDevice* physicalDevice,
                  Instance* instance);
    ~LogicalDevice() override;
    LogicalDevice(LogicalDevice&& other) noexcept;
    LogicalDevice& operator=(LogicalDevice&& other) noexcept;

    /**
     * @return created vulkan logical device
     */
    [[nodiscard]] VkDevice get() const noexcept
    {
        return _device;
    }

    /**
     * @param queueName queue name specified when created
     * @return vulkan queue wrapping class created
     */
    [[nodiscard]] Queue* getQueuePtr(const std::string& queueName);

    /**
     * @return render pass and framebuffer manager
     */
    [[nodiscard]] RenderPassCollector* getRenderPassCollector() const
    {
        return _renderPassCollector;
    }

    /**
     * @return descriptor set allocator pool which manage set and pool laid in
     * each layout
     */
    [[nodiscard]] DescriptorSetAllocatorPool* getDescriptorSetAllocatorPool()
        const
    {
        return _descriptorSetAllocatorPool;
    }

 public:
    /**
     * @param title swapchain window title name
     * @param resolution window resolution to create
     * @return swapchain wrappging class instance
     */
    std::shared_ptr<SwapChain> addSwapChain(const char* title,
                                            const glm::ivec2 resolution);

    /**
     * @param swapChainIndex swapchain index to querying
     * @return swapchain ref-counted instance matched to given index
     */
    [[nodiscard]] inline const std::shared_ptr<SwapChain>& getSwapChain(
        const uint32_t swapChainIndex) const
    {
        VOX_ASSERT(swapChainIndex < _swapChains.size(),
                   "Given Index({}), Num SwapChains({})", swapChainIndex,
                   _swapChains.size());
        return _swapChains[swapChainIndex];
    }

 public:
    /**
     * release resources which derived from this logical device
     */
    void releaseDedicatedResources();

    /**
     * release logical device and resources which derived from this logical
     * device.
     */
    void release();

 private:
    PhysicalDevice* _physicalDevice = nullptr;
    Instance* _instance = nullptr;
    VkDevice _device{ VK_NULL_HANDLE };
    std::unordered_map<std::string, Queue*> _queueMap{};
    Queue* _mainQueue = nullptr;
    std::vector<std::shared_ptr<SwapChain>> _swapChains;
    RenderResourceMemoryPool* _renderResourceMemoryPool = nullptr;
    RenderPassCollector* _renderPassCollector = nullptr;
    DescriptorSetAllocatorPool* _descriptorSetAllocatorPool = nullptr;
};
}  // namespace VoxFlow

#endif