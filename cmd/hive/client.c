#include <stdio.h>
#include <stdlib.h>

#include "ipc.h"

int main(int argc, char** argv) {
  IpcClient client;
  IpcClientInit(&client, uv_loop_new());
  IpcClientRunDefault(&client);
  IpcClientFree(&client);
  return EXIT_SUCCESS;
}
