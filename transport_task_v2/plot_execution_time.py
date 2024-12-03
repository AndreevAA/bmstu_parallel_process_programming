# plot_execution_time.py

import matplotlib.pyplot as plt

# Чтение данных из файла
data = []
with open('execution_time.txt', 'r') as f:
    for line in f:
        # Преобразуем данные в list перед распаковкой
        values = line.strip().split()
        num_nodes = int(values[0])
        num_procs = int(values[1])
        time = float(values[2])
        data.append((num_nodes, num_procs, time))

# Построение графика
nodes = [d[0] for d in data]
times = [d[2] for d in data]
procs = [d[1] for d in data]

plt.figure(figsize=(10, 6))
scatter = plt.scatter(nodes, times, c=procs, cmap='viridis', s=100, edgecolor='black')
plt.colorbar(scatter, label='Количество процессоров')
plt.xlabel('Количество узлов')
plt.ylabel('Время выполнения (сек)')
plt.title('Зависимость времени выполнения от размера задачи и количества процессоров')
plt.grid(True)
plt.savefig('execution_time_plot.png')  # Сохранение графика в файл
plt.show()