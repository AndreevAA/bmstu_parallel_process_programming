#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

// Размер буфера для передачи данных
#define SIZE 1024

int main(int argc, char **argv)
{
    int myrank, nprocs, len;
    char name[MPI_MAX_PROCESSOR_NAME];
    int *buf; // Указатель на буфер для хранения данных
    MPI_Status st; // Статус для операций MPI

    // Выделение памяти для буфера
    buf = (int *)malloc(sizeof(int) * (SIZE * 1024 + 100));

    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    MPI_Get_processor_name(name, &len);

    // Сообщение о запуске процесса
    printf("Hello from processor %s[%d] %d of %d\n", name, len, myrank, nprocs);

    // Процессы с четными рангами
    if (myrank % 2 == 0)
    {
        // Проверка, что процесс не является последним
        if (myrank < nprocs - 1)
        {
            int i, cl, sz = SIZE; 
            double time;

            // Инициализация буфера данными
            for (i = 0; i < SIZE * 1024; i++)
                buf[i] = i + 10;

            for (cl = 0; cl < 11; cl++)
            {
                time = MPI_Wtime(); // Запись времени начала
                for (i = 0; i < 100; i++)
                {
                    // Отправка данных процессу с рангом myrank + 1
                    MPI_Send(buf, sz, MPI_INT, myrank + 1, 10, MPI_COMM_WORLD);
                    // Получение данных от процесса с рангом myrank + 1
                    MPI_Recv(buf, sz + 100, MPI_INT, myrank + 1, 20, MPI_COMM_WORLD, &st);
                }
                time = MPI_Wtime() - time; // Вычисление времени выполнения
                
                // Печать времени и производительности
                printf("[%d] Time = %lf  Data=%9.0f KByte\n",
                       myrank,
                       time,
                       sz * sizeof(int) * 200.0 / 1024);
                printf("[%d]  Bandwith[%d] = %lf MByte/sek\n",
                       myrank,
                       cl,
                       sz * sizeof(int) * 200 / (time * 1024 * 1024));
                sz *= 2; // Увеличение размера буфера для следующего цикла
            }
        }
        else
            // Если процесс не выполняет никаких действий, вывести сообщение
            printf("[%d] Idle\n", myrank);
    }
    else // Процессы с нечетными рангами
    {
        int i, cl, sz = SIZE;

        // Инициализация буфера данными
        for(i = 0; i < SIZE * 1024; i++)
            buf[i] = i + 100;

        // Цикл для передачи данных
        for (cl = 0; cl < 11; cl++)
        {
            for (i = 0; i < 100; i++)
            {
                // Отправка данных процессу с рангом myrank - 1
                MPI_Send(buf, sz, MPI_INT, myrank - 1, 20, MPI_COMM_WORLD);
                // Получение данных от процесса с рангом myrank - 1
                MPI_Recv(buf, sz + 100, MPI_INT, myrank - 1, 10, MPI_COMM_WORLD, &st);
            }
            sz *= 2; // Увеличение размера буфера для следующего цикла
        }
    }
    
    // Завершение работы MPI
    MPI_Finalize();
    printf("--------------\n");

    return 0;
}