import pandas as pd
import matplotlib.pyplot as plt

# Чтение данных из файла
data = pd.read_csv('timing_results.csv', names=['Vertices', 'Processors', 'Time'])

# График времени в зависимости от числа процессоров
plt.figure(figsize=(10, 5))
for vertices, group in data.groupby('Vertices'):
    plt.plot(group['Processors'], group['Time'], marker='o', label=f'Vertices: {vertices}')

plt.xlabel('Number of Processors')
plt.ylabel('Time (seconds)')
plt.title('Time vs Processors')
plt.legend()
plt.grid(True)
plt.savefig('time_vs_processors.png')
plt.show()

# График времени в зависимости от размерности задачи
plt.figure(figsize=(10, 5))
for processors, group in data.groupby('Processors'):
    plt.plot(group['Vertices'], group['Time'], marker='o', label=f'Processors: {processors}')

plt.xlabel('Number of Vertices')
plt.ylabel('Time (seconds)')
plt.title('Time vs Vertices')
plt.legend()
plt.grid(True)
plt.savefig('time_vs_vertices.png')
plt.show()