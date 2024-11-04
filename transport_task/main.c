#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define INF 1000000

// Печать матрицы для отладки
void printMatrix(int *matrix, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%d ", matrix[i * cols + j]);
        }
        printf("\n");
    }
}

// Чтение данных задачи из файла
void readData(const char *filename, int **vertexImbalance, int **edges, int *numVertices, int *numEdges, int *numProducts) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Error opening file\n");
        exit(1);
    }

    fscanf(file, "%d %d %d", numVertices, numEdges, numProducts);

    *vertexImbalance = (int *)malloc((*numVertices) * (*numProducts) * sizeof(int));
    *edges = (int *)malloc((*numEdges) * 3 * sizeof(int)); // 3 for: from, to, weight

    // Чтение данных вершин
    for (int i = 0; i < *numVertices; i++) {
        for (int j = 0; j < *numProducts; j++) {
            fscanf(file, "%d", &((*vertexImbalance)[i * (*numProducts) + j]));
        }
    }

    // Чтение данных ребер
    for (int i = 0; i < *numEdges; i++) {
        int from, to, weight;
        fscanf(file, "%d %d %d", &from, &to, &weight);
        (*edges)[i * 3] = from;
        (*edges)[i * 3 + 1] = to;
        (*edges)[i * 3 + 2] = weight;
    }

    fclose(file);
}

// Реализация оптимального распределения потоков
void optimizeFlows(int *vertexImbalance, int *edges, int numVertices, int numEdges, int numProducts, int rank, int size) {
    // Алгоритм оптимизации транспортных потоков
    // Например, расширение метода потенциалов (первоначальная задача может быть решена через метод наименьшей стоимости)

    // Настройка данных
    int *localFlows = (int *)calloc(numEdges * numProducts, sizeof(int));

    // Пример распределения по процессам
    int start = rank * (numEdges / size);
    int end = (rank + 1) * (numEdges / size);
    if (rank == size - 1) {
        end = numEdges;
    }

    // Простой алгоритм: попытка сбалансировать потоки
    for (int i = start; i < end; i++) {
        int from = edges[i * 3];
        int to = edges[i * 3 + 1];
        int weight = edges[i * 3 + 2];

        for (int p = 0; p < numProducts; p++) {
            // Если у вершины откуда отправляются есть избыток, а у получателя дефицит - перевести
            int supply = vertexImbalance[from * numProducts + p];
            int demand = vertexImbalance[to * numProducts + p];

            int transfer = (supply > 0 && demand < 0) ? (supply < -demand ? supply : -demand) : 0;
            localFlows[i * numProducts + p] += transfer;
            vertexImbalance[from * numProducts + p] -= transfer;
            vertexImbalance[to * numProducts + p] += transfer;
        }
    }

    // Суммирование всех потоков на 0-ом процессе
    int *globalFlows = NULL;
    if (rank == 0) {
        globalFlows = (int *)malloc(numEdges * numProducts * sizeof(int));
    }
    MPI_Reduce(localFlows, globalFlows, numEdges * numProducts, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    free(localFlows);

    // Вывод результатов
    if (rank == 0) {
        FILE *outputFile = fopen("output.txt", "w");
        fprintf(outputFile, "Optimized Flows:\n");
        for (int i = 0; i < numEdges; i++) {
            fprintf(outputFile, "Edge %d (%d -> %d): ", i, edges[i * 3], edges[i * 3 + 1]);
            for (int p = 0; p < numProducts; p++) {
                fprintf(outputFile, "%d ", globalFlows[i * numProducts + p]);
            }
            fprintf(outputFile, "\n");
        }
        fclose(outputFile);
        free(globalFlows);
    }
}

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc != 2) {
        if (rank == 0) {
            fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        }
        MPI_Finalize();
        return 1;
    }

    int *vertexImbalance, *edges;
    int numVertices, numEdges, numProducts;

    if (rank == 0) {
        readData(argv[1], &vertexImbalance, &edges, &numVertices, &numEdges, &numProducts);
    }

    // Распространение данных задачи по всем процессам
    MPI_Bcast(&numVertices, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&numEdges, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&numProducts, 1, MPI_INT, 0, MPI_COMM_WORLD);
    
    if (rank != 0) {
        vertexImbalance = (int *)malloc(numVertices * numProducts * sizeof(int));
        edges = (int *)malloc(numEdges * 3 * sizeof(int));
    }
    MPI_Bcast(vertexImbalance, numVertices * numProducts, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(edges, numEdges * 3, MPI_INT, 0, MPI_COMM_WORLD);

    // Оптимизация потоков
    double start_time, end_time;
    MPI_Barrier(MPI_COMM_WORLD);
    start_time = MPI_Wtime();

    optimizeFlows(vertexImbalance, edges, numVertices, numEdges, numProducts, rank, size);

    MPI_Barrier(MPI_COMM_WORLD);
    end_time = MPI_Wtime();

    double elapsed_time = end_time - start_time;
    
    if (rank == 0) {
        FILE *file = fopen("timing_results.csv", "a");
        if (file) {
            fprintf(file, "%d,%d,%.6f\n", numVertices, size, elapsed_time);
            fclose(file);
        }
    }

    // Освобождение памяти
    free(vertexImbalance);
    free(edges);

    MPI_Finalize();
    return 0;
}