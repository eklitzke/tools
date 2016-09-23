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
#include <iostream>

#include <fstream>
#include <sstream>
#include <string>
#include <vector>


std::vector<std::string> SplitString(const std::string &s, char delim) {
  std::istringstream ss(s);
  std::string item;
  std::vector<std::string> tokens;
  while (std::getline(ss, item, delim)) {
    tokens.push_back(item);
  }
  return tokens;
}

std::string JoinString(const std::vector<std::string> &vs, char delim) {
  std::ostringstream ss;
  const auto end = vs.end() - 1;
  for (auto it = vs.begin(); it != end; ++it) {
    ss << *it << delim;
  }
  ss << *end;
  return ss.str();
}

int main(int argc, char **argv) {
  size_t max_size = 30;
  if (argc >= 2) {
    max_size = static_cast<size_t>(std::strtoul(argv[1], nullptr, 10));
  }
  std::string pwd(getenv("PWD"));
  std::ofstream of("/home/evan/log", std::ofstream::out);
  if (!of.good()) {
    return 1;
  }
  of << pwd;
  of.close();

  if (pwd.empty()) {
    return 1;
  }
  const std::string home_dir(getenv("HOME"));
  if (pwd.substr(0, home_dir.size()) == home_dir) {
    pwd = std::string("~") + pwd.substr(home_dir.size());
  }
  if (pwd.size() <= max_size) {
    std::cout << pwd << "\n";
    return 0;
  }
  std::string candidate;
  std::vector<std::string> parts = SplitString(pwd, '/');
  for (size_t i = 0; i < parts.size() -1; i++) {
    parts[i] = parts[i][0];
    candidate = JoinString(parts, '/');
    if (candidate.size() <= max_size) {
      std::cout << candidate << "\n";
      return 0;
    }
  }
  std::cout << candidate << "\n";
  return 0;
}
