import matplotlib.pyplot as plt
import numpy as np

names = ['data/intra_ping_pong.txt', 'data/inter_ping_pong.txt']
labels = ['on-node', 'off-node']

p = np.zeros((2,2))
bandwidth = np.zeros(2)
latency = np.zeros(2)

fig = plt.figure(figsize=(16, 10))
for j in range(2):   
    file_name = names[j]
    ff = np.loadtxt(file_name)    
    f = ff[0:20,:] # Use the first 20 points for fitting latency
    p[j, :]= np.polyfit(f[:, 0], f[:, 1], 1)   

    plt.plot(f[:, 0], f[:,1], label=labels[j], marker='o', linewidth=2.5)

bandwidth = 1. / p[:, 0] / 1e9  # GB/s
latency = p[:, 1] / 1e-6  # us

print(f"On-Node: \nLatency: {latency[0]} us\nBandwidth: {bandwidth[0]} GB/s")
print()
print(f"Off-Node: \nLatency: {latency[1]} us\nBandwidth: {bandwidth[1]} GB/s")

plt.xlabel('Message Size [B]')
plt.ylabel('Time [s]')
plt.legend()

plt.show()