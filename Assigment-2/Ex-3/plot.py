import matplotlib.pyplot as plt
import numpy as np

threads = np.array([1, 2, 4, 8, 12, 16, 20, 24, 28, 32])

f_serial = np.loadtxt("maxloc-data/maxloc_serial.txt")
mean_serial = np.full(threads.shape, f_serial.mean())
std_serial = np.full(threads.shape, f_serial.std())

names = {}
names['_local_'] = 0
names['_pad_'] = 1
names['_critical_'] = 2

mean = np.zeros((3, threads.shape[0]))
std = np.zeros((3, threads.shape[0]))

for name, val in names.items():
    for i in range(threads.shape[0]):
        file_name = 'maxloc-data/''maxloc'+name+str(threads[i])+'.txt'
        f = np.loadtxt(file_name)
        mean[val][i] = f.mean()
        std[val][i] = f.std()

fig = plt.figure(figsize=(15, 10))

plt.errorbar(threads, mean[0], std[0], c='C1',
             label="local", marker='o', linewidth=2.5)
plt.errorbar(threads, mean_serial, std_serial, c='C3',
             label="serial", marker='o', linewidth=2.5)
plt.errorbar(threads, mean[2], std[2], c='C4',
             label="critical", marker='o', linewidth=2.5)
plt.errorbar(threads, mean[1], std[1], c='C2',
             label="pad", marker='o', linewidth=2.5)

plt.xlabel('num_threads')
plt.ylabel('Time [s]')
plt.legend(loc='center right')
plt.show()
