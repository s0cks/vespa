#include <stdio.h>
#include <stdlib.h>

#include "ipc.h"
#include "log.h"
#include "manifest.h"
#include "orchestrator.h"

static inline char* DigestToStr(const uint8_t* bytes, const uint64_t nbytes) {
  char* digest = malloc(sizeof(char) * nbytes + 1);
  if (!digest)
    return NULL;

  memcpy(digest, bytes, nbytes);
  digest[nbytes] = '\0';
  return digest;
}

static inline bool OnPing(IpcServerClient* client, Message* message) {
  uint8_t* bytes = message->ping.digest;
  uint64_t nbytes = message->ping.digest_len;

#ifdef VESPA_DEBUG
  char* digest = DigestToStr(bytes, nbytes);
  if (!digest) {
    LOG_ERROR("failed to get digest str from ping");
    exit(1);
  }
  LOG_INFO("received ping: %s\n", (const char*)digest);
  free(digest);
#endif  // VESPA_DEBUG

  Message* pong = malloc(sizeof(Message));
  InitPongMessage(pong, bytes, nbytes);
  IpcServerClientWrite(client, pong);
  return true;
}

int main(int argc, char** argv) {
  IpcServer server;
  IpcServerInit(&server, uv_loop_new());
  server.OnPing = &OnPing;
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
