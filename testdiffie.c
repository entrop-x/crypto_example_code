#include <stdio.h>
#include <time.h>
#include <stdlib.h>


#define basis 10000
#define basisbits 14
#define nofdigits 7
#define printlevel 0

/* "number" is a structure with nofdigits digits, the normal number in the
   ring structure we consider ; number1 has one more digit to allow for the 
   overflow when multiplying a number with a single digit and other intermediate
   results ; bignum has twice as much digits, to allow for the product of two numbers
   before the modulo operation ; we also need bignum1 which has one single extra digit
   more than bignum for overflow results when working with the product */
typedef struct digits {unsigned int digit[nofdigits];} number;
typedef struct digitus {unsigned int digit[nofdigits + 1];} number1;
typedef struct digitis {unsigned int digit[nofdigits * 2];} bignum;
typedef struct digitas {unsigned int digit[nofdigits*2+1];} bignum1;

int readnum(number* x);
void printnum(number* x);
void difnum(number* x, number* y, number* s);
void sumnum(number* x, number* y, number* s);
int isbigger(number* x, number* y);
int isbigger1(number1* x, number1* y);
void digittimesnumber(int dig,number* x,number1* s);
void bigmodulo(bignum* x,number* r);
void timesnum(number* x, number* y, number* p);
void dividenum(number* x, number* y, number* q, number* r);
void exteuclid(number* a, number* b, number* gcd, number* s, number* t);
void inversenum(number* x,number* invx);
void divide2(number* x, number* q, int* r);
void power(number* x, number* y, number* xy);
int isprimefermat(number* x, int k);
void findnextprimefermat(number* x, number* y, int k);
int isprimerabin(number* x, int k);
void findnextprimerabin(number* x, number* y, int k);
void randomgen(number* myran);
void makebig(int small, number* big);
void rsakeygen(int nbits, number* p1, number* p2, number* n, number* pkey, number* skey);
void rsacrypt(number* cleartext,number* pkey, number* n, number* cyphertext);
void diffiesetup(int nbits, number* dhprime, number* dhgen);
void diffiekeypair(number* dhprime, number* dhgen, number* pkey, number* skey);
void diffiecommon(number* dhprime,number* pkeyA, number* skeyB, number* comkey);

number modulo = {{7,5,8}};  // warning: lsd first !

/* ---------------------------------------------- */

int main()
{
  number aa;
  number bb;
  number a;
  number b;
  number q;

  number dhprime;
  number dhgen;

  number pkeyA;
  number skeyA;

  number pkeyB;
  number skeyB;

  number comkeyA;
  number comkeyB;

  int reterr;

  int i; 
  int j;

  /* init of the random number generator */
  srand((unsigned int)time(NULL));

  /* printf("In ring modulo : \n");
  printnum(&modulo);
  printf("Read number a (lsd first) \n");
  do
    {
    reterr = readnum(&aa);
    }
  while (reterr != 0);
  */
  /* printf("Read number b (lsd first) \n");
  do 
    {
    reterr = readnum(&bb);
    }
  while (reterr != 0); */
  // dividenum(&aa,&modulo,&q,&a);
  // dividenum(&bb,&modulo,&q,&b);
  // printf("The number a is : \n");
  // printnum(&a);
  //printf("The number b is : \n");
  //printnum(&b);
  for (i = 0 ; i < 100 ; i++)
    {
    printf("Setup %d \n",i);
    diffiesetup(75, &dhprime, &dhgen);
    for (j = 0 ; j < 100 ; j++)
      {
      diffiekeypair(&dhprime, &dhgen, &pkeyA, &skeyA);
      diffiekeypair(&dhprime, &dhgen, &pkeyB, &skeyB);
      diffiecommon(&dhprime,&pkeyA, &skeyB, &comkeyB);  
      diffiecommon(&dhprime,&pkeyB, &skeyA, &comkeyA);
      if (isbigger(&comkeyB,&comkeyA) != 0)
        {
        printf("ERROR !  No common key ! \n");  
        printf("Generating a D-H setup : \n");
        printf("The D-H prime is: \n");
        printnum(&dhprime);
        printf("and the generator is: \n");
        printnum(&dhgen);
        printf("For Alice, the private key is: \n");
        printnum(&skeyA);
        printf("and her public key is: \n");
        printnum(&pkeyA);
        printf("while for Bob, the private key is: \n");
        printnum(&skeyB);
        printf("and his public key is: \n");
        printnum(&pkeyB);
        printf("Alice finds as a common secret with Bob: \n");
        printnum(&comkeyA);
        printf("while Bob finds as a common secret with Alice: \n");
        printnum(&comkeyB);
        }
      }
    }
}  // end main

/* ----------------------------------------------- */

