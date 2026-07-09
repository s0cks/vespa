#include <stdio.h>
#include <stdlib.h>

#include "supervisor.h"

int main(int argc, char** argv) {
  Supervisor supervisor;
  if (!SupervisorInit(&supervisor)) {
    fprintf(stderr, "error: failed to initialize supervisor\n");
    return EXIT_FAILURE;
  }

  const int result = SupervisorRunDefault(&supervisor);
  if (result) {
    fprintf(stderr, "error: failed to run supervisor: %s\n", uv_strerror(result));
    return EXIT_FAILURE;
  }

  SupervisorFree(&supervisor);
  return EXIT_SUCCESS;
}
