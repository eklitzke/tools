// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <queue>
#include <sstream>
#include <string>
#include <vector>

#include <dirent.h>
#include <sys/types.h>

class Compare {
public:
  bool operator()(const std::string &a, const std::string &b) {
    return a.size() > b.size();
  }
};

typedef std::priority_queue<std::string, std::vector<std::string>, Compare> pq;

std::string TrimPath(std::string path) {
  while (!path.empty() && path.back() == '/') {
    path = path.substr(0, path.size() - 1);
  }
  return path;
}

int main(int argc, char **argv) {
  if (argc <= 2) {
    return 0;
  }
  const std::string root = TrimPath(argv[1]);
  if (root.empty()) {
    return 1;
  }
  const std::string target(argv[2]);

  pq candidates;
  candidates.push(root);
  while (!candidates.empty()) {
    // the current directory we're checking
    const std::string &cur = candidates.top();

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
        subdirs.clear();
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
    candidates.pop();
    for (const auto &s : subdirs) {
      candidates.push(s);
    }
  }

  return 0;
}
