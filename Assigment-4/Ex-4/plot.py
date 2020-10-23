import matplotlib.pyplot as plt
import numpy as np

size = [10000, 20000, 50000, 100000, 200000, 500000]
gpu = [16, 32, 64, 128, 256]

labels = ['CPU', 'GPU']

cpu_means = np.zeros(len(size))
gpu_means = np.zeros((len(size), len(gpu)))

fig = plt.figure(figsize=(16, 10))
for i in range(len(size)):
    for j in range(len(gpu)):
        f = np.loadtxt('data/particle_'+str(size[i])+'_'+str(gpu[j])+'.txt')       
        cpu_means[i] += f[:, 0].mean()
        gpu_means[i, j] = f[:, 1].mean()

plt.plot(size, cpu_means/len(gpu), label=labels[0], linewidth=2.5)
for j in range(len(gpu)):
    plt.plot(size, gpu_means[:, j], label=labels[1]+'-'+str(gpu[j]), linewidth=2.5)

plt.title("Particle Simulation on CPU and GPU")    
plt.xlabel('Num Particles')
plt.ylabel('Times [ms]')
plt.legend()
# plt.savefig('Figures/particle.png', format='png', dpi=200)
plt.show()
