import numpy as np

class quadrotor():
    def __init__(self, para):
        [self.m, self.j, self.k, self.l, self.r, self.ca, self.cb] = para
        g = 9.81
        self.A = np.zeros((6,6))
        self.A[0][1] = 1
        self.A[1][4] = g
        self.A[2][3] =  1
        self.A[4][5] = 1

        self.B = np.zeros((6,2))
        self.B[3][0] = 1/self.m
        self.B[5][1] = 1/self.j

        self.Bw = np.zeros((6,1))
        self.Bw[1][0] = 1/self.m
        self.Bw[3][0] = 1/self.m
        self.Bw[5][0] = 1/self.j

        self.Const = np.zeros((6,1))
        self.Const[3][0] = -g
        
        #print(self.A)
        #print(self.B)
        #print(self.Bw)
        #print(self.Const)

    def model(self, x, u, d):
        dx = np.matmul(self.A, x) + np.matmul(self.B,u) + self.Bw*d + self.Const
        return dx

    def step(self, x, u, d, delta_t):
        # 4th-order Runge-Kutta
        k1 = self.model(x, u, d)
        k2 = self.model(x+delta_t/2*k1, u, d)
        k3 = self.model(x+delta_t/2*k2, u, d)
        k4 = self.model(x+delta_t*k3, u, d)
        x_next = x + delta_t/6 * (k1 + 2*k2 + 2*k3 + k4)
        return x_next