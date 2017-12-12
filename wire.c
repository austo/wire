#include <limits.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define WIRE_IS_SPECIFIER(c)                                             \
  ((c) == 'c' || (c) == 's' || (c) == 'd' || (c) == 'i' || (c) == 'o' || \
   (c) == 'x' || (c) == 'X' || (c) == 'u' || (c) == 'f' || (c) == 'F' || \
   (c) == 'e' || (c) == 'E' || (c) == 'a' || (c) == 'A' || (c) == 'g' || \
   (c) == 'G' || (c) == 'n' || (c) == 'p')

#define WIRE_IS_FLOAT_SPECIFIER(c)                                       \
  ((c) == 'a' || (c) == 'A' || (c) == 'e' || (c) == 'E' || (c) == 'f' || \
   (c) == 'F' || (c) == 'g' || (c) == 'G')

static char const syms[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K',
                     'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V',
                     'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g',
                     'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r',
                     's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '0', '1', '2',
                     '3', '4', '5', '6', '7', '8', '9', '+', '/'};

int wire_b64encode(char* buf, long k, int pad) {
  int neg = 0;
  if (k < 0) {
    neg = 1;
    k = -k;
  }
  char scratch[50];
  int i = 0;

  while (k) {
    long m = k % 64;
    scratch[i++] = syms[m];
    k >>= 6;
  }

  if (neg) {
    buf[0] = '-';
  }

  while (pad > 0 && (neg + i) < pad) {
    buf[neg++] = 'A';
  }

  for (int j = 0, p = i - 1; j < i; j++, p--) {
    buf[neg + j] = scratch[p];
  }
  buf[neg + i] = '\0';
  return neg + i;
}

static long do_b64decode(char const** sref, const char tok, const int n) {
  int i = 0, neg = 0;
  char const* s = *sref;
  if (*s == '-') {
    neg = 1;
    i++;
  }
  while (s[i] == 'A') {
    i++;
  }
  long x = 0;
  for (; i < n && s[i] != tok; i++) {
    x *= 64;
    if (s[i] >= 'A' && s[i] <= 'Z') {
      x += (s[i] - 65);
    } else if (s[i] >= 'a' && s[i] <= 'z') {
      x += (s[i] - 71);
    } else if (s[i] >= '0' && s[i] <= '9') {
      x += (s[i] + 4);
    } else if (s[i] == '+') {
      x += 62;
    } else if (s[i] == '/') {
      x += 63;
    }
  }
  if (neg) {
    x = -x;
  }
  *sref = (s + i);
  return x;
}

long wire_b64decode(char const* s) {
  return do_b64decode(&s, '\0', INT_MAX);
}

long wire_b64decodeT(char const** s, char tok) {
  return do_b64decode(s, tok, INT_MAX);
}

long wire_b64decodeN(char const** s, int n) {
  return do_b64decode(s, '\0', n);
}

int wire_b64format(char* buf, char const* fmt, ...) {
  va_list ap;
  char c, *s = buf, scratch[64];

  enum { PASS, BEGIN, QUAL };
  int state = PASS, i, pad;

  va_start(ap, fmt);
  while ((c = *fmt++) != '\0') {
    if (c == '%') {
      pad = 0;
      i = 0;
      if (state == BEGIN) {
        *s++ = '%';
        state = PASS;
      } else {
        state = BEGIN;
        scratch[i++] = c;
      }
    } else if (c == '0' && state == BEGIN) {
      state = QUAL;
      scratch[i++] = c;
    } else if (c >= '0' && c <= '9' && state == QUAL) {
      pad *= 10;
      pad += (c - 48);
      scratch[i++] = c;
    } else if (state == QUAL || state == BEGIN) {
      if (c == 'k') {
        s += wire_b64encode(s, va_arg(ap, int), pad);
        state = PASS;
      } else if (WIRE_IS_SPECIFIER(c)) {
        scratch[i++] = c;
        scratch[i] = '\0';
        if (WIRE_IS_FLOAT_SPECIFIER(c)) {
          s += sprintf(s, scratch, va_arg(ap, double));
        } else {
          s += sprintf(s, scratch, va_arg(ap, void*));
        }
        state = PASS;
      } else {
        scratch[i++] = c;
      }
    } else if (state == PASS) {
      *s++ = c;
    }
  }
  *s = '\0';
  va_end(ap);
  return s - buf;
}

