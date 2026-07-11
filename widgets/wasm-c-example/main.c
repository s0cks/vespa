#include "vespa_sdk.h"

VESPA_WIDGET_INIT() {
  VespaPrint("init", 4);
  return true;
}

VESPA_WIDGET_VIEW() {
  VespaPrint("view", 4);
  return true;
}

VESPA_WIDGET_UPDATE() {
  VespaPrint("update", 6);
  return true;
}
