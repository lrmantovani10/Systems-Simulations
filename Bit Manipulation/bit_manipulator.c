int absoluteValue(int x) {
  /* explore the argument x's sign and masking properties to compute absolute value*/
  int sign = (x >> 31);
  int result = ((~x + 1) & sign) + ((~sign) & x);
  return result;
}

int addingOK(int x, int y) {
  /* check sign of x and y and use overflow properties with bitwise OR to compute value*/
  int sign_x = (x >> 31);
  int sign_y = (y >> 31);
  int negative_overflow = (sign_x & sign_y) & (!((x + y) >> 31));
  int positive_overflow = ((!sign_x) & (!sign_y)) & ((x + y) >> 31);
  int result = !(positive_overflow | negative_overflow);
  return result;
}

int allBitsEven(int x) {
  /* apply masking to x and subtract from the mask to determine value*/
  int comparator = (0X55 << 24) + (0X55 << 16) + (0X55 << 8) + 0X55;
  int comparison = x & comparator;
  int result = !(comparison + (~comparator + 1));
  return result;
}

int computeExclamation(int x) {
  /* Use overflow, sign (+ or -), and shifting properties to obtain value*/
  int negative = (x >> 31);
  int T_max = ~(0X1 << 31);
  int positive = ((x + T_max) >> 31) | (x >> 30);
  int result = (~(negative | positive) & 0X1);
  return result;
}

int countBits(int x) {
  /* Use masking, shifting, and addition to compute value. Mask by every 4 bits
  and shift these values to the first 4 bits to compute sum.*/
  int m = ((0X11) + (0X11 << 8) + (0X11 << 16) + (0X11 << 24));
  int p1 = x & m;
  int p2 = (x >> 1) & m;
  int p3 = (x >> 2) & m;
  int p4 = (x >> 3) & m;
  int sum = (p1 + p2 + p3 + p4);
  int result = ((sum >> 28) & 0XF) + ((sum >> 24) & 0XF) + ((sum >> 20) & 0XF) + ((sum >> 16) & 0XF) +
                ((sum >> 12) & 0XF) + ((sum >> 8) & 0XF) + ((sum >> 4) & 0XF) + (sum & 0XF);

  return result;
}

int bitNotOr(int x, int y) {
  /* Use negation and bitwise AND to determine value*/
  int result = (~x) & (~y);
  return result;
}

int swapBytes(int x, int n, int m) {
  /* Use shifiting and negation to remove bytes "m" and "n" from x. Then, replace them with
  each other using shifting, bitwise AND operations, and addition. Lastly, use OR to
  obtain final value.*/
  int swap1 = ~(0XFF << (n << 3));
  int swap2 = ~(0XFF << (m << 3));
  int cleaned_x = x & swap1 & swap2;
  int insert1 = ((x >> (n << 3)) & 0XFF) << (m << 3);
  int insert2 = ((x >> (m << 3)) & 0XFF) << (n << 3);
  int result = cleaned_x | insert1 | insert2;
  return result;
}

int conditionalLogic(int x, int y, int z) {
  /* Check if x is nonzero using !, use shifiting and AND to check if x is odd
  and !x is nonzero. Then, sum the values obtained from these two operations
  to obtain final result.*/
  int x_cond = (!!x);
  int y_check = ((x_cond << 31) >> 31);
  int y_term = y & y_check;
  int z_check = (((!x_cond) << 31) >> 31);
  int z_term = z & z_check;
  int result = y_term + z_term;
  return result;
}

int roundThreeFourths(int x) {
  /* Add x three times and then use shifting, masking, and OR to obtain x * 3/4.
  To do so, determine if the 1st or 2nd bits of x are 1 so that we know if there
  are any "flags" raised.*/
  int mult_3 = x + x + x;
  int sign = (mult_3 >> 31);
  int flag = (x & 0X1) | ((x >> 0X1) & 0X1);
  int result = (mult_3 >> 2) + (sign & flag);
  return result;
}

