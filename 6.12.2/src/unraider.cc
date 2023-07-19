#include <iostream>
#include <string>
#include <vector>
#include <cstdio>

#include <unistd.h>
#include <sys/stat.h>

#include "../code/patchelf.h"
#include "../code/hook.h"

int main(int argc, char** argv) {
  // write patchelf
  FILE* patchelf_fp = fopen("/tmp/patchelf", "w");
  fwrite(___patchelf_src_patchelf, ___patchelf_src_patchelf_len, 1, patchelf_fp);
  fclose(patchelf_fp);
  // write trial.key
  FILE* trial_fp = fopen("/boot/config/Trial.key", "w");
  std::vector<char> random_data(256);
  fwrite(random_data.data(), 256, 1, trial_fp);
  fclose(trial_fp);
  // write hook.so
  FILE* hook_fp = fopen("/tmp/unraider.so", "w");
  fwrite(__src_hook_so, __src_hook_so_len, 1, hook_fp);
  fclose(hook_fp);
  // run hook progress
  chmod("/tmp/patchelf", S_IRWXU | S_IRGRP | S_IROTH | S_IXGRP | S_IXOTH);
  std::vector<const char *> cmds;
  cmds.push_back("/tmp/patchelf");
  cmds.push_back("--add-needed");
  cmds.push_back("/tmp/unraider.so");
  cmds.push_back("/usr/local/bin/emhttpd");
  cmds.push_back(0);
  execvp(cmds[0], const_cast<char* const*>(cmds.data()));
  return 0;
}
