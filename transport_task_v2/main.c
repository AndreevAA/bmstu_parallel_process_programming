// multi_commodity_transport.c

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <glpk.h>
#include <time.h>

#define MAX_PRODUCTS 100
#define MAX_NODES 1000
#define MAX_EDGES 10000

// Структуры данных
typedef struct {
    int id;
    double surplus_deficit[MAX_PRODUCTS]; // Вектор дефицита/избытка по продуктам
} Node;

typedef struct {
    int id;
    int from;
    int to;
    double capacity; // Пропускная способность рёбер
    double cost;     // Стоимость транспортировки единицы продукта
} Edge;

// Глобальные переменные
int num_products;
int num_nodes;
int num_edges;
Node nodes[MAX_NODES];
Edge edges[MAX_EDGES];

// Функция для инициализации данных
void init_data(int products, int nodes_count, int edges_count) {
    srand(time(NULL));
    num_products = products;
    num_nodes = nodes_count;
    num_edges = edges_count;

    // Инициализация узлов
    for (int i = 0; i < num_nodes; i++) {
        nodes[i].id = i;
        for (int p = 0; p < num_products; p++) {
            nodes[i].surplus_deficit[p] = rand() % 200 - 100; // Значения от -100 до 100
        }
    }

    // Инициализация рёбер
    int edge_count = 0;
    while (edge_count < num_edges) {
        int from = rand() % num_nodes;
        int to = rand() % num_nodes;
        if (from != to) { // Предотвращение петель
            edges[edge_count].id = edge_count;
            edges[edge_count].from = from;
            edges[edge_count].to = to;
            edges[edge_count].capacity = rand() % 100 + 1; // Пропускная способность от 1 до 100
            edges[edge_count].cost = (double)(rand() % 100 + 1); // Стоимость от 1 до 100
            edge_count++;
        }
    }
}

void solve_lp(int product_index) {
    // Создание проблемной задачи
    glp_prob *lp = glp_create_prob();
    glp_set_prob_name(lp, "Multi-commodity Transport");
    glp_set_obj_dir(lp, GLP_MIN);

    // Количество переменных
    int num_vars = num_edges;

    // Определение переменных
    glp_add_cols(lp, num_vars);
    for (int i = 1; i <= num_vars; i++) {
        glp_set_col_bnds(lp, i, GLP_LO, 0.0, 0.0); // Переменные >= 0
        glp_set_obj_coef(lp, i, edges[i - 1].cost); // Стоимость транспортировки
    }

    // Количество ограничений
    int num_constraints = num_nodes;
    glp_add_rows(lp, num_constraints);

    // Матрица ограничений
    int matrix_size = num_edges * 2;
    int *ia = (int *)malloc((1 + matrix_size) * sizeof(int));
    int *ja = (int *)malloc((1 + matrix_size) * sizeof(int));
    double *ar = (double *)malloc((1 + matrix_size) * sizeof(double));
    int idx = 1;

    for (int i = 0; i < num_nodes; i++) {
        glp_set_row_bnds(lp, i + 1, GLP_FX, nodes[i].surplus_deficit[product_index], nodes[i].surplus_deficit[product_index]);

        // Хранилище для уникальных индексов
        int added_edges[MAX_EDGES] = {0}; // Для отслеживания добавленных рёбер
        int edge_count = 0;

        // Приток и отток
        for (int j = 0; j < num_edges; j++) {
            if (edges[j].from == nodes[i].id) {
                ia[idx] = i + 1;
                ja[idx] = j + 1;
                ar[idx] = 1.0;
                idx++;
                added_edges[j] = 1;
            }
            if (edges[j].to == nodes[i].id && !added_edges[j]) {
                ia[idx] = i + 1;
                ja[idx] = j + 1;
                ar[idx] = -1.0;
                idx++;
            }
        }
    }

    // Загрузка матрицы
    glp_load_matrix(lp, idx - 1, ia, ja, ar);

    // Решение задачи
    int result = glp_simplex(lp, NULL);
    if (result != 0) {
        fprintf(stderr, "Ошибка в решении задания для продукта %d\n", product_index);
        glp_delete_prob(lp);
        free(ia);
        free(ja);
        free(ar);
        return; // Завершить решение
    }

    // Получение результатов
    double z = glp_get_obj_val(lp);
    // Сохранение результатов
    char filename[256];
    sprintf(filename, "solution_product_%d.txt", product_index);
    FILE *f = fopen(filename, "w");
    fprintf(f, "Результаты для продукта %d\n", product_index);
    fprintf(f, "Минимальная стоимость: %f\n", z);
    for (int i = 1; i <= num_vars; i++) {
        double x = glp_get_col_prim(lp, i);
        if (x > 0) {
            fprintf(f, "Ребро %d (от %d к %d): %f\n", edges[i - 1].id, edges[i - 1].from, edges[i - 1].to, x);
        }
    }
    fclose(f);

    // Освобождение памяти
    glp_delete_prob(lp);
    free(ia);
    free(ja);
    free(ar);
}

// Основная функция
int main(int argc, char *argv[]) {
    int rank, size;
    double start_time, end_time;

    MPI_Init(&argc, &argv);
    start_time = MPI_Wtime();
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Тестируем разные размерности
    int test_sizes[][3] = {
        {5, 10, 20},
        {10, 20, 40},
        {20, 50, 80},
        {30, 100, 150},
        {50, 200, 300}
    };

    for (int i = 0; i < sizeof(test_sizes) / sizeof(test_sizes[0]); i++) {
        if (rank == 0) {
            init_data(test_sizes[i][0], test_sizes[i][1], test_sizes[i][2]);
        }

        // Распространение данных всем процессам
        MPI_Bcast(&num_products, 1, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Bcast(&num_nodes, 1, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Bcast(&num_edges, 1, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Bcast(nodes, num_nodes * sizeof(Node), MPI_BYTE, 0, MPI_COMM_WORLD);
        MPI_Bcast(edges, num_edges * sizeof(Edge), MPI_BYTE, 0, MPI_COMM_WORLD);

        // Разбиение продуктов между процессорами
        int products_per_proc = num_products / size;
        int remainder = num_products % size;
        int start_prod = rank * products_per_proc + (rank < remainder ? rank : remainder);
        int end_prod = start_prod + products_per_proc + (rank < remainder ? 1 : 0);

        // Обработка продуктов
        for (int p = start_prod; p < end_prod; p++) {
            solve_lp(p);
            printf("Процесс %d обработал продукт %d\n", rank, p);
        }

        // Измерение времени выполнения
        end_time = MPI_Wtime();
        double elapsed_time = end_time - start_time;

        // Сбор времени выполнения со всех процессов
        double max_time;
        MPI_Reduce(&elapsed_time, &max_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

        // Главный процесс записывает время выполнения в файл
        if (rank == 0) {
            FILE *time_file = fopen("execution_time.txt", "a");
            fprintf(time_file, "%d %d %f\n", num_nodes, size, max_time);
            fclose(time_file);
        }
    }

    MPI_Finalize();
    return 0;
}