// Efficiently find a Go project.

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>

#include <dirent.h>
#include <sys/types.h>

const std::vector<std::string> ignore_dirs{".git", ".", ".."};

int main(int argc, char **argv) {
  if (argc <= 1) {
    return 0;
  }
  const std::string gopath = getenv("GOPATH");
  if (gopath.empty()) {
    return 1;
  }
  const std::string target(argv[1]);
  std::vector<std::string> candidates{gopath + "/src"};
  while (!candidates.empty()) {
    // the current directory we're checking
    const std::string cur = candidates.back();
    candidates.pop_back();

    DIR *dir = opendir(cur.c_str());
    if (dir == nullptr) {
      perror("opendir()");
      return 1;
    }

    for (;;) {
      dirent *de = readdir(dir);
      if (de == nullptr) {
        break;
      }
      if (de->d_type != DT_DIR) {
        continue;
      }
      const std::string name(de->d_name);
      bool ignore = false;
      for (const auto &ignore_dir : ignore_dirs) {
        if (ignore_dir == name) {
          ignore = true;
          break;
        }
      }
      if (ignore) {
        continue;
      }

      std::ostringstream os;
      os << cur << '/' << name;
      if (name == target) {
        std::cout << os.str() << "\n";
        closedir(dir);
        return 0;
      }
      candidates.push_back(os.str());
    }
    closedir(dir);

    // sort so that the directory with the shortest name is at the back
    std::sort(candidates.begin(),
              candidates.end(),
              [](const std::string &a, const std::string &b) {
                return a.size() > b.size();
              });
  }
  return 0;
}
