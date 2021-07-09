#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <pthread.h>
#include <sys/epoll.h>
#include <sys/inotify.h>

#include "status.h"

static int epoll;
struct file_list_entry {
  const char* name;
  int wd;
};
static struct file_list_entry files[] = {
  /* { "/sys/class/power_supply/BAT0/capacity", 0 } */
  { "/home/ivanp/test", 0 }
};

static void errptr(const char* fcn_name, int line) {
  fprintf(stderr,"%d: %s(): %s\n",line,fcn_name,strerror(errno));
}
#define ERR(FCN_NAME) errptr(FCN_NAME,__LINE__)

static int read_file(const char* fname, char* buff, int n) {
  const int fd = open(fname, O_RDONLY);
  if (fd == -1) {
    ERR("open");
    return -1;
  }
  struct stat sb;
  if (fstat(fd,&sb) < 0) {
    ERR("fstat");
    return -1;
  }
  if (!S_ISREG(sb.st_mode)) {
    fprintf(stderr,"not a regular file");
    return -1;
  }
  if (sb.st_size > n) {
    fprintf(stderr,"file is larger than expected");
    return -1;
  }
  ssize_t nread = read(fd,buff,n);
  if (nread < 0) {
    ERR("read");
    return -1;
  }
  buff[nread] = '\0';
  close(fd);
  return 0;
}

static void* epoll_loop(void* arg) {
  struct epoll_event e;
  char inotify_buffer[sizeof(struct inotify_event)+64];
  for (;;) {
    int n = epoll_wait((int)epoll, &e, 1, -1);
    if (n < 0) ERR("epoll_wait");
    else while (n--) {
      /* uint32_t flags = e.events; */

      read(e.data.fd, inotify_buffer, sizeof(inotify_buffer));
      /* struct inotify_event* event = (struct inotify_event*)inotify_buffer; */

      char str[32];
      if (!read_file(files[0].name,str,31))
        updatestatus2(str);
    }
  }
  return NULL;
}

static int epoll_add(int epoll, int fd) {
  struct epoll_event event = {
    .events = EPOLLIN | EPOLLET,
    .data = { .fd = fd }
  };
  return epoll_ctl(epoll,EPOLL_CTL_ADD,fd,&event);
}

void poll_status(void) {
  epoll = epoll_create1(0);
  if (epoll == -1) {
    ERR("epoll_create1");
    return;
  }

  int inot = inotify_init();
  if (inot == -1) {
    ERR("inotify_init");
    return;
  }

  if (( files[0].wd = inotify_add_watch(
    inot, files[0].name, IN_MODIFY | IN_CREATE
  )) == -1) {
    ERR("inotify_add_watch");
    return;
  }

  if (epoll_add(epoll,inot) == -1) {
    ERR("epoll_add");
    return;
  }

  pthread_t thread_id;
  pthread_create(&thread_id, NULL, epoll_loop, NULL);
}
