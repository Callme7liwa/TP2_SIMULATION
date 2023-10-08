/* 
   -- Paramètres de la Distribution Gaussienne :
      Ce programme génère des échantillons aléatoires à partir d'une distribution gaussienne
      avec une moyenne et un écart-type spécifiés. Il calcule ensuite et affiche la moyenne
      et l'écart-type de l'échantillon.

   -- Entrées : 
      -> MEAN (double) : Moyenne de la distribution gaussienne.
      -> STDDEV (double) : Écart-type de la distribution gaussienne.

   -- Sorties :
      -> Moyenne et écart-type de l'échantillon généré.
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define NUM_SAMPLES 1000000
#define MEAN 10.0
#define STDDEV 3.0

/* Calculate the Gaussian density function */
double gaussian_density(double x) {
    return (1.0 / (sqrt(2.0 * M_PI))) * exp(-0.5 * x * x);
}

/* Generate a random number from a Gaussian distribution */
double generate_gaussian(double mean, double stddev) {
    double x, y;
    do {
        x = ((double)rand() / RAND_MAX) * 2.0 - 1.0;  // Random number in [-1, 1]
        y = ((double)rand() / RAND_MAX) * gaussian_density(0.0);  // Random number in [0, max density]
    } while (y > gaussian_density(x));

    return mean + x * stddev;
}

int main() {
    srand(time(NULL));  // Initialize random seed

    double sum = 0.0;
    double sum_squared = 0.0;

    for (int i = 0; i < NUM_SAMPLES; i++) {
        double sample = generate_gaussian(MEAN, STDDEV);
        sum += sample;
        sum_squared += sample * sample;
    }

    double sample_mean = sum / NUM_SAMPLES;
    double sample_stddev = sqrt((sum_squared / NUM_SAMPLES) - (sample_mean * sample_mean));

    printf("-- Gaussian Distribution Parameters --\n");
    printf("Mean: %lf\n", sample_mean);
    printf("Standard Deviation: %lf\n", sample_stddev);

    return 0;
}
