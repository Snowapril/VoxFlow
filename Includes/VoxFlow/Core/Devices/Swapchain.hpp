// Author : snowapril

#ifndef VOXEL_FLOW_SWAPCHAIN_HPP
#define VOXEL_FLOW_SWAPCHAIN_HPP

#include <volk/volk.h>
#include <VoxFlow/Core/Utils/NonCopyable.hpp>
#include <glm/vec2.hpp>

namespace VoxFlow
{
class Instance;
class PhysicalDevice;
class Queue;

class SwapChain : NonCopyable
{
 public:
    explicit SwapChain(const Instance* instance,
                       const PhysicalDevice* physicalDevice,
                       const LogicalDevice* logicalDevice,
                       const Queue* presentSupportQueue, const char* title,
                       const glm::ivec2 resolution) noexcept;
    ~SwapChain();
    SwapChain(SwapChain&& other) noexcept;
    SwapChain& operator=(SwapChain&& other) noexcept;

    [[nodiscard]] inline VkSwapchainKHR get() const noexcept
    {
        return _swapChain;
    }

    [[nodiscard]] inline VkSurfaceKHR getSurface() const noexcept
    {
        return _surface;
    }

    bool create(bool vsync = false);

 private:
    VkSwapchainKHR _swapChain = VK_NULL_HANDLE;
    VkSurfaceKHR _surface = VK_NULL_HANDLE;
    GLFWwindow* _window = nullptr;
    Instance* _instance = nullptr;
    PhysicalDevice* _physicalDevice = nullptr;
    LogicalDevice* _logicalDevice = nullptr;
    Queue* _queue = nullptr;
    glm::ivec2* _resolution;
    VkFormat _surfaceFormat = VK_FORMAT_UNDEFINED;
    VkColorSpaceKHR _colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    std::vector<VkImage> _swapChainImages;
    std::vector<VkImageView> _swapChainImageViews;
};
}  // namespace VoxFlow

#endif