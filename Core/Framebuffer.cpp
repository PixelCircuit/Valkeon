#include "Framebuffer.hpp"
#include <stdexcept>

void Framebuffer::create(VkDevice device, VkRenderPass renderPass,
                         const std::vector<VkImageView> &swapChainImageViews,
                         VkExtent2D extent) {
  framebuffers.resize(swapChainImageViews.size());

  for (size_t i = 0; i < swapChainImageViews.size(); i++) {
    VkImageView attachments[] = {swapChainImageViews[i]};

    VkFramebufferCreateInfo framebufferInfo{
        VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO};
    framebufferInfo.renderPass = renderPass;
    framebufferInfo.attachmentCount = 1;
    framebufferInfo.pAttachments = attachments;
    framebufferInfo.width = extent.width;
    framebufferInfo.height = extent.height;
    framebufferInfo.layers = 1;

    if (vkCreateFramebuffer(device, &framebufferInfo, nullptr,
                            &framebuffers[i]) != VK_SUCCESS) {
      throw std::runtime_error("Failed to create framebuffer!");
    }
  }
}

void Framebuffer::cleanup(VkDevice device) {
  for (auto framebuffer : framebuffers) {
    vkDestroyFramebuffer(device, framebuffer, nullptr);
  }
  framebuffers.clear();
}