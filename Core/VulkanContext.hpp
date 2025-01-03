#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#include "GLFW/glfw3.h"

#include <CommandPool.hpp>
#include <string>
#include <vector>
#include <vulkan/vulkan.h>

class VulkanContext {
public:
  void createInstance(const std::vector<const char *> &extensions);
  void createSurface(VkInstance instance, GLFWwindow *window,
                     VkSurfaceKHR *surface);
  void pickPhysicalDevice(VkSurfaceKHR surface);
  void createLogicalDevice();
  void cleanup();
  uint32_t findMemoryType(VulkanContext &context, uint32_t typeFilter,
                        VkMemoryPropertyFlags properties);
  VkFormat findDepthFormat();

  void createDepthResources(VulkanContext& context,
                       uint32_t width, uint32_t height, VkFormat depthFormat,
                       VkImage& depthImage, VkDeviceMemory& depthImageMemory);
  void createImageView (VulkanContext& context, VkImage image, VkFormat format,
                    VkImageAspectFlags aspectFlags, VkImageView& imageView);

  void transitionImageLayout(VkDevice device, VkCommandPool commandPool,
                                  VkQueue graphicsQueue, VkImage image,
                                  VkFormat format, VkImageLayout oldLayout,
                                  VkImageLayout newLayout);


  VkInstance getInstance() const { return instance; }
  VkDevice getDevice() const { return device; }
  VkPhysicalDevice getPhysicalDevice() const { return physicalDevice; }
  VkQueue getGraphicsQueue() const { return graphicsQueue; }
  VkQueue getPresentQueue() const { return presentQueue; }
  uint32_t getGraphicsQueueFamilyIndex() const { return graphicsQueueFamilyIndex; }
  uint32_t getPresentQueueFamilyIndex() const { return presentQueueFamilyIndex; }
  std::vector<const char *> getRequiredExtensions();

  CommandPool commandPool;



private:
  VkInstance instance = VK_NULL_HANDLE;
  VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
  VkDevice device = VK_NULL_HANDLE;
  VkQueue graphicsQueue = VK_NULL_HANDLE;
  VkQueue presentQueue = VK_NULL_HANDLE;
  uint32_t graphicsQueueFamilyIndex = UINT32_MAX;
  uint32_t presentQueueFamilyIndex = UINT32_MAX;
  bool enableValidationLayers = true;
  std::vector<const char *> validationLayers = {"VK_LAYER_KHRONOS_validation"};
  std::vector<const char *> deviceExtensions = {
      VK_KHR_SWAPCHAIN_EXTENSION_NAME};

  bool checkValidationLayerSupport();
  VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

};