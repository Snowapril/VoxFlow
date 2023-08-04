// Author : snowapril

#include <VoxFlow/Core/Devices/LogicalDevice.hpp>
#include <VoxFlow/Core/Resources/RenderResourceGarbageCollector.hpp>
#include <VoxFlow/Core/Resources/RenderResourceMemoryPool.hpp>
#include <VoxFlow/Core/Resources/StagingBuffer.hpp>
#include <VoxFlow/Core/Utils/DebugUtil.hpp>
#include <VoxFlow/Core/Utils/Logger.hpp>

namespace VoxFlow
{
StagingBuffer::StagingBuffer(std::string&& debugName,
                             LogicalDevice* logicalDevice,
                             RenderResourceMemoryPool* renderResourceMemoryPool)
    : RenderResource(std::move(debugName), logicalDevice,
                     renderResourceMemoryPool)
{
}

StagingBuffer::~StagingBuffer()
{
    release();
}

bool StagingBuffer::makeAllocationResident(const BufferInfo& bufferInfo)
{
    VOX_ASSERT(bufferInfo._usage != BufferUsage::Unknown,
               "BufferUsage must be specified");

    _bufferInfo = bufferInfo;
    VkBufferCreateInfo bufferCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .size = bufferInfo._size,
        .usage =
            VK_BUFFER_USAGE_TRANSFER_SRC | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices = nullptr
    };

    VmaAllocationCreateInfo vmaCreateInfo = {
        .flags =
            VMA_ALLOCATION_CREATE_STRATEGY_MIN_MEMORY_BIT,  // TODO(snowapril) :
                                                            // choose best one
        .usage = VMA_MEMORY_USAGE_AUTO,
        .requiredFlags = 0,
        .preferredFlags = 0,
        .memoryTypeBits = 0,
        .pool = VK_NULL_HANDLE,
        .pUserData = nullptr
    };

    VK_ASSERT(vmaCreateBuffer(_renderResourceMemoryPool->get(),
                              &bufferCreateInfo, &vmaCreateInfo, &_vkBuffer,
                              &_allocation, nullptr));

    if (_vkBuffer == VK_NULL_HANDLE)
    {
        VOX_ASSERT(false, "Failed to initialize buffer({})", _debugName);
        return false;
    }

#if defined(VK_DEBUG_NAME_ENABLED)
    DebugUtil::setObjectName(_logicalDevice, _vkBuffer, _debugName.c_str());
#endif
    return true;
}

void StagingBuffer::release()
{
    if (_permanentMappedAddress != nullptr)
    {
        vmaUnmapMemory(_renderResourceMemoryPool->get(), _allocation);
        _permanentMappedAddress = nullptr;
    }

    if (_vkBuffer != VK_NULL_HANDLE)
    {
        VmaAllocator vmaAllocator = _renderResourceMemoryPool->get();
        VkBuffer vkBuffer = _vkBuffer;
        VmaAllocation vmaAllocation = _allocation;

        _logicalDevice->getRenderResourceGarbageCollector()
            ->pushRenderResourceGarbage(RenderResourceGarbage(
                std::move(_accessedFences),
                [vmaAllocator, vkBuffer, vmaAllocation]() {
                    vmaDestroyBuffer(vmaAllocator, vkBuffer, vmaAllocation);
                }));

        _vkBuffer = VK_NULL_HANDLE;
        _allocation = VK_NULL_HANDLE;
    }
}

void* StagingBuffer::map()
{
    VOX_ASSERT(
        static_cast<uint32_t>(_bufferInfo._usage & (BufferUsage::Readback |
                                                    BufferUsage::Upload)) > 0,
        "StagingBuffer without Readback flag must not be mapped");

    if (_permanentMappedAddress == nullptr)
    {
        void* memoryAddress = nullptr;
        VK_ASSERT(vmaMapMemory(_renderResourceMemoryPool->get(), _allocation,
                               &memoryAddress));
        _permanentMappedAddress = memoryAddress;
    }

    return _permanentMappedAddress;
}

void StagingBuffer::unmap()
{
    // TODO(snowapril) : consider unmap or not
}
}  // namespace VoxFlow