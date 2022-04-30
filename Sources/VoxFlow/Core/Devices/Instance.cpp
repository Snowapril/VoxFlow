// Author : snowapril

#include <VoxFlow/Core/Devices/Instance.hpp>
#include <VoxFlow/Core/Utils/Logger.hpp>
#include <VoxFlow/Core/Utils/DecisionMaker.hpp>

namespace VoxFlow
{
Instance::Instance(const Context& ctx)
{
    VK_ASSERT(volkInitialize() == VK_SUCCESS);

    VkApplicationInfo appInfo = { .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
                                  .pNext = nullptr,
                                  .pApplicationName = ctx.appTitle.c_str(),
                                  .applicationVersion = VK_MAKE_VERSION(
                                      ctx.majorVersion, ctx.minorVersion, 0),
                                  .pEngineName = ctx.appEngine.c_str(),
                                  .engineVersion = 0,
                                  .apiVersion = 0 };

    uint32_t extensionCount;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    std::vector<VkExtensionProperties> extensionProperties(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount,
                                           extensionProperties.data());

    std::vector<const char*> usedExtensions;
    std::vector<void*> featureStructs;
    VK_ASSERT(DecisionMaker::pickExtensions(usedExtensions, extensionProperties,
                                            ctx.instanceExtensions,
                                            featureStructs) == VK_SUCCESS);

    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
    std::vector<VkLayerProperties> layerProperties(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, layerProperties.data());

    std::vector<const char*> usedLayers;
    VK_ASSERT(DecisionMaker::pickLayers(usedLayers, layerProperties,
                                        ctx.instanceLayers) == VK_SUCCESS);

    const VkInstanceCreateInfo instanceInfo = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .pApplicationInfo = &appInfo,
        .enabledLayerCount = static_cast<uint32_t>(usedLayers.size()),
        .ppEnabledLayerNames = usedLayers.data(),
        .enabledExtensionCount = static_cast<uint32_t>(usedExtensions.size()),
        .ppEnabledExtensionNames = usedExtensions.data()
    };

    VK_ASSERT(vkCreateInstance(&instanceInfo, nullptr, &_instance) ==
              VK_SUCCESS);
    volkLoadInstance(_instance);
}

Instance::~Instance()
{
    release();
}

Instance::Instance(Instance&& instance) noexcept
    : _instance(std::move(instance._instance))
{
    // Do nothing
}

Instance& Instance::operator=(Instance && instance) noexcept
{
    if (this != &instance)
    {
        _instance = std::move(instance._instance);
    }
    return *this;
}

void Instance::release()
{
    if (_instance != VK_NULL_HANDLE)
    {
        vkDestroyInstance(_instance, nullptr);
        _instance = VK_NULL_HANDLE;
    }
}
}