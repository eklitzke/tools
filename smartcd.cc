// Efficiently find a project.

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>

#include <dirent.h>
#include <sys/types.h>

int main(int argc, char **argv) {
  if (argc <= 2) {
    return 0;
  }
  const std::string root(argv[1]);
  if (root.empty()) {
    return 1;
  }
  const std::string target(argv[2]);
  std::vector<std::string> candidates{root};
  while (!candidates.empty()) {
    // the current directory we're checking
    const std::string &cur = candidates.back();

    DIR *dir = opendir(cur.c_str());
    if (dir == nullptr) {
      perror("opendir()");
      return 1;
    }

    // Collect subdirectories into subdirs... but stop if we encounter a .git
    // directory at any point. If no .git directory is found then we'll merge
    // subdirs into candidates and resort;
    bool hadgit = false;
    std::vector<std::string> subdirs;
    for (;;) {
      dirent *de = readdir(dir);
      if (de == nullptr) {
        break;
      }
      if (de->d_type != DT_DIR) {
        continue;
      }
      if (strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0) {
        continue;
      }
      if (strcmp(de->d_name, ".git") == 0) {
        hadgit = true;
        continue;
      }

      if (strcmp(de->d_name, target.c_str()) == 0) {
        std::cout << cur << "/" << target << "\n";
        closedir(dir);
        return 0;
      }

      if (!hadgit) {
        std::ostringstream os;
        os << cur << '/' << de->d_name;
        subdirs.push_back(os.str());
      }
    }
    closedir(dir);
    candidates.pop_back();

    if (!hadgit) {
      candidates.insert(candidates.end(), subdirs.begin(), subdirs.end());

      // sort so that the directory with the shortest name is at the back
      std::sort(candidates.begin(),
                candidates.end(),
                [](const std::string &a, const std::string &b) {
                  return a.size() > b.size();
                });
    }
  }

  return 0;
}
