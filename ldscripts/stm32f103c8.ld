MEMORY
{
ram (xrw)      : ORIGIN = 0x20000000, LENGTH = 20K
rom (rx)      : ORIGIN = 0x8000000, LENGTH = 64K
}

INCLUDE ../libopencm3/lib/libopencm3_stm32f1.ld
