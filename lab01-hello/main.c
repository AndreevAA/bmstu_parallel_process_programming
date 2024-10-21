#include <stdio.h>
#include <mpi.h>

int main(int argc, char ** argv)
{
    int myrank, nprocs, len;  // Объявляем переменные для текущего ранга, общего количества процессов и длины имени процессора
    char name[MPI_MAX_PROCESSOR_NAME]; // Массив для хранения имени процессора

    // Инициализация MPI
    MPI_Init(&argc, &argv);

    // Получение общего количества процессов
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    // Получение ранга текущего процесса
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    // Получение имени процессора
    MPI_Get_processor_name(name, &len);

    // Вывод сообщения от текущего процесса
    printf("Hello from host %s[%d] %d of %d\n", name, len, myrank, nprocs);

    // Завершение работы MPI
    MPI_Finalize();

    return 0; // Возвращаем 0, чтобы указать, что программа завершилась успешно
}