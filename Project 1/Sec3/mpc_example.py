from casadi import *
import numpy as np
import matplotlib.pyplot as plt
from Spring_damperEnv import linearsystem

para = np.array([1, 1, 1])
sys3 = linearsystem(para)

# Simulation settings
t_end = 20
delta_t = 0.01
t_sample = 0.05
N_iteration = int(t_end/delta_t)
ratio = int(t_sample/delta_t)
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
horizon = 15
Q     = np.diag([2, 3, 2.5])
R     = 0.75
upper_u = 2
lower_u = -2
# Define MPC
class mpc():
    def __init__(self,horizon,t_sample,Q,R,para,upper_u, lower_u):
        self.N = horizon
        self.dt = t_sample
        self.Q, self.R = Q, R
        self.m, self.kp, self.kv = para[0], para[1], para[2]
        #set state variables
        self.x,self.dx,self.v = SX.sym('x'), SX.sym('dx'), SX.sym('v')
        self.state = vertcat(self.x, self.dx, self.v)
        self.n_state = 3
        #set control variable
        self.ctrl = SX.sym('u')
        self.n_ctrl = 1
        self.upper_u = upper_u
        self.lower_u = lower_u

    def SetDynamics(self):
        self.A_a = vertcat(
            horzcat(0, 1, 0),
            horzcat(-self.kp/self.m, -self.kv/self.m, 0),
            horzcat(-1, 0, 0)
        )
        self.B_a = vertcat(0, 1, 0)
        self.dyn = mtimes(self.A_a,self.state)+mtimes(self.B_a, self.ctrl)
        self.Disdyn = self.state + self.dt*self.dyn
        self.Ddyn_fn = Function('Ddyn', [self.state, self.ctrl], [self.Disdyn], ['s', 'ctrl'], ['Ddynf'])

    def SetCostDyn(self):
        # tracking error
        self.dJ = mtimes(transpose(self.state), mtimes(self.Q, self.state)) + self.R * self.ctrl**2
        self.dJ_fn = Function('dJ', [self.state, self.ctrl], [self.dJ], ['s', 'ctrl'], ['dJf'])

    def MPCsolver(self, state0):
        # start with an empty NLP
        w = [] # optimal trajectory lis
        w0 = [] # initial guess of optimal trajectory
        lbw = [] # lower boundary of optimal variables
        ubw = [] # upper boundary of optimal variables
        g   = [] # equality or inequality constraints
        lbg = [] # lower boundary of constraints
        ubg = [] # upper boundary of constraints

        # initial state
        Xk = SX.sym('X0', self.n_state, 1)
        w += [Xk]
        state_ini = []
        for k in range(len(state0)):
            state_ini += [state0[k, 0]]
        w0  += self.n_state*[0]
        lbw += state_ini
        ubw += state_ini
        J  = 0
        # formulate NLP
        for t in range(self.N):
            # new NLP variables for control input
            Uk = SX.sym('u_'+ str(t), self.n_ctrl, 1)
            w += [Uk]
            lbw += self.n_ctrl*[self.lower_u]
            ubw += self.n_ctrl*[self.upper_u]
            w0  += self.n_ctrl*[0]

            # integrate the cost function till the end of horizon
            J += self.dJ_fn(s=Xk, ctrl=Uk)['dJf']
            Xnext = self.Ddyn_fn(s=Xk, ctrl=Uk)['Ddynf']
            # next state based on the discrete model
            Xk  = SX.sym('X_'+ str(t+1), self.n_state, 1)
            w  += [Xk]
            lbw += self.n_state*[-1e10] # no constraints on state
            ubw += self.n_state*[1e10]
            w0  += self.n_state*[0]
            # add equality constraint
            g   += [Xnext - Xk]
            lbg += self.n_state*[0]
            ubg += self.n_state*[0]

        # create an NLP solver
        print_level = 0
        opts = {'ipopt.print_level': print_level, 'ipopt.sb': 'yes', 'print_time': print_level}
        prob = {'f': J, 'x': vertcat(*w), 'g': vertcat(*g)}
        solver = nlpsol('solver', 'ipopt', prob, opts)

        # solve the NLP
        sol = solver(x0=w0, lbx=lbw, ubx=ubw, lbg=lbg, ubg=ubg)
        w_opt = sol['x'].full().flatten()

        # take the optimal control
        sol_traj = np.concatenate((w_opt, self.n_ctrl*[0]))
        sol_traj = np.reshape(sol_traj,(-1, self.n_state+self.n_ctrl))
        control_traj=np.delete(sol_traj[:, self.n_state:], -1, 0)

        # implement the first component of control to the system
        u_star = control_traj[0, :]
        return u_star

# main loop
MPC = mpc(horizon, t_sample, Q, R, para, upper_u, lower_u)
MPC.SetDynamics()
MPC.SetCostDyn()
for i in range(N_iteration):
    v += (r - state0[0, 0]) * delta_t
    X_a = np.vstack((state[0, 0], state[1, 0], v))
    if (i%ratio) ==0:
        u_star = MPC.MPCsolver(X_a)
    state_next = sys3.step(state0, u_star, d, delta_t)
    # update state and time
    state0 = state_next
    time += delta_t
    Time[i + 1] = time
    Ctrl[i + 1] = u_star
    state[:, i + 1:i + 2] = state0
    #print('step:', i + 1, 'x=', state0[0, 0], 'u=', u_star)

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
