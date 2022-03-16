#include "chainblocks.h"
#include <bundled/entry.edn.h>
#include <spdlog/spdlog.h>

extern "C" {
CHAINBLOCKS_API __cdecl void *cbLispCreate(const char *path);
CHAINBLOCKS_API __cdecl void *cbLispCreateSub(void *parent);
CHAINBLOCKS_API __cdecl void cbLispDestroy(void *env);
CHAINBLOCKS_API __cdecl CBBool cbLispEval(void *env, const char *str,
                                          CBVar *output = nullptr);
}

int main() {
  auto env = cbLispCreate("");

  const char *code = (const char *)bundled_entry_edn_getData();
  std::string wrappedCode = fmt::format("(do {})\n", code);

  CBVar output{};
  cbLispEval(env, wrappedCode.c_str(), &output);

  return 0;
}