
#pragma once

#include <vector>
#include <vulkan/vulkan.h>

class VulkanContext; // forward declaration

class Swapchain {

public:
  void create(VulkanContext &context, VkSurfaceKHR surface, uint32_t width,
              uint32_t height);
  void cleanup(VkDevice device);

  VkFormat getFormat() const { return swapchainFormat; }
  VkExtent2D getExtent() const { return swapchainExtent; }
  VkSwapchainKHR getSwapchain() const { return swapchain; }
  const std::vector<VkImageView> &getImageViews() const {
    return swapchainImageViews;
  }

private:
  VkSwapchainKHR swapchain = VK_NULL_HANDLE;
  VkFormat swapchainFormat;
  VkExtent2D swapchainExtent;
  std::vector<VkImageView> swapchainImageViews;

  void createImageViews(VkDevice device, const std::vector<VkImage> &images);
};