#pragma once

#include <vector>
#include <vulkan/vulkan.h>

class CommandPool {

public:
  void create(VkDevice device, uint32_t graphicsQueueFamilyIndex);
  void allocateCommandBuffers(VkDevice device, uint32_t bufferCount,
                              std::vector<VkCommandBuffer> &commandBuffers);
  void cleanup(VkDevice device);

  VkCommandPool getCommandPool() const { return commandPool; }

private:
  VkCommandPool commandPool = VK_NULL_HANDLE;
};