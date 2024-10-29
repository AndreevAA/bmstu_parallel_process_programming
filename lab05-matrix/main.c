#include <stdio.h>
#include <mpi.h>

int main(int argc, char ** argv) {
    int myrank, nprocs, len;
    char name[MPI_MAX_PROCESSOR_NAME];

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    MPI_Get_processor_name(name, &len);

    printf("Hello from processor %s[%d] %d of %d\n", name, len, myrank, nprocs);

    // для чётных процессов
    // они отправляют сообщения
    if (myrank % 2 == 0) {
        // в случае если существует следующий процесс
        // т.е общее кол-во процессов не превышено
        if (myrank + 1 < nprocs) {
            int matrix[5][5]; // массив, который будем отправлять
            int *b = matrix[0]; // указатель на 1-ый эл-т массива

            // заполняем двумерный массив с использованием указателя
            for (int i = 0; i < 25; i++) {
                *(b++) = i + 1;
            }

            // отправляем массив matrix следующему процессу

            // matrix -- указатель на отправляемые данные
            // 25 -- кол-во отправляемых эл-тов
            // MPI_INT -- тип отправляемых данных
            // myrank + 1 -- ранг процесса, которому отправляем сообщения
            // 10 -- тег для идентификации отправляемых сообщений
            // MPI_COMM_WORLD -- коммуникатор, определяющий группу процессов, которые могут общаться друг с другом
            // (в данном случае используем "всемирный" коммуникатор, включающий все потоки, запущенные в рамках программы)
            MPI_Send(matrix, 25, MPI_INT, myrank + 1, 10, MPI_COMM_WORLD);
            printf("procs[%d] sent such matrix to procs[%d]:\n", myrank, myrank + 1);

            // выводим отправленный массив на экран
            for (int i = 0; i < 5; i++) {
                for (int j = 0; j < 5; j++) {
                    printf("%4d", matrix[i][j]);
                }
                printf("\n");
            }
            printf("\n");
        }
    // для нечётных процессов
    // они получают сообщения
    } else {
        int matrix[5][5]; // массив для получения данных

        MPI_Status st; // статус операции получения сообщения
        MPI_Datatype vectorType, structType; // переменные для определения пользовательских типов данных

        MPI_Aint address[2]; // массив для хранения адресов полей пользовательского типа данных
        MPI_Datatype newType[2]; // массив для хранения типов данных для создаваемого пользовательского типа
        int fieldsLen[2]; // массив для хранения длин соответствующих полей пользовательского типа данных

        // определение пользовательского MPI типа vectorType
        // это столбец из 5 целых чисел, взятых с шагом 5

        // 5 -- кол-во эл-тов
        // 1 -- кол-во эл-тов в каждом "срезе"
        // 5 -- шаг между эл-тами (т.е. каждый эл-т сдвигается на 5 позиций в памяти)
        // MPI_INT -- базовый тип данных (здесь это целое число)
        MPI_Type_vector(5, 1, 5, MPI_INT, &vectorType);

        // адреса для создания структуры
        address[0] = 0; // начало 1-ого поля
        address[1] = sizeof(int); // начало 2-ого поля (равно размеру целого числа)

        // 1-ый эл-т структуры -- созданный пользовательский тип
        // 2-ой -- верхняя граница структуры (выше этой границы нет данных)
        newType[0] = vectorType;
        newType[1] = MPI_UB;

        // длины полей в структуре
        fieldsLen[0] = 1;
        fieldsLen[1] = 1;

        // создание нового пользовательского типа данных structType, состоящего из 2-ух эл-тов
        // это тип для передачи векторных данных между процессами

        // 2 -- кол-во полей
        // fieldsLen, address, newType -- соответствующие массивы
        MPI_Type_create_struct(2, fieldsLen, address, newType, &structType);

        // делаем новый тип доступным для использования в операциях
        MPI_Type_commit(&structType);

        // освобождаем память, занятую типом vectorType (т.к. он нам больше не нужен)
        MPI_Type_free(&vectorType);

        // получаем массив целых чисел от предыдущего процесса с использованием созданного типа structType
        // это транспонированная матрица
        MPI_Recv(matrix, 5, structType, myrank - 1, 10, MPI_COMM_WORLD, &st);
        printf("procs[%d] received transposed matrix from procs[%d]\nHere it is:\n", myrank, myrank - 1);

        // выводим полученный массив на экран
        for (int i = 0; i < 5; i++) {
            for (int j = 0; j < 5; j++) {
                printf("%4d", matrix[i][j]);
            }
            printf("\n");
        }
        printf("\n");

        // освобождаем память, занятую типом structType
        MPI_Type_free(&structType);
    }

    MPI_Finalize();
    return 0;
}
