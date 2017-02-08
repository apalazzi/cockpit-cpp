/*
    Copyright 2017 Andrea Palazzi <palazziandrea@yahoo.it>

    This file is part of cockpit-c++.

    ed-autopilot-cockpit is free software: you can redistribute it and/or modify
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

#include <ringbuffer.h>

void init_ringbuffer(ringbuffer* buf) {
    buf->begin = buf->buffer;
    buf->end=buf->buffer;
}

int write(ringbuffer* buf, const char* src, int len) {
    for (int i=0; i< len; i++) {
        if (buf->end+1 == buf->begin) {
            return i;
        }
        *(buf->end)=src[i];
        buf->end++;
        if (buf->end == buf->buffer+RING_BUFFER_SIZE) {
            buf->end = buf->buffer;
        }
    }
    return len;
}

int read(ringbuffer* buf, char* dst, int len, char separator) {
    for (int i=0; i<len; i++) {
        if (buf->begin == buf->end) {
            if (i>0) {
                dst[i] = 0;
            }
            return i;
        }
        dst[i] = *(buf->begin);
        buf->begin++;
        if (buf->begin == buf->buffer+RING_BUFFER_SIZE) {
            buf->begin = buf->buffer;
        }
        if (dst[i]==separator) {
            dst[i] = 0;
            return i;
        }
    }
    return len;
}


