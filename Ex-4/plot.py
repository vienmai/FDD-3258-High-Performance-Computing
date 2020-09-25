import matplotlib.pyplot as plt
import numpy as np

threads = np.array([1, 2, 4, 8, 12, 16, 20, 24, 28, 32])

mean = np.zeros(threads.shape)
std = np.zeros(threads.shape)

for i in range(threads.shape[0]):
    file_name = 'DFTW-data/''dftw_'+str(threads[i])+'.txt'
    f = np.loadtxt(file_name)
    mean[i] = f[0]
    std[i] = f[1]

fig, [ax1, ax2] = plt.subplots(1, 2, figsize=(16, 8))

# ax1.plot(threads, mean, label="$N=10000$")
ax1.errorbar(threads, mean, std, c='C0',
             label="$N=10000$", marker='o', linewidth=2.5)
ax2.errorbar(threads, mean[0] / mean, std,
             label="$N=10000$", marker='o', linewidth=2.5)

ax1.set_xlabel('num_threads')
ax1.set_ylabel('Time [s]')
ax2.set_xlabel('num_threads')
ax2.set_ylabel('Speed-up')
ax1.legend()
ax2.legend()
plt.show()