int wire_b64scan(char const* buf, char const* fmt, ...) {
  va_list ap;
  enum { PASS, BEGIN, QUAL };
  char c, end;
  int state = PASS, i, pad, lim;
  char const* s = buf;

  va_start(ap, fmt);
  while ((c = *fmt++) != '\0') {
    if (c == '%') {
      if (state == BEGIN) {
        state = PASS;
      } else {
        state = BEGIN;
        pad = 0;
      }
    } else if ((c == '0' || c == '.') && state == BEGIN) {
      state = QUAL;
    } else if (c >= '0' && c <= '9' && state == QUAL) {
      pad *= 10;
      pad += (c - 48);
    } else if (state == QUAL || state == BEGIN) {
      if (c == 'k') {
        if (!pad) {
          lim = INT_MAX;
          end = *fmt;
        } else {
          lim = pad;
          end = '\0';
        }
        int* k = va_arg(ap, int*);
        *k = do_b64decode(&s, end, lim);
        state = PASS;
      } else if (c == 's') {
        char* sarg = va_arg(ap, char*);
        i = 0;
        if (!pad) {
          lim = INT_MAX;
          end = *fmt;
        } else {
          lim = pad;
          end = '\0';
        }
        while (i++ < lim && *s != end) {
          *sarg++ = *s++;
        }
        *sarg = '\0';
        state = PASS;
      }
    } else if (state == PASS) {
      s++;
    }
  }
  va_end(ap);
  return s - buf;
}

int wire_pack(uint8_t* buf, char const* fmt, ...) {
  char const* p;
  uint8_t* bp = buf;

  int pad, j, ended;
  uint8_t* cp;
  uint16_t s;
  uint32_t i;
  uint64_t l;

  va_list ap;
  va_start(ap, fmt);

  for (p = fmt; *p != '\0'; p++) {
    if (*p == '%') {
      pad = 0;
      p++;
      while (*p >= '0' && *p <= '9') {
        pad *= 10;
        pad += (*p++ - 48);
      }
      if (pad == 0 || *p != 'c') {
        return -1;
      }
      cp = va_arg(ap, uint8_t*);
      ended = 0;
      for (j = 0; j < pad; j++) {
        if (ended || cp[j] == '\0') {
          ended = 1;
          *bp++ = '\0';
        } else {
          *bp++ = cp[j];
        }
      }
      continue;
    }
    switch (*p) {
      case 'c': /* uint8_t */
        *bp++ = va_arg(ap, int);
        break;
      case 's': /* uint16_t */
        s = va_arg(ap, int);
        *bp++ = s >> 8;
        *bp++ = s;
        break;
      case 'i': /* uint32_t */
        i = va_arg(ap, uint32_t);
        *bp++ = i >> 24;
        *bp++ = i >> 16;
        *bp++ = i >> 8;
        *bp++ = i;
        break;
      case 'l': /* uint64_t */
        l = va_arg(ap, uint64_t);
        *bp++ = l >> 56;
        *bp++ = l >> 48;
        *bp++ = l >> 40;
        *bp++ = l >> 32;
        *bp++ = l >> 24;
        *bp++ = l >> 16;
        *bp++ = l >> 8;
        *bp++ = l;
        break;
      default:
        return -1;
    }
  }
  va_end(ap);
  return bp - buf;
}

int wire_unpack(uint8_t const* buf, char const* fmt, ...) {
  char const* p;
  uint8_t const* bp = buf;

  int j, pad;
  uint8_t* cp;
  uint16_t* sp;
  uint32_t* ip;
  uint64_t* lp;

  va_list ap;
  va_start(ap, fmt);

  for (p = fmt; *p != '\0'; p++) {
    if (*p == '%') {
      pad = 0;
      p++;
      while (*p >= '0' && *p <= '9') {
        pad *= 10;
        pad += (*p++ - 48);
      }
      if (pad == 0 || *p != 'c') {
        return -1;
      }
      cp = va_arg(ap, uint8_t*);
      for (j = 0; j < pad; j++) {
        cp[j] = *bp++;
      }
      cp[j] = '\0';
      continue;
    }
    switch (*p) {
      case 'c':
        cp = va_arg(ap, uint8_t*);
        *cp = *bp++;
        break;
      case 's':
        sp = va_arg(ap, uint16_t*);
        *sp = *bp++ << 8;
        *sp |= *bp++;
        break;
      case 'i':
        ip = va_arg(ap, uint32_t*);
        *ip = *bp++ << 24;
        *ip |= *bp++ << 16;
        *ip |= *bp++ << 8;
        *ip |= *bp++;
        break;
      case 'l':
        lp = va_arg(ap, uint64_t*);
        *lp = ((uint64_t)*bp++) << 56;
        *lp |= ((uint64_t)*bp++) << 48;
        *lp |= ((uint64_t)*bp++) << 40;
        *lp |= ((uint64_t)*bp++) << 32;
        *lp |= ((uint64_t)*bp++) << 24;
        *lp |= ((uint64_t)*bp++) << 16;
        *lp |= ((uint64_t)*bp++) << 8;
        *lp |= ((uint64_t)*bp++);
        break;
      default:
        return -1;
    }
  }
  va_end(ap);
  return bp - buf;
}
