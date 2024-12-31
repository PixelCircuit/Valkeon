#pragma once

#include <VulkanContext.hpp>
#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <vulkan/vulkan.h>

class Buffer {

public:
  Buffer() = default;
  ~Buffer() = default;

  // Delete copy constructor and copy assignment
  Buffer(const Buffer &) = delete;
  Buffer &operator=(const Buffer &) = delete;

  // Allow move semantics
  Buffer(Buffer &&other) noexcept;
  Buffer &operator=(Buffer &&other) noexcept;

  // Creates a buffer and allocates memory
  void create(VulkanContext &context, VkDeviceSize size,
              VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);

  // Cleans up the buffer and frees memory
  void cleanup(VkDevice device);

  // Returns the buffer handle
  VkBuffer getBuffer() const { return buffer; }

  // Returns the memory handle
  VkDeviceMemory getMemory() const { return bufferMemory; }

private:
  VkBuffer buffer = VK_NULL_HANDLE;
  VkDeviceMemory bufferMemory = VK_NULL_HANDLE;


};
