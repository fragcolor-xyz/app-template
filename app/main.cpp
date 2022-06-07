#include "shards.h"
#include <bundled/entry.edn.h>
#include <spdlog/spdlog.h>

extern "C" {
SHARDS_API __cdecl void *shLispCreate(const char *path);
SHARDS_API __cdecl void *shLispCreateSub(void *parent);
SHARDS_API __cdecl void shLispDestroy(void *env);
SHARDS_API __cdecl SHBool shLispEval(void *env, const char *str,
                                     SHVar *output = nullptr);
}

int main() {
  auto env = shLispCreate("");

  const char *code = (const char *)bundled_entry_edn_getData();
  std::string wrappedCode = fmt::format("(do {})\n", code);

  SHVar output{};
  shLispEval(env, wrappedCode.c_str(), &output);

  return 0;
}