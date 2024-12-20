#include "Swapchain.hpp"
#include "VulkanContext.hpp"
#include <stdexcept>

struct SwapchainSupportDetails {
  VkSurfaceCapabilitiesKHR capabilities{};
  std::vector<VkSurfaceFormatKHR> formats;
  std::vector<VkPresentModeKHR> presentModes;
};

static SwapchainSupportDetails querySwapchainSupport(VkPhysicalDevice device,
                                                     VkSurfaceKHR surface) {
  SwapchainSupportDetails details;
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface,
                                            &details.capabilities);

  uint32_t formatCount = 0;
  vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
  details.formats.resize(formatCount);
  vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount,
                                       details.formats.data());

  uint32_t presentCount = 0;
  vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentCount,
                                            nullptr);
  details.presentModes.resize(presentCount);
  vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentCount,
                                            details.presentModes.data());

  return details;
}

static VkSurfaceFormatKHR chooseSwapSurfaceFormat(
    const std::vector<VkSurfaceFormatKHR> &availableFormats) {
  for (auto &format : availableFormats) {
    if (format.format == VK_FORMAT_B8G8R8A8_UNORM &&
        format.colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR) {
      return format;
    }
  }
  return availableFormats.front();
}

static VkPresentModeKHR
chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &modes) {
  // Prefer MAILBOX if available
  for (auto &mode : modes) {
    if (mode == VK_PRESENT_MODE_MAILBOX_KHR)
      return mode;
  }
  return VK_PRESENT_MODE_FIFO_KHR; // guaranteed to be available
}

static VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &caps,
                                   uint32_t width, uint32_t height) {
  if (caps.currentExtent.width != UINT32_MAX) {
    return caps.currentExtent;
  } else {
    VkExtent2D actualExtent = {width, height};
    actualExtent.width =
        std::max(caps.minImageExtent.width,
                 std::min(caps.maxImageExtent.width, actualExtent.width));
    actualExtent.height =
        std::max(caps.minImageExtent.height,
                 std::min(caps.maxImageExtent.height, actualExtent.height));
    return actualExtent;
  }
}

void Swapchain::create(VulkanContext &context, VkSurfaceKHR surface,
                       uint32_t width, uint32_t height) {
  auto support = querySwapchainSupport(context.getPhysicalDevice(), surface);
  auto surfaceFormat = chooseSwapSurfaceFormat(support.formats);
  auto presentMode = chooseSwapPresentMode(support.presentModes);
  auto extent = chooseSwapExtent(support.capabilities, width, height);

  uint32_t imageCount = support.capabilities.minImageCount + 1;
  if (support.capabilities.maxImageCount > 0 &&
      imageCount > support.capabilities.maxImageCount) {
    imageCount = support.capabilities.maxImageCount;
  }

  VkSwapchainCreateInfoKHR createInfo{
      VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR};
  createInfo.surface = surface;
  createInfo.minImageCount = imageCount;
  createInfo.imageFormat = surfaceFormat.format;
  createInfo.imageColorSpace = surfaceFormat.colorSpace;
  createInfo.imageExtent = extent;
  createInfo.imageArrayLayers = 1;
  createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

  uint32_t queueFamilyIndices[] = {context.getGraphicsQueueFamilyIndex(),
                                   context.getGraphicsQueueFamilyIndex()};
  createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
  createInfo.queueFamilyIndexCount = 1;
  createInfo.pQueueFamilyIndices = queueFamilyIndices;

  createInfo.preTransform = support.capabilities.currentTransform;
  createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  createInfo.presentMode = presentMode;
  createInfo.clipped = VK_TRUE;

  VkDevice device = context.getDevice();
  if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapchain) !=
      VK_SUCCESS) {
    throw std::runtime_error("Failed to create swapchain!");
  }

  uint32_t actualImageCount = 0;
  vkGetSwapchainImagesKHR(device, swapchain, &actualImageCount, nullptr);
  std::vector<VkImage> images(actualImageCount);
  vkGetSwapchainImagesKHR(device, swapchain, &actualImageCount, images.data());

  swapchainFormat = surfaceFormat.format;
  swapchainExtent = extent;

  createImageViews(device, images);
}

void Swapchain::createImageViews(VkDevice device,
                                 const std::vector<VkImage> &images) {
  swapchainImageViews.resize(images.size());
  for (size_t i = 0; i < images.size(); i++) {
    VkImageViewCreateInfo viewInfo{VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
    viewInfo.image = images[i];
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = swapchainFormat;
    viewInfo.components = {
        VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY,
        VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY};
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    if (vkCreateImageView(device, &viewInfo, nullptr,
                          &swapchainImageViews[i]) != VK_SUCCESS) {
      throw std::runtime_error(
          "Failed to create image view for swapchain image!");
    }
  }
}

void Swapchain::cleanup(VkDevice device) {
  for (auto iv : swapchainImageViews) {
    vkDestroyImageView(device, iv, nullptr);
  }
  if (swapchain) {
    vkDestroySwapchainKHR(device, swapchain, nullptr);
  }
}