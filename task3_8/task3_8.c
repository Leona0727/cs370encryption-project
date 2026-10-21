#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <openssl/evp.h>

#define RUNS 100
#define HASH_SIZE 16777216

// Get the first 24 bits of a SHA-256 hash 
uint32_t get_hash (const char *text){
    unsigned char hash [EVP_MAX_MD_SIZE];
    unsigned int length;

    if ( EVP_Digest(text, strlen(text), hash, &length, EVP_sha256(), NULL) != 1) {
                 printf("Hash calculation failed\n");
                exit(1);
}

    return ( (uint32_t) hash [0] << 16) | ((uint32_t)hash[1] << 8) | hash[2];
}

//Find a message with the same hash as the fixed message 
uint64_t weak_test(int run ) {
    char target_text[50];
    char text[80];

    snprintf (target_text, sizeof(target_text),  "target-%d", run);

    uint32_t target = get_hash(target_text);

    for (uint64_t i = 1; ; i++) {
        snprintf(text, sizeof(text), "candidate-%d-%llu", run, (unsigned long long)i);

        if (get_hash(text) == target) {
            return i;
        }
    }
}

// Find any two messages with the same hash 
uint64_t strong_test(int run)
{
    unsigned char *used = calloc(HASH_SIZE, 1);
    char text[50];

    if (used == NULL) {
        printf ( "Memory allocation failed\n");
        return 0;
    }

    for (uint64_t i = 1; ; i++) {
        snprintf ( text, sizeof(text), "message-%d-%llu",  run, (unsigned long long)i);

         uint32_t hash = get_hash(text);

        if(used[hash]) {
            free (used);
            return i;
        }

        used [hash] = 1;
    }
}

int main(void)
{
    double weak_total = 0;
    double strong_total = 0;

for (int i = 1; i <= RUNS; i++) {
    uint64_t weak = weak_test(i);
    uint64_t strong =  strong_test(i);

        weak_total += weak;
        strong_total += strong;

        printf("Run %d: weak = %llu, strong = %llu\n",     i + 1, (unsigned long long)weak,(unsigned long long)strong);
    }

    printf("\nAverage weak trials: %.2f\n", weak_total / RUNS);

    printf("Average strong trials: %.2f\n", strong_total / RUNS);

    return 0;
}
