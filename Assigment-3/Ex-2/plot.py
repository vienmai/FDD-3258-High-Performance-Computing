import matplotlib.pyplot as plt
import numpy as np

threads = np.array([ 4, 8, 16, 32, 64, 128])

mean = np.zeros(threads.shape)

pre_name = ['data/''send_recv_mpi_pi_',
            'data/''tree_mpi_pi_', 
            'data/''nonblock_mpi_pi_',
            'data/''gather_mpi_pi_',
            'data/''reduced_mpi_pi_']

labels = ['send-recv', 'tree-comn', 'non-block', 'gather', 'reduced']

fig, [ax1, ax2] = plt.subplots(1, 2, figsize=(16, 8))
for j in range(5):
    for i in range(threads.shape[0]):
        file_name = pre_name[j]+str(threads[i])+'.txt'
        f = np.loadtxt(file_name)
        mean[i] = f.mean()

    # fig, [ax1, ax2] = plt.subplots(1, 2, figsize=(16, 8))

    ax1.plot(threads, mean, label=labels[j], marker='o', linewidth=2.5)
    ax2.plot(threads, mean[0] / mean, label=labels[j], marker='o', linewidth=2.5)

ax1.set_xlabel('Num_Prrocess')
ax1.set_ylabel('Time [s]')
ax2.set_xlabel('Num_Process')
ax2.set_ylabel('Speed-up vs 4 processes')
ax1.set_title("$N=10^9$ Samples")
ax2.set_title("$N=10^9$ Samples")
ax1.legend()
ax2.legend()
  
plt.show()