int readnum(number* x)
  /* This function reads in the digits of a number from stdin */
  /* The reading is from lsb to msb                           */
  {
  printf("Read digits : \n");
  int i;
  int mydigit;
  
  for (i = 0; i < nofdigits ; i++)
    {
    scanf("%d",&mydigit);
    if (mydigit < 0)
      {
      printf("negative digit ! \n");
      return 1;
      }
    if (mydigit >= basis)
      {
      printf("digit out of range ! \n");
      return 1;
      }
    x->digit[i] = mydigit;
    }
  return 0;
  }  // end readnum

/* ----------------------------------------------- */

void printnum(number* x)
  /* this function prints out a number on stdout */
  {
  int i;
  for (i = nofdigits - 1 ; i >= 0 ; i--)
    {
    printf("%d ",x->digit[i]);
    }
  printf("\n");
  } // end printnum

/* ----------------------------------------------- */

void difnum(number* x, number* y, number* s)
  /* under the condition that x and y are numbers within
     the modulo ring, s is the difference of x - y in the
     modulo ring */
  {
  int i;
  int borrow;
  int dd;
  int ss;
  int carry;
  borrow = 0;
  // calculation of the difference using borrowing
  // but not taking into account the modular structure
  if (printlevel > 0)
    printf("Difference without modular structure: \n");
  for (i = 0 ; i < nofdigits ; i++)
    {
    dd = x->digit[i] - y->digit[i] - borrow;
    if (printlevel > 0)
      printf("i is %d and dd is %d ",i,dd);
    if (dd < 0)
      {
      dd += basis;
      borrow = 1;
      }
    else
      {
      borrow = 0;
      }
    s->digit[i] = dd;
    if (printlevel > 0)
      printf(" --> borrow is %d and finally digit is %d \n",borrow,dd);
    }
  // in the case of a "negative" result, 
  // we add one time the modulo size.
  if (borrow != 0)
    {
    if (printlevel > 0)
      printf("Negative ! \n");
    carry = 0;
    for (i = 0; i < nofdigits ; i++)
      {
      ss = s->digit[i] + modulo.digit[i] + carry;
      carry = ss / basis;
      s->digit[i] = ss % basis;
      if (printlevel > 0)
        printf("  -> ss is %d, carry is %d and digit is %d \n",ss,carry,s->digit[i]);
      }
    }
  }  // end difnum

/* ----------------------------------------------- */

int isbigger(number* x, number* y)
  /* indicates whether a number x is larger than the number 
     y in the modulo field (this is an Euclid function) */
  {
  int bibi;
  int i;
  bibi = 0;
  for (i = nofdigits - 1 ; i >= 0 ; i--)
    {
    if (x->digit[i] > y->digit[i])
      {
      bibi = 1;
      break;
      }
    else if (x->digit[i] < y->digit[i])
      {
      bibi = -1;
      break;
      }
    }
  return bibi;
  }  // end isbigger

/* ----------------------------------------------- */

int isbigger1(number1* x, number1* y)
  /* indicates whether a number x is larger than the number 
     y with extended length */
  {
  int bibi;
  int i;
  bibi = 0;
  for (i = nofdigits ; i >= 0 ; i--)
    {
    if (x->digit[i] > y->digit[i])
      {
      bibi = 1;
      break;
      }
    else if (x->digit[i] < y->digit[i])
      {
      bibi = -1;
      break;
      }
    }
  return bibi;
  }  // end isbigger

/* ----------------------------------------------- */

void sumnum(number* x, number* y, number* s)
  /* under the assumption that x and y are within the modulo 
     ring, s is the sum x + y in this ring */
  {
  int i;
  int carry;
  int ss;
  int borrow;
  int dd;
  number sups;
  carry = 0;
  /* calculate sum without modulo structure */
  if (printlevel > 0)
    printf("Sum without modular structure \n");
  for (i = 0 ; i < nofdigits ; i++)
    {
    ss = x->digit[i] + y->digit[i] + carry;
    carry = ss / basis;
    sups.digit[i] = ss % basis;
    if (printlevel > 0)
      printf("i is %d, ss is %d, carry is %d and digit is %d \n",i,ss,carry,sups.digit[i]);
    }
  /* if "overflow" we subtract one time the modulo */
  if (carry != 0)
    {
    borrow = 0;
    if (printlevel > 0)
      printf("Overflow ! \n");
    for (i = 0 ; i < nofdigits ; i++)
      {
      dd = sups.digit[i] - modulo.digit[i] - borrow;
      if (printlevel > 0)
        printf("i is %d, dd is %d ",i,dd);
      if (dd < 0)
        {
        dd += basis;
        borrow = 1;
        }
      else
        {
        borrow = 0;
        }
      if (printlevel > 0)
        printf(" -> after correction: dd is %d and borrow is %d \n",dd,borrow);
      sups.digit[i] = dd;
      }
    if (borrow != carry)
      {
      printf("Internal inconsistency: Dikken ambras !! \n");
      }
    }
  /* if the number is bigger than the modulo we subtract modulo.
     note that the subtraction fills in the output ;
     if the subtraction is not needed, we need to copy 
     the result into the output */
  int big;
  big = isbigger(&sups,&modulo);
  if (big >= 0)
    {
    difnum(&sups,&modulo,s);
    }
  else
    {
    for (i = 0 ; i < nofdigits ; i++)
      {
      s->digit[i] = sups.digit[i];
      }
    }
  }  // end sumnum

