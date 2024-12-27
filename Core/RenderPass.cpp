#include "RenderPass.hpp"
#include <stdexcept>

void RenderPass::create(VkDevice device, VkFormat swapChainImageFormat) {
  VkAttachmentDescription colorAttachment{};
  colorAttachment.format = swapChainImageFormat;
  colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
  colorAttachment.loadOp =
      VK_ATTACHMENT_LOAD_OP_CLEAR; // Clear the framebuffer at the start
  colorAttachment.storeOp =
      VK_ATTACHMENT_STORE_OP_STORE; // Store the framebuffer after rendering
  colorAttachment.stencilLoadOp =
      VK_ATTACHMENT_LOAD_OP_DONT_CARE; // No stencil buffer
  colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  colorAttachment.initialLayout =
      VK_IMAGE_LAYOUT_UNDEFINED; // Don't care about previous layout
  colorAttachment.finalLayout =
      VK_IMAGE_LAYOUT_PRESENT_SRC_KHR; // Layout for presentation

  VkAttachmentReference colorAttachmentRef{};
  colorAttachmentRef.attachment = 0; // Attachment index
  colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkSubpassDescription subpass{};
  subpass.pipelineBindPoint =
      VK_PIPELINE_BIND_POINT_GRAPHICS; // Graphics pipeline
  subpass.colorAttachmentCount = 1;
  subpass.pColorAttachments = &colorAttachmentRef; // Color attachment

  VkSubpassDependency dependency{};
  dependency.srcSubpass =
      VK_SUBPASS_EXTERNAL;   // Implicit subpass before render pass
  dependency.dstSubpass = 0; // Our subpass
  dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.srcAccessMask = 0;
  dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

  VkRenderPassCreateInfo renderPassInfo{
      VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO};
  renderPassInfo.attachmentCount = 1;
  renderPassInfo.pAttachments = &colorAttachment;
  renderPassInfo.subpassCount = 1;
  renderPassInfo.pSubpasses = &subpass;
  renderPassInfo.dependencyCount = 1;
  renderPassInfo.pDependencies = &dependency;

  if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass) !=
      VK_SUCCESS) {
    throw std::runtime_error("Failed to create render pass!");
  }
}

void RenderPass::cleanup(VkDevice device) {
  if (renderPass != VK_NULL_HANDLE) {
    vkDestroyRenderPass(device, renderPass, nullptr);
    renderPass = VK_NULL_HANDLE;
  }
}