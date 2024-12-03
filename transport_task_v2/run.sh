mpicc -o main main.c -lglpk
mpirun -np 8 ./main
python3 plot_execution_time.py 