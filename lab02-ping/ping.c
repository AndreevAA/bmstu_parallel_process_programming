#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define SIZE 1024 // Определение размера буфера для передачи данных

int main(int argc, char **argv)
{
    int myrank, nprocs, len;  
    char name[MPI_MAX_PROCESSOR_NAME]; 
    int *buf; // Указатель на буфер для передачи данных
    MPI_Status st; // Структура для хранения статуса передачи сообщений

    // Выделение памяти для буфера передачи данных
    buf = (int *)malloc(sizeof(int) * (SIZE * 1024 + 100));

    // Инициализация MPI
    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    MPI_Get_processor_name(name, &len);

    printf("Hello from processor %s[%d] %d of %d\n", name, len, myrank, nprocs);

    // Логика передачи данных между процессами
    if (myrank % 2 == 0) // Четные процессы
    {
        if (myrank < nprocs - 1) // Если это не последний процесс
        {
            int i, cl, sz = SIZE; 
            double time; 

            // Инициализация буфера данными
            for (i = 0; i < SIZE * 1024; i++)
                buf[i] = i + 10;

            // Цикл для тестирования передачи на различных размерах данных
            for (cl = 0; cl < 11; cl++)
            {
                time = MPI_Wtime(); 
                for (i = 0; i < 100; i++) // Передача 100 раз
                {
                    // Отправка и получение сообщений между процессами
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
                sz *= 2; // Увеличение размера передачи данных
            }
        }
        else // Если процесс последний
            printf("[%d] Idle\n", myrank);
    }
    else // Нечетные процессы
    {
        int i, cl, sz = SIZE; // Переменные для цикла и размера данных
        // Цикл для получения сообщений от предыдущего процесса
        for (cl = 0; cl < 11; cl++)
        {
            for (i = 0; i < 100; i++)
            {
                // Получение сообщения и отправка ответа
                MPI_Recv(buf, sz + 100, MPI_INT, myrank - 1, 10, MPI_COMM_WORLD, &st);
                MPI_Send(buf, sz, MPI_INT, myrank - 1, 20, MPI_COMM_WORLD);
            }
            sz *= 2;
        }
    }

    MPI_Finalize();
    return 0; // Указание на успешное завершение программы
}