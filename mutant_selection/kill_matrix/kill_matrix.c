#include "kill_matrix.h"

#include <stdio.h>
#include <stdlib.h>

void data_read_file(const char *filepath, data_t *data) {
    FILE *file = fopen(filepath, "r");

    if (file == NULL) {
        perror("Error opening kill_matrix file");
        exit(EXIT_FAILURE);
    }

    fscanf(file, "%zu %zu", &data->num_mutants, &data->num_tests);

    data->mutants = malloc(data->num_mutants * sizeof(mutant_t));
    data->tests = malloc(data->num_tests * sizeof(test_t));
    data->simulation_matrix = malloc(data->num_mutants * sizeof(killed_t *));
    data->kill_matrix = malloc(data->num_mutants * sizeof(int *));

    for (int i = 0; i < data->num_mutants; i++) {
        data->simulation_matrix[i] = malloc(data->num_tests * sizeof(killed_t));
        data->kill_matrix[i] = malloc(data->num_tests * sizeof(int));

        for (int j = 0; j < data->num_tests; j++) {
            fscanf(file, "%d", &data->kill_matrix[i][j]);
        }
    }

    if (fscanf(file, "%*d") != EOF) {
        printf("Wrong format for file %s\n", filepath);
        exit(EXIT_FAILURE);
    }

    fclose(file);
}

void mutants_to_csv(const char *filepath, data_t *data) {
    FILE *file = fopen(filepath, "w");

    if (file == NULL) {
        perror("Could not write to output file");
        exit(EXIT_FAILURE);
    }

    fprintf(file, "mutant_index,mutant_kill_count,mutant_test_count,mutant_is_killed,mutant_is_hard_to_kill\n");
    for (int i = 0; i < data->num_mutants; i++) {
        mutant_t *mutant = &data->mutants[i];

        fprintf(file, "%d,%d,%d,%s,%s\n", mutant->index, mutant->kill_count, mutant->kill_count,
                mutant->kill_count > 0 ? "yes" : "no", mutant->hard_to_kill ? "yes" : "no");
    }

    fclose(file);
}

void simulation_matrix_to_csv(const char *filepath, data_t *data) {
    FILE *file = fopen(filepath, "w");

    if (file == NULL) {
        perror("Could not write to output file");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < data->num_mutants; i++) {
        for (int j = 0; j < data->num_tests - 1; j++) {
            fprintf(file, "%d,", data->simulation_matrix[i][j]);
        }
        fprintf(file, "%d\n", data->simulation_matrix[i][data->num_tests - 1]);
    }

    fclose(file);
}

void clear_simulation_matrix(data_t *data) {
    for (int i = 0; i < data->num_mutants; i++) {
        for (int j = 0; j < data->num_tests; j++) {
            data->simulation_matrix[i][j] = UNTESTED;
        }
    }
}

void clear_mutants(data_t *data) {
    for (int i = 0; i < data->num_mutants; i++) {
        mutant_t *mutant = &data->mutants[i];

        mutant->index = i;
        mutant->kill_count = 0;
        mutant->test_count = 0;
        mutant->hard_to_kill = true;
        mutant->tests = NULL;
    }
}

void clear_tests(data_t *data) {
    for (int i = 0; i < data->num_tests; i++) {
        data->tests[i].index = i;
    }
}

size_t count_hard_mutants(data_t *data) {
    size_t count = 0UL;
    for (int i = 0; i < data->num_mutants; i++) {
        if (data->mutants[i].hard_to_kill) {
            count++;
        }
    }
    return count;
}
