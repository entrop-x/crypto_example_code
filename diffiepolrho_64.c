#include <stdio.h>
#include <time.h>
#include <stdlib.h>


#define basis 1000
#define basisbits 10
#define nofdigits 20
#define printlevel 0

/* "number" is a structure with nofdigits digits, the normal number in the
   ring structure we consider ; number1 has one more digit to allow for the 
   overflow when multiplying a number with a single digit and other intermediate
   results ; bignum has twice as much digits, to allow for the product of two numbers
   before the modulo operation ; we also need bignum1 which has one single extra digit
   more than bignum for overflow results when working with the product */
typedef struct digits {unsigned long long digit[nofdigits];} number;
typedef struct digitus {unsigned long long digit[nofdigits + 1];} number1;
typedef struct digitis {unsigned long long digit[nofdigits * 2];} bignum;
typedef struct digitas {unsigned long long digit[nofdigits*2+1];} bignum1;

int readnum(number* x);
void printnum(number* x);
void difnum(number* x, number* y, number* s, number* modulo);
void sumnum(number* x, number* y, number* s, number* modulo);
int isbigger(number* x, number* y);
int isbigger1(number1* x, number1* y);
void digittimesnumber(long long dig,number* x,number1* s);
void bigmodulo(bignum* x,number* r,number* modulo);
void timesnum(number* x, number* y, number* p, number* modulo);
void dividenum(number* x, number* y, number* q, number* r);
void exteuclid(number* a, number* b, number* gcd, number* s, number* t);
void inversenum(number* x,number* invx, number* modulo);
void divide2(number* x, number* q, long long* r);
void power(number* x, number* y, number* xy, number* modulo);
int isprimefermat(number* x, int k);
void findnextprimefermat(number* x, number* y, int k, number* modulo);
int isprimerabin(number* x, int k);
void findnextprimerabin(number* x, number* y, int k, number* modulo);
void randomgen(number* myran, number* modulo);
void makebig(long long small, number* big);
void rsakeygen(int nbits, number* p1, number* p2, number* n, number* pkey, number* skey);
void rsacrypt(number* cleartext,number* pkey, number* n, number* cyphertext);
void diffiesetup(int nbits, number* dhprime, number* dhgen);
void diffiekeypair(number* dhprime, number* dhgen, number* pkey, number* skey);
void diffiecommon(number* dhprime,number* pkeyA, number* skeyB, number* comkey);
void clone(number* x, number* c);
void diffiepolrhohelper(number* dhprime, number* dhprimem1, number* dhgen, number* onethird, number* twothird, number* pkey, number* x, number* a, number* b);
long long diffiepolrho(long long n, number* dhprime, number* dhgen, number* pkey, number* skey);


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

  number skeyAguess;
  number skeyBguess;

  number mymodulo;
  number p1;
  number p2;
  number n;
  number pkey;
  number skey;
  number cypher;
  number cleartext;

  long long trials;

  int i;

  const int rsabits = 80;

  /* init of the random number generator */
  srand((unsigned long long)time(NULL));

  for (i = 0 ; i < nofdigits ; i++)
    {
    if (i * basisbits < rsabits)
      {
      mymodulo.digit[i] = basis - 1;
      }
    else
      {
      mymodulo.digit[i] = 0;
      }
    }

  for (i = 0 ; i < 5000 ; i++)
    {
    randomgen(&aa,&mymodulo);
    dividenum(&aa,&mymodulo,&q,&a);
    rsakeygen(80, &p1, &p2, &n, &pkey, &skey);
    rsacrypt(&a,&pkey,&n,&cypher);
    rsacrypt(&cypher,&skey,&n,&cleartext);
    if (i % 10 == 0)
      {
      printf("Test %d \n",i);
      }
    if (isbigger(&cleartext,&a) != 0)
      {
      printf("ERROR !! \n");
      printf("The number a is : \n");
      printnum(&a);

      printf("generating an RSA key \n");

      printf("The first prime is \n");
      printnum(&p1);
      printf("The second prime is \n");
      printnum(&p2);
      printf("The public n is \n");
      printnum(&n);
      printf("The public key is \n");
      printnum(&pkey);
      printf("The private key is \n");
      printnum(&skey);
      printf("The cypher text becomes: \n");
      printnum(&cypher);
      printf("... and back to the cleartext: \n");  
      printnum(&cleartext);
      }
    }  

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
  printf("Generating a D-H setup : \n");
  diffiesetup(36, &dhprime, &dhgen);
  printf("The D-H prime is: \n");
  printnum(&dhprime);
  printf("and the generator is: \n");
  printnum(&dhgen);
  printf("For Alice, the private key is: \n");
  diffiekeypair(&dhprime, &dhgen, &pkeyA, &skeyA);
  printnum(&skeyA);
  printf("and her public key is: \n");
  printnum(&pkeyA);
  printf("while for Bob, the private key is: \n");
  diffiekeypair(&dhprime, &dhgen, &pkeyB, &skeyB);
  printnum(&skeyB);
  printf("and his public key is: \n");
  printnum(&pkeyB);
  diffiecommon(&dhprime,&pkeyA, &skeyB, &comkeyB);  
  diffiecommon(&dhprime,&pkeyB, &skeyA, &comkeyA);  
  printf("Alice finds as a common secret with Bob: \n");
  printnum(&comkeyA);
  printf("while Bob finds as a common secret with Alice: \n");
  printnum(&comkeyB);
  printf("we try to find the secret key of Alice: \n");
  if ((trials = diffiepolrho(1465000000, &dhprime,&dhgen, &pkeyA, &skeyAguess)) == 0)
    {
    printf("failure to find it.... \n");
    }
  else
    {
    printf("we found a candidate for Alice's secret key !  We needed %llutrials. It is : \n",trials);
    printnum(&skeyAguess);
    if (isbigger(&skeyAguess,&skeyA) == 0)
      {
      printf("SUCCESS !! \n");
      }
    else
      {
      printf("FAILURE !! \n");
      }
    }
  printf("we try to find the secret key of Bob: \n");
  if ((trials = diffiepolrho(1465000000, &dhprime,&dhgen, &pkeyB, &skeyBguess)) == 0)
    {
    printf("failure to find it.... \n");
    }
  else
    {
    printf("we found a candidate for Bob's secret key !  We needed %llutrials. It is : \n",trials);
    printnum(&skeyBguess);
    if (isbigger(&skeyBguess,&skeyB) == 0)
      {
      printf("SUCCESS !! \n");
      }
    else
      {
      printf("FAILURE !! \n");
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
  long long mydigit;
  int u;
  
  for (i = 0; i < nofdigits ; i++)
    {
    u = scanf("%llu",&mydigit);
    if (mydigit < 0)
      {
      printf("Error: negative digit ! \n");
      return 1;
      }
    if (mydigit >= basis)
      {
      printf("Error: digit out of range ! \n");
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
    printf("%llu ",x->digit[i]);
    }
  printf("\n");
  } // end printnum

/* ----------------------------------------------- */

void difnum(number* x, number* y, number* s, number* modulo)
  /* under the condition that x and y are numbers within
     the modulo ring, s is the difference of x - y in the
     modulo ring */
  {
  int i;
  long long borrow;
  long long dd;
  long long ss;
  long long carry;
  borrow = 0;
  // calculation of the difference using borrowing
  // but not taking into account the modular structure
  if (printlevel > 0)
    printf("Difference without modular structure: \n");
  for (i = 0 ; i < nofdigits ; i++)
    {
    dd = x->digit[i] - y->digit[i] - borrow;
    if (printlevel > 0)
      printf("i is %d and dd is %llu",i,dd);
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
      printf(" --> borrow is %llu and finally digit is %llu\n",borrow,dd);
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
      ss = s->digit[i] + modulo->digit[i] + carry;
      carry = ss / basis;
      s->digit[i] = ss % basis;
      if (printlevel > 0)
        printf("  -> ss is %llu, carry is %llu and digit is %llu\n",ss,carry,s->digit[i]);
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

void sumnum(number* x, number* y, number* s, number* modulo)
  /* under the assumption that x and y are within the modulo 
     ring, s is the sum x + y in this ring */
  {
  int i;
  long long carry;
  long long ss;
  long long borrow;
  long long dd;
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
      printf("i is %d, ss is %llu, carry is %llu and digit is %llu\n",i,ss,carry,sups.digit[i]);
    }
  /* if "overflow" we subtract one time the modulo */
  if (carry != 0)
    {
    borrow = 0;
    if (printlevel > 0)
      printf("Overflow ! \n");
    for (i = 0 ; i < nofdigits ; i++)
      {
      dd = sups.digit[i] - modulo->digit[i] - borrow;
      if (printlevel > 0)
        printf("i is %d, dd is %llu",i,dd);
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
        printf(" -> after correction: dd is %llu and borrow is %llu\n",dd,borrow);
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
  big = isbigger(&sups,modulo);
  if (big >= 0)
    {
    difnum(&sups,modulo,s,modulo);
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

void digittimesnumber(long long dig,number* x,number1* s)
/* multiplication of a single digit with a number */
  {
  int i;
  long long carry;
  long long ss;
  carry = 0;
  if (printlevel > 0)
    printf("Digit times number; digit is %llu\n",dig);
  for (i = 0 ; i < nofdigits ; i++)
    {
    ss = dig * x->digit[i] + carry;
    s->digit[i] = ss % basis;
    carry = ss / basis;
    if (printlevel > 0)
      printf("i is %d, ss is %llu, carry is %llu and digit is %llu\n",i,ss,carry,s->digit[i]);
    }
  s->digit[nofdigits] = carry;
  }

/* ----------------------------------------------- */

void bigmodulo(bignum* x,number* r,number* modulo)
  {
  int i;
  int k;
  long long km;
  long long ll;
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
    ll = km / (modulo->digit[nofdigits - 1] + 1);
    /* we copy the relevant part of the dividend */
    /* we have to copy nofdigits + 1 digits      */
    if (printlevel > 0)
      printf("i is %llu. Estimated q: %llu; Relevant part of dividend : \n",i,ll);
    for (k = i + 1 ; k >= i - nofdigits + 1 ; k--)
      {
      high.digit[k - i + nofdigits - 1] = rr.digit[k];
      if (printlevel > 0)      
        printf("digit %llu is %llu\n",k,rr.digit[k]);
      }
    /* we have to find out how many times to subtract "modulo" 
       before this number is bigger than the relevant part of the 
       dividend */
    do
      {
      digittimesnumber(ll,modulo,&testhigh);
      ll++;
      }
    while (isbigger1(&high,&testhigh) >= 0);
    ll--;
    ll--;
    if (printlevel > 0)
      printf("Quotient digit number %llu is %llu\n",i,ll);
    /* we will now do the subtraction */
    digittimesnumber(ll,modulo,&testhigh);
    long long borrow;
    long long dd;
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

void timesnum(number* x, number* y, number* p, number* modulo)
/* multiplication in the modular field */
  {
  int i;
  int j;
  bignum pp;
  long long ss;
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
  bigmodulo(&pp,p,modulo);
  } // end multiplynum

/* ----------------------------------------------- */

void dividenum(number* x, number* y, number* q, number* r)
/* Euclidean division */
  {
  int i;
  int k;
  long long km;
  long long ll;
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
      printf("Quotient estimate %llu\n",ll);
      printf("Relevant part of dividend : \n");
      }
    /* we copy the relevant part of the dividend */
    /* we have to copy nofdigits + 1 digits      */
    for (k = i + 1 ; k >= i - divlen ; k--)
      {
      high.digit[k - i + divlen] = rr.digit[k];
      if (printlevel > 0) 
        printf("digit %d is %llu\n",k,rr.digit[k]);
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
      printf("Quotient digit number %d is %llu\n",i,ll);
    q->digit[i - divlen] = ll;
    /* we will now do the subtraction */
    digittimesnumber(ll,y,&testhigh);
    long long borrow;
    long long dd;
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
  number maxmodulo;
  long long i;
  for (i = 0 ; i < nofdigits ; i++)
    {
    gcd->digit[i] = a->digit[i];
    rm1.digit[i] = b->digit[i];
    s1.digit[i] = 0;
    t1.digit[i] = 0;
    s->digit[i] = 0;
    t->digit[i] = 0;
    zero.digit[i] = 0;
    maxmodulo.digit[i] = basis - 1;
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
    difnum(gcd,&r,&d,&maxmodulo);                
    dividenum(&d,&rm1,&q,&dodo);       // q = { (r-2) - r } / (r-1)
    timesnum(&q,&s1,&dodo,&maxmodulo);
    difnum(s,&dodo,&sn,&maxmodulo);               // s = (s-2) - q * (s-1)
    timesnum(&q,&t1,&dodo,&maxmodulo);
    difnum(t,&dodo,&tn,&maxmodulo);               // t = (t-2) - q * (t-1)
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

void inversenum(number* x,number* invx, number* modulo)
  {
  number maxmodulo;
  number gcd;
  number s;
  number een;
  number zero;
  number invx1;
  number q;
  int i;
  if (modulo->digit[nofdigits-1] >= basis/2)
    {
    printf("Error: number space not large enough to apply extended Euclid! \n");
    return;
    }
  for (i = 0 ; i < nofdigits ; i++)
    {
    maxmodulo.digit[i] = basis - 1;
    een.digit[i] = 0;
    zero.digit[i] = 0;
    invx->digit[i] = 0;
    }
  if (isbigger(x,&zero) == 0)
    {
    printf("Inverse of zero not possible !\n");
    return;
    }
  /* we apply the extended Euclidean algorithm to find 1/x */
  een.digit[0] = 1;
  exteuclid(modulo,x,&gcd,&s,&invx1);
  if (isbigger(&gcd,&een) != 0)
    {
    printf("GCD is not one ! \n");
    }
  if (isbigger(&invx1,modulo) >= 0)
    {
    sumnum(&invx1,modulo,invx,&maxmodulo);
    }
  else
    {
    sumnum(&invx1,&zero,invx,&maxmodulo);
    }
  }  // end inversenum

/* ----------------------------------------------- */

void divide2(number* x, number* q, long long* r)
/* Euclidean division by two */
  {
  int i;
  long long rest;
  rest = 0;
  long long divi;
  for (i = nofdigits - 1 ; i >= 0 ; i--)
    {
    divi = rest * basis + x->digit[i];
    q->digit[i] = divi / 2;
    rest = divi % 2;
    }
  *r = rest;
  }  // end divide2

/* ----------------------------------------------- */

void power(number* x, number* y, number* xy, number* modulo)
/* number x to the power y */
  {
  int i;
  int j;
  number p;
  number s;
  number zero;
  number half;
  number full;
  long long uneven;
  long long bitsequence[basisbits * nofdigits];
  long long bitcounter;
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
    timesnum(xy,xy,&s,modulo);  // we square
    timesnum(&s,x,&p,modulo);   // we multiply
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
  number zero;
  number one;
  number maxmodulo;
  number testaraw;
  number q;
  number testa;
  number xminus1;
  number testresult;
  int kcnt;
  int isstillprime;
  int i;

  for (i = 0 ; i < nofdigits ; i++)
    {
    zero.digit[i] = 0;
    one.digit[i] = 0;
    maxmodulo.digit[i] = basis - 1;
    }
  one.digit[0] = 1;
  difnum(x,&one,&xminus1,&maxmodulo);
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
      dividenum(&testaraw,x,&q,&testa);
      }
    while (isbigger(&testa,&zero) == 0);

    /* we calculate a^(x-1)*/
    power(&testa,&xminus1,&testresult,x);
    /* we do the Fermat test */
    if (isbigger(&testresult,&one) != 0)
      {
      isstillprime = 1;
      }
    } // end of loop over number of random tests
  while (isstillprime == 0 && kcnt < k);
  return 1 - isstillprime;
  }  // end isprimefermat

/* ----------------------------------------------- */

void findnextprimefermat(number* x, number* y, int k, number* modulo)
/* using Fermat's primality test, finds the first prime that follows x */
  {
  number nextone;
  number nextnextone;
  number one;
  number q;
  long long r;
  long long i;
  for (i = 0 ; i < nofdigits ; i++)
    {
    one.digit[i] = 0;
    }
  one.digit[0] = 1;
  divide2(x,&q,&r);
  if (r == 1)
    {
    // x is uneven, OK
    sumnum(x,&one,&nextone,modulo);
    }
  else
    {
    // x is even, we need to add one
    sumnum(x,&one,&nextnextone,modulo);
    sumnum(&nextnextone,&one,&nextone,modulo);
    }
  do
    {
    sumnum(&nextone,&one,&nextnextone,modulo);
    sumnum(&nextnextone,&one,&nextone,modulo);
    }
  while (isprimefermat(&nextnextone,k) == 0);
  difnum(&nextone,&one,y,modulo);
  } // end findnextprimefermat

/* ----------------------------------------------- */

int isprimerabin(number* x, int k)
/* applies the Miller-Rabin primality test to the number x with k trials randomly selected in the range 2..x-1 */
/* the function returns 1 when the number x is most probably a prime, and 0 when it is for sure not a prime    */
  {
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
  long long u;
  long long uu;
  long long r;

  /* we put temporarily the modulus equal to x */
  for (i = 0 ; i < nofdigits ; i++)
    {
    zero.digit[i] = 0;
    one.digit[i] = 0;
    }
  one.digit[0] = 1;
  /* we obtain the composition of x - 1 = 2^u * r */
  difnum(x,&one,&rabin,x);
  difnum(x,&one,&xminus1,x);
  sumnum(&rabin,&zero,&q,x);
  u = 0;
  do
    {
    sumnum(&q,&zero,&rabin,x);
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
      dividenum(&testaraw,x,&q,&testa);
      }
    while (isbigger(&testa,&zero) == 0 || isbigger(&testa,&one) == 0 || isbigger(&testa,&xminus1) == 0);
    /* we start the Miller-Rabin test */
    power(&testa,&rabin,&testresult,x);
    /* the first part of the test verifies whether this test results in a 1 or a x-1 */
    if (isbigger(&testresult,&one) != 0 && isbigger(&testresult,&xminus1) != 0)
      {
      uu = 0;
      do
        {
        uu++;
        // we take the square of the a^m and put it back in testa
        timesnum(&testresult,&testresult,&testa2,x);
        sumnum(&testa2,&zero,&testresult,x);
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
  return 1 - isstillprime;
  }  // end isprimefermat

/* ----------------------------------------------- */

void findnextprimerabin(number* x, number* y, int k, number* modulo)
/* using Miller-Rabin's primality test, finds the first prime that follows x */
  {
  number nextone;
  number nextnextone;
  number one;
  number q;
  long long r;
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
    sumnum(x,&one,&nextone,modulo);
    }
  else
    {
    // x is even, we need to add one
    sumnum(x,&one,&nextnextone,modulo);
    sumnum(&nextnextone,&one,&nextone,modulo);
    }
  do
    {
    sumnum(&nextone,&one,&nextnextone,modulo);
    sumnum(&nextnextone,&one,&nextone,modulo);
    }
  while (isprimerabin(&nextnextone,k) == 0);
  difnum(&nextone,&one,y,modulo);
  } // end findnextprimerabin

/* ----------------------------------------------- */

void randomgen(number* myran,number* modulo)
/* generate a random number in the modular ring */
  {
  number rawran;
  number q;
  long long i;

  for (i = 0 ; i < nofdigits ; i++)
    {
      rawran.digit[i] = rand() % basis ; 
    }
  dividenum(&rawran,modulo,&q,myran);
  } // end randomgen

/* ----------------------------------------------- */

void makebig(long long small, number* big)
/* convert a machine integer into a big number */
  {
  int i;
  long long r;
  long long q;
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
  number modulo;
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
  long long i;

  /* we first test that the number of bits of the keys is less than half the available number of bits in our integers */
  if (nbits > basisbits * (nofdigits - 1) / 2 ) 
    {
    printf("rsakeygen error: nbits too large. \n");
    }

  /* we set the modulus to the maximum */
  for (i = 0 ; i < nofdigits ; i++)
    {
    modulo.digit[i] = basis - 1;
    zero.digit[i] = 0;
    one.digit[i] = 0;
    two.digit[i] = 0;
    }
  one.digit[0] = 1;
  two.digit[0] = 2;
  /* we calculate the highest possible number in agreement with our number of bits */
  makebig(nbits,&nbitsbig);
  power(&two,&nbitsbig,&allbits,&modulo);
  /* we find the first prime p1 */
  do
    {
    randomgen(&ran1,&modulo);
    dividenum(&ran1,&allbits,&dummy,&ranlim);
    findnextprimerabin(&ranlim,p1,20,&modulo);
    }
  while (isbigger(p1,&allbits) != -1);
  /* we find the second prime p2 */
  do
    {
    randomgen(&ran1,&modulo);
    dividenum(&ran1,&allbits,&dummy,&ranlim);
    findnextprimerabin(&ranlim,p2,20,&modulo);
    }
  while (isbigger(p2,&allbits) != -1);
  timesnum(p1,p2,n,&modulo);
  difnum(p1,&one,&p1m1,&modulo);
  difnum(p2,&one,&p2m1,&modulo);
  timesnum(&p1m1,&p2m1,&phin,&modulo);
  /* we now find a number e (called pkey) such that gcd(e,phin) = 1 */
  do
    {
    randomgen(&ran1,&modulo);
    dividenum(&ran1,&phin,&dummy,pkey);
    exteuclid(pkey, &phin, &gcd, &dummy, &dummy2);
    }
  while (isbigger(&gcd,&one) != 0);
  /* finally, we calculate the private key which is the inverse of pkey in the ring modulo phin */

  inversenum(pkey,skey,&phin);

  } // end rsakeygen

/* ----------------------------------------------- */

void rsacrypt(number* cleartext,number* pkey, number* n, number* cyphertext)
/* this procedure uses an existing public key to do RSA encryption */
/* it is also the procedure to decrypt: one puts the private key in, and the cypher text */
  {

  power(cleartext,pkey,cyphertext,n);

  }  // end rsacrypt


/* ----------------------------------------------- */

void diffiesetup(int nbits, number* dhprime, number* dhgen)
/* this procedure generates a Diffie-Hellman setup */
  {
  number maxmodulo;
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
    maxmodulo.digit[i] = basis - 1;
    zero.digit[i] = 0;
    one.digit[i] = 0;
    two.digit[i] = 0;
    }
  one.digit[0] = 1;
  two.digit[0] = 2;
  /* we calculate the highest possible number in agreement with our number of bits (minus 1) */
  makebig(nbits-1,&nbitsbig);
  power(&two,&nbitsbig,&allbits,&maxmodulo);
  /* we try to find a strong prime, that is a prime number which is equal to 2*q + 1
     where q is also a prime number */
  do
    {
    /* we find the prime firstprime */
    do
      {
      randomgen(&ranlim,&allbits);
      findnextprimerabin(&ranlim,&firstprime,10,&maxmodulo);
      }
    while (isbigger(&firstprime,&allbits) != -1);
    printf("got a first prime \n");
    printnum(&firstprime);
    /* now, we repeat, such that 2*firstprime + 1 is also a prime: dhprime */
    timesnum(&firstprime,&two,&firstprime2,&maxmodulo);
    sumnum(&firstprime2,&one,dhprime,&maxmodulo);
    }
  while (isprimerabin(dhprime,10) == 0);
  /* we find the generator dhgen and test whether it is a primitive root */
  do
    {
    randomgen(dhgen,&firstprime2);
    // note that at this point, firstprime2 is dhprime minus 1
    power(dhgen,&firstprime,&test,dhprime);
    }
  while (isbigger(&test,&one) == 0 || isbigger(dhgen,&zero) == 0);
 
  } // end diffiesetup

/* ----------------------------------------------- */

void diffiekeypair(number* dhprime, number* dhgen, number* pkey, number* skey)
/* This routine generates a key pair if a given Diffie-Hellman setup is specified */
  {
  number ran1;
  number dummy;
  number one;
  number dhprimem1;
  long long i;  

  /* we put the current modulus away and set the modulus to its maximum */
  for (i = 0 ; i < nofdigits ; i++)
    {
    one.digit[i] = 0;
    }
  one.digit[0] = 1;
  difnum(dhprime,&one,&dhprimem1,dhprime);
  do
    {
    randomgen(skey,&dhprimem1);
    }
  while (isbigger(skey,&one) != 1);
  power(dhgen,skey,pkey,dhprime);

  } // end diffiekeypair

/* ----------------------------------------------- */

void diffiecommon(number* dhprime,number* pkeyA, number* skeyB, number* comkey)
/* given one's own private key, and the other party's public key 
the common secret key is found within a given DH setup */
  {

  power(pkeyA,skeyB,comkey,dhprime);
   
  } // end diffiecommon

/* ----------------------------------------------- */

void clone(number* x, number* c)
  {
  int i;
  for (i = 0 ; i < nofdigits ; i++)
    {
    c->digit[i] = x->digit[i];
    }
  }  // end clone

/* ----------------------------------------------- */

void diffiepolrhohelper(number* dhprime, number* dhprimem1, number* dhgen, number* onethird, number* twothird, number* pkey, number* x, number* a, number* b)
/* helper function for the Pollard's Rho method against Diffie-Hellman */
  {
  number newx;
  number newa;
  number newb;
  number one;
  int i;

  makebig(1,&one);

  if (isbigger(x,onethird) == -1) 
    {
    /* we are in the first third */
    timesnum(x,pkey,&newx,dhprime);
    clone(a,&newa);
    sumnum(b,&one,&newb,dhprimem1);
    }
  else if (isbigger(x,twothird) == -1)
    {
    /* we are in the second third */
    timesnum(x,x,&newx,dhprime);
    sumnum(a,a,&newa,dhprimem1);
    sumnum(b,b,&newb,dhprimem1);
    }
  else
    {
    /* we are in the last third */
    timesnum(x,dhgen,&newx,dhprime);
    sumnum(a,&one,&newa,dhprimem1);
    clone(b,&newb);
    }

  /* we overwrite the inputs */
  clone(&newa,a);
  clone(&newb,b);
  clone(&newx,x);
  
  }  // end diffiepolrhohelper


/* ----------------------------------------------- */

long long diffiepolrho(long long n, number* dhprime, number* dhgen, number* pkey, number* skey)
/* we implement the Pollard' Rho attack on a Diffie Hellman secret key */
/* when the function returns a non-zero number, we have found a solution, when it returns 0, we didn't 
   The non-zero number is equal to the number of steps needed */
  {

  number three;
  number zero;
  number onethird;
  number twothird;
  number dummy;
  number one;
  number grouporder;
  long long i;
  long long j;
  number oldmodulo;
  number turtoisex;
  number turtoisea;
  number turtoiseb;
  number harex;
  number harea;
  number hareb;
  number difa;
  number difb;
  number abgcd;
  number abngcd;
  number divgrouporder;
  number dummy1;
  number dummy2;
  number difared;
  number difbred;
  number inversedifbred;
  number skey2;
  number pkeytest;
  long long toreturn;


  /* we prepare the onethird and twothird needed by the helper function */
  makebig(1,&one);
  makebig(3,&three); 
  makebig(0,&zero);
  difnum(dhprime,&one,&grouporder,dhprime);
  dividenum(&grouporder,&three,&onethird,&dummy);
  sumnum(&onethird,&onethird,&twothird,dhprime);

  /* we prepare the turtoise-hare loop */
  makebig(0,&turtoisea);
  makebig(0,&turtoiseb);
  makebig(0,&harea);
  makebig(0,&hareb);
  makebig(1,&turtoisex);
  makebig(1,&harex);
  i = 0;
  do
    {
    i++;
    /* next turtoise */
    diffiepolrhohelper(dhprime,&grouporder,dhgen,&onethird,&twothird,pkey,&turtoisex,&turtoisea,&turtoiseb);
    /* next hare */
    diffiepolrhohelper(dhprime,&grouporder,dhgen,&onethird,&twothird,pkey,&harex,&harea,&hareb);    
    diffiepolrhohelper(dhprime,&grouporder,dhgen,&onethird,&twothird,pkey,&harex,&harea,&hareb);    
    }
  while (i < n && isbigger(&harex,&turtoisex) != 0); 

  if (isbigger(&harex,&turtoisex) != 0)
    {
    /* we didn't find a solution */
    toreturn = 0;
    }
  else
    {
    /* if we are here, then we did find a solution, we simply have to calculate it 
       the calculation takes place in the modular ring modulo p - 1 */
    difnum(&turtoisea,&harea,&difa,&grouporder);
    difnum(&hareb,&turtoiseb,&difb,&grouporder);

    if (isbigger(&difb,&zero) == 0)
      {
      /*failure: the difference is 0 */
      toreturn = 0;
      }
    else
      {
      /* because p - 1 is not a prime, we cannot guarantee that 1/difb exists.
         at least we increase our chances by simplifying by the gcd of difa and difb and the group order 
         we then try to solve the inversion in a smaller group  (order divided by GCD) */
      exteuclid(&difa,&difb,&abgcd,&dummy1,&dummy2);
      exteuclid(&abgcd,&grouporder,&abngcd,&dummy1,&dummy2);
      printf("GCD is: \n");
      printnum(&abngcd);
      dividenum(&difa,&abngcd,&difared,&dummy1);
      dividenum(&difb,&abngcd,&difbred,&dummy2);
      dividenum(&grouporder,&abngcd,&divgrouporder,&dummy1);
      inversenum(&difbred,&inversedifbred,&divgrouporder);
      timesnum(&inversedifbred,&difared,skey,&divgrouporder);
      /* we now test the candidates, which are of the form skey + k * divgrouporder */
      j = 0;
      clone(skey,&skey2);
      do
        {
        j++;
        clone(&skey2,skey);
        power(dhgen,skey,&pkeytest,dhprime);
        sumnum(skey,&divgrouporder,&skey2,&grouporder);
        }
      while(j < n && isbigger(&pkeytest,pkey) != 0);
      if (isbigger(&pkeytest,pkey) == 0)
        {
        toreturn = i;
        }
      else
        {
        /* failure, we ran out of trials with k */
        toreturn = 0;
        }
      }
    }   
  return toreturn;
  }  // end diffiepolrho

