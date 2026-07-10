#include <stdio.h>
#include <stdlib.h>

#include "ipc.h"
#include "manifest.h"
#include "orchestrator.h"

int main(int argc, char** argv) {
  IpcServer server;
  IpcServerInit(&server, uv_loop_new());
  IpcServerRunDefault(&server);
  IpcServerFree(&server);

  // Manifest manifest;
  // const char* data = "{\"apiVersion\": \"v1\", \"kind\": \"Process\", \"metadata\": { \"name\": \"test\" } }";
  // if (!ManifestParseJson(&manifest, data)) {
  //   fprintf(stderr, "failed to parse manifest json\n");
  //   return EXIT_FAILURE;
  // }
  //
  // fprintf(stdout, "parsed %s manifest with name: %s\n", ManifestKindToStr(manifest.kind), manifest.name);
  //
  // Orchestrator orc;
  // OrchestratorInit(&orc);
  // const int status = OrchestratorRunDefault(&orc);
  // fprintf(stdout, "status: %d\n", status);
  //
  // while (true) {
  //   wl_display_dispatch(orc.display);
  // }
  //
  // OrchestratorFree(&orc);
  return EXIT_SUCCESS;
}
