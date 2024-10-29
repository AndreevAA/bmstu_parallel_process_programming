#include <stdio.h>        
#include <stdlib.h>       
#include <mpi.h>          

#define SIZE 1024         // Определение размера буфера для передачи данных

// Функция для суммирования элементов массива с использованием пользовательской операции MPI
void sum_element10(void *bufin, void *bufout, int *len, MPI_Datatype *datatype) {
    int *in = bufin;    
    int *out = bufout;  

    int x, y;           // Переменные для хранения значений из буферов
    // Проход по всем элементам указанных в len
    for (size_t i = 0; i < *len; i++)
    {
        x = *in;       
        y = *out;      

        // Проверка: если x не кратно 10, присвоить 0
        if (x % 10)
            x = 0;
        // Проверка: если y не кратно 10, присвоить 0
        if (y % 10)
            y = 0;

        *out = x + y;  // Сложение x и y, результат записывается в выходной буфер
        in++;          // Переход к следующему элементу входного буфера
        out++;         // Переход к следующему элементу выходного буфера
    }
}

int main(int argc, char **argv) {
    int myrank, nprocs, len; // Переменные для хранения ранга процесса, общего количества процессов и длины имени процессора
    char name[MPI_MAX_PROCESSOR_NAME]; // Массив для хранения имени процессора
    int *buf, *outbuf, *reduce_buf; // Указатели на буферы для отправки, получения и редукции данных
    MPI_Status st; // Переменная для хранения статуса операций MPI

    // Инициализация MPI
    MPI_Init(&argc, &argv);
    MPI_Op op; // Переменная для пользовательской операции MPI
    MPI_Op_create(sum_element10, 1, &op); // Создание пользовательской операции
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs); // Получение общего числа процессов
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank); // Получение ранга текущего процесса
    MPI_Get_processor_name(name, &len); // Получение имени текущего процессора

    // Вывод информации о текущем процессе
    printf("Hello from processor %s[%d] %d of %d\n", name, len, myrank, nprocs);

    // Проверка: если процессов меньше 2, выдать сообщение об ошибке и завершить программу
    if (nprocs < 2) {
        printf("Too small set of processors!!\n");
        MPI_Finalize(); // Завершение работы MPI
        return 1; // Завершение программы
    }

    int i;

    // Процесс с рангом 1 выделяет память и инициализирует буфер
    if (myrank == 1) {
        buf = (int*)malloc(sizeof(int) * (SIZE * nprocs)); // Выделение памяти под массив
        for (i = 0; i < SIZE * nprocs; i++) {
            buf[i] = i * 2; // Инициализация буфера, заполняя его четными числами
        }
        // Вывод первых нескольких значений буфера для проверки
        for (i = 0; i < nprocs; i++)
            printf("My[%2d] Buf[%2d] = %8d %8d %8d ...\n", myrank, i, buf[i * SIZE], 
                buf[i * SIZE + 1], buf[i * SIZE + 2]);
    } else
        buf = NULL; // Буфер для остальных процессов не нужен

    // Выделение памяти для выходного буфера
    outbuf = (int*)malloc(sizeof(int) * (SIZE));

    // Распределение данных из buf всем процессам
    MPI_Scatter(buf, SIZE, MPI_INT, outbuf, SIZE, MPI_INT, 1, MPI_COMM_WORLD);

    // Вывод полученных данных каждым процессом
    printf("My[%2d] outBuf = %8d %8d %8d ...\n", myrank, outbuf[0], outbuf[1], outbuf[2]);

    // Процесс с рангом 0 выделяет память для буфера редукции
    if (myrank == 0) {
        reduce_buf = (int*)malloc(sizeof(int) * (SIZE)); 
    } else
        reduce_buf = NULL; // Остальные процессы не используют этот буфер

    // Выполнение операции редукции, собирая данные в reduce_buf
    MPI_Reduce(outbuf, reduce_buf, SIZE, MPI_INT, op, 0, MPI_COMM_WORLD);

    // Процесс с рангом 0 выводит результаты редукции
    if (myrank == 0)
        printf("My[%2d] redBuf = %8d %8d %8d ...\n", myrank, reduce_buf[0], reduce_buf[1], reduce_buf[2]);

    // Завершение работы MPI
    MPI_Finalize();
    return 0; // Завершение программы
}
