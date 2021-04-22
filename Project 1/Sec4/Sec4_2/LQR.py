from control import *
import numpy as np
import matplotlib.pyplot as plt
from quadrotor import quadrotor

[m, J, k, l, r, Ca, Cb] = [1.5, 0.1, 1e-7, 0.15, 0.1, 0.44, 1.8]
g = 9.81

def neg_sqrt(a):
    return np.sqrt(a) if a > 0 else -np.sqrt(-a)

def run(Q, R):
    para = np.array([m, J, k, l, r, Ca, Cb], dtype = object)
    sys = quadrotor(para)

    # Matrices for the augmented system
    A_a = np.zeros((9,9))
    A_a[0][1] = 1
    A_a[1][4] = g
    A_a[2][3] = 1
    A_a[4][5] = 1
    A_a[6][0] = -1
    A_a[7][2] = -1
    A_a[8][4] = -1

    B_a = np.zeros((9,2))
    B_a[3][0] = 1/sys.m
    B_a[5][1] = 1/sys.j

    # Simulation settings
    t_end = 10
    delta_t = 0.001
    N_iteration = int(t_end/delta_t)
    state = np.zeros((6, N_iteration+1))
    Time  = np.zeros((N_iteration+1))
    Ctrl  = np.zeros((2, N_iteration+1)).T
    w  = np.zeros((2, N_iteration+1)).T
    
    # Control Mapping
    Ctrl_M = np.linalg.inv(np.array([(k, k), (k*l , -k*l)]))

    # Disturbance is set on every loop, initialise to 0.
    d     = np.zeros((6,1)) 

    #Initial Conditions and Reference
    Ref     = np.zeros((6,1)) 
    Ref[2][0] = 0.3
    state0 = np.copy(Ref)
    state0[2][0] = 1
    state[:, 0:1] = state0

    time = 0
    Time[0] = time
    Ctrl[0] = 0

    # y = Cx + Du
    C = np.zeros((3,6))
    C[0][0] = 1
    C[1][2] = 1
    C[2][4] = 1

    v     = np.zeros((3,1)) # initial extended state

    K, S, E = lqr(A_a, B_a, Q, R)
    print(np.around(K,1))

    # main loop
    for i in range(N_iteration):
        # Calculate disturbance 
        z = state0[2][0]
        theta = state0[4][0]

        dTi = m * g * Ca * np.exp(-Cb * z/r)
        d[1][0] = dTi * np.sin(theta)
        d[3][0] = dTi * np.cos(theta)
        d[5][0] = dTi * l

        # controller
        v += np.matmul(C,(Ref - state0)) * delta_t
        X_a = np.vstack((state0,v))
        u  = - np.matmul(K, X_a) 
        state_next = sys.step(state0, u, d, delta_t)

        # update state and time
        state0 = state_next
        time += delta_t
        Time[i+1] = time
        Ctrl[i+1] = u.T
        w2 = Ctrl_M * u
        w[i+1] = np.array((neg_sqrt(w2[0][0]),neg_sqrt(w2[1][0]))).T
        state[:, i+1:i+2] = state0
        #print('step:', i+1, 'd=', d[5][0])

    plt.figure(1)
    plt.plot(Time, state[0,:], 'r', linewidth=1.5)
    plt.plot(Time, state[2,:], 'g', linewidth=1.5)
    plt.plot(Time, state[4,:], 'b', linewidth=1.5)
    plt.xlabel('time[s]')
    plt.ylabel('x,z[m], theta [rad]')
    plt.grid()
    plt.show()

    plt.figure(2)
    plt.plot(Time, w[:,0],'y', linewidth=1.5)
    plt.plot(Time, w[:,1],'k', linewidth=1.5)
    plt.xlabel('time[s]')
    plt.ylabel('control[N]')
    plt.grid()
    plt.show()

# Weighting matrices for LQR
Q   = np.diag([1,1,1,1,1,1,1,2e3,1])
R   = np.diag([2e-5,1e-3])

run(Q, R)