/* ----------------------------------------------- */

void digittimesnumber(int dig,number* x,number1* s)
/* multiplication of a single digit with a number */
  {
  int i;
  int j;
  int carry;
  int ss;
  carry = 0;
  if (printlevel > 0)
    printf("Digit times number; digit is %d \n",dig);
  for (i = 0 ; i < nofdigits ; i++)
    {
    ss = dig * x->digit[i] + carry;
    s->digit[i] = ss % basis;
    carry = ss / basis;
    if (printlevel > 0)
      printf("i is %d, ss is %d, carry is %d and digit is %d \n",i,ss,carry,s->digit[i]);
    }
  s->digit[nofdigits] = carry;
  }

/* ----------------------------------------------- */

void bigmodulo(bignum* x,number* r)
  {
  int i;
  int k;
  int km;
  int ll;
  bignum1 rr;
  number1 high;
  number1 testhigh;
  /* We copy the dividend into a working number */
  for (i = 2*nofdigits - 1; i >= 0 ; i--)
    {
    rr.digit[i] = x->digit[i];
    }
  rr.digit[2*nofdigits] = 0;
  /* we will now apply long division, digit by digit */
  for (i = 2*nofdigits - 1 ; i >= nofdigits - 1 ; i--)
    {
    /* we estimate a lower boundary on the digit of the quotient */
    km = rr.digit[i+1]*basis + rr.digit[i];
    ll = km / (modulo.digit[nofdigits - 1] + 1);
    /* we copy the relevant part of the dividend */
    /* we have to copy nofdigits + 1 digits      */
    if (printlevel > 0)
      printf("i is %d . Estimated q: %d ; Relevant part of dividend : \n",i,ll);
    for (k = i + 1 ; k >= i - nofdigits + 1 ; k--)
      {
      high.digit[k - i + nofdigits - 1] = rr.digit[k];
      if (printlevel > 0)      
        printf("digit %d is %d \n",k,rr.digit[k]);
      }
    /* we have to find out how many times to subtract "modulo" 
       before this number is bigger than the relevant part of the 
       dividend */
    do
      {
      digittimesnumber(ll,&modulo,&testhigh);
      ll++;
      }
    while (isbigger1(&high,&testhigh) >= 0);
    ll--;
    ll--;
    if (printlevel > 0)
      printf("Quotient digit number %d is %d \n",i,ll);
    /* we will now do the subtraction */
    digittimesnumber(ll,&modulo,&testhigh);
    int borrow;
    int dd;
    borrow = 0;
    for (k = 0 ; k <= nofdigits ; k++)
      {
      dd = high.digit[k] - testhigh.digit[k] - borrow;
      if (dd < 0)
        {
        dd = dd + basis;
        borrow = 1;
        }
      else
        {
        borrow = 0;
        }
      high.digit[k] = dd;
      }
    if (high.digit[nofdigits] !=0 || borrow != 0)
      {
      printf("Internal inconsistency: Vried ambras ! \n");
      }
    /* we update the intermediate result */
    for (k = i + 1; k >= i - nofdigits + 1; k--)
      {
      rr.digit[k] = high.digit[k - i + nofdigits - 1];
      }
    }
  /* we copy the remainder into the result */
  for (i = nofdigits - 1 ; i >= 0 ; i--)
    {
    r->digit[i] = rr.digit[i];
    }
  }  // end bigmodulo

/* ----------------------------------------------- */

void timesnum(number* x, number* y, number* p)
/* multiplication in the modular field */
  {
  int i;
  int j;
  bignum pp;
  int ss;
  for (i = 0 ; i < 2*nofdigits ; i++)
    {
    pp.digit[i] = 0;
    }
  /* rough multiplication, out of scope of basis per digit */
  for (i = 0 ; i < nofdigits ; i++)
    {
    for (j = 0 ; j < nofdigits ; j++)
      {
      pp.digit[i+j] += x->digit[i] * y->digit[j];
      }
    }
  /* limiting each digit to the basis, generalized carry */
  for (i = 0 ; i < 2 * nofdigits - 1 ; i++)
    {
    ss = pp.digit[i];
    pp.digit[i+1] += ss / basis;
    pp.digit[i] = ss % basis;
    }
  /* applying modulo */
  bigmodulo(&pp,p);
  } // end multiplynum

/* ----------------------------------------------- */

