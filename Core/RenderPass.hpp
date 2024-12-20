#pragma once
#include <vulkan/vulkan.h>

class RenderPass {

public:
  void create(VkDevice device, VkFormat swapChainImageFormat);
  void cleanup(VkDevice device);

  VkRenderPass getRenderPass() const { return renderPass; }

private:
  VkRenderPass renderPass = VK_NULL_HANDLE;
};