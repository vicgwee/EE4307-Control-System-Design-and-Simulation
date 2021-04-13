% Compute the control parameters 
clear

%reference model params
ratio_m = 1; 
w_m     = 2; 
para_m  = [ratio_m, w_m];
A_m     = [0, 1;
          -w_m^2, -2*ratio_m*w_m];
Q       = diag([1,100]);
P       = lyap(A_m, Q);

%adaptive law params
Gamma = diag([1,1]);
gamma = 10; 

%calculate perfect gains
a = [6.2];
b = [0.25 1];
[w_p, ratio_p] = damp(tf(a,b));
A_p = [0, 1;
      -w_p^2, -2*ratio_p*w_p];
g = w_p^2;

theta_x_p = [0, 1] * (A_m - A_p)/g;
theta_r_p = w_m^2/g;