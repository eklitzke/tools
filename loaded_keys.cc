// Display the loaded SSH keys for the bash prompt

#include <cstdio>
#include <unistd.h>

#include <iostream>
#include <string>
#include <set>

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
    std::string s;
    std::set<char> chars;
    while (std::cin.good()) {
      std::getline(std::cin, s);
      if (s.find(" [Valid until ") != std::string::npos) {
        chars.insert('u');
      } else if (ends_with(s, " (RSA-CERT)")) {
        chars.insert('r');
      } else if (ends_with(s, " (ED25519)")) {
        chars.insert('e');
      }
    }

    // print keys to cout
    if (!chars.empty()) {
      std::cout.put(' ');
      for (const auto &c : chars) {
        std::cout.put(c);
      }
    }
    return 0;
  }
}
