import numpy as np
import matplotlib.pyplot as plt
from Spring_damperEnv import linearsystem

para = np.array([1, 1, 1])
sys = linearsystem(para)
t_end = 10
delta_t = 0.01
N_iteration = int(t_end/delta_t)
state = np.zeros((2, N_iteration+1))
Time  = np.zeros((N_iteration+1))
Ctrl  = np.zeros((N_iteration+1))
d     = 2 # constant disturbance
k11, k12, K2 = 7, 3, -8 # control gain
r     = 1 # reference
state0 = np.zeros((2, 1)) # initial states
state[:, 0:1] = state0
time = 0
Time[0] = time
Ctrl[0] = 0
v     = 0 # initial extended state
# main loop
for i in range(N_iteration):
    # controller
    v += (r-state0[0, 0])*delta_t
    u  = -k11*state0[0, 0] -k12*state0[1, 0] -K2*v
    state_next = sys.step(state0, u, d, delta_t)
    # update state and time
    state0 = state_next
    time += delta_t
    Time[i+1] = time
    Ctrl[i+1] = u
    state[:, i+1:i+2] = state0
    print('step:', i+1, 'x=', state0[0, 0], 'u=', u)

plt.figure(1)
plt.plot(Time, state[0, :], linewidth=1.5)
plt.xlabel('time[s]')
plt.ylabel('position[m]')
plt.grid()
plt.show()
plt.figure(2)
plt.plot(Time, Ctrl, linewidth=1.5)
plt.xlabel('time[s]')
plt.ylabel('control[N]')
plt.grid()
plt.show()
