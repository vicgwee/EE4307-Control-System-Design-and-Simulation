import numpy as np
import matplotlib.pyplot as plt
from quadrotor import quadrotor

para = np.array([1.5, 0.1, 1e-7, 0.15, 0.1, 0.44, 1.8])
sys = quadrotor(para)
t_end = 10
delta_t = 0.01
N_iteration = int(t_end/delta_t)
state = np.zeros((6, N_iteration+1))
Time  = np.zeros((N_iteration+1))
Ctrl  = np.zeros((2, N_iteration+1)).T
d     = np.zeros((6,1))
d[5][0] = 0.1 # constant angular disturbance

#Controller Gains
K     = np.zeros((2,6))

R     = np.zeros((6,1)) # reference
R[2][0] = 0.3

state0 = R
state[:, 0:1] = state0
time = 0
Time[0] = time
Ctrl[0] = 0
v     = R # initial extended state
# main loop
for i in range(N_iteration):
    # controller
    v += (R-state0)*delta_t
    u  = - np.matmul(K,state0)
    state_next = sys.step(state0, u, d, delta_t)

    # update state and time
    state0 = state_next
    time += delta_t
    Time[i+1] = time
    Ctrl[i+1] = u.T
    state[:, i+1:i+2] = state0
    print('step:', i+1, 'x=', state0[0, 0], 'u=', u)

plt.figure(1)
plt.plot(Time, state[0,:], 'r', linewidth=1.5)
plt.plot(Time, state[2,:], 'b', linewidth=1.5)
plt.xlabel('time[s]')
plt.ylabel('x,z [m]')
plt.grid()
plt.show()

plt.figure(2)
plt.plot(Time, state[4,:], 'r', linewidth=1.5)
plt.xlabel('time[s]')
plt.ylabel('theta [rad]')
plt.grid()
plt.show()

plt.figure(3)
plt.plot(Time, Ctrl, linewidth=1.5)
plt.xlabel('time[s]')
plt.ylabel('control[N]')
plt.grid()
plt.show()
