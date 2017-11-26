#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>


#define basis 10000
#define basisbits 14
#define nofdigits 8
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

/* "elliptic" is a structure that contains a number and a boolean that tells us whether
   the number is "zero" or not, because elliptic curves have a non-number neutral element */
typedef struct digitos {bool zero ; number x ; number y;} elliptic;

/* "ellipcurve" is a structure that contains the parameters of an elliptic curve on a modular field */
typedef struct digitossos {number prime ; number a ; number b ; elliptic generator; } ellipcurve;

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
void elgamalencrypt(number* dhprime, number* key, number* cleartext, number* cypher);
void elgamaldecrypt(number* dhprime, number* key, number* cypher, number* cleartext);
void randomgenmod(number* mod,number* myran);
void ellipcopy(elliptic* orig, elliptic* copy);
void ellipsum(ellipcurve* mycurve,elliptic* a, elliptic* b, elliptic* sum);
void printelliptic(elliptic* n);
void elliptictimes(ellipcurve* mycurve,elliptic* x, number* y, elliptic* xy);
void ellipdiffiekeypair(ellipcurve* mycurve,number* skey, elliptic* pkey);
int legendresymbol(number* prime,number* a);
void cipolla(number* prime, number* square, number* root1, number* root2);
int ellipyfromx(ellipcurve* mycurve,number* x,number* y1,number* y2);
void toycurve(ellipcurve* mycurve,int nbits);

number modulo = {{7,5,8}};  // warning: lsd first !

/* ---------------------------------------------- */