void dividenum(number* x, number* y, number* q, number* r)
/* Euclidean division */
  {
  int i;
  int k;
  int km;
  int ll;
  int divlen;
  number zero;
  number1 rr;
  number1 high;
  number1 testhigh;
  /* We copy the dividend into an intermediate working number */
  for (i = nofdigits - 1; i >= 0 ; i--)
    {
    rr.digit[i] = x->digit[i];
    q->digit[i] = 0;  // we put to zero the quotient
    zero.digit[i] = 0;
    }
  rr.digit[nofdigits] = 0;
  if (isbigger(y,&zero) == 0)
    {
    printf("Division by zero doesn't work ! \n");
    return;
    }
  /* we determine the number of active digits in the divider */
  for (divlen = nofdigits - 1 ; divlen >= 0 ; divlen--)
    {
    if (y->digit[divlen] != 0)
      {
      break;
      }
    }
  if (printlevel > 0) 
    printf("Divlen is : %d \n",divlen);
  /* divlen will contain the index of the first 
     non-zero divider digit */
  /* we will now apply long division, digit by digit */
  for (i = nofdigits - 1 ; i >= divlen ; i--)
    {
    /* we estimate a lower boundary on the digit of the quotient */
    km = rr.digit[i+1]*basis + rr.digit[i];
    ll = km / (y->digit[divlen] + 1);
    if (printlevel > 0)
      {
      printf("Quotient estimate %d \n",ll);
      printf("Relevant part of dividend : \n");
      }
    /* we copy the relevant part of the dividend */
    /* we have to copy nofdigits + 1 digits      */
    for (k = i + 1 ; k >= i - divlen ; k--)
      {
      high.digit[k - i + divlen] = rr.digit[k];
      if (printlevel > 0) 
        printf("digit %d is %d \n",k,rr.digit[k]);
      }
    for (k = divlen + 2 ; k < nofdigits + 1 ; k++)
      {
      high.digit[k] = 0;
      }
    /* we have to find out how many times to subtract "modulo" 
       before this number is bigger than the relevant part of the 
       dividend */
    do
      {
      digittimesnumber(ll,y,&testhigh);
      ll++;
      }
    while (isbigger1(&high,&testhigh) >= 0);
    ll--;
    ll--;
    if (printlevel > 0)
      printf("Quotient digit number %d is %d \n",i,ll);
    q->digit[i - divlen] = ll;
    /* we will now do the subtraction */
    digittimesnumber(ll,y,&testhigh);
    int borrow;
    int dd;
    borrow = 0;
    for (k = 0 ; k <= divlen + 1 ; k++)
      {
      dd = high.digit[k] - testhigh.digit[k] - borrow;
      if (dd < 0)
        {
        dd = dd + basis;
        borrow = 1;
        }
      else
        {
        borrow = 0;
        }
      high.digit[k] = dd;
      }
    if (high.digit[divlen + 1] !=0 || borrow != 0)
      {
      printf("Internal inconsistency: Hiel vried ambras ! \n");
      }
    /* we update the intermediate result */
    for (k = i+1 ; k >= i - divlen ; k--)
      {
      rr.digit[k] = high.digit[k - i + divlen];
      }
    }
  /* we copy the remainder into the result */
  for (i = nofdigits - 1 ; i >= 0 ; i--)
    {
    r->digit[i] = rr.digit[i];
    }
  }  // end dividenum

/* ----------------------------------------------- */

void exteuclid(number* a, number* b, number* gcd, number* s, number* t)
  {
  number rm1;  // (r-1)  ; gcd is (r-2)
  number s1;   // (s-1)  ; s is (s-2)
  number t1;   // (t-1)  ; t is (t-2)
  number q;    
  number r;
  number d;
  number dodo;
  number sn;
  number tn;
  number zero;
  int i;
  for (i = 0 ; i < nofdigits ; i++)
    {
    gcd->digit[i] = a->digit[i];
    rm1.digit[i] = b->digit[i];
    s1.digit[i] = 0;
    t1.digit[i] = 0;
    s->digit[i] = 0;
    t->digit[i] = 0;
    zero.digit[i] = 0;
    }
  if (isbigger(a,&zero) == 0 || isbigger(b,&zero) == 0)
    {
    printf("Algorithm doesn't work with 0 as input !\n");
    return;
    } 
  s->digit[0] = 1;
  t1.digit[0] = 1;
  do
    {
    /* Extended Euclidean algorithm iteration */
    dividenum(gcd,&rm1,&q,&r);         // r = (r-2) mod (r-1)
    difnum(gcd,&r,&d);                
    dividenum(&d,&rm1,&q,&dodo);       // q = { (r-2) - r } / (r-1)
    timesnum(&q,&s1,&dodo);
    difnum(s,&dodo,&sn);               // s = (s-2) - q * (s-1)
    timesnum(&q,&t1,&dodo);
    difnum(t,&dodo,&tn);               // t = (t-2) - q * (t-1)
    /* preparing next iteration i -> i+1 */
    for (i = 0 ; i < nofdigits ; i++)
      {
      gcd->digit[i] = rm1.digit[i];    // (r-1) -> (r-2)
      rm1.digit[i]  = r.digit[i];       // r     -> (r-1)
      s->digit[i]   = s1.digit[i];       // (s-1) -> (s-2)
      s1.digit[i]   = sn.digit[i];       // s     -> (s-1)
      t->digit[i]   = t1.digit[i];       // (t-1) -> (t-2)
      t1.digit[i]   = tn.digit[i];       // t     -> (t-1)
      }
    }
  while (isbigger(&r,&zero) != 0);
  }  // end exteuclid

