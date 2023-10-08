/*
    -- computeCumulativeProbabilities:
        Dans ce programme, l'objectif est de calculer les probabilités cumulatives.
       
    -- En entrée  :  
        -> décomptes de classes fournis 
    -- En sortie  :
        <- les probabilités cumulatives stockées dans un tableau de type double* 
*/


#include <stdio.h>
#include <stdlib.h> 
#include <time.h>
#include <math.h>



/* Period parameters */  
#define N 624
#define M 397
#define MATRIX_A 0x9908b0dfUL   /* constant vector a */
#define UPPER_MASK 0x80000000UL /* most significant w-r bits */
#define LOWER_MASK 0x7fffffffUL/* least significant r bits */

static unsigned long mt[N]; /* the array for the state vector  */
static int mti=N+1; /* mti==N+1 means mt[N] is not initialized */

/* initializes mt[N] with a seed */
void init_genrand(unsigned long s)
{
    mt[0]= s & 0xffffffffUL;
    for (mti=1; mti<N; mti++) {
        mt[mti] = 
	    (1812433253UL * (mt[mti-1] ^ (mt[mti-1] >> 30)) + mti); 
        /* See Knuth TAOCP Vol2. 3rd Ed. P.106 for multiplier. */
        /* In the previous versions, MSBs of the seed affect   */
        /* only MSBs of the array mt[].                        */
        /* 2002/01/09 modified by Makoto Matsumoto             */
        mt[mti] &= 0xffffffffUL;
        /* for >32 bit machines */
    }
}

/* initialize by an array with array-length */
/* init_key is the array for initializing keys */
/* key_length is its length */
/* slight change for C++, 2004/2/26 */
void init_by_array(unsigned long init_key[], int key_length)
{
    int i, j, k;
    init_genrand(19650218UL);
    i=1; j=0;
    k = (N>key_length ? N : key_length);
    for (; k; k--) {
        mt[i] = (mt[i] ^ ((mt[i-1] ^ (mt[i-1] >> 30)) * 1664525UL))
          + init_key[j] + j; /* non linear */
        mt[i] &= 0xffffffffUL; /* for WORDSIZE > 32 machines */
        i++; j++;
        if (i>=N) { mt[0] = mt[N-1]; i=1; }
        if (j>=key_length) j=0;
    }
    for (k=N-1; k; k--) {
        mt[i] = (mt[i] ^ ((mt[i-1] ^ (mt[i-1] >> 30)) * 1566083941UL))
          - i; /* non linear */
        mt[i] &= 0xffffffffUL; /* for WORDSIZE > 32 machines */
        i++;
        if (i>=N) { mt[0] = mt[N-1]; i=1; }
    }

    mt[0] = 0x80000000UL; /* MSB is 1; assuring non-zero initial array */ 
}

/* generates a random number on [0,0xffffffff]-interval */
unsigned long genrand_int32(void)
{
    unsigned long y;
    static unsigned long mag01[2]={0x0UL, MATRIX_A};
    /* mag01[x] = x * MATRIX_A  for x=0,1 */

    if (mti >= N) { /* generate N words at one time */
        int kk;

        if (mti == N+1)   /* if init_genrand() has not been called, */
            init_genrand(5489UL); /* a default initial seed is used */

        for (kk=0;kk<N-M;kk++) {
            y = (mt[kk]&UPPER_MASK)|(mt[kk+1]&LOWER_MASK);
            mt[kk] = mt[kk+M] ^ (y >> 1) ^ mag01[y & 0x1UL];
        }
        for (;kk<N-1;kk++) {
            y = (mt[kk]&UPPER_MASK)|(mt[kk+1]&LOWER_MASK);
            mt[kk] = mt[kk+(M-N)] ^ (y >> 1) ^ mag01[y & 0x1UL];
        }
        y = (mt[N-1]&UPPER_MASK)|(mt[0]&LOWER_MASK);
        mt[N-1] = mt[M-1] ^ (y >> 1) ^ mag01[y & 0x1UL];

        mti = 0;
    }
  
    y = mt[mti++];

    /* Tempering */
    y ^= (y >> 11);
    y ^= (y << 7) & 0x9d2c5680UL;
    y ^= (y << 15) & 0xefc60000UL;
    y ^= (y >> 18);

    return y;
}