int main()
{
  number myrand;
  number halfmyrand;
  number myprime;
  number zero;
  ellipcurve curve1;
  ellipcurve curve2;
  elliptic first;
  elliptic second;
  elliptic third;
  elliptic fourth;
  number mybig;
  unsigned int i;
  int leg;
  number root1;
  number root2;
  number root12;
  number root22;
  int nofy;
  number y1;
  number y2;
  
  for (i = 0 ; i < nofdigits ; i++)
    {
    modulo.digit[i] = basis - 1;
    }
  toycurve(&curve1,95);
  printf("The chosen prime is: \n");
  printnum(&(curve1.prime));
  printf("The a and b values are: \n");
  printnum(&(curve1.a));
  printnum(&(curve1.b));
  first.zero = false;
  second.zero = false;
  ellipcopy(&(curve1.generator),&first);
  ellipsum(&curve1,&first,&first,&third);
  printf("printing sum:\n");
  printelliptic(&third);
  nofy = ellipyfromx(&curve1,&third.x,&y1,&y2);
  printf("To this x, we can reconstruct %d possible y values:\n",nofy);
  printnum(&y1);
  printnum(&y2);
  printf("testing repeated sum vs. times \n");
  for (i = 3 ; i < 100 ; i++)
    {
    ellipsum(&curve1,&first,&third,&second);
    ellipcopy(&second,&third);
    //printf("Adding %d: \n",i);
    //printelliptic(&third);
    makebig(i,&mybig);
    elliptictimes(&curve1,&first,&mybig,&fourth);
    // printf("With power \n");
    // printelliptic(&fourth);
    if (isbigger(&third.x,&fourth.x) != 0 && !(third.zero))
      {
      printf("elliptictimes failure !\n");
      }
    nofy = ellipyfromx(&curve1,&third.x,&y1,&y2);
    if ( (isbigger(&y1,&third.y) != 0 && isbigger(&y2,&third.y) != 0) || nofy == 0)
      {
      printf("point left curve ! \n");
      }
    }
  printf("Testing Legendre symbol in field \n");
  for (i = 0 ; i < nofdigits ; i++)
    {
    modulo.digit[i] = curve1.prime.digit[i];
    }
  for (i = 0 ; i < 100 ; i++)
    {
    randomgenmod(&(curve1.prime),&mybig);
    // sumnum(&myrand,&zero,&mybig);
    leg = legendresymbol(&(curve1.prime),&mybig);
    // printf("Legendresymbol for i = %d is %d.\n",i,leg);
    if (leg == 1)
      {
      // printf("==> Testing square root with Cippola:\n");
      cipolla(&(curve1.prime),&mybig,&root1,&root2);
      timesnum(&root1,&root1,&root12);
      timesnum(&root2,&root2,&root22);
      if (isbigger(&root12,&mybig) == 0 && isbigger(&root22,&mybig) == 0)
        {
        // printf("OK!\n");
        }
      else 
        {
        printf("cipolla FAILURE !! \n");
        printf("the number was: \n");
        printnum(&mybig);
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
  int rr;
  int mydigit;
  
  for (i = 0; i < nofdigits ; i++)
    {
    rr = scanf("%d",&mydigit);
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
    printf("got a first prime \n");
    printnum(&firstprime);
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

  /* we put the current modulus away and set the modulus to the D-H value */
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

void elgamalencrypt(number* dhprime, number* key, number* cleartext, number* cypher)
/* given a common D-H key and its prime, a cleartext is encrypted with the Elgamal technique */
  {
  number oldmodulo;
  int i;

  /* we put the current modulus away and set the modulus to the D-H value */
  for (i = 0 ; i < nofdigits ; i++)
    {
    oldmodulo.digit[i] = modulo.digit[i];
    modulo.digit[i] = dhprime->digit[i];
    }

  timesnum(key,cleartext,cypher);

  /* we put the original modulo parameter back */
  for (i = 0 ; i < nofdigits ; i++)
    {
    modulo.digit[i] = oldmodulo.digit[i];
    }  
  }  // end elgamalencrypt

/* ----------------------------------------------- */

void elgamaldecrypt(number* dhprime, number* key, number* cypher, number* cleartext)
/* given a common D-H key and its prime, a cleartext is encrypted with the Elgamal technique */
  {
  number oldmodulo;
  int i;
  number inversekey;

  /* we put the current modulus away and set the modulus to its maximum */
  for (i = 0 ; i < nofdigits ; i++)
    {
    oldmodulo.digit[i] = modulo.digit[i];
    modulo.digit[i] = dhprime->digit[i];
    }
  inversenum(key,&inversekey);
  timesnum(&inversekey,cypher,cleartext);

  /* we put the original modulo parameter back */
  for (i = 0 ; i < nofdigits ; i++)
    {
    modulo.digit[i] = oldmodulo.digit[i];
    }  
  }  // end elgamaldecrypt

/* ----------------------------------------------- */

void randomgenmod(number* mod,number* myran)
/* generate a random number in the modular ring */
  {
  number rawran;
  number q;
  int i;

  for (i = 0 ; i < nofdigits ; i++)
    {
      rawran.digit[i] = rand() % basis ; 
    }
  dividenum(&rawran,mod,&q,myran);
  } // end randomgen

/* ----------------------------------------------- */

void ellipcopy(elliptic* orig, elliptic* copy)
/* copies an elliptic point into another one */
  {
  int i;
  copy->zero = orig->zero;
  for (i = 0 ; i < nofdigits ; i++)
    {
    copy->x.digit[i] = orig->x.digit[i];
    copy->y.digit[i] = orig->y.digit[i];
    }
  }  // end ellipcopy

/* ----------------------------------------------- */

void ellipsum(ellipcurve* mycurve,elliptic* a, elliptic* b, elliptic* sum)
/* calculates the sum of two elliptic points on a given curve */
  {
  number s;
  number zeero;
  number two;
  number three;
  number xa2;
  number xa2t3;
  number xa2t3pa;
  number yat2;
  number invyat2;
  number xamxs;
  number stxamxs;
  number s2;
  number xat2;
  number yamyb;
  number xamxb;
  number invxamxb;
  number s2mxa;
  number oldmodulo;
  unsigned int i;
  
  // we apply the prime number of the modular field of the curve
  for (i = 0 ; i < nofdigits ; i++)
    {
    oldmodulo.digit[i] = modulo.digit[i];
    modulo.digit[i] = mycurve->prime.digit[i];
    }
  makebig(0,&zeero);
  if (a->zero && b->zero)
    {
    // we have zero plus zero
    sum->zero = true;
    }
  else if (a->zero)
    {
    // a is zero, so the sum is b
    ellipcopy(b,sum);
    }
  else if (b->zero)
    {
    // b is zero, so the sum is a
    ellipcopy(a,sum);
    }
  else if (isbigger(&(a->x),&(b->x)) == 0)
    {
    // both points a and b are on a vertical line
    // there are only two possibilities: a = b != 0 or a = -b
    if (isbigger(&(a->y),&(b->y)) == 0 && isbigger(&(a->y),&zeero) != 0)
      {
      // a is b, so we have 2a
      sum->zero = false;
      timesnum(&(a->x),&(a->x),&xa2);
      makebig(2,&two);
      makebig(3,&three);
      timesnum(&three,&xa2,&xa2t3);
      sumnum(&xa2t3,&(mycurve->a),&xa2t3pa);
      timesnum(&two,&(a->y),&yat2);
      inversenum(&yat2,&invyat2);
      timesnum(&xa2t3pa,&invyat2,&s);
      timesnum(&s,&s,&s2);
      timesnum(&two,&(a->x),&xat2);
      difnum(&s2,&xat2,&(sum->x));
      difnum(&(a->x),&(sum->x),&xamxs);
      timesnum(&xamxs,&s,&stxamxs);
      difnum(&stxamxs,&(a->y),&(sum->y));
      }
    else
      {
      // a is minus b, so we have zero
      sum->zero = true;
      }
    }
  else
    {
    // a and b are different, not on a vertical line, and not zero
    sum->zero = false;
    difnum(&(a->x),&(b->x),&xamxb);
    difnum(&(a->y),&(b->y),&yamyb);
    inversenum(&xamxb,&invxamxb);
    timesnum(&yamyb,&invxamxb,&s);
    timesnum(&s,&s,&s2);
    difnum(&s2,&(a->x),&s2mxa);
    difnum(&s2mxa,&(b->x),&(sum->x));
    difnum(&(a->x),&(sum->x),&xamxs);
    timesnum(&xamxs,&s,&stxamxs);
    difnum(&stxamxs,&(a->y),&(sum->y));  
    }
  /* we put the original modulo parameter back */
  for (i = 0 ; i < nofdigits ; i++)
    {
    modulo.digit[i] = oldmodulo.digit[i];
    }  
  } // end ellipsum

/* ----------------------------------------------- */

void printelliptic(elliptic* n)
/* printing out an elliptic point */
  {
  if (n->zero)
    {
    printf("Zero.\n");
    }
  else
    {
    printf("x: ");
    printnum(&(n->x));
    printf("y: ");
    printnum(&(n->y));
    }
  } // end printelliptic

/* ----------------------------------------------- */

void elliptictimes(ellipcurve* mycurve, elliptic* x, number* y, elliptic* xy)
/* point x to the power y */
  {
  int i;
  int j;
  elliptic p;
  elliptic s;
  number zero;
  number half;
  number full;
  int uneven;
  int bitsequence[basisbits * nofdigits];
  int bitcounter;
  xy->zero = true;
  for (i = 0 ; i < nofdigits ; i++)
    {
    zero.digit[i] = 0;
    full.digit[i] = y->digit[i];
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
    ellipsum(mycurve,xy,xy,&s);  // we "square"
    ellipsum(mycurve,&s,x,&p);   // we "multiply"
    if (bitsequence[i] == 1)
      {
      ellipcopy(&p,xy);
      }
    else
      {
      ellipcopy(&s,xy);
      }
    }
  }  // end elliptictimes

/* ----------------------------------------------- */

void ellipdiffiekeypair(ellipcurve* mycurve,number* skey, elliptic* pkey)
  /* generate a private/public key pair on an elliptic curve */
  {
  int dummy;
  number ndummy;
  number halfprime;
  number ran1;
  number one;

  makebig(1,&one);
  // we divide the curve prime by two, to have a lower estimate of the
  // cyclic group order
  divide2(&(mycurve->prime),&halfprime,&dummy);
  do
    {
    randomgen(&ran1);
    dividenum(&ran1,&halfprime,&ndummy,skey);
    }
  while (isbigger(skey,&one) != 1);
  elliptictimes(mycurve,&(mycurve->generator),skey,pkey);
  } // end ellipdiffiekeypair

/* ----------------------------------------------- */

void ellipdiffiecommon(ellipcurve* mycurve,elliptic* pkeyA,number* skeyB,elliptic* commonkey)
  /* calculate the common key to the public key of Alice and the private key of Bob */
  {
  elliptictimes(mycurve,pkeyA,skeyB,commonkey);
  }

/* ----------------------------------------------- */

int legendresymbol(number* prime,number* a)
  /* implementation of the Legendre symbol that verifies whether a 
     is a square residue in the modular field with prime */
  {
  unsigned int i;
  number oldmodulo;
  number zero;
  number one;
  number mone;
  number pm1d2;
  number blegendre;
  int rr;
  int result;

  // we apply the prime number
  for (i = 0 ; i < nofdigits ; i++)
    {
    oldmodulo.digit[i] = modulo.digit[i];
    modulo.digit[i] = prime->digit[i];
    }
  makebig(0,&zero);
  makebig(1,&one);
  difnum(&zero,&one,&mone);
  divide2(&mone,&pm1d2,&rr);
  result = 15;
  if (rr != 0)
    {
    printf("legendresymbol: failure !\n");
    result = 2;
    }
    if (result == 15)
    {
    power(a,&pm1d2,&blegendre);
    if (isbigger(&blegendre,&one) == 0)
      {
      result = 1;
      }
    else if(isbigger(&blegendre,&zero) == 0)
      {
      result = 0;
      }
    else if (isbigger(&blegendre,&mone) == 0)
      {
      result = -1;
      }
    else
      {
      printf("legendresymbol: internal inconsistency!\n");
      result = 2;
      }
    }
  /* we put the original modulo parameter back */
  for (i = 0 ; i < nofdigits ; i++)
    {
    modulo.digit[i] = oldmodulo.digit[i];
    }  
  return result; 
  }

/* ----------------------------------------------- */

void cipolla(number* prime, number* square, number* root1, number* root2)
  {
  int i;
  unsigned int j;
  number oldmodulo;
  number a;
  number a2;
  number a2msquare;
  number zero;
  number one;
  number two;
  number pm1;
  number pm1d2;
  int rr;
  number pp1d2;
  number half;
  number full;
  int uneven;
  int bitsequence[basisbits * nofdigits];
  int bitcounter;
  number root1u;
  number root1v;
  number root1u2;
  number root1v2;
  number root1v2w2;
  number root1uv;
  number root1svw2;
  number root1sua;
  number root1sva;
  number su;
  number sv;
  number pu;
  number pv;
  
  // first, we check whether "square" is a quadratic residue
  if (legendresymbol(prime,square) != 1)
    {
    printf("cipolla: Trying to find a square root of a square non-residue !\n");
    return;
    }
  // we apply the prime number
  for (i = 0 ; i < nofdigits ; i++)
    {
    oldmodulo.digit[i] = modulo.digit[i];
    modulo.digit[i] = prime->digit[i];
    }  
  // finding a non-square residue a^2 - n
  do
    {
    randomgenmod(prime,&a);
    timesnum(&a,&a,&a2);
    difnum(&a2,square,&a2msquare);
    }
  while (legendresymbol(prime,&a2msquare) != -1);
  makebig(0,&zero);
  makebig(1,&one);
  makebig(2,&two);
  difnum(&zero,&one,&pm1);
  divide2(&pm1,&pm1d2,&rr);
  sumnum(&pm1d2,&one,&pp1d2);

  // preparing the square and multiply algorithm in F_p x F_p
  for (i = 0 ; i < nofdigits ; i++)
    {
    full.digit[i] = pp1d2.digit[i];
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
  makebig(1,&root1u);
  makebig(0,&root1v);
  
  /* we will now find (a + 1 sqrt(a^2 - square))^(pp1d2)
  /* the square-and-multiply algorithm */
  for (i = bitcounter - 1 ; i >= 0 ; i--)
    {
    // we calculate root1^2 which is (u^2 + v^2*w^2, 2*u*v)
    timesnum(&root1u,&root1u,&root1u2);
    timesnum(&root1v,&root1v,&root1v2);
    timesnum(&root1v2,&a2msquare,&root1v2w2);
    sumnum(&root1u2,&root1v2w2,&su);
    timesnum(&root1u,&root1v,&root1uv);
    timesnum(&two,&root1uv,&sv);
    // we calculate s*(a,1) which is (sv*w^2 + su*a,su + a*sv)
    timesnum(&sv,&a2msquare,&root1svw2);
    timesnum(&a,&su,&root1sua);
    sumnum(&root1svw2,&root1sua,&pu);
    timesnum(&a,&sv,&root1sva);
    sumnum(&root1sva,&su,&pv);
    // we pick the right one for this bit
    if (bitsequence[i] == 1)
      {
      for (j = 0 ; j < nofdigits ; j++)
        {
        root1u.digit[j] = pu.digit[j];
        root1v.digit[j] = pv.digit[j];
        }
      }
    else
      {
      for (j = 0 ; j < nofdigits ; j++)
        {
        root1u.digit[j] = su.digit[j];
        root1v.digit[j] = sv.digit[j];
        }
      }
    }
  /* now we test whether the v-part of the root has become 0 */
  if (isbigger(&root1v,&zero) != 0)
    {
    printf("cipolla: inconsistency: v-part non-zero !\n");
    }

  /* root1u is one of the roots, -root1u is the other one */
  sumnum(&zero,&root1u,root1);
  difnum(&zero,&root1u,root2);
  /* we put the original modulo parameter back */
  for (i = 0 ; i < nofdigits ; i++)
    {
    modulo.digit[i] = oldmodulo.digit[i];
    } 
  }  // end cipolla

/* ----------------------------------------------- */

int ellipyfromx(ellipcurve* mycurve,number* x,number* y1,number* y2)
  /* calculates the two possible y-values for a given x-value on the curve
     if ever that x-value is possible.  The integer return value gives us the number
     of y-values (0,1 or 2) that are returned */
  {
  int i;
  number oldmodulo;
  number x2;
  number x3;
  number ax;
  number axpb;
  number yp2;
  int leg;
  int result;
  
  // we apply the prime number
  for (i = 0 ; i < nofdigits ; i++)
    {
    oldmodulo.digit[i] = modulo.digit[i];
    modulo.digit[i] = mycurve->prime.digit[i];
    }  
  // we calculate the y^2 corresponding to the x for the given curve
  timesnum(x,x,&x2);
  timesnum(&x2,x,&x3);
  timesnum(&(mycurve->a),x,&ax);
  sumnum(&ax,&(mycurve->b),&axpb);
  sumnum(&x3,&axpb,&yp2);
  leg = legendresymbol(&(mycurve->prime),&yp2);
  if (leg == -1)
    {
    result = 0;
    }
  else if (leg == 0)
    {
    result = 1;
    makebig(0,y1);
    makebig(0,y2);
    }
  else if (leg == 1)
    {
    cipolla(&(mycurve->prime),&yp2,y1,y2);
    result = 2;
    }
  else
    {
    result = -1;
    printf("ellipyfromx: inconsistency ! \n");
    }
    

  /* we put the original modulo parameter back */
  for (i = 0 ; i < nofdigits ; i++)
    {
    modulo.digit[i] = oldmodulo.digit[i];
    } 
  return result;
  }  // end ellipyfromx

/* ----------------------------------------------- */

void toycurve(ellipcurve* mycurve,int nbits)
  /* inefficient algorithm to generate small elliptic curves */
  /* impossible to use for real crypto */
  {

  number oldmodulo;
  number zero;
  number one;
  number two;
  number nbitsbig;
  number allbits;
  number ranlim;
  int i;
  int yok;
  number dummy;

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
  /* we just find a prime number of the order of n bits */
  do
    {
    randomgenmod(&allbits,&ranlim);
    findnextprimerabin(&ranlim,&(mycurve->prime),10);
    }
  while (isbigger(&(mycurve->prime),&allbits) == 1);

  /* we generate a random curve (random a and b) */
  randomgenmod(&(mycurve->prime),&(mycurve->a));
  randomgenmod(&(mycurve->prime),&(mycurve->b));
 
  /* we now have to find a random point on the curve.
     For this, we have to generate a random x value, and 
     verify whether the corresponding y^2 = x^3 + ax + b
     is a square residue */
  mycurve->generator.zero = false;
  do
    {
    randomgenmod(&(mycurve->prime),&(mycurve->generator.x));
    yok = ellipyfromx(mycurve,&(mycurve->generator.x),&(mycurve->generator.y),&dummy);
    }
  while (yok < 2);

  /* we put the original modulo parameter back */
  for (i = 0 ; i < nofdigits ; i++)
    {
    modulo.digit[i] = oldmodulo.digit[i];
    }
  }  // end toycurve

