/*
 * CS:APP Data Lab
 *
 * <Please put your name and userid here>
 *
 * bits.c - Source file with your solutions to the Lab.
 *          This is the file you will hand in to your instructor.
 *
 * WARNING: Do not include the <stdio.h> header; it confuses the dlc
 * compiler. You can still use printf for debugging without including
 * <stdio.h>, although you might get a compiler warning. In general,
 * it's not good practice to ignore compiler warnings, but in this
 * case it's OK.
 */

#if 0
/*
 * Instructions to Students:
 *
 * STEP 1: Read the following instructions carefully.
 */

You will provide your solution to the Data Lab by
editing the collection of functions in this source file.

INTEGER CODING RULES:

  Replace the "return" statement in each function with one
  or more lines of C code that implements the function. Your code
  must conform to the following style:

  int Funct(arg1, arg2, ...) {
      /* brief description of how your implementation works */
      int var1 = Expr1;
      ...
      int varM = ExprM;

      varJ = ExprJ;
      ...
      varN = ExprN;
      return ExprR;
  }

  Each "Expr" is an expression using ONLY the following:
  1. Integer constants 0 through 255 (0xFF), inclusive. You are
      not allowed to use big constants such as 0xffffffff.
  2. Function arguments and local variables (no global variables).
  3. Unary integer operations ! ~
  4. Binary integer operations & ^ | + << >>

  Some of the problems restrict the set of allowed operators even further.
  Each "Expr" may consist of multiple operators. You are not restricted to
  one operator per line.

  You are expressly forbidden to:
  1. Use any control constructs such as if, do, while, for, switch, etc.
  2. Define or use any macros.
  3. Define any additional functions in this file.
  4. Call any functions.
  5. Use any other operations, such as &&, ||, -, or ?:
  6. Use any form of casting.
  7. Use any data type other than int.  This implies that you
     cannot use arrays, structs, or unions.


  You may assume that your machine:
  1. Uses 2s complement, 32-bit representations of integers.
  2. Performs right shifts arithmetically.
  3. Has unpredictable behavior when shifting an integer by more
     than the word size.

EXAMPLES OF ACCEPTABLE CODING STYLE:
  /*
   * pow2plus1 - returns 2^x + 1, where 0 <= x <= 31
   */
  int pow2plus1(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     return (1 << x) + 1;
  }

  /*
   * pow2plus4 - returns 2^x + 4, where 0 <= x <= 31
   */
  int pow2plus4(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     int result = (1 << x);
     result += 4;
     return result;
  }

FLOATING POINT CODING RULES

For the problems that require you to implent floating-point operations,
the coding rules are less strict.  You are allowed to use looping and
conditional control.  You are allowed to use both ints and unsigneds.
You can use arbitrary integer and unsigned constants.

You are expressly forbidden to:
  1. Define or use any macros.
  2. Define any additional functions in this file.
  3. Call any functions.
  4. Use any form of casting.
  5. Use any data type other than int or unsigned.  This means that you
     cannot use arrays, structs, or unions.
  6. Use any floating point data types, operations, or constants.


NOTES:
  1. Use the dlc (data lab checker) compiler (described in the handout) to
     check the legality of your solutions.
  2. Each function has a maximum number of operators (! ~ & ^ | + << >>)
     that you are allowed to use for your implementation of the function.
     The max operator count is checked by dlc. Note that '=' is not
     counted; you may use as many of these as you want without penalty.
  3. Use the btest test harness to check your functions for correctness.
  4. Use the BDD checker to formally verify your functions
  5. The maximum number of ops for each function is given in the
     header comment for each function. If there are any inconsistencies
     between the maximum ops in the writeup and in this file, consider
     this file the authoritative source.

/*
 * STEP 2: Modify the following functions according the coding rules.
 *
 *   IMPORTANT. TO AVOID GRADING SURPRISES:
 *   1. Use the dlc compiler to check that your solutions conform
 *      to the coding rules.
 *   2. Use the BDD checker to formally verify that your solutions produce
 *      the correct answers.
 */


