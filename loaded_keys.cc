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

#include <cstdio>
#include <unistd.h>

#include <iostream>
#include <string>

#define HAVE_USSH 1
#define HAVE_ED25519 2

static bool ends_with(std::string const &value, std::string const &ending) {
  if (ending.size() > value.size()) {
    return false;
  }
  return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

int main(int argc, char **argv) {
  // shortcut if there is no ssh-agent available
  if (getenv("SSH_AUTH_SOCK") == nullptr) {
    return 0;
  }
  int pipefd[2];
  if (pipe(pipefd) == -1) {
    perror("pipe()");
    return 1;
  }

  pid_t child = fork();
  if (child == -1) {
    perror("fork()");
    return 1;
  }

  if (child == 0) {
    // the child becomes ssh-add -l
    close(pipefd[0]);
    dup2(pipefd[1], 1);
    execlp("ssh-add", "ssh-add", "-l", nullptr);
  } else {
    // hacks to make cin be the pipe
    dup2(pipefd[0], 0);
    close(pipefd[1]);

    // parse the input
    int state = 0;
    std::string s;
    while (std::cin.good()) {
      std::getline(std::cin, s);
      if (s.find(" [Valid until ") != std::string::npos) {
        state |= HAVE_USSH;
      } else if (ends_with(s, " (ED25519)")) {
        state |= HAVE_ED25519;
      }
    }

    switch (state) {
    case HAVE_USSH:
      std::cout << " /";
      break;
    case HAVE_ED25519:
      std::cout << " \\";
      break;
    case (HAVE_USSH | HAVE_ED25519):
      std::cout << " +";
      break;
    }
    return 0;
  }
}
