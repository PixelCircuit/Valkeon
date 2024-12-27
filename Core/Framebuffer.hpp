#pragma once
#include <vector>
#include <vulkan/vulkan.h>

class Framebuffer {

public:
  void create(VkDevice device, VkRenderPass renderPass,
              const std::vector<VkImageView> &swapChainImageViews,
              VkExtent2D extent);
  void cleanup(VkDevice device);

  const std::vector<VkFramebuffer> &getFramebuffers() const {
    return framebuffers;
  }

private:
  std::vector<VkFramebuffer> framebuffers;
};