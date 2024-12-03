#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

// Определяем размер буфера
#define SIZE 1024

int main(int argc, char **argv)
{
    // Переменные для хранения ранга текущего процесса, общего количества процессов и других параметров
    int myrank, nprocs, len;
    char name[MPI_MAX_PROCESSOR_NAME]; // Имя процессора
    int *buf, *bufI; // Указатели на буферы для отправки и получения данных
    MPI_Status st; // Статус для операций MPI

    // Выделение памяти для буферов
    buf = (int *)malloc(sizeof(int) * (SIZE * 1024 + 100)); // Основной буфер
    bufI = (int *)malloc(sizeof(int) * (SIZE * 1024 + 100)); // Буфер для приема данных

    // Инициализация MPI
    MPI_Init(&argc, &argv);

    // Получение общего числа процессов и ранга текущего процесса
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    // Получение имени процессора
    MPI_Get_processor_name(name, &len);

    // Вывод информации о текущем процессе
    printf("Hello from processor %s[%d] %d of %d\n", name, len, myrank, nprocs);

    // Если ранг текущего процесса четный
    if (myrank % 2 == 0)
    {
        // Проверка, что процесс не является последним
        if (myrank < nprocs - 1)
        {
            int i, cl, sz = SIZE; // Индекс, цикл и начальный размер
            double time; // Переменная для измерения времени

            // Инициализация буфера данными (от 10 до SIZE*1024+9)
            for (i = 0; i < SIZE * 1024; i++)
                buf[i] = i + 10;

            // Цикл для многократных экспериментов
            for (cl = 0; cl < 11; cl++)
            {
                // Запуск таймера
                time = MPI_Wtime();
                for (i = 0; i < 100; i++)
                {
                    // Выполнение операции Sendrecv - отправка и прием данных в одной функции
                    MPI_Sendrecv(buf, sz, MPI_INT, myrank + 1, 10, 
                                 bufI, sz + 100, MPI_INT, myrank + 1, 20, 
                                 MPI_COMM_WORLD, &st);
                    // MPI_Send(buf, sz, MPI_INT, myrank + 1, 10, MPI_COMM_WORLD);
                    // MPI_Recv(buf, sz + 100, MPI_INT, myrank + 1, 20, MPI_COMM_WORLD, &st);
                }
                // Время выполнения операции
                time = MPI_Wtime() - time;
                
                // Вывод времени и пропускной способности
                printf("[%d] Time = %lf  Data=%9.0f KByte\n",
                       myrank,
                       time,
                       sz * sizeof(int) * 200.0 / 1024);
                printf("[%d]  Bandwith[%d] = %lf MByte/sek\n",
                       myrank,
                       cl,
                       sz * sizeof(int) * 200 / (time * 1024 * 1024));
                sz *= 2; // Увеличение размера сообщения в два раза
            }
        }
        else
            // Сообщение о том, что процесс неактивен
            printf("[%d] Idle\n", myrank);
    }
    else
    {
        // Если ранг нечетный, процесс выполняет обратные действия
        int i, cl, sz = SIZE;

        // Инициализация буфера данными (от 100 до SIZE*1024+99)
        for(i = 0; i < SIZE * 1024; i++)
            buf[i] = i + 100;

        // Цикл для многократных экспериментов
        for (cl = 0; cl < 11; cl++)
        {
            for (i = 0; i < 100; i++)
            {
                // Отправка и прием данных: отправка от текущего процесса к предыдущему
                MPI_Send(buf, sz, MPI_INT, myrank - 1, 20, MPI_COMM_WORLD);
                MPI_Recv(buf, sz + 100, MPI_INT, myrank - 1, 10, MPI_COMM_WORLD, &st);
            }
            sz *= 2; // Увеличение размера сообщения в два раза
        }
    }
    
    // Завершение работы MPI
    MPI_Finalize();
    // Вывод разделительной линии перед завершением
    printf("--------------\n");

    return 0;
}