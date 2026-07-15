#ifndef VESPA_LOG_H
#define VESPA_LOG_H

#include <stdarg.h>

#ifndef UV_OK
#define UV_OK 0
#endif  // UV_OK

#define _LOG_TO_STREAM(Stream, format, ...) fprintf(Stream, "[%s:%d] " format "\n", __FILE__, __LINE__, ##__VA_ARGS__)

#define LOG_INFO(format, ...)               _LOG_TO_STREAM(stdout, format, ##__VA_ARGS__)
#define LOG_ERROR(format, ...)              _LOG_TO_STREAM(stderr, "error: " format, ##__VA_ARGS__)

#ifdef VESPA_DEBUG

#define LOG_DEBUG(format, ...) LOG_INFO(format, ##__VA_ARGS__)

#else

#define LOG_DEBUG(format, ...)

#endif  // VESPA_DEBUG

#ifndef CHECK_UV
#define CHECK_UV(Status, Format, ...)                               \
  ({                                                                \
    const int status = (Status);                                    \
    if (status != UV_OK)                                            \
      LOG_ERROR(Format ": %s", ##__VA_ARGS__, uv_strerror(status)); \
  })
#endif  // CHECK_UV

#endif  // VESPA_LOG_H
