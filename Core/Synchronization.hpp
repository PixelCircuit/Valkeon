#pragma once

#include <vector>
#include <vulkan/vulkan.h>

class Synchronization {

public:
  Synchronization();
  void create(VkDevice device, uint32_t maxFramesInFlight);
  void cleanup(VkDevice device);

  VkSemaphore& acquireSemaphore(uint32_t index) { return imageAvailableSemaphores[index]; }
  const VkSemaphore& acquireSemaphore(uint32_t index) const { return imageAvailableSemaphores[index]; }

    VkSemaphore& renderSemaphore(uint32_t index) { return renderFinishedSemaphores[index]; }
    const VkSemaphore& renderSemaphore(uint32_t index) const { return renderFinishedSemaphores[index]; }

    VkFence& inFlightFence(uint32_t index) { return inFlightFences[index]; }
    const VkFence& inFlightFence(uint32_t index) const { return inFlightFences[index]; }

private:
  std::vector<VkSemaphore> imageAvailableSemaphores;
  std::vector<VkSemaphore> renderFinishedSemaphores;
  std::vector<VkFence> inFlightFences;
};