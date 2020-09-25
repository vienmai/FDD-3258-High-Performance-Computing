import matplotlib.pyplot as plt
import numpy as np

threads = np.array([1, 2, 4, 8, 12, 16, 20, 24, 28, 32])

mean = np.zeros(threads.shape)
std = np.zeros(threads.shape)

for i in range(threads.shape[0]):
    file_name = 'stream-data/''copy_'+str(threads[i])+'.txt'
    f = np.loadtxt(file_name)
    mean[i] = f.mean()
    std[i] = f.std()

fig = plt.figure(figsize=(12, 10))
plt.errorbar(threads, mean, std, linewidth=2, marker='o')
plt.ylabel("Bandwidth (MB/s)")
plt.xlabel("num_threads")
plt.show()

# Second plot

mean = np.zeros(3)
std = np.zeros(3)

file_name = 'stream-data/''copy_static_32.txt'
f = np.loadtxt(file_name)
mean[0] = f.mean()
std[0] = f.std()


file_name = 'stream-data/''copy_dynamic_32.txt'
f = np.loadtxt(file_name)
mean[1] = f.mean()
std[1] = f.std()


file_name = 'stream-data/''copy_guided_32.txt'
f = np.loadtxt(file_name)
mean[2] = f.mean()
std[2] = f.std()


fig = plt.figure(figsize=(12, 10))
plt.errorbar(0, mean[0], std[0], linewidth=2.5, marker='o', label="static")
plt.errorbar(1, mean[1], std[1], linewidth=2.5, marker='o', label="dynamic")
plt.errorbar(2, mean[2], std[2], linewidth=2.5, marker='o', label="guided")

plt.ylabel("Bandwidth (MB/s)")
plt.xticks([])
plt.legend(loc='upper center')
# plt.savefig('Ex-2/Figures/stream_schedule.png', format='png', dpi=200)
plt.show()
