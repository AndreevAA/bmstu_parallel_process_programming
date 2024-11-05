#include <stdio.h>        
#include <stdlib.h>       
#include <mpi.h>          

#define SIZE 1024         // Определение размера буфера

int main(int argc, char **argv)
{
    // Объявление переменных для хранения ранга процесса, общего количества процессов и имени процессора
    int myrank, nprocs, len;
    char name[MPI_MAX_PROCESSOR_NAME]; // Массив для хранения имени процессора
    int *buf, *bufI;                   // Указатели на буферы для отправки и получения данных
    MPI_Status st;                     // Переменная для хранения статуса операции MPI
    MPI_Request re;                    // Переменная для хранения запроса MPI

    // Выделение памяти для буферов (основного и приемного)
    buf = (int *)malloc(sizeof(int) * (SIZE * 1024 + 100)); 
    bufI = (int *)malloc(sizeof(int) * (SIZE * 1024 + 100));

    // Инициализация MPI
    MPI_Init(&argc, &argv);

    // Получение информации о количестве процессов и текущем ранге
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    // Получение имени текущего процессора
    MPI_Get_processor_name(name, &len);

    // Вывод информации о текущем процессе
    printf("Hello from processor %s[%d] %d of %d\n", name, len, myrank, nprocs);

    // Проверка, является ли ранг текущего процесса четным
    if (myrank % 2 == 0)
    {
        // Если это не последний процесс
        if (myrank < nprocs - 1)
        {
            int i, cl, sz = SIZE; // Инициализация индекса, цикла и начального размера
            double time;          // Переменная для временных измерений

            // Инициализация буфера значениями от 10 до (SIZE * 1024 + 9)
            for (i = 0; i < SIZE * 1024; i++)
                buf[i] = i + 10;

            // Цикл для изменения размера передаваемых данных
            for (cl = 0; cl < 11; cl++)
            {
                time = MPI_Wtime(); // Начало отсчета времени
                for (i = 0; i < 100; i++)
                {
                    // Неблокирующая отправка данных на следующий процесс
                    MPI_Isend(buf, sz, MPI_INT, myrank + 1, 10, MPI_COMM_WORLD, &re);
                    // Блокирующее получение данных от следующего процесса
                    MPI_Recv(bufI, sz + 100, MPI_INT, myrank + 1, 20, MPI_COMM_WORLD, &st);

                    // Ожидание завершения отправки
                    MPI_Wait(&re, &st);
                }

                // Завершение временного измерения и вывод результатов
                time = MPI_Wtime() - time;
                printf("[%d] Time = %lf  Data=%9.0f KByte\n",
                       myrank,
                       time,
                       sz * sizeof(int) * 200.0 / 1024);
                printf("[%d]  Bandwith[%d] = %lf MByte/sek\n",
                       myrank,
                       cl,
                       sz * sizeof(int) * 200 / (time * 1024 * 1024));
                sz *= 2; // Увеличение размера передаваемых данных вдвое
            }
        }
        else
            printf("[%d] Idle\n", myrank); // Если процесс "праздный", вывод соответствующего сообщения
    }
    else
    {
        int i, cl, sz = SIZE; // Инициализация индекса, цикла и начального размера
        // Цикл для изменения размера передаваемых данных
        for (cl = 0; cl < 11; cl++)
        {
            // Цикл для отправки и получения данных
            for (i = 0; i < 100; i++)
            {
                // Неблокирующая отправка данных на предыдущий процесс
                MPI_Issend(buf, sz, MPI_INT, myrank - 1, 20, MPI_COMM_WORLD, &re);
                // Блокирующее получение данных от предыдущего процесса
                MPI_Recv(bufI, sz + 100, MPI_INT, myrank - 1, 10, MPI_COMM_WORLD, &st);

                // Ожидание завершения отправки
                MPI_Wait(&re, &st);
            }
            sz *= 2; // Увеличение размера передаваемых данных вдвое
        }
    }
   
    // Завершение работы MPI
    MPI_Finalize();
    printf("--------------\n");

    // Завершение программы
    return 0;
}