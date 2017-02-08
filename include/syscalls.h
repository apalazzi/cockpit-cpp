/*
    Copyright 2017 Andrea Palazzi <palazziandrea@yahoo.it>

    This file is part of ed-autopilot-cockpit.

    ed-autopilot-cockpit is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    ed-autopilot-cockpit is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with ed-autopilot-cockpit.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef INCLUDE_SYSCALLS_H_
#define INCLUDE_SYSCALLS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdarg.h>
#include <errno.h>
#include <sys/stat.h>

#undef errno
extern int errno;

int _close(int fildes);
int _execve(char* name, char** argv, char** env);
int _fork(void);
int _fstat(int fildes, struct stat* st);
int _getpid(void);
int _isatty(int file);
int _kill(int pid, int sig);
int _link(char* existing, char* _new);
int _lseek(int file, int ptr, int dir);
int _open(char* file, int flags, int mode);
int _read(int file, char* ptr, int len);
int _readlink(const char* path, char* buf, size_t bufsize);
int _stat(const char* file, struct stat* st);
int _symlink(const char* path1, const char* path2);
int _unlink(char* name);
int _wait(int* status);
int _write(int file, char* ptr, int len);
void exit(int code);

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_SYSCALLS_H_ */
