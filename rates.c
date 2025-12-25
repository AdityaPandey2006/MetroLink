#include <stdio.h>
#include <stdlib.h>
#include "rates.h"
#define MAX_LINE 100
#define PRICING_FILE "pricing.txt"

// Function 1: Get price for a given distance
int get_price_for_distance(float distance) {
    FILE *file = fopen(PRICING_FILE, "r");
    if (!file) {
        perror("Error opening pricing file");
        return -1;
    }

    float min_d, max_d;
    int price;
    while (fscanf(file, "%f %f %d", &min_d, &max_d, &price) == 3) {
        if (distance >= min_d && distance < max_d) {
            fclose(file);
            return price;
        }
    }

    fclose(file);
    return -1; // No matching range
}

// Function 2: Update a range's price (or add if not found)
int update_price_range(float min_d, float max_d, int new_price) {
    FILE *file = fopen(PRICING_FILE, "r");
    if (!file) {
        perror("Error opening pricing file");
        return -1;
    }

    FILE *temp = fopen("temp.txt", "w");
    if (!temp) {
        perror("Error creating temp file");
        fclose(file);
        return -1;
    }

    float file_min, file_max;
    int file_price;
    int updated = 0;

    while (fscanf(file, "%f %f %d", &file_min, &file_max, &file_price) == 3) {
        if (file_min == min_d && file_max == max_d) {
            fprintf(temp, "%.2f %.2f %d\n", min_d, max_d, new_price);
            updated = 1;
        } else {
            fprintf(temp, "%.2f %.2f %d\n", file_min, file_max, file_price);
        }
    }

    if (!updated) {
        fprintf(temp, "%.2f %.2f %d\n", min_d, max_d, new_price);
    }

    fclose(file);
    fclose(temp);
    remove(PRICING_FILE);
    rename("temp.txt", PRICING_FILE);

    return 0;
}
