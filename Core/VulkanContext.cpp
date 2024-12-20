#include "VulkanContext.hpp"
#include <cstring>
#include <iostream>
#include <stdexcept>

#ifdef _WIN32
#include <GLFW/glfw3.h> // for glfwGetRequiredInstanceExtensions
#endif

const std::vector<const char *> validationLayers = {
    "VK_LAYER_KHRONOS_validation"};

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

void VulkanContext::initialize(VkSurfaceKHR surface) {
  createInstance();
  pickPhysicalDevice(surface);
  createLogicalDevice();
}

void VulkanContext::cleanup() {
  if (device) {
    vkDestroyDevice(device, nullptr);
  }
  if (instance) {
    vkDestroyInstance(instance, nullptr);
  }
}

void VulkanContext::createInstance() {
  if (enableValidationLayers && !checkValidationLayerSupport()) {
    throw std::runtime_error("Validation layers requested but not available!");
  }

  VkApplicationInfo appInfo{VK_STRUCTURE_TYPE_APPLICATION_INFO};
  appInfo.pApplicationName = "Hello Triangle";
  appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.pEngineName = "Valkeon";
  appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.apiVersion = VK_API_VERSION_1_1;

  uint32_t glfwExtensionCount = 0;
  const char **glfwExtensions =
      glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

  VkInstanceCreateInfo createInfo{VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};
  createInfo.pApplicationInfo = &appInfo;
  createInfo.enabledExtensionCount = glfwExtensionCount;
  createInfo.ppEnabledExtensionNames = glfwExtensions;

  if (enableValidationLayers) {
    createInfo.enabledLayerCount = (uint32_t)validationLayers.size();
    createInfo.ppEnabledLayerNames = validationLayers.data();
  } else {
    createInfo.enabledLayerCount = 0;
  }

  if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
    throw std::runtime_error("Failed to create Vulkan instance!");
  }
}

bool VulkanContext::checkValidationLayerSupport() {
  uint32_t layerCount;
  vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
  std::vector<VkLayerProperties> layers(layerCount);
  vkEnumerateInstanceLayerProperties(&layerCount, layers.data());

  for (const char *layerName : validationLayers) {
    bool found = false;
    for (auto &layerProp : layers) {
      if (strcmp(layerName, layerProp.layerName) == 0) {
        found = true;
        break;
      }
    }
    if (!found)
      return false;
  }
  return true;
}

void VulkanContext::pickPhysicalDevice(VkSurfaceKHR surface) {
  uint32_t deviceCount = 0;
  vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
  if (deviceCount == 0) {
    throw std::runtime_error("No Vulkan-compatible GPUs found!");
  }

  std::vector<VkPhysicalDevice> devices(deviceCount);
  vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

  for (auto &dev : devices) {
    // Check for needed queue families
    uint32_t queueCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(dev, &queueCount, nullptr);
    std::vector<VkQueueFamilyProperties> queueFamilies(queueCount);
    vkGetPhysicalDeviceQueueFamilyProperties(dev, &queueCount,
                                             queueFamilies.data());

    int graphicsIndex = -1;
    int presentIndex = -1;
    for (int i = 0; i < (int)queueFamilies.size(); i++) {
      if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
        graphicsIndex = i;
      }
      VkBool32 presentSupport = false;
      vkGetPhysicalDeviceSurfaceSupportKHR(dev, i, surface, &presentSupport);
      if (presentSupport) {
        presentIndex = i;
      }
      if (graphicsIndex >= 0 && presentIndex >= 0)
        break;
    }

    if (graphicsIndex >= 0 && presentIndex >= 0) {
      physicalDevice = dev;
      graphicsQueueFamilyIndex = (uint32_t)graphicsIndex;
      presentQueueFamilyIndex = (uint32_t)presentIndex;
      return;
    }
  }

  throw std::runtime_error(
      "Failed to find a suitable GPU with required queue families!");
}

void VulkanContext::createLogicalDevice() {
  float queuePriority = 1.0f;
  std::vector<VkDeviceQueueCreateInfo> queueInfos;

  VkDeviceQueueCreateInfo graphicsQueueInfo{
      VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO};
  graphicsQueueInfo.queueFamilyIndex = graphicsQueueFamilyIndex;
  graphicsQueueInfo.queueCount = 1;
  graphicsQueueInfo.pQueuePriorities = &queuePriority;
  queueInfos.push_back(graphicsQueueInfo);

  if (presentQueueFamilyIndex != graphicsQueueFamilyIndex) {
    VkDeviceQueueCreateInfo presentQueueInfo{
        VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO};
    presentQueueInfo.queueFamilyIndex = presentQueueFamilyIndex;
    presentQueueInfo.queueCount = 1;
    presentQueueInfo.pQueuePriorities = &queuePriority;
    queueInfos.push_back(presentQueueInfo);
  }

  VkPhysicalDeviceFeatures deviceFeatures{};
  VkDeviceCreateInfo createInfo{VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO};
  createInfo.queueCreateInfoCount = (uint32_t)queueInfos.size();
  createInfo.pQueueCreateInfos = queueInfos.data();
  createInfo.pEnabledFeatures = &deviceFeatures;

  // Swapchain extension is needed
  const char *deviceExtensions[] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
  createInfo.enabledExtensionCount = 1;
  createInfo.ppEnabledExtensionNames = deviceExtensions;

  if (enableValidationLayers) {
    createInfo.enabledLayerCount = (uint32_t)validationLayers.size();
    createInfo.ppEnabledLayerNames = validationLayers.data();
  } else {
    createInfo.enabledLayerCount = 0;
  }

  if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) !=
      VK_SUCCESS) {
    throw std::runtime_error("Failed to create logical device!");
  }

  vkGetDeviceQueue(device, graphicsQueueFamilyIndex, 0, &graphicsQueue);
  vkGetDeviceQueue(device, presentQueueFamilyIndex, 0, &presentQueue);
}