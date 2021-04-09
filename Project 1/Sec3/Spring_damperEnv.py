import numpy as np

class linearsystem():
    def __init__(self, para):
        self.m, self.kp, self.kv = para[0], para[1], para[2]
    def model(self, x, u, d):
        A = np.array([[0, 1], [-self.kp/self.m,-self.kv/self.m]])
        B = np.array([[0, 1/self.m]]).T
        Bw = B
        dx = np.matmul(A, x) + B*u + Bw*d
        return dx
    def step(self, x, u, d, delta_t):
        # 4th-order Runge-Kutta
        k1 = self.model(x, u, d)
        k2 = self.model(x+delta_t/2*k1, u, d)
        k3 = self.model(x+delta_t/2*k2, u, d)
        k4 = self.model(x+delta_t*k3, u, d)
        x_next = x + delta_t/6 * (k1 + 2*k2 + 2*k3 + k4)
        return x_next