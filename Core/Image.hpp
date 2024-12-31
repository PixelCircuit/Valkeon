//
// Created by a00847261 on 2024-12-31.
//

#pragma once
#include <VulkanContext.hpp>
#include <vulkan/vulkan_core.h>

class Image {

private:
  VkFormat findSupportedDepthFormat(VulkanContext& context);

  void createDepthResources(VulkanContext& context,
                         uint32_t width, uint32_t height, VkFormat depthFormat,
                         VkImage& depthImage, VkDeviceMemory& depthImageMemory);
  void createImageView (VulkanContext& context, VkImage image, VkFormat format,
                    VkImageAspectFlags aspectFlags, VkImageView& imageView);
};
