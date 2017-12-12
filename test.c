#include <stdio.h>
#include <limits.h>
#include <assert.h>
#include <string.h>

#include <wire.h>

typedef struct {
  int id;
  char code[9];
  char name[25];
  short genBit;
  short currency;
  int64_t tkflg;
  short booktype;
  short cshtr;
  short benchmark;
  short model;
} acct_s;

int encodeAccount(char* buf, acct_s const* acct) {
  return wire_pack((uint8_t*)buf, "i%8c%24csslssss", acct->id, acct->code, acct->name,
              acct->genBit, acct->currency, acct->tkflg, acct->booktype,
              acct->cshtr, acct->benchmark, acct->model);
}

int decodeAccount(acct_s* acct, char const* buf) {
  int n = wire_unpack((uint8_t*)buf, "i%8c%24csslssss", &acct->id, acct->code, acct->name,
                 &acct->genBit, &acct->currency, &acct->tkflg, &acct->booktype,
                 &acct->cshtr, &acct->benchmark, &acct->model);
  return n;
}

void printAccount(acct_s const* acct) {
  printf("acct_s{\n");
  printf("  id:        %d\n", acct->id);
  printf("  code:      \"%s\"\n", acct->code);
  printf("  name:      \"%s\"\n", acct->name);
  printf("  genBit:    %d\n", acct->genBit);
  printf("  currency:  %d\n", acct->currency);
  printf("  tkflg:     %lld\n", acct->tkflg);
  printf("  booktype:  %d\n", acct->booktype);
  printf("  cshtr:     %d\n", acct->cshtr);
  printf("  benchmark: %d\n", acct->benchmark);
  printf("  model:     %d\n", acct->model);
  printf("}\n");
}

void accountEqual(acct_s const* lhs, acct_s const* rhs) {
  assert(lhs->id == rhs->id);
  assert(strcmp(lhs->code, rhs->code) == 0);
  assert(strcmp(lhs->name, rhs->name) == 0);
  assert(lhs->genBit == rhs->genBit);
  assert(lhs->currency == rhs->currency);
  assert(lhs->tkflg == rhs->tkflg);
  assert(lhs->booktype == rhs->booktype);
  assert(lhs->cshtr == rhs->cshtr);
  assert(lhs->benchmark == rhs->benchmark);
  assert(lhs->model == rhs->model);
}

void testEncodeDecodeAccount() {
  acct_s a1 = {.id = -43,
               .code = "snafu",
               .name = "Sharktooth",
               .genBit = -21,
               .currency = 132,
               .tkflg = /*((uint64_t)1 << 31),*/ ((int64_t)INT_MIN) - 2,
               .booktype = 4,
               .cshtr = 6,
               .benchmark = 7,
               .model = 8},
         a2;

  char buf[128] = {0};
  int n1 = encodeAccount(buf, &a1);
  printf("encoded account...\n");
  int n2 = decodeAccount(&a2, buf);
  printf("decoded account...\n");
  printf("enc len = %d, dec len = %d\n", n1, n2);
  printf("\na1 = ");
  printAccount(&a1);
  printf("\na2 = ");
  printAccount(&a2);
  accountEqual(&a1, &a2);
}

int main(int argc, char** argv) {
  // char buf[50];
  // long k = argc > 1 ? strtol(argv[1], NULL, 10) : 256;
  // printf("%ld\n", k);
  // b64encode(buf, k, 5);
  // printf("%s\n", buf);
  // long q = b64decode(buf);
  // printf("%ld\n", q);
  // int n = b64format(buf, "%s:%k:%x:%d:%04k:%.2f%%", "foo", 100, 100, 100,
  // 100,
  //                   1.25);
  // printf("%s, length = %d\n", buf, n);

  // char scratch[32];
  // int k1, k2, k3;
  // b64scan("hello, this is Bz, B0 123", "%014s %02k, %02k %k", scratch, &k1,
  // &k2,
  //         &k3);

  // printf("scratch = %s, k1 = %d, k2 = %d, k3 = %d\n", scratch, k1, k2, k3);

  // b64scan("hello:Bz:AB0:123", "%s:%02k:%k:%k", scratch, &k1, &k2, &k3);

  // printf("scratch = %s, k1 = %d, k2 = %d, k3 = %d\n", scratch, k1, k2, k3);

  // char c;
  // ushort s1, s2;
  // ulong l1, l2;
  // uchar buf2[64];
  // int nn = pack(buf2, "lcs%10cls", 777, 'A', 10, "Austin", 666, 20);
  // printf("packed %d bytes\n", nn);
  // nn = unpack(buf2, "lcs%10cls", &l1, &c, &s1, scratch, &l2, &s2);
  // printf("unpacked %d bytes\n", nn);
  // printf("l1 = %lu, c = %c, s1 = %d, scratch = \"%s\", l2 = %lu, s2 = %d\n",
  // l1, c, s1, scratch, l2, s2);

  testEncodeDecodeAccount();

  printf("sizeof(int) = %lu\n", sizeof(int));
  printf("sizeof(unsigned int) = %lu\n", sizeof(unsigned int));
  printf("sizeof(long) = %lu\n", sizeof(long));
  printf("sizeof(unsigned long) = %lu\n", sizeof(unsigned long));
  printf("sizeof(char) = %lu\n", sizeof(char));

  int x = -1;
  unsigned y = x;
  printf("x = %d, y = %u\n", x, y);
  printf("signed y = %d\n", (int)y);
  printf("(int)(y >> 1) = %d\n", ((int)(y >> 1)));

  return 0;
}