#endif
/*
 * bitAnd - x&y using only ~ and |
 *   Example: bitAnd(6, 5) = 4
 *   Legal ops: ~ |
 *   Max ops: 8
 *   Rating: 1
 */
int bitAnd(int x, int y) {
  return ~(~x | ~y);
}


/*
 * getByte - Extract byte n from word x
 *   Bytes numbered from 0 (LSB) to 3 (MSB)
 *   Examples: getByte(0x12345678,1) = 0x56
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 6
 *   Rating: 2
 */
int getByte(int x, int n) {

  return (x >> (n << 3) )& 0xff;
}
/*
 * logicalShift - shift x to the right by n, using a logical shift
 *   Can assume that 0 <= n <= 31
 *   Examples: logicalShift(0x87654321,4) = 0x08765432
 *   Legal ops: ~ & ^ | + << >>
 *   Max ops: 20
 *   Rating: 3
 */
int logicalShift(int x, int n) {
    int var = ((0x1 << 31) >> n) << 1;
    return (x >> n) & ~var;
}
/*
 * bitCount - returns count of number of 1's in word
 *   Examples: bitCount(5) = 2, bitCount(7) = 3
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 40
 *   Rating: 4
 */
int bitCount(int x) {
    int var55, var33, var0f, varff, varffff;

    var55 = 0x55;
    var55 |= (var55 << 8);
    var55 |= (var55 << 16);
    x = (x & var55) + ((x >> 1) & var55);
    //printf("var = %d, x = %d ", var55, x);

    var33 = 0x33;
    var33 |= (var33 << 8);
    var33 |= (var33 << 16);
    x = (x & var33) + ((x >> 2) & var33);

    var0f = 0x0f;
    var0f |= (var0f << 8);
    var0f |= (var0f << 16);
    x = (x & var0f) + ((x >> 4) & var0f);

    varff = 0xff;
    varff |= (varff << 16);
    x = (x & varff) + ((x >> 8) & varff);

    varffff = (varff >> 16);
    varffff |= (varffff << 2);
    return (x & varffff) + ((x >> 16) & varffff);
}
/*
 * bang - Compute !x without using !
 *   Examples: bang(3) = 0, bang(0) = 1
 *   Legal ops: ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 4
 */
int bang(int x) {
  x |= x >> 1;
  x |= x >> 2;
  x |= x >> 4;
  x |= x >> 8;
  x |= x >> 16;
  return (x + 1) & 0x1;
}
/*
 * tmin - return minimum two's complement integer
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 4
 *   Rating: 1
 */
int tmin(void) {
  return 0x1 << 31;
}
/*
 * fitsBits - return 1 if x can be represented as an
 *  n-bit, two's complement integer.
 *   1 <= n <= 32
 *   Examples: fitsBits(5,3) = 0, fitsBits(-4,3) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 2
 */
int fitsBits(int x, int n) {
    int var = x >> (n + ~0);
  return !var | !(~var);
}
/*
 * divpwr2 - Compute x/(2^n), for 0 <= n <= 30
 *  Round toward zero
 *   Examples: divpwr2(15,1) = 7, divpwr2(-33,4) = -2
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 2
 */
int divpwr2(int x, int n) {
    int isPositive = !(x >> 31);
    int isAllZero = !(x & ((0x1 << n) + ~0));
    return (x >> n) + (!isPositive & !isAllZero);
}
/*
 * negate - return -x
 *   Example: negate(1) = -1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 5
 *   Rating: 2
 */
int negate(int x) {
  return ~x + 1;
}
/*
 * isPositive - return 1 if x > 0, return 0 otherwise
 *   Example: isPositive(-1) = 0.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 8
 *   Rating: 3
 */
int isPositive(int x) {
  return !(x >> 31) + ~!x + 1;
}
/*
 * isLessOrEqual - if x <= y  then return 1, else return 0
 *   Example: isLessOrEqual(4,5) = 1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 24
 *   Rating: 3
 */
int isLessOrEqual(int x, int y) {
    int isEqual = (x >> 31) ^ (y >> 31); //if x and y have same sign, isEqual will be 0x0, otherwise, 0x11111111
    return (~isEqual & !((y + ~x + 1) >> 31)) + (isEqual & (!(y >> 31)));
}
/*
 * ilog2 - return floor(log base 2 of x), where x > 0
 *   Example: ilog2(16) = 4
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 90
 *   Rating: 4
 */
