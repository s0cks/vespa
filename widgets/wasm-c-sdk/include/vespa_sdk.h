#ifndef VESPA_WASM_H
#define VESPA_WASM_H

#include <stdbool.h>
#include <stdint.h>

#ifndef VESPA_SDK
#define VESPA_SDK extern
#endif  // VESPA_SDK

#define DECLARE_VESPA_SDK_FUNC(ReturnType, Name, Args) VESPA_SDK ReturnType Name Args

DECLARE_VESPA_SDK_FUNC(void, VespaPrint, (const char* message, uint32_t message_len));

#define _VESPA_WIDGET_FUNC(ReturnType, Name) __attribute__((visibility("default"))) ReturnType Name()

#define VESPA_WIDGET_INIT()                  _VESPA_WIDGET_FUNC(bool, init)
#define VESPA_WIDGET_VIEW()                  _VESPA_WIDGET_FUNC(bool, view)
#define VESPA_WIDGET_UPDATE()                _VESPA_WIDGET_FUNC(bool, update)

#endif  // VESPA_WASM_H
