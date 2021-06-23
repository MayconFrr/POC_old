#pragma clang diagnostic push
#pragma ide diagnostic ignored "bugprone-sizeof-expression"
#define STB_DS_IMPLEMENTATION

#include "smc.h"
#include "../stb_ds.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NUM_AGENTS 5
#define TESTS_PER_AGENT 5

mutant_t **hard_mutants = NULL;
mutant_t **remaining_mutants = NULL;

int main(int argc, char *argv[]) {
    if (argc < 3) {
        puts("Usage: ./main MATRIX_INPUT MUTANTS_OUTPUT MATRIX_OUTPUT");
        exit(EXIT_FAILURE);
    }

    data_t data;
    data_read_file(argv[1], &data);

    clear_simulation_matrix(&data);
    clear_mutants(&data);
    clear_tests(&data);

    // Initialize remaining needed structures
    arrsetlen(remaining_mutants, data.num_mutants);
    for (int i = 0; i < data.num_mutants; i++) {
        mutant_t *mutant = &data.mutants[i];
        remaining_mutants[i] = mutant;
        mutant->hard_to_kill = false;
        arrsetlen(mutant->tests, data.num_tests);
        for (int j = 0; j < data.num_tests; j++) {
            mutant->tests[j] = &data.tests[data.num_tests - j - 1];
        }
    }

    clock_t start = clock();
    smc(&data);
    clock_t end = clock();

    mutants_to_csv(argv[2], &data);
    simulation_matrix_to_csv(argv[3], &data);

    printf("num_mutants,num_tests,hard_count,runtime(ms)\n");
    printf("%zu,%zu,%zu,%Lf\n", data.num_mutants, data.num_tests, count_hard_mutants(&data),
           1000.0L * (end - start) / CLOCKS_PER_SEC);

    for (int i = 0; i < data.num_mutants; i++) {
        arrfree(data.mutants[i].tests);
    }
    arrfree(hard_mutants);
    arrfree(remaining_mutants);

    return 0;
}


size_t update_hard(data_t* data);

void smc(data_t *data) {
    size_t limit = 0;
    while (arrlen(remaining_mutants) != 0) {
        agent_t agents[NUM_AGENTS];

        // Assign a mutant and a set of tests to each agent;
        for (int j = 0; j < NUM_AGENTS; j++) {
            if (arrlen(remaining_mutants) != 0) {
                agents[j].mutant = NULL;
                agents[j].tests = NULL;
                arrsetcap(agents[j].tests, TESTS_PER_AGENT);

                mutant_t *mutant = arrpop(remaining_mutants);

                if (mutant->kill_count + arrlen(mutant->tests) > limit) {
                    agents[j].mutant = mutant;

                    for (int k = 0; k < TESTS_PER_AGENT; k++) {
                        if (arrlen(mutant->tests) != 0) {
                            arrput(agents[j].tests, arrpop(mutant->tests));
                        }
                    }
                } else {
                    mutant->hard_to_kill = true;
                    arrput(hard_mutants, mutant);
                }
            }
        }

        // Each agent applies its set of tests to its mutant
        // Each agent updates the kill matrix
        #pragma omp parallel for default(none) shared(data, agents) num_threads(NUM_AGENTS)
        for (int j = 0; j < NUM_AGENTS; j++) {
            if (agents[j].mutant != NULL) {
                agent_t *agent = &agents[j];

                for (int k = 0; k < arrlen(agent->tests); k++) {
                    if (data->kill_matrix[agent->mutant->index][agent->tests[k]->index] == 1) {
                        data->simulation_matrix[agent->mutant->index][agent->tests[k]->index] = KILLED;
                        agent->mutant->kill_count++;
                    } else {
                        data->simulation_matrix[agent->mutant->index][agent->tests[k]->index] = NOT_KILLED;
                    }
                    agent->mutant->test_count++;
                }
            }
            arrfree(agents[j].tests);
        }
        limit = update_hard(data);
    }
}

size_t update_hard(data_t *data) {
    size_t min = data->num_tests;
    size_t max = 0;

    arrsetlen(remaining_mutants, 0);

    for (int i = 0; i < data->num_mutants; i++) {
        int kill_count = data->mutants[i].kill_count;

        if (kill_count > max) {
            max = kill_count;
        }
        if (kill_count < min) {
            min = kill_count;
        }
    }

    if (min > max) {
        min = 0;
    }

    size_t limit = min + (max - min) / 4;

    for (int i = 0; i < data->num_mutants; i++) {
        mutant_t *mutant = &data->mutants[i];
        int kill_count = mutant->kill_count;

        if (kill_count <= limit && arrlen(mutant->tests) != 0 && !mutant->hard_to_kill) {
            arrput(remaining_mutants, mutant);
        } else if (kill_count <= limit && arrlen(mutant->tests) == 0 && !mutant->hard_to_kill) {
            mutant->hard_to_kill = true;
            arrput(hard_mutants, mutant);
        }
    }

    return limit;
}

#pragma clang diagnostic pop