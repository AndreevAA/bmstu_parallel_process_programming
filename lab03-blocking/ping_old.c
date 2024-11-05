#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define SIZE 1024 // Определение размера основного буфера для передачи данных

int main(int argc, char **argv)
{
    int myrank, nprocs, len; 
    char name[MPI_MAX_PROCESSOR_NAME]; 
    int *buf; // Указатель на массив данных для передачи
    MPI_Status st; // Статус сообщения после отправки/получения

    // Выделение памяти для буфера
    buf = (int *)malloc(sizeof(int) * (SIZE * 1024 + 100));

    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    MPI_Get_processor_name(name, &len); 

    printf("Hello from processor %s[%d] %d of %d\n", name, len, myrank, nprocs);

    // Если ранг процесса четный
    if (myrank % 2 == 0)
    {
        // Убедимся, что процесс не является последним
        if (myrank < nprocs - 1)
        {
            int i, cl, sz = SIZE;
            double time;

            // Инициализация буфера
            for (i = 0; i < SIZE * 1024; i++)
                buf[i] = i + 10;

            // Измеряем время передачи данных при различных размерах буфера
            for (cl = 0; cl < 11; cl++)
            {
                time = MPI_Wtime(); // Запоминаем текущее время
                for (i = 0; i < 100; i++)
                {
                    // Отправляем и получаем данные
                    MPI_Send(buf, sz, MPI_INT, myrank + 1, 10, MPI_COMM_WORLD);
                    MPI_Recv(buf, sz + 100, MPI_INT, myrank + 1, 20, MPI_COMM_WORLD, &st);
                }
                time = MPI_Wtime() - time; 
                
                printf("[%d] Time = %lf  Data=%9.0f KByte\n",
                       myrank,
                       time,
                       sz * sizeof(int) * 200.0 / 1024);
                printf("[%d]  Bandwith[%d] = %lf MByte/sek\n",
                       myrank,
                       cl,
                       sz * sizeof(int) * 200 / (time * 1024 * 1024));
                sz *= 2; // Увеличиваем размер буфера
            }
        }
        else // Если процесс не отправляет данные (последний)
            printf("[%d] Idle\n", myrank);
    }
    else // Если ранг нечетный
    {
        int i, cl, sz = SIZE;
        // Обработка получения и отправки данных в зависимости от размера буфера
        for (cl = 0; cl < 11; cl++)
        {
            for (i = 0; i < 100; i++)
            {
                MPI_Recv(buf, sz + 100, MPI_INT, myrank - 1, 10, MPI_COMM_WORLD, &st);
                MPI_Send(buf, sz, MPI_INT, myrank - 1, 20, MPI_COMM_WORLD);
            }
            sz *= 2; // Увеличиваем размер буфера
        }
    }
    
    MPI_Finalize();
    printf("--------------\n");
    
    return 0;
}