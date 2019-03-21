#include <stdio.h>

int foo_a(int n)
{
   int i, x = 0;
   for (i = 0; i < n; i++)
      x += i;
   return x;
}

int main(void)
   {
      int beast = 1;

      // what will this print?
      printf("%d\n", beast << 1);

      // notice that << 1 does not change beast. for that,
      // we would have to use beast = beast << 1
      printf("%d\n", beast);

      // now, what do these print? are you starting to see a pattern?
      // what do their underlying binary representations look like after
      // the bitshifting takes place?
      printf("%d\n", beast << 2);
      printf("%d\n", beast << 3);
      printf("%d\n", beast << 4);

      // what will this print?
      printf("%d\n", 108 << 3);

      // and what's going on here?
      printf("%d\n", beast >> 2);

      // what will 'beast' be after shifting left by 31?
      // hint: what will it look like in binary? (then convert that to base 10)
      beast = beast << 31;
      printf("%d\n", beast);

      // what is going on here?
      beast = ~beast;
      printf("%d\n", beast);
      printf("%u\n", beast);

      // what will the binary representation of 'beast' look like after the following
      // code is executed, and what will the output look like in decimal?
      beast = ~1;
      printf("%d\n", beast);

      // and what about this?
      beast = ~0;
      printf("%d\n", beast);
      printf("%u\n", beast);

      // here's something interesting. do you see a pattern? can you explain it?
      printf("%d\n", ~75);
      printf("%d\n", ~78);

      // and finally, what's going on here?
      printf("%d\n", 0xBEEF);
      printf("%d\n", 020);

      return 0;
   }
