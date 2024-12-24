#include "Buffer.hpp"
#include "VulkanContext.hpp"

Buffer::Buffer(Buffer &&other) noexcept
    : buffer(other.buffer), bufferMemory(other.bufferMemory) {
  other.buffer = VK_NULL_HANDLE;
  other.bufferMemory = VK_NULL_HANDLE;
}

Buffer &Buffer::operator=(Buffer &&other) noexcept {
  if (this != &other) {
    buffer = other.buffer;
    bufferMemory = other.bufferMemory;

    other.buffer = VK_NULL_HANDLE;
    other.bufferMemory = VK_NULL_HANDLE;
  }
  return *this;
}

void Buffer::create(VulkanContext &context, VkDeviceSize size,
                    VkBufferUsageFlags usage,
                    VkMemoryPropertyFlags properties) {
  // Create buffer
  VkBufferCreateInfo bufferInfo{};
  bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferInfo.size = size;
  bufferInfo.usage = usage;
  bufferInfo.sharingMode =
      VK_SHARING_MODE_EXCLUSIVE; // Assume exclusive for simplicity

  if (vkCreateBuffer(context.getDevice(), &bufferInfo, nullptr, &buffer) !=
      VK_SUCCESS) {
    throw std::runtime_error("Failed to create buffer!");
  }

  // Get memory requirements
  VkMemoryRequirements memRequirements;
  vkGetBufferMemoryRequirements(context.getDevice(), buffer, &memRequirements);

  // Allocate memory
  VkMemoryAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocInfo.allocationSize = memRequirements.size;
  allocInfo.memoryTypeIndex =
      findMemoryType(context, memRequirements.memoryTypeBits, properties);

  if (vkAllocateMemory(context.getDevice(), &allocInfo, nullptr,
                       &bufferMemory) != VK_SUCCESS) {
    throw std::runtime_error("Failed to allocate buffer memory!");
  }

  // Bind memory to buffer
  vkBindBufferMemory(context.getDevice(), buffer, bufferMemory, 0);
}

void Buffer::cleanup(VkDevice device) {
  if (buffer != VK_NULL_HANDLE) {
    vkDestroyBuffer(device, buffer, nullptr);
  }
  if (bufferMemory != VK_NULL_HANDLE) {
    vkFreeMemory(device, bufferMemory, nullptr);
  }
}

uint32_t Buffer::findMemoryType(VulkanContext &context, uint32_t typeFilter,
                                VkMemoryPropertyFlags properties) {
  VkPhysicalDeviceMemoryProperties memProperties;
  vkGetPhysicalDeviceMemoryProperties(context.getPhysicalDevice(),
                                      &memProperties);

  for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
    if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags &
                                    properties) == properties) {
      return i;
    }
  }

  throw std::runtime_error("Failed to find suitable memory type!");
}