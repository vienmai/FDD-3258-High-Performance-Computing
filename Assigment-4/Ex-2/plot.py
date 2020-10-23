import matplotlib.pyplot as plt
import numpy as np

names = ['host2device.txt', 'device2host.txt', 'device2device.txt']
labels = ['host_to_device', 'device_to_host', 'device_to_device']

fig = plt.figure(figsize=(16, 8))
for j in range(3):
    file_name = names[j]
    f = np.loadtxt('data/'+file_name)       
    
    plt.plot(f[:, 0], f[:,1], label=labels[j], linewidth=2.5)

plt.title("CUDA Bandwidth Test")    
plt.xlabel('Message Size [B]')
plt.ylabel('Bandwidth [GB/s]')
plt.legend()
plt.xscale('log')

# plt.savefig('Figures/bandwidth.png', format='png', dpi=200)

plt.show()
