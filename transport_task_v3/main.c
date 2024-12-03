#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_PRODUCTS 100
#define MAX_NODES 50
#define MAX_EDGES 200

typedef struct {
    int id;
    double *supply_demand; // Вектор дефицита/избытка по продуктам
} Node;

typedef struct {
    int source;
    int target;
    double weight;
} Edge;

int main(int argc, char *argv[]) {
    int rank, size;
    int i, j, k;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int num_products, num_nodes, num_edges;

    double *products = NULL;
    Node *nodes = NULL;
    Edge *edges = NULL;

    // Главный процесс загружает данные
    if (rank == 0) {
        // Загрузка количества продуктов, вершин и ребер
        num_products = /* ... */;
        num_nodes = /* ... */;
        num_edges = /* ... */;

        // Инициализация массивов
        products = (double *)malloc(num_products * sizeof(double));
        nodes = (Node *)malloc(num_nodes * sizeof(Node));
        edges = (Edge *)malloc(num_edges * sizeof(Edge));

        // Загрузка данных продуктов
        for (i = 0; i < num_products; i++) {
            products[i] = /* ... */;
        }

        // Загрузка данных вершин
        for (i = 0; i < num_nodes; i++) {
            nodes[i].id = i;
            nodes[i].supply_demand = (double *)malloc(num_products * sizeof(double));
            for (j = 0; j < num_products; j++) {
                nodes[i].supply_demand[j] = /* ... */;
            }
        }

        // Загрузка данных ребер
        for (i = 0; i < num_edges; i++) {
            edges[i].source = /* ... */;
            edges[i].target = /* ... */;
            edges[i].weight = /* ... */;
        }
    }

    // Распространение количества продуктов, вершин и ребер
    MPI_Bcast(&num_products, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&num_nodes, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&num_edges, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Распространение продуктов
    if (rank != 0) {
        products = (double *)malloc(num_products * sizeof(double));
    }
    MPI_Bcast(products, num_products, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // Распространение вершин
    if (rank != 0) {
        nodes = (Node *)malloc(num_nodes * sizeof(Node));
        for (i = 0; i < num_nodes; i++) {
            nodes[i].supply_demand = (double *)malloc(num_products * sizeof(double));
        }
    }
    for (i = 0; i < num_nodes; i++) {
        MPI_Bcast(nodes[i].supply_demand, num_products, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    }

    // Распространение ребер
    if (rank != 0) {
        edges = (Edge *)malloc(num_edges * sizeof(Edge));
    }
    MPI_Bcast(edges, num_edges * sizeof(Edge), MPI_BYTE, 0, MPI_COMM_WORLD);

    // Разделение продуктов между процессами
    int products_per_proc = num_products / size;
    int remainder = num_products % size;
    int start_product = rank * products_per_proc + ((rank < remainder) ? rank : remainder);
    int end_product = start_product + products_per_proc + (rank < remainder ? 1 : 0);

    // Локальный массив для результатов
    double **local_flows = (double **)malloc((end_product - start_product) * sizeof(double *));
    for (i = 0; i < (end_product - start_product); i++) {
        local_flows[i] = (double *)malloc(num_edges * sizeof(double));
    }

    // Выполнение алгоритма для назначенных продуктов
    for (k = start_product; k < end_product; k++) {
        int product_index = k - start_product;
        // Реализация метода потенциалов для продукта k
        // Инициализация матрицы затрат и потенциалов
        // Поиск оптимального плана
        // Заполнение local_flows[product_index][i] для каждого ребра
    }

    // Сбор результатов на главный процесс
    if (rank == 0) {
        double **global_flows = (double **)malloc(num_products * sizeof(double *));
        for (i = 0; i < num_products; i++) {
            global_flows[i] = (double *)malloc(num_edges * sizeof(double));
        }
    }

    // Использование MPI_Gatherv для сбора данных разной длины
    // ...

    // Главный процесс записывает результаты в файл
    if (rank == 0) {
        FILE *output = fopen("results.txt", "w");
        if (output == NULL) {
            perror("Ошибка открытия файла");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        // Запись результатов
        for (k = 0; k < num_products; k++) {
            fprintf(output, "Продукт %d:\n", k);
            for (i = 0; i < num_edges; i++) {
                fprintf(output, "Ребро %d-%d: поток = %f\n", edges[i].source, edges[i].target, global_flows[k][i]);
            }
            fprintf(output, "\n");
        }

        fclose(output);
    }

    // Освобождение памяти
    // ...

    MPI_Finalize();
    return 0;
}