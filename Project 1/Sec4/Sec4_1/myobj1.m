function f=myobj1(u)
global Ty M Izz lf lr dt xx vx N X % global variables 
k1=0.1;k2=0.004;k3=0.2; %weights for optimization
f=k1*u(1)^2;  % u^2+delta_u^2
for i=1:N-1
    f=f+k1*u(i+1)^2+k3*(u(i+1)-u(i))^2;
end
shape=10; dx1=50; dx2=4; dy1=Ty; Xs1=2.3*vx;
vy0=xx(3,1);  wr0=xx(4,1);  fy_0=xx(2,1);  s_y0=xx(1,1); X_predict=X;
for i=1:N
    X_DOT=vx*cos(fy_0)-vy0*sin(fy_0);
    X_predict=X_predict+X_DOT*dt;
    z1=shape/dx1*(X_predict-Xs1)-shape/dx2;
    Y_ref=dy1/2*(1+tanh(z1));
    alpha1=-((vy0+lf*wr0)/vx-u(i))*180/pi;
    alpha2=-(vy0-lr*wr0)/vx*180/pi;
    Fy1=alpha1*1250;
    Fy2=alpha2*755;
    vy=vy0+(2*Fy2/M+2*Fy1/M-vx*wr0)*dt;
    wr=wr0+(lf*2*Fy1/Izz-lr*2*Fy2/Izz)*dt;
    fy=fy_0+wr0*dt;
    sy=s_y0+vy0*cos(fy_0)*dt+vx*sin(fy_0)*dt;
    vy0=vy;  wr0=wr;  fy_0=fy;  s_y0=sy;
    f=f+k2*(sy-Y_ref)^2;  % calculate position error
end
end