/* ----------------------------------------------- */

void inversenum(number* x,number* invx)
  {
  number oldmodulo;
  number maxmodulo;
  number gcd;
  number s;
  number een;
  number zero;
  number invx1;
  number q;
  int i;
  /* we put the modulo value in a safe place, and put the 
     current modulo parameter to its maximum */
  if (modulo.digit[nofdigits-1] >= basis/2)
    {
    printf("Error: number space not large enough to apply extended Euclid! \n");
    return;
    }
  for (i = 0 ; i < nofdigits ; i++)
    {
    maxmodulo.digit[i] = basis - 1;
    oldmodulo.digit[i] = modulo.digit[i];
    modulo.digit[i] = maxmodulo.digit[i];
    een.digit[i] = 0;
    zero.digit[i] = 0;
    invx->digit[i] = 0;
    }
  if (isbigger(x,&zero) == 0)
    {
    printf("Inverse of zero not possible !\n");
    for (i = 0 ; i < nofdigits ; i++)
      {
      modulo.digit[i] = oldmodulo.digit[i];
      }
    return;
    }
  /* we apply the extended Euclidean algorithm to find 1/x */
  een.digit[0] = 1;
  exteuclid(&oldmodulo,x,&gcd,&s,&invx1);
  if (isbigger(&gcd,&een) != 0)
    {
    printf("GCD is not one ! \n");
    }
  if (isbigger(&invx1,&oldmodulo) >= 0)
    {
    sumnum(&invx1,&oldmodulo,invx);
    }
  else
    {
    sumnum(&invx1,&zero,invx);
    }
  /* we put the original modulo parameter back */
  for (i = 0 ; i < nofdigits ; i++)
    {
    modulo.digit[i] = oldmodulo.digit[i];
    }
  }  // end inversenum

/* ----------------------------------------------- */

void divide2(number* x, number* q, int* r)
/* Euclidean division by two */
  {
  int i;
  int rest;
  rest = 0;
  int divi;
  for (i = nofdigits - 1 ; i >= 0 ; i--)
    {
    divi = rest * basis + x->digit[i];
    q->digit[i] = divi / 2;
    rest = divi % 2;
    }
  *r = rest;
  }  // end divide2

/* ----------------------------------------------- */

void power(number* x, number* y, number* xy)
/* number x to the power y */
  {
  int i;
  int j;
  number p;
  number s;
  number zero;
  number half;
  number full;
  int uneven;
  int bitsequence[basisbits * nofdigits];
  int bitcounter;
  for (i = 0 ; i < nofdigits ; i++)
    {
    xy->digit[i] = 0;
    zero.digit[i] = 0;
    full.digit[i] = y->digit[i];
    }
  // catching the case where x is zero.
  if (isbigger(x,&zero) != 0)
    {
    xy->digit[0] = 1;
    }
  bitcounter = 0;
  /* we find the binary representation of the power */
  do
    {
    divide2(&full,&half,&uneven);
    bitsequence[bitcounter] = uneven;
    for (i = 0 ; i < nofdigits ; i++)
      {
      full.digit[i] = half.digit[i];
      }
    bitcounter++;
    }
  while (isbigger(&half,&zero) > 0);
  /* the square-and-multiply algorithm */
  for (i = bitcounter - 1 ; i >= 0 ; i--)
    {
    timesnum(xy,xy,&s);  // we square
    timesnum(&s,x,&p);   // we multiply
    if (bitsequence[i] == 1)
      {
      for (j = 0 ; j < nofdigits ; j++)
        {
        xy->digit[j] = p.digit[j];
        }
      }
    else
      {
      for (j = 0 ; j < nofdigits ; j++)
        {
        xy->digit[j] = s.digit[j];
        }
      }
    }
  }  // end power

/* ----------------------------------------------- */

