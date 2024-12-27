#pragma once

#include <vector>
#include <vulkan/vulkan.h>

class Pipeline {

public:
  void createBasicPipeline(VkDevice device, VkRenderPass renderPass,
                           VkExtent2D extent, VkPipelineLayout &pipelineLayout,
                           VkPipeline &pipeline);

private:
  VkShaderModule createShaderModule(VkDevice device,
                                    const std::vector<char> &code);
};