int ilog2(int x) {
    int sign, shift;

    sign = !!(x >> 16);
    shift += (sign << 4);
    x >>= (sign << 4);

    sign = !!(x >> 8);
    shift += (sign << 3);
    x >>= (sign << 3);

    sign = !!(x >> 4);
    shift += (sign << 2);
    x >>= (sign << 2);

    sign = !!(x >> 2);
    shift += (sign << 1);
    x >>= (sign << 1);

    sign = !!(x >> 1);
    shift += (sign);
    x >>= (sign);
    return shift;
}
/*
 * float_neg - Return bit-level equivalent of expression -f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representations of
 *   single-precision floating point values.
 *   When argument is NaN, return argument.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 10
 *   Rating: 2
 */
unsigned float_neg(unsigned uf) {
    return ((uf | 0x807fffff) == 0xffffffff && (uf & 0x007fffff) != 0) ? uf : (uf ^ 0x80000000);
}
/*
 * float_i2f - Return bit-level equivalent of expression (float) x
 *   Result is returned as unsigned int, but
 *   it is to be interpreted as the bit-level representation of a
 *   single-precision floating point values.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned float_i2f(int x) {
    int sign, count = 0, bigger, equal;
    unsigned abs, absRounded, expRounded, absTmp, res = 0;

    if (x == 0) return 0;


    sign = !(x >> 31);
    if (sign) {
        abs = x;
    } else {
        abs = ~x + 1;
    }

    absTmp = abs;
    while ((absTmp & 0x80000000) == 0) {
        absTmp <<= 1;
        count++;
    }


    if (count < 9) { //Normalized
        //please pay great attention to rounding when you try to leave out some bits
        //(abs >> (7 - count) & 0x1) && (abs >> (8 - count) & 0x1)
        bigger = (abs & (((0x1 << (8-count) ) -1))) > (0x1 << (7-count));   //the part which is going to be leaved out is bigger than half of the left least significant bit
        equal = (abs & (((0x1 << (8-count) ) -1))) == (0x1 << (7-count));
        if (bigger || (equal && (abs >> (7 - count) & 0x1) && (abs >> (8 - count) & 0x1)))
         {
            absRounded = (abs >> (8 - count)) + 1;
            expRounded = 31 - count + 127;
            if (absRounded > 0x00ffffff) {
                absRounded = 0x00800000;
                expRounded = 31 - count + 127 + 1;
            }
            //printf("Carried.. abs = %u \n", abs);
        } else {
            absRounded = (abs >> (8 - count));
            expRounded = 31 - count + 127;
            //printf("expR = %u\n", expRounded);
        }
        //printf("count = %d, res = %u, abstmp = %u, abs = %u, expR = %u, absR = %u\n",count, res, absTmp, abs, expRounded, absRounded);
        res = absRounded & 0x807fffff;
        res |= expRounded << 23;
    } else { //Denormalized
        res = (abs << (count - 8)) & 0x807fffff;
        res |= ((31 - count + 127) << 23);
    }
    if (!sign) res |= 0x80000000;
    return res;
}
/*
 * float_twice - Return bit-level equivalent of expression 2*f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representation of
 *   single-precision floating point values.
 *   When argument is NaN, return argument
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned float_twice(unsigned uf) {

    unsigned exp, frac, res;

    //printf("uf = %.8x, res = %u \n", uf, res);

    exp = uf & 0x7f800000;
    frac = uf & 0x007fffff;
    if (exp == 0) {   //denormalized
        return (uf & 0x80000000)| (frac << 1);
    }

    if (exp == 0x7f800000) {
        return uf;
    }

    res = uf + 0x00800000;


    return res;
}
/**
void show(unsigned x)
{

    unsigned char *start = (unsigned char *) &x;
    int i, len = 4;
    for (i = 0; i < len ; i++)
    {
        printf(" %.2x", start[len - i]);
    }
    printf("\n");

}
**/







