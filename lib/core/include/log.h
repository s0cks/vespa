#ifndef VESPA_LOG_H
#define VESPA_LOG_H

#define _LOG_TO_STREAM(Stream, format, ...) fprintf(Stream, "[%s:%d] " format "\n", __FILE__, __LINE__, ##__VA_ARGS__)

#define LOG_INFO(format, ...)               _LOG_TO_STREAM(stdout, format, ##__VA_ARGS__)
#define LOG_ERROR(format, ...)              _LOG_TO_STREAM(stderr, "error: " format, ##__VA_ARGS__)

#ifdef VESPA_DEBUG

#define LOG_DEBUG(format, ...) LOG_INFO(format, ##__VA_ARGS__)

#else

#define LOG_DEBUG(format, ...)

#endif  // VESPA_DEBUG

#endif  // VESPA_LOG_H
