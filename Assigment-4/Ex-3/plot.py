import matplotlib.pyplot as plt
import numpy as np

size = [10000, 20000, 50000, 100000, 200000, 500000, 1000000, 2000000, 5000000, 10000000]
labels = ['CPU', 'GPU', 'GPU-no-MemCp']

means = np.zeros((len(size), 2))
means_no_memcp = np.loadtxt('data/nvprof.txt')

fig = plt.figure(figsize=(16, 10))
for i in range(len(size)):
    f = np.loadtxt('data/saxpy_'+str(size[i])+'.txt')       
    means[i, 0] = f[:, 0].mean()
    means[i, 1] = f[:, 1].mean()

plt.plot(size, means[:,0], label=labels[0], linewidth=2.5)
plt.plot(size, means[:,1], label=labels[1], linewidth=2.5)
plt.plot(size, means_no_memcp[:, 1]/1000.0, label=labels[2], linewidth=2.5)

plt.title("Saxpy on CPU and GPU")    
plt.xlabel('Array Size')
plt.ylabel('Times [ms]')
plt.legend()
plt.savefig('Figures/saxpy.png', format='png', dpi=200)
plt.show()