/* generates a random number on [0,0x7fffffff]-interval */
long genrand_int31(void)
{
    return (long)(genrand_int32()>>1);
}

/* generates a random number on [0,1]-real-interval */
double genrand_real1(void)
{
    return genrand_int32()*(1.0/4294967295.0); 
    /* divided by 2^32-1 */ 
}

/* generates a random number on [0,1)-real-interval */
double genrand_real2(void)
{
    return genrand_int32()*(1.0/4294967296.0); 
    /* divided by 2^32 */
}

/* generates a random number on (0,1)-real-interval */
double genrand_real3(void)
{
    return (((double)genrand_int32()) + 0.5)*(1.0/4294967296.0); 
    /* divided by 2^32 */
}

/* generates a random number on [0,1) with 53-bit resolution*/
double genrand_res53(void) 
{ 
    unsigned long a=genrand_int32()>>5, b=genrand_int32()>>6; 
    return(a*67108864.0+b)*(1.0/9007199254740992.0); 
} 

/* Fonction pour calculer les probabilités cumulatives */
double* computeCumulativeProbabilities(int numClasses, const int* classCounts) {
    /* Vérifie que les paramètres d'entrée sont valides */
    if (numClasses <= 0 || classCounts == NULL) {
        printf("Paramètres d'entrée invalides.\n");
        return NULL;
    }

    /* Alloue de la mémoire pour stocker les probabilités cumulatives */
    double* cumulativeProbs = (double*)malloc(numClasses * sizeof(double));
    if (cumulativeProbs == NULL) {
        printf("Erreur lors de l'allocation de mémoire.\n");
        return NULL;
    }

    /* Calcule le total des décomptes de classes */
    int total = 0;
    for (int i = 0; i < numClasses; i++) {
        if (classCounts[i] < 0) {
            printf("Comptage de classe invalide : %d\n", classCounts[i]);
            free(cumulativeProbs); // Libère la mémoire en cas d'erreur
            return NULL;
        }
        total += classCounts[i];
    }

    /* Vérifie que le total n'est pas nul, sinon les probabilités ne peuvent pas être calculées */
    if (total == 0) {
        printf("Le décompte total est zéro, impossible de calculer les probabilités.\n");
        free(cumulativeProbs); // Libère la mémoire en cas d'erreur
        return NULL;
    }

    /* Calcule les probabilités cumulatives */
    double cumulativeProb = 0.0;
    for (int i = 0; i < numClasses; i++) {
        cumulativeProb += (double)classCounts[i] / total;
        cumulativeProbs[i] = cumulativeProb;
    }

    return cumulativeProbs;
}

int main() {
    /* Définition du nombre de classes et de leurs décomptes */
    int numClasses = 3;
    int classCounts[] = {500, 150, 350};

    /* Calcul des probabilités cumulatives à partir des décomptes de classes */
    double* cumulativeProbs = computeCumulativeProbabilities(numClasses, classCounts);

    /* Vérification si l'allocation mémoire a réussi */
    if (cumulativeProbs != NULL) {
        printf("-- Probabilités cumulatives :\n");
        for (int i = 0; i < numClasses; i++) {
            printf(" - Classe %d : %lf\n", i + 1, cumulativeProbs[i]);
        }
        /* Libération de la mémoire allouée pour les probabilités cumulatives */
        free(cumulativeProbs);
    } else {
        /* Gestion d'erreur en cas d'échec de l'allocation mémoire */
        printf("Erreur lors de l'allocation de mémoire.\n");
        return EXIT_FAILURE;
    }

    /* Indique la fin du programme */
    return EXIT_SUCCESS;
}