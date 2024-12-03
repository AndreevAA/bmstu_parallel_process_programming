#!/bin/bash
#SBATCH --job-name="myHELLO_1024"  # Название задания
#SBATCH --partition=debug             # Указывает на использование "debug" раздела
#SBATCH --nodes=10                    # Количество узлов для выполнения задания
#SBATCH --time=0-00:02:00             # Максимальное время выполнения задания
#SBATCH --ntasks-per-node=1           # Количество задач на узел (1 задача)
#SBATCH --mem=1992                     # Объем памяти для каждого узла (1992 MB)

# Запуск MPI-программы с 8 процессами
mpirun -np 8 a.out

#sbatch название job