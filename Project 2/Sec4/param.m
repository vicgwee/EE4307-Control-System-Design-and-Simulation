% Compute the control parameters 
clear all

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