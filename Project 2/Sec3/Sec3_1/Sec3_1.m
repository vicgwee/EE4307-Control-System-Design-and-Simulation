%initial
N_1 = 2; N_2 = 2; 

%save data
N1 = zeros(5000,1); 
N2 = zeros(5000,1);

%iteration 
T = 0.01;

for i = 1:1000
    N1(i) = N_1 + T * (-N_1 + N_1*N_2);
    N2(i) = N_2 + T * (N_2 - N_1*N_2);
    N_1 = N1(i);
    N_2 = N2(i);
end

N_1 = N_1/2; 
N_2 = N_2/2;
N1(1000) = N_1; 
N2(1000) = N_2;

for i = 1001:5  000
    N1(i) = N_1 + T * (-N_1 + N_1*N_2);
    N2(i) = N_2 + T * (N_2 - N_1*N_2);
    N_1 = N1(i);
    N_2 = N2(i);
end

hold on; 
plot(N1); 
plot(N2);
hold off;
    