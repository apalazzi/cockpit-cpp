/*
    Copyright 2017 Andrea Palazzi <palazziandrea@yahoo.it>

    This file is part of cockpit-c++.

    cockpit-c++ is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    cockpit-c++ is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with cockpit-c++.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef INCLUDE_RINGBUFFER_H_
#define INCLUDE_RINGBUFFER_H_

#ifdef __cplusplus
extern "C" {
#endif

#define RING_BUFFER_SIZE 128

typedef struct {
    char buffer[RING_BUFFER_SIZE];
    char* begin;
    char* end;
} ringbuffer;

int write(ringbuffer* buffer, const char* src, int size);
int read(ringbuffer* buffer, char* dst, int size, char separator);
void init_ringbuffer(ringbuffer* buffer);

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_RINGBUFFER_H_ */