int isprimefermat(number* x, int k)
/* applies the Fermat primality test to the number x with k trials randomly selected in the range 2..x-1    */
/* the function returns 1 when the number x is most probably a prime, and 0 when it is for sure not a prime */
  {
  number oldmodulo;
  number zero;
  number one;
  number testaraw;
  number q;
  number testa;
  number xminus1;
  number testresult;
  int kcnt;
  int isstillprime;
  int i;

  /* we put temporarily the modulus equal to x */
  for (i = 0 ; i < nofdigits ; i++)
    {
    oldmodulo.digit[i] = modulo.digit[i];
    modulo.digit[i] = x->digit[i];
    zero.digit[i] = 0;
    one.digit[i] = 0;
    }
  one.digit[0] = 1;
  difnum(x,&one,&xminus1);
  isstillprime = 0;
  kcnt = -1;
  do
    {
    kcnt++;
    /* we generate a new random test number that is not zero */
    do
      {
      for (i = 0 ; i < nofdigits ; i++)
        {
        testaraw.digit[i] = rand() % basis ; 
        }
      dividenum(&testaraw,&modulo,&q,&testa);
      }
    while (isbigger(&testa,&zero) == 0);

    /* we calculate a^(x-1)*/
    power(&testa,&xminus1,&testresult);
    /* we do the Fermat test */
    if (isbigger(&testresult,&one) != 0)
      {
      isstillprime = 1;
      }
    } // end of loop over number of random tests
  while (isstillprime == 0 && kcnt < k);
  /* we put the original modulo parameter back */
  for (i = 0 ; i < nofdigits ; i++)
    {
    modulo.digit[i] = oldmodulo.digit[i];
    }
  return 1 - isstillprime;
  }  // end isprimefermat

/* ----------------------------------------------- */

void findnextprimefermat(number* x, number* y, int k)
/* using Fermat's primality test, finds the first prime that follows x */
  {
  number nextone;
  number nextnextone;
  number one;
  number q;
  int r;
  int i;
  for (i = 0 ; i < nofdigits ; i++)
    {
    one.digit[i] = 0;
    }
  one.digit[0] = 1;
  divide2(x,&q,&r);
  if (r == 1)
    {
    // x is uneven, OK
    sumnum(x,&one,&nextone);
    }
  else
    {
    // x is even, we need to add one
    sumnum(x,&one,&nextnextone);
    sumnum(&nextnextone,&one,&nextone);
    }
  do
    {
    sumnum(&nextone,&one,&nextnextone);
    sumnum(&nextnextone,&one,&nextone);
    }
  while (isprimefermat(&nextnextone,k) == 0);
  difnum(&nextone,&one,y);
  } // end findnextprimefermat

/* ----------------------------------------------- */

int isprimerabin(number* x, int k)
/* applies the Miller-Rabin primality test to the number x with k trials randomly selected in the range 2..x-1 */
/* the function returns 1 when the number x is most probably a prime, and 0 when it is for sure not a prime    */
  {
  number oldmodulo;
  number zero;
  number one;
  number testaraw;
  number q;
  number testa;
  number testa2;
  number xminus1;
  number rabin;
  number testresult;
  int kcnt;
  int isstillprime;
  int i;
  int u;
  int uu;
  int r;

  /* we put temporarily the modulus equal to x */
  for (i = 0 ; i < nofdigits ; i++)
    {
    oldmodulo.digit[i] = modulo.digit[i];
    modulo.digit[i] = x->digit[i];
    zero.digit[i] = 0;
    one.digit[i] = 0;
    }
  one.digit[0] = 1;
  /* we obtain the composition of x - 1 = 2^u * r */
  difnum(x,&one,&rabin);
  difnum(x,&one,&xminus1);
  sumnum(&rabin,&zero,&q);
  u = 0;
  do
    {
    sumnum(&q,&zero,&rabin);
    divide2(&rabin,&q,&r);
    u++;
    }
  while (r == 0);
  u--;
  /* at this point, the rabin exponent r is in rabin, and u has the power of 2 */
  if (isbigger(x,&one) != 1)
    {
    isstillprime = 1;
    }
  else
    {
    isstillprime = 0;
    }
  kcnt = -1;
  do
    {
    kcnt++;
    /* we generate a new random test number that is not zero */
    do
      {
      for (i = 0 ; i < nofdigits ; i++)
        {
        testaraw.digit[i] = rand() % basis ; 
        }
      dividenum(&testaraw,&modulo,&q,&testa);
      }
    while (isbigger(&testa,&zero) == 0 || isbigger(&testa,&one) == 0 || isbigger(&testa,&xminus1) == 0);
    /* we start the Miller-Rabin test */
    power(&testa,&rabin,&testresult);
    /* the first part of the test verifies whether this test results in a 1 or a x-1 */
    if (isbigger(&testresult,&one) != 0 && isbigger(&testresult,&xminus1) != 0)
      {
      uu = 0;
      do
        {
        uu++;
        // we take the square of the a^m and put it back in testa
        timesnum(&testresult,&testresult,&testa2);
        sumnum(&testa2,&zero,&testresult);
        if (isbigger(&testresult,&one) == 0)
          {
          isstillprime = 1;
          }
        }  // end of loop over squaring
      while (uu < u && isbigger(&testresult,&xminus1) != 0);
      // final test on last entity
      if (isbigger(&testresult,&xminus1) != 0)
        {
        isstillprime = 1;
        }       
      }  // end need for a loop test
    }  // next random number
  while (isstillprime == 0 && kcnt < k);
  /* we put the original modulo parameter back */
  for (i = 0 ; i < nofdigits ; i++)
    {
    modulo.digit[i] = oldmodulo.digit[i];
    }
  return 1 - isstillprime;
  }  // end isprimefermat

