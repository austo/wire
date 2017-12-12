#ifndef INCLUDED_WIRE
#define INCLUDED_WIRE

#include <stdint.h>

int wire_b64encode(char* buf, long k, int pad);
long wire_b64decode(char const* s);
long wire_b64decodeT(char const** s, char tok);
long wire_b64decodeN(char const** s, int n);
int wire_b64format(char* buf, char const* fmt, ...);
int wire_b64scan(char const* buf, char const* fmt, ...);
int wire_pack(uint8_t* buf, char const* fmt, ...);
int wire_unpack(uint8_t const* buf, char const* fmt, ...);

#endif
