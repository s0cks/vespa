#ifdef VESPA_HAS_JSON

#include <rapidjson/rapidjson.h>
#include <rapidjson/reader.h>
#include <rapidjson/stringbuffer.h>

#include <iostream>
#include <string>

#include "manifest.h"

struct ManifestJsonHandler {
  Manifest* manifest;
  std::string current_key = "";

  auto Null() -> bool {
    std::cout << "Null()\n";
    return true;
  }

  auto Bool(bool b) -> bool {
    std::cout << "Bool(" << (b ? "true" : "false") << ")\n";
    return true;
  }

  auto RawNumber(const char* str, rapidjson::SizeType length, bool copy) -> bool {
    std::string numStr(str, length);
    std::cout << "Raw Number Encountered: " << numStr << "\n";

    return true;  // Return true to keep parsing, false to abort
  }

  auto Int(int i) -> bool {
    std::cout << "Int(" << i << ")\n";
    return true;
  }

  auto Uint(unsigned u) -> bool {
    std::cout << "Uint(" << u << ")\n";
    return true;
  }

  auto Int64(int64_t i) -> bool {
    std::cout << "Int64(" << i << ")\n";
    return true;
  }

  auto Uint64(uint64_t u) -> bool {
    std::cout << "Uint64(" << u << ")\n";
    return true;
  }

  auto Double(double d) -> bool {
    std::cout << "Double(" << d << ")\n";
    return true;
  }

  auto String(const char* str, rapidjson::SizeType length, bool copy) -> bool {
    std::cout << "String(" << std::string(str, length) << ", length=" << length << ")\n";
    return true;
  }

  auto StartObject() -> bool {
    std::cout << "StartObject()\n";
    return true;
  }

  auto Key(const char* str, rapidjson::SizeType length, bool copy) -> bool {
    current_key = std::string(str, length);
    std::cout << "Key(" << current_key << ")\n";
    return true;
  }

  auto EndObject(rapidjson::SizeType memberCount) -> bool {
    std::cout << "EndObject(members=" << memberCount << ")\n";
    return true;
  }

  auto StartArray() -> bool {
    std::cout << "StartArray()\n";
    return true;
  }

  auto EndArray(rapidjson::SizeType elementCount) -> bool {
    std::cout << "EndArray(elements=" << elementCount << ")\n";
    return true;
  }
};

auto ManifestParseJson(Manifest* m, const char* data) -> bool {
  rapidjson::StringStream stream(data);
  rapidjson::Reader reader;
  ManifestJsonHandler handler{
      .manifest = m,
  };

  // Execute the SAX parsing operation
  if (reader.Parse<rapidjson::kParseDefaultFlags>(stream, handler)) {
    std::cout << "\nParsing completed successfully!\n";
    return true;
  }

  std::cerr << "\nParsing failed at offset: " << reader.GetErrorOffset() << "\n";
  return false;
}

#endif  // VESPA_HAS_JSON
