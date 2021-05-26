#ifndef POC_KILL_MATRIX_H
#define POC_KILL_MATRIX_H

#include <stddef.h>
#include <stdbool.h>

typedef enum {
    NOT_KILLED = -1,
    UNTESTED = 0,
    KILLED = 1,
} killed_t;

typedef struct {
    int index;
} test_t;

typedef struct {
    int index;
    int kill_count;
    int test_count;
    bool hard_to_kill;
    test_t** tests;
} mutant_t;

typedef struct {
    mutant_t *mutant;
    test_t **tests;
} agent_t;

typedef struct {
    size_t num_mutants;
    size_t num_tests;
    test_t *tests;
    mutant_t *mutants;
    killed_t **simulation_matrix;
    int **kill_matrix;
} data_t;

void data_read_file(const char *filepath, data_t *data);
void mutants_to_csv(const char *filepath, data_t *data);
void simulation_matrix_to_csv(const char *filepath, data_t *data);

void clear_simulation_matrix(data_t *data);
void clear_mutants(data_t *data);
void clear_tests(data_t *data);

size_t count_hard_mutants(data_t *data);

#endif //POC_KILL_MATRIX_H
