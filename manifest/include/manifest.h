#ifndef VESPA_MANIFEST_H
#define VESPA_MANIFEST_H

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

#define FOR_EACH_MANIFEST_TYPE(V) \
  V(Process)                      \
  V(Widget)

// clang-format off
typedef enum {
  kInvalidManifest = 0,
#define DEFINE_KIND(Name) k##Name##Manifest,
  FOR_EACH_MANIFEST_TYPE(DEFINE_KIND)
#undef DEFINE_KIND
  kTotalNumberOfManifestKinds,
} ManifestKind;
// clang-format on

static inline const char* ManifestKindToStr(ManifestKind rhs) {
  switch (rhs) {
#define DEFINE_TOSTRING(Name) \
  case k##Name##Manifest:     \
    return #Name;

    FOR_EACH_MANIFEST_TYPE(DEFINE_TOSTRING)
#undef DEFINE_TOSTRING

    case kInvalidManifest:
      return "Invalid";
    default:
      return "Unknown";
  }
}

typedef struct {
  ManifestKind kind;
  const char* apiVersion;
  const char* name;
} Manifest;

#ifdef VESPA_HAS_JSON
bool ManifestParseJson(Manifest* manifest, const char* data);
#endif  // VESPA_HAS_JSON

#ifdef VESPA_HAS_YAML
bool ManifestParseYaml(Manifest* manifest, const char* data);
#endif  // VESPA_HAS_YAML

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // VESPA_MANIFEST_H