int bitsFit(int x, int n) {
  /* check if the sign of x (1 for negative and 0 for positive) is 1 and greater
  than or equal to the minimum integer allowed or 0 and less than the max integer allowed
  in a 32-bit number.*/
  int n_minus = (n + (~0X0));
  int x_sign = (x >> 31);
  int less_thaneq_max = !(x >> n_minus);
  int more_thaneq_min = !((x >> n_minus) + 1);
  int result = (x_sign & more_thaneq_min) | ((!x_sign) & less_thaneq_max);
  return result;
}

int getByteN(int x, int n) {
  /* explore shifting and masking properties to compute value*/
  int result = (x >> (n << 3)) & 0XFF;
  return result;
}

int posGreatestBit(int x) {
  /* check if x is either negative or not and use shifting and OR to compute
  pre_result variable if not negative. Use masking to compute the pre_result
  value bitwise AND itself negated and shifted to the right and then do a 
  bitwise OR the minimum possible number, in the case of negatives.*/
  int negative = ((x >> 31) << 31);
  int pre_result = x | (x>>1) | (x>>2) | (x>>3) | (x>>4) | (x>>5) | (x>>6) | 
                  (x>>7) | (x>>8) | (x>>9) | (x>>10) | (x>>11) | (x>>12) | (x>>13) | 
                  (x>>14) | (x>>15) | (x>>16) | (x>>17) | (x>>18) | (x>>19) | (x>>20) | 
                  (x>>21) | (x>>22) | (x>>23) | (x>>24) | (x>>25) | (x>>26) | (x>>27) | 
                  (x>>28) | (x>>29) | (x>>30) | (x>>31);
  int result = ((pre_result & ((~pre_result) >> 1)) | negative);
  return result;
}

int propositionalLogic(int x, int y) {
  /* explore !, OR, and XOR properties to compute value*/
  int result = ((!x) | !(x ^ y));
  return result;
}

int checkAsciiDigit(int x) {
  /* check if two conditions determined by shifiting, negation, and
  addition are met*/
  int condition1 = !((x + (~0X30 + 1)) >> 31);
  int condition2 = !((0x39 + (~x + 1)) >> 31);
  int result = condition1 & condition2;
  return result;
}

int areEqual(int x, int y) {
  /* check if x minus the value of y is zero*/
  int result = !(x + (~y + 1));
  return result;
}

int firstLess(int x, int y) {
  /* check if there is a sign difference between x and y and use addition, shifting,
  OR, and AND operations to determine is x is less than y in case the signs are the same.*/
  int sign_x = (x >> 31);
  int sign_y = (y >> 31);
  int sign_diff = (sign_x ^ sign_y);
  int x_less = sign_diff & (sign_x);
  int diff = y + (~x + 1);
  int cond1 = (!sign_diff) & (!((diff >> 31) & 0X1));
  int cond2 = (!sign_diff) & (!!(diff));
  int result = (cond1 & cond2) | (x_less & 0X1);
  return result;
}

int isNotNegative(int x) {
  /* check if last bit in x is zero*/
  int result = !((x >> 31) & (0X1));
  return result;
}

int isPowerTwo(int x) {
  /* check that x is not negative, not zero, and the greatest bit in x 
  follows a property involving its neighboring bits. Conformity to this property 
  is determined through negation, bit shifitng, and AND operations.*/
  int negative = (x >> 31);
  int zero = !(x);
  int positive = !((~(x + (~0X0)) & x) + (~x + 1));
  int result = !(negative) & !(zero) & positive;
  return result;
}

int isMinComplement(int x) {
  /* check that x is not zero and use nagation and addition to make sure that
  adding x with x and subtracting -1 from the result is 0*/
  int zero = !(x);
  int nonzero = !(~(x + (~0X0) + x));
  int result = nonzero & (!zero);
  return result;
}

int giveMinusOne(void) {
  /* explore negation to compute value from 0*/
  return (~ 0X0);
}

int leftRotation(int x, int n) {
  /* use shifting, negation, addition, and masking to separate the left
  bits of x and insert them into the x with these bits removed. Do this using OR. */
  int left_shift = (x << n);
  int compare_n = ~((~0X0) << n);
  int left_bits = (x >> (32 + (~n + 1))) & compare_n;
  int result = left_shift | left_bits;
  return result;
}