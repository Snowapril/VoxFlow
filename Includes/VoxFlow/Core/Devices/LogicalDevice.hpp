// Author : snowapril

#ifndef VOXEL_FLOW_LOGICAL_DEVICE_HPP
#define VOXEL_FLOW_LOGICAL_DEVICE_HPP

#include <volk/volk.h>
#include <VoxFlow/Core/Devices/Context.hpp>
#include <VoxFlow/Core/Devices/PhysicalDevice.hpp>
#include <VoxFlow/Core/Devices/Queue.hpp>
#include <VoxFlow/Core/Utils/NonCopyable.hpp>
#include <VoxFlow/Core/Utils/RendererCommon.hpp>
#include <memory>
#include <unordered_map>
#include <glm/vec2.hpp>
#include <functional>

namespace VoxFlow
{
class SwapChain;
class PhysicalDevice;
class Instance;

class LogicalDevice : NonCopyable
{
 public:
    LogicalDevice(const Context& ctx, PhysicalDevice* physicalDevice, Instance* instance);
    ~LogicalDevice() override;
    LogicalDevice(LogicalDevice&& other) noexcept;
    LogicalDevice& operator=(LogicalDevice&& other) noexcept;

    [[nodiscard]] VkDevice get() const noexcept
    {
        return _device;
    }
    [[nodiscard]] Queue* getQueuePtr(
        const std::string& queueName);

 public:
     // Create new swapChain with given desc.
    std::shared_ptr<SwapChain> addSwapChain(const char* title,
                                            const glm::ivec2 resolution);


    // Execute given executor for each swapchain created.
    void executeOnEachSwapChain(
        std::function<void(std::shared_ptr<SwapChain>)> swapChainExecutor);

 public:
     // Release logical device and resources which derived from current logical device.
    void release();

 private:
    PhysicalDevice* _physicalDevice = nullptr;
    Instance* _instance = nullptr;
    VkDevice _device{ VK_NULL_HANDLE };
    std::unordered_map<std::string, Queue*> _queueMap{};
    Queue* _mainQueue = nullptr;
    std::vector<std::shared_ptr<SwapChain>> _swapChains;
};
}  // namespace VoxFlow

#endif