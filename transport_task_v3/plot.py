import subprocess
import time
import matplotlib.pyplot as plt

# Параметры для тестирования
product_counts = [10, 50, 100]
node_counts = [10, 30, 50]
edge_counts = [50, 150, 300]
processor_counts = [1, 2, 4, 8]

results = []

for num_products in product_counts:
    for num_nodes in node_counts:
        for num_edges in edge_counts:
            times = []
            for num_processors in processor_counts:
                # Генерация данных для теста
                # Здесь можно вызвать функцию, которая создаёт файлы с данными для программы
                # generate_test_data(num_products, num_nodes, num_edges)

                # Команда для запуска MPI-программы
                cmd = f"mpirun -np {num_processors} ./main"

                # Измерение времени выполнения
                start_time = time.time()
                subprocess.run(cmd, shell=True)
                end_time = time.time()

                exec_time = end_time - start_time
                times.append(exec_time)

                print(f"Продукты: {num_products}, Вершины: {num_nodes}, Ребра: {num_edges}, Процессоры: {num_processors}, Время: {exec_time:.2f} секунд")

            results.append({
                'num_products': num_products,
                'num_nodes': num_nodes,
                'num_edges': num_edges,
                'processor_counts': processor_counts.copy(),
                'times': times.copy()
            })

# Построение графиков
for res in results:
    plt.figure(figsize=(10, 6))
    plt.plot(res['processor_counts'], res['times'], marker='o')
    plt.title(f"Влияние количества процессоров\nПродукты: {res['num_products']}, Вершины: {res['num_nodes']}, Ребра: {res['num_edges']}")
    plt.xlabel("Количество процессоров")
    plt.ylabel("Время выполнения (секунд)")
    plt.xticks(res['processor_counts'])
    plt.grid(True)
    plt.savefig(f"performance_np_{res['num_products']}_nn_{res['num_nodes']}_ne_{res['num_edges']}.png")
    plt.close()