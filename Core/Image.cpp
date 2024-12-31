//
// Created by a00847261 on 2024-12-31.
//

#include "Image.hpp"

#include <VulkanContext.hpp>
#include <stdexcept>
#include <vector>
VkFormat Image::findSupportedDepthFormat(VulkanContext& context) {

  // Define candidate depth formats in order of preference
  std::vector<VkFormat> depthFormats = {
    VK_FORMAT_D32_SFLOAT,
    VK_FORMAT_D32_SFLOAT_S8_UINT,
    VK_FORMAT_D24_UNORM_S8_UINT
};

  for (VkFormat format : depthFormats) {
    VkFormatProperties props;
    vkGetPhysicalDeviceFormatProperties(context.getPhysicalDevice(), format, &props);

    // Check if format supports depth stencil attachment
    if (props.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
      return format;
    }
  }

  throw std::runtime_error("Failed to find a supported depth format!");
}


void Image::createDepthResources(VulkanContext &context, uint32_t width,
                                 uint32_t height, VkFormat depthFormat,
                                 VkImage &depthImage,
                                 VkDeviceMemory &depthImageMemory) {

  VkImageCreateInfo imageInfo{};
  imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  imageInfo.imageType = VK_IMAGE_TYPE_2D;
  imageInfo.format = depthFormat;
  imageInfo.extent.width = width;
  imageInfo.extent.height = height;
  imageInfo.extent.depth = 1; // 2D image
  imageInfo.mipLevels = 1;
  imageInfo.arrayLayers = 1;
  imageInfo.samples = VK_SAMPLE_COUNT_1_BIT; // No multisampling
  imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
  imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
  imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

  if (vkCreateImage(context.getDevice(), &imageInfo, nullptr, &depthImage) !=
      VK_SUCCESS) {
    throw std::runtime_error("Failed to create depth image!");
  }

  VkMemoryRequirements memRequirements;
  vkGetImageMemoryRequirements(context.getDevice(), depthImage,
                               &memRequirements);

  VkMemoryAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocInfo.allocationSize = memRequirements.size;
  allocInfo.memoryTypeIndex =
      context.findMemoryType(context, memRequirements.memoryTypeBits,
                             VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

  if (vkAllocateMemory(context.getDevice(), &allocInfo, nullptr,
                       &depthImageMemory) != VK_SUCCESS) {
    throw std::runtime_error("Failed to allocate depth image memory!");
  }

  vkBindImageMemory(context.getDevice(), depthImage, depthImageMemory, 0);
}
void Image::createImageView(VulkanContext &context, VkImage image,
                            VkFormat format, VkImageAspectFlags aspectFlags,
                            VkImageView &imageView) {
  VkImageViewCreateInfo viewInfo{};
  viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  viewInfo.image = image;
  viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
  viewInfo.format = format;
  viewInfo.subresourceRange.aspectMask = aspectFlags;
  viewInfo.subresourceRange.baseMipLevel = 0;
  viewInfo.subresourceRange.levelCount = 1;
  viewInfo.subresourceRange.baseArrayLayer = 0;
  viewInfo.subresourceRange.layerCount = 1;

  if(vkCreateImageView(context.getDevice(), &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
    throw std::runtime_error("Failed to create depth image view!");
  }
}