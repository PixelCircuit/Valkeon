#include "Applications/HelloTriangleApplication.hpp"
#include "Applications/ModelLoadingApplication.hpp"

#include <cstdlib>
#include <functional>


int main() {
  ModelLoadingApplication app;

  try {
    app.run();
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
