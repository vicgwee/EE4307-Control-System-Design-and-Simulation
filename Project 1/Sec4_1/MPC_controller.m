function y = MPC_controller(x)
global Ty M Izz lf lr dt xx vx N X % global variables 

%  input variable
vy=x(1);                  % lateral velocity， m/s
vx=x(2);                 % longitudinal velocity, m/s
fy=x(3);                  % Yaw angle，rad
wr=x(4);                  % Yaw rate rad/s
Y=x(5);                  % Y position， m
X=x(6);                   % X position，m
t=x(7);                   % Time, s
% main code

% intalization
dt=0.05;                  % Discrete timestep, s
% vehicle parameters
M=1270;                   % vehicle mass, kg
Izz=1536.7;               % moment of inertia of the vehicle
lf=1.015;                 % distance from the center of gravity to the front axles, m
lr=1.895;                 % distance from the center of gravity to the front axles, m
% before 2s lane-keeping with fix predictive horizon(1s), after 2s change lane
% in 3s with receding predictive horizon, after 5s lane-keeping with fix predictive horizon (1s)
yf = 4;
yf2 = 2;
start = 1;
middle = 4; 
final = 7;
T_lane_keep=1;
if t<start
    tf=T_lane_keep;
    Ty=0;
elseif t<middle
    tf=middle-t;
    Ty=yf;
elseif t<final
    tf=final-t
    Ty=yf2;
else
    tf = T_lane_keep;
    Ty=yf2;
end

N=max(round(tf/dt),round(T_lane_keep/dt)); %predictive horizon points
xx=zeros(4,1);
xx(1,1)=Y; xx(2,1)=fy; xx(3,1)=vy; xx(4,1)= wr;

u=ones(N,1)*0; % control variables--delta_f, N is the length of predictive horizon
options=optimset('Algorithm','interior-point','TolFun',1e-6,'LargeScale','on','MaxFunEvals',1e10, 'MaxIter',1e10);
[uu,fval,exitflag]=fmincon(@myobj1,u,[],[],[],[],[],[],@mycon1,options);

% output variable
y(1)=uu(1); % the output variable only contains 1 dimention; the example only use the first variable in the control horizon, uu is the whole optimized control sequence in MPC

end
