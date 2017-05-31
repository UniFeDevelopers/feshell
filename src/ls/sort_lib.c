#include "./ls_lib.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/*
 * SORT BY TIME
 */

void merge_time(dir_entry *a, int p, int q, int r) {
    int i, j, k;
    int n1 = q - p + 1;
    int n2 = r - q;
    dir_entry *L = malloc(n1 * sizeof(dir_entry));
    dir_entry *R = malloc(n2 * sizeof(dir_entry));
    memcpy(L, a + p, n1 * sizeof(dir_entry));
    memcpy(R, a + q + 1, n2 * sizeof(dir_entry));
    for (i = 0, j = 0, k = p; k <= r; k++) {
        if (i == n1) a[k] = R[j++];
        else if (j == n2) a[k] = L[i++];
        else if (L[i].time <= R[j].time) a[k] = L[i++];
        else a[k] = R[j++];
    }
    free(L);
    free(R);
    return;
}

void insertion_sort_time(dir_entry *a, int p, int r) {
    int i;
    for (i = p + 1; i <=r; i++) {
        dir_entry key = a[i];
        int j = i - 1;
        while (j >= p && a[j].time < key.time) {
            a[j + 1] = a[j];
            j = j - 1;
        }
        a[j + 1] = key;
    }
}

void mixed_sort_time(dir_entry *a, int p, int r, int K) {
    if (p >= r) return;
    if (r - p < K) insertion_sort_time(a, p, r);
    else {
        int q = (p + r) / 2;
        mixed_sort_time(a, p, q, K);
        mixed_sort_time(a, q + 1, r, K);
        merge_time(a, p, q, r);
    }
}


/*
 * ALPHANUMERIC SORT
 */

void merge_alpha(dir_entry *a, int p, int q, int r) {
    int i, j, k;
    int n1 = q - p + 1;
    int n2 = r - q;
    dir_entry *L = malloc(n1 * sizeof(dir_entry));
    dir_entry *R = malloc(n2 * sizeof(dir_entry));
    memcpy(L, a + p, n1 * sizeof(dir_entry));
    memcpy(R, a + q + 1, n2 * sizeof(dir_entry));
    for (i = 0, j = 0, k = p; k <= r; k++) {
        if (i == n1) a[k] = R[j++];
        else if (j == n2) a[k] = L[i++];
        else if (strcmp(L[i].name, R[j].name) > 0) a[k] = L[i++];
        else a[k] = R[j++];
    }
    free(L);
    free(R);
    return;
}

void insertion_sort_alpha(dir_entry *a, int p, int r) {
    int i;
    for (i = p + 1; i <=r; i++) {
        dir_entry key = a[i];
        int j = i - 1;
        while (j >= p && strcmp(a[j].name, key.name) > 0) {
            a[j + 1] = a[j];
            j = j - 1;
        }
        a[j + 1] = key;
    }
}

void mixed_sort_alpha(dir_entry *a, int p, int r, int K) {
    if (p >= r) return;
    if (r - p < K) insertion_sort_alpha(a, p, r);
    else {
        int q = (p + r) / 2;
        mixed_sort_alpha(a, p, q, K);
        mixed_sort_alpha(a, q + 1, r, K);
        merge_alpha(a, p, q, r);
    }
}


/*
 * MAIN SORT FUNCTION
 */

void sort(dir_entry *a, int num_ents, char *sort_mode) {
    if (!strcmp(sort_mode, "time")) mixed_sort_time(a, 0, num_ents, SORT_K);
    else if (!strcmp(sort_mode, "alpha")) mixed_sort_alpha(a, 0, num_ents, SORT_K);
    else {
        fprintf(stderr, "\n-sort: invalid mode\n");
        exit(1);
    }
}
