#include "cap.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define CAP 0.05L

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

    clock_t start = clock();
    cap(&data);
    clock_t end = clock();

    mutants_to_csv(argv[2], &data);
    simulation_matrix_to_csv(argv[3], &data);

    printf("num_mutants,num_tests,hard_count,runtime(ms)\n");
    printf("%zu,%zu,%zu,%Lf\n", data.num_mutants, data.num_tests, count_hard_mutants(&data),
           1000.0L * (end - start) / CLOCKS_PER_SEC);

    return 0;
}

void cap(data_t *data) {
    #pragma omp parallel for default(none) shared(data)
    for (int i = 0; i < data->num_mutants; i++) {
        for (int j = 0; j < data->num_tests; j++) {
            if (data->kill_matrix[i][j] == 1) {
                data->simulation_matrix[i][j] = KILLED;
                data->mutants[i].kill_count++;
                if (data->mutants[i].kill_count > data->num_tests * CAP) {
                    data->mutants[i].hard_to_kill = false;
                    break;
                }
            } else if (data->kill_matrix[i][j] == 0) {
                data->simulation_matrix[i][j] = NOT_KILLED;
            }
            data->mutants[i].test_count++;
        }
    }
}
