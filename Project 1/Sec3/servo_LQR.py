from control import *
import numpy as np
import matplotlib.pyplot as plt
from Spring_damperEnv import linearsystem

def run(Q, R):
    para = np.array([1, 1, 1])
    sys2 = linearsystem(para)

    # Matrices for the augmented system
    A_a = np.array([[0, 1, 0], [-1, -1, 0], [-1, 0, 0]])
    B_a = np.array([[0, 1, 0]]).T

    # Simulation settings
    t_end = 20
    delta_t = 0.01
    N_iteration = int(t_end/delta_t)
    state = np.zeros((2, N_iteration+1))
    Time  = np.zeros((N_iteration+1))
    Ctrl  = np.zeros((N_iteration+1))
    d     = 2 # constant disturbance
    r     = 1 # reference
    state0 = np.zeros((2, 1)) # initial states
    state[:, 0:1] = state0
    time = 0
    Time[0] = time
    Ctrl[0] = 0
    v     = 0 # initial extended state
    K, S, E = lqr(A_a, B_a, Q, R)
    # main loop
    for i in range(N_iteration):
        # controller
        v += (r-state0[0, 0])*delta_t
        X_a = np.vstack((state[0, 0], state[1, 0], v))
        u   = np.matmul(-K, X_a)
        state_next = sys2.step(state0, u, d, delta_t)
        # update state and time
        state0 = state_next
        time += delta_t
        Time[i+1] = time
        Ctrl[i+1] = u
        state[:, i+1:i+2] = state0
        # print('step:', i+1, 'x=', state0[0, 0], 'u=', u)

    print("K is: ", K)
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

# Weighting matrices for LQR

run(np.diag([1, 1, 2]),1)
run(np.diag([1, 1, 2]),2)
run(np.diag([1, 1, 2]),30)
