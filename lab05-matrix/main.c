#include <stdio.h>          
#include <mpi.h>            

int main(int argc, char **argv)
{
    int myrank, nprocs, nlen;            // Объявление переменных: myrank (номер процесса), nprocs (количество процессов), nlen (длина имени процессора)
    char name[MPI_MAX_PROCESSOR_NAME];   // Массив для хранения имени процессора

    MPI_Init(&argc, &argv);              // Инициализация MPI

    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);  // Получение общего количества процессов
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);  // Получение ранга текущего процесса
    MPI_Get_processor_name(name, &nlen);      // Получение имени процессора

    printf("Hello from host %s[%d] %d of %d\n", name, nlen, myrank, nprocs);

    // Если ранг процесса четный
    if (myrank % 2 == 0)
    {
        // Если следующий процесс существует (проверка на границу)
        if (myrank + 1 < nprocs)
        {
            int a[10][10];            
            int *b = a[0];            

            // Заполнение массива значениями от 0 до 99
            for (int i = 0; i < 100; i++)
                *(b++) = i;

            MPI_Send(a, 100, MPI_INT, myrank + 1, 20, MPI_COMM_WORLD);
        }
    }
    else // Если ранг процесса нечетный
    {
        int a[10][10];                // Объявление массива для получения данных

        MPI_Status st;                
        MPI_Datatype col, col1;       
        MPI_Aint adr[2];              // Массив для хранения смещений
        MPI_Datatype tp[2];           
        int len[2];                   

        // Создание векторного типа для извлечения столбцов
        MPI_Type_vector(10, 1, 10, MPI_INT, &col);
        MPI_Type_commit(&col);        // Подтверждение определения типа

        // Определение смещений
        adr[0] = 0;                    // Смещение для первого элемента
        adr[1] = sizeof(int);         // Смещение для второго типа (не используется верхняя граница)

        tp[0] = col;                  // Первый тип - это столбец
        tp[1] = MPI_INT;              // Второй тип - целое число

        len[0] = 10;                  // Количество элементов в первом типе
        len[1] = 1;                   // Количество элементов (1) для второго типа

        // Создание производного типа данных с помощью MPI_Type_create_struct
        MPI_Type_create_struct(2, len, adr, tp, &col1);
        MPI_Type_commit(&col1);      // Подтверждение нового типа данных

        MPI_Type_free(&col);          // Освобождение временного типа


        printf("Receive %s[%d] %d of %d\n", name, nlen, myrank, nprocs);
        // Условие для определения, как именно получать данные
        if (myrank % 4 == 1)
            MPI_Recv(a, 100, MPI_INT, myrank - 1, 20, MPI_COMM_WORLD, &st);  // Получение данных в первом формате
        else
            MPI_Recv(a, 10, col1, myrank - 1, 20, MPI_COMM_WORLD, &st);       // Получение данных во втором формате

        MPI_Type_free(&col1);         // Освобождение производного типа данных

        
        for (int i = 0; i < 10; i++)
        {
            for (int j = 0; j < 10; j++)
                printf("%6d", a[i][j]); 
            printf("\n");               
        }
    }

    MPI_Finalize();                   
    return 0;                         
}