import matplotlib.pyplot as plt
import numpy as np

threads = np.array([ 1, 2, 4, 8, 12, 16, 20, 24, 28, 32])

mean = np.zeros(threads.shape)
std = np.zeros(threads.shape)

for i in range(threads.shape[0]):
    file_name = 'n-body-data/''n-body_basic_'+str(threads[i])+'.txt'
    f = np.loadtxt(file_name)
    # print(f)
    mean[i] = f.mean()

fig, [ax1, ax2] = plt.subplots(1, 2, figsize=(16, 8))

ax1.plot(threads, mean, label="$N=3000$", marker='o', linewidth=2.5)
ax2.plot(threads, mean[0] / mean, label="$N=3000$", marker='o', linewidth=2.5)

ax1.set_xlabel('num_threads')
ax1.set_ylabel('Time [s]')
ax2.set_xlabel('num_threads')
ax2.set_ylabel('Speed-up')
ax1.legend()
ax2.legend()
plt.show()

# =====================================

for i in range(threads.shape[0]):
    file_name = 'n-body-data/''n-body_reduced_'+str(threads[i])+'.txt'
    f = np.loadtxt(file_name)
    # print(f)
    mean[i] = f.mean()

fig, [ax1, ax2] = plt.subplots(1, 2, figsize=(16, 8))

ax1.plot(threads, mean, label="$N=3000$", marker='o', linewidth=2.5)
ax2.plot(threads, mean[0] / mean, label="$N=3000$", marker='o', linewidth=2.5)

ax1.set_xlabel('num_threads')
ax1.set_ylabel('Time [s]')
ax2.set_xlabel('num_threads')
ax2.set_ylabel('Speed-up')
ax1.legend()
ax2.legend()
plt.show()

# ============================================

for i in range(threads.shape[0]):
    file_name = 'n-body-data/''n-body_reduced_cyclic_'+str(threads[i])+'.txt'
    f = np.loadtxt(file_name)
    # print(f)
    mean[i] = f.mean()

fig, [ax1, ax2] = plt.subplots(1, 2, figsize=(16, 8))

ax1.plot(threads, mean, label="$N=3000$", marker='o', linewidth=2.5)
ax2.plot(threads, mean[0] / mean, label="$N=3000$", marker='o', linewidth=2.5)

ax1.set_xlabel('num_threads')
ax1.set_ylabel('Time [s]')
ax2.set_xlabel('num_threads')
ax2.set_ylabel('Speed-up')
ax1.legend()
ax2.legend()
plt.show()
