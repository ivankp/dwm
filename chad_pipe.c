#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>

#include "chad_pipe.h"

#define STR1(X) #X
#define STR(X) STR1(X)

#define ERR(FNAME) \
  fprintf(stderr, STR(__LINE__) ": " FNAME "(): %s\n", strerror(errno))

// https://gist.github.com/yalue/cbc2a246bcc7e71824c45f31cf48cee8

int chad_pipe(char* buf, size_t len, char* const prog[]) {
  int pipe1[2], pipe2[2];
  if (pipe(pipe1) != 0) { ERR("pipe"); return -1; }
  if (pipe(pipe2) != 0) { ERR("pipe"); return -1; }

  pid_t pid = fork();
  if (pid < 0) { ERR("fork"); return -1; }
  if (pid == 0) { // this is the child process
    close(pipe1[1]); // close the write end
    close(pipe2[0]); // close the read end
    if (dup2(pipe1[0], STDIN_FILENO ) < 0) { ERR("dup2"); return -1; }
    if (dup2(pipe2[1], STDOUT_FILENO) < 0) { ERR("dup2"); return -1; }
    if (execvp(prog[0],prog) < 0) { ERR("execvp"); return -1; }
    // child process is replaced by exec()
  }

  // this is the original process
  close(pipe1[0]); // close the read end
  close(pipe2[1]); // close the write end
  if (write(pipe1[1], buf, strlen(buf)+1) <= 0) { ERR("write"); return -1; }
  close(pipe1[1]); // send EOF
  wait(NULL); // wait for the child

  ssize_t nread = read(pipe2[0],buf,sizeof(buf));
  close(pipe2[0]);
  if (nread < 0) { ERR("read"); return -1; }
  if (nread) {
    if (nread >= sizeof(buf)) nread = sizeof(buf)-1;
    buf[nread] = '\0';
  }

  return 0;
}
