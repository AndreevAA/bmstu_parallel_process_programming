Программа `ping-resolve.c`, написанная на C с использованием интерфейса MPI (Message Passing Interface), предназначена для реализации обмена сообщениями между процессами в параллельных вычислениях. Давайте подробнее рассмотрим её структуру и функциональность:

### Описание программы `ping-resolve.c`

1. **Инициализация MPI**:
   Программа начинает с инициализации MPI, что позволяет процессам взаимодействовать друг с другом. Она получает количество процессов и ранг текущего процесса, а также имя процессора.

2. **Буферы**:
   Создаются два буфера для отправки и получения данных. Они инициализируются для хранения целых чисел.

3. **Параллельная работа**:
   - Если ранг процесса четный, он выполняет отправку и получение данных с помощью функции `MPI_Sendrecv`, которая одновременно отправляет и получает данные.
   - Ранжируемые по четности и нечетности процессы обмениваются данными. Процесс с четным рангом обрабатывает данные и отправляет их процессу с рангом на единицу больше.
   - Процесс с нечетным рангом, в свою очередь, отправляет данные процессу с рангом на единицу меньше.

4. **Измерение времени**:
   В каждом цикле фиксируется время выполнения операции передачи данных, что позволяет измерить пропускную способность сети.

5. **Увеличение размера сообщения**:
   Размер сообщения, передаваемого между процессами, увеличивается вдвое на каждой итерации цикла, что помогает протестировать производительность при разных объемах передаваемых данных.

6. **Вывод результатов**:
   В конце программы выводится время выполнения передачи и пропускная способность, что может быть полезно для анализа производительности системы.

### Скрипт запуска `job1.sh`

Скрипт `job1.sh` используется для запуска программы в кластерной среде с использованием менеджера очередей SLURM. Вот что он содержит:

- Объявления параметров задания:
  - Имя задания: `myHELLO`.
  - Параметры раздела, количество узлов и время исполнения.
  - Количество задач на узел.

- Команда для запуска программы с помощью MPI:
  - Использование `mpirun` для запуска 5 процессов программы `a.out`.




Комментарии:
Потеряется буфер Bufi на SendRecv. Сделать обмен.