/* ----------------------------------------------- */

void findnextprimerabin(number* x, number* y, int k)
/* using Miller-Rabin's primality test, finds the first prime that follows x */
  {
  number nextone;
  number nextnextone;
  number one;
  number q;
  int r;
  int i;
  for (i = 0 ; i < nofdigits ; i++)
    {
    one.digit[i] = 0;
    }
  one.digit[0] = 1;
  divide2(x,&q,&r);
  if (r == 1)
    {
    // x is uneven, OK
    sumnum(x,&one,&nextone);
    }
  else
    {
    // x is even, we need to add one
    sumnum(x,&one,&nextnextone);
    sumnum(&nextnextone,&one,&nextone);
    }
  do
    {
    sumnum(&nextone,&one,&nextnextone);
    sumnum(&nextnextone,&one,&nextone);
    }
  while (isprimerabin(&nextnextone,k) == 0);
  difnum(&nextone,&one,y);
  } // end findnextprimerabin

/* ----------------------------------------------- */

void randomgen(number* myran)
/* generate a random number in the modular ring */
  {
  number rawran;
  number q;
  int i;

  for (i = 0 ; i < nofdigits ; i++)
    {
      rawran.digit[i] = rand() % basis ; 
    }
  dividenum(&rawran,&modulo,&q,myran);
  } // end randomgen

/* ----------------------------------------------- */

void makebig(int small, number* big)
/* convert a machine integer into a big number */
  {
  int i;
  int r;
  int q;
  for (i = 0 ; i < nofdigits ; i++)
    {
    big->digit[i] = 0;
    }
  i = 0;
  q = small;
  do
    {
    r = q % basis;
    q = q / basis;
    big->digit[i] = r;
    i++;
    }
  while (q > 0); 
  } // end makebig

/* ----------------------------------------------- */

void rsakeygen(int nbits, number* p1, number* p2, number* n, number* pkey, number* skey)
  {
  number oldmodulo;
  number zero;
  number one;
  number two;
  number allbits;
  number nbitsbig;
  number ran1;
  number ranlim;
  number dummy;
  number dummy2;
  number p1m1;
  number p2m1;
  number phin;
  number gcd;
  int i;

  /* we first test that the number of bits of the keys is less than half the available number of bits in our integers */
  if (nbits > basisbits * (nofdigits - 1) / 2 ) 
    {
    printf("rsakeygen error: nbits too large. \n");
    }

  /* we put the current modulus away and set the modulus to the maximum */
  for (i = 0 ; i < nofdigits ; i++)
    {
    oldmodulo.digit[i] = modulo.digit[i];
    modulo.digit[i] = basis - 1;
    zero.digit[i] = 0;
    one.digit[i] = 0;
    two.digit[i] = 0;
    }
  one.digit[0] = 1;
  two.digit[0] = 2;
  /* we calculate the highest possible number in agreement with our number of bits */
  makebig(nbits,&nbitsbig);
  power(&two,&nbitsbig,&allbits);
  /* we find the first prime p1 */
  do
    {
    randomgen(&ran1);
    dividenum(&ran1,&allbits,&dummy,&ranlim);
    findnextprimerabin(&ranlim,p1,20);
    }
  while (isbigger(p1,&allbits) != -1);
  /* we find the second prime p2 */
  do
    {
    randomgen(&ran1);
    dividenum(&ran1,&allbits,&dummy,&ranlim);
    findnextprimerabin(&ranlim,p2,20);
    }
  while (isbigger(p2,&allbits) != -1);
  timesnum(p1,p2,n);
  difnum(p1,&one,&p1m1);
  difnum(p2,&one,&p2m1);
  timesnum(&p1m1,&p2m1,&phin);
  /* we now find a number e (called pkey) such that gcd(e,phin) = 1 */
  do
    {
    randomgen(&ran1);
    dividenum(&ran1,&phin,&dummy,pkey);
    exteuclid(pkey, &phin, &gcd, &dummy, &dummy2);
    }
  while (isbigger(&gcd,&one) != 0);
  /* finally, we calculate the private key which is the inverse of pkey in the ring modulo phin */
  /* we write phin in the modulus */
  for (i = 0 ; i < nofdigits ; i++)
    {
    modulo.digit[i] = phin.digit[i];
    }
  inversenum(pkey,skey);
  /* we put the original modulo parameter back */
  for (i = 0 ; i < nofdigits ; i++)
    {
    modulo.digit[i] = oldmodulo.digit[i];
    }  
  } // end rsakeygen

/* ----------------------------------------------- */

