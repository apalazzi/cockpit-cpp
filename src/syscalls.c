//
// This file is part of the µOS++ III distribution.
// Parts of this file are from the newlib sources, issued under GPL.
// Copyright (c) 2014 Liviu Ionescu
//
// modified by Andrea Palazzi <palazziandrea@yahoo.it> 2017

#include <syscalls.h>

extern int errno;

caddr_t _sbrk(int incr)
{
    extern char end;
    static unsigned char *heap = NULL;
    unsigned char *prev_heap;

    if (heap == NULL) {
        heap = (unsigned char *) &end;
    }
    prev_heap = heap;

    incr = (incr + 3) & (~3); // align value to 4
    heap += incr;

    return (caddr_t) prev_heap;
}

int __attribute__((weak))
_close(int fildes __attribute__((unused)))
{
    errno = ENOSYS;
    return -1;
}

int __attribute__((weak))
_execve(char* name __attribute__((unused)),
        char** argv __attribute__((unused)),
        char** env __attribute__((unused)))
{
    errno = ENOSYS;
    return -1;
}

int __attribute__((weak))
_fork(void)
{
    errno = ENOSYS;
    return -1;
}

int __attribute__((weak))
_fstat(int fildes __attribute__((unused)),
       struct stat* st __attribute__((unused)))
{
    errno = ENOSYS;
    return -1;
}

int __attribute__((weak))
_getpid(void)
{
    errno = ENOSYS;
    return -1;
}

int __attribute__((weak))
_isatty(int file __attribute__((unused)))
{
    errno = ENOSYS;
    return 0;
}

int __attribute__((weak))
_kill(int pid __attribute__((unused)), int sig __attribute__((unused)))
{
    errno = ENOSYS;
    return -1;
}

int __attribute__((weak))
_link(char* existing __attribute__((unused)),
      char* _new __attribute__((unused)))
{
    errno = ENOSYS;
    return -1;
}

int __attribute__((weak))
_lseek(int file __attribute__((unused)),
       int ptr __attribute__((unused)),
       int dir __attribute__((unused)))
{
    errno = ENOSYS;
    return -1;
}

int __attribute__((weak))
_open(char* file __attribute__((unused)),
      int flags __attribute__((unused)),
      int mode __attribute__((unused)))
{
    errno = ENOSYS;
    return -1;
}

int __attribute__((weak))
_read(int file __attribute__((unused)),
      char* ptr __attribute__((unused)),
      int len __attribute__((unused)))
{
    errno = ENOSYS;
    return -1;
}

int __attribute__((weak))
_readlink(const char* path __attribute__((unused)),
          char* buf __attribute__((unused)),
          size_t bufsize __attribute__((unused)))
{
    errno = ENOSYS;
    return -1;
}

int __attribute__((weak))
_stat(const char* file __attribute__((unused)),
      struct stat* st __attribute__((unused)))
{
    errno = ENOSYS;
    return -1;
}

int __attribute__((weak))
_symlink(const char* path1 __attribute__((unused)),
         const char* path2 __attribute__((unused)))
{
    errno = ENOSYS;
    return -1;
}

int __attribute__((weak))
_unlink(char* name __attribute__((unused)))
{
    errno = ENOSYS;
    return -1;
}

int __attribute__((weak))
_wait(int* status __attribute__((unused)))
{
    errno = ENOSYS;
    return -1;
}

int __attribute__((weak))
_write(int file __attribute__((unused)),
       char* ptr __attribute__((unused)),
       int len __attribute__((unused)))
{
    errno = ENOSYS;
    return -1;
}

void
__attribute__((weak))
_exit(int code __attribute__((unused)))
{
    while (1);
}

void
__attribute__((weak,noreturn))
abort(void)
{
    _exit(1);
}