void rsacrypt(number* cleartext,number* pkey, number* n, number* cyphertext)
/* this procedure uses an existing public key to do RSA encryption */
/* it is also the procedure to decrypt: one puts the private key in, and the cypher text */
  {
  number oldmodulo;
  int i;

  /* we put the current modulus away and set the modulus to n */
  for (i = 0 ; i < nofdigits ; i++)
    {
    oldmodulo.digit[i] = modulo.digit[i];
    modulo.digit[i] = n->digit[i];
    }
  power(cleartext,pkey,cyphertext);
    /* we put the original modulo parameter back */
  for (i = 0 ; i < nofdigits ; i++)
    {
    modulo.digit[i] = oldmodulo.digit[i];
    }  
  }  // end rsacrypt


/* ----------------------------------------------- */

void diffiesetup(int nbits, number* dhprime, number* dhgen)
/* this procedure generates a Diffie-Hellman setup */
  {
  number oldmodulo;
  number zero;
  number one;
  number two;
  number nbitsbig;
  number allbits;
  number ran1;
  number dummy;
  number ranlim;
  number firstprime;
  number firstprime2;
  number test;
  int i;

  /* we put the current modulus away and set the modulus to its maximum */
  for (i = 0 ; i < nofdigits ; i++)
    {
    oldmodulo.digit[i] = modulo.digit[i];
    modulo.digit[i] = basis - 1;
    zero.digit[i] = 0;
    one.digit[i] = 0;
    two.digit[i] = 0;
    }
  one.digit[0] = 1;
  two.digit[0] = 2;
  /* we calculate the highest possible number in agreement with our number of bits (minus 1) */
  makebig(nbits-1,&nbitsbig);
  power(&two,&nbitsbig,&allbits);
  /* we try to find a strong prime, that is a prime number which is equal to 2*q + 1
     where q is also a prime number */
  do
    {
    /* we find the prime firstprime */
    do
      {
      randomgen(&ran1);
      dividenum(&ran1,&allbits,&dummy,&ranlim);
      findnextprimerabin(&ranlim,&firstprime,10);
      }
    while (isbigger(&firstprime,&allbits) != -1);
    //printf("got a first prime \n");
    //printnum(&firstprime);
    /* now, we repeat, such that 2*firstprime + 1 is also a prime: dhprime */
    timesnum(&firstprime,&two,&firstprime2);
    sumnum(&firstprime2,&one,dhprime);
    }
  while (isprimerabin(dhprime,10) == 0);
  /* we now put our Diffie-Hellman strong prime as a modulus */
  for (i = 0 ; i < nofdigits ; i++)
    {
    modulo.digit[i] = dhprime->digit[i];
    }
  /* we find the generator dhgen and test whether it is a primitive root */
  do
    {
    randomgen(&ran1);
    // note that at this point, firstprime2 is dhprime minus 1
    dividenum(&ran1,&firstprime2,&dummy,dhgen);
    power(dhgen,&firstprime,&test);
    }
  while (isbigger(&test,&one) == 0 || isbigger(dhgen,&zero) == 0);
  /* we now have a dhprime which is a strong prime, and

  /* we put the original modulo parameter back */
  for (i = 0 ; i < nofdigits ; i++)
    {
    modulo.digit[i] = oldmodulo.digit[i];
    }   
  } // end diffiesetup

/* ----------------------------------------------- */

void diffiekeypair(number* dhprime, number* dhgen, number* pkey, number* skey)
/* This routine generates a key pair if a given Diffie-Hellman setup is specified */
  {
  number oldmodulo;
  number ran1;
  number dummy;
  number one;
  number dhprimem1;
  int i;  

  /* we put the current modulus away and set the modulus to its maximum */
  for (i = 0 ; i < nofdigits ; i++)
    {
    oldmodulo.digit[i] = modulo.digit[i];
    modulo.digit[i] = dhprime->digit[i];
    one.digit[i] = 0;
    }
  one.digit[0] = 1;
  difnum(dhprime,&one,&dhprimem1);
  do
    {
    randomgen(&ran1);
    dividenum(&ran1,&dhprimem1,&dummy,skey);
    }
  while (isbigger(skey,&one) != 1);
  power(dhgen,skey,pkey);

  /* we put the original modulo parameter back */
  for (i = 0 ; i < nofdigits ; i++)
    {
    modulo.digit[i] = oldmodulo.digit[i];
    }   
  } // end diffiekeypair

/* ----------------------------------------------- */

void diffiecommon(number* dhprime,number* pkeyA, number* skeyB, number* comkey)
/* given one's own private key, and the other party's public key 
the common secret key is found within a given DH setup */
  {
  number oldmodulo;
  int i;

  /* we put the current modulus away and set the modulus to its maximum */
  for (i = 0 ; i < nofdigits ; i++)
    {
    oldmodulo.digit[i] = modulo.digit[i];
    modulo.digit[i] = dhprime->digit[i];
    }

  power(pkeyA,skeyB,comkey);

  /* we put the original modulo parameter back */
  for (i = 0 ; i < nofdigits ; i++)
    {
    modulo.digit[i] = oldmodulo.digit[i];
    }     
  } // end diffiecommon

/* ----------------------------------------------- */

