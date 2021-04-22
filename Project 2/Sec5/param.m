%Initial Conditions
eta_h0 = [0.2; -0.5; pi/18];
v_h0 = [0; 0; 0]; 

%Tuning
tuning = 0;
kp = 42;
ki = 105; 
kd = 6.56; 

%--------------------------------------------
%Given 
m = 10;
Iz = 2;
xg = 0.01;
yg = 0.01;
Lh = 0.145;
Wh = 0.1;
gamma = [- pi/4 pi/4 3 * pi/4 -3 * pi/4];
Xu = -7.2;
Yv = -7.7;
Nr = -3;
Xu_dot = -2.9;
Yv_dot = -3;
Nr_dot = -3.3;
Rref = 1;
rref = pi/60;
rho0 = -pi/2;
Vmin = 0;
Vmax = 0.1; 
Vc = (Vmin + Vmax)/2;
M_A = -diag([Xu_dot Yv_dot Nr_dot]);
D_h = -diag([Xu Yv Nr]);
Vc0 = (Vmin+Vmax)/2;
%--------------------------------------------

% Find initial conditions
psi0 = eta_h0(3,1);
v_r0 = v_h0 - Vc0 * [cos(psi0); -sin(psi0); 0];

if(tuning)
    eta_ref0 = [0;0;0];
else
    eta_ref0 = [ Rref*cos(rho0); 
                Rref*(1+sin(rho0));
                pi/2 + rho0];
end

error0 = eta_h0 - eta_ref0;

%--------------------------------------------
% Initialise matrices
%Helpers
cp = cos(psi0);
sp = sin(psi0);
Jh = [ cp -sp 0;
       sp cp  0;
       0  0   1];  

%Change coordinates
vh = v_h0;
u = vh(1,1);
v = vh(2,1);
r = vh(3,1);

vr = v_r0;

%Find M_eta
M_eta = diag([m m Iz]);
M_eta(1,3) = -m *(yg*cp + xg*sp);
M_eta(2,3) = m  *(xg*cp - yg*sp);
M_eta(3,1) = M_eta(1,3);
M_eta(3,2) = M_eta(2,3);

%Find C_eta
C_eta = zeros(3,3);
C_eta(1,2) = r;
C_eta(1,3) = -cp*(v + r*xg) - sp*(u - r*yg);
C_eta(2,1) = -r;
C_eta(2,3) =  cp*(u - r*yg) - sp*(v + r*xg);
C_eta(3,1) =   v*cp + u*sp;
C_eta(3,2) = -(u*cp - v*sp);
C_eta = m * C_eta;

%Find B_eta
cg = cos(gamma);
sg = sin(gamma);
B_eta = zeros(3,4);
B_eta(1,1) = cos(gamma(1)+ psi0);
B_eta(2,1) = sin(gamma(1)+ psi0);

B_eta(1,2) = cg(2)*cp - sg(1)*sp;
B_eta(1,3) = cg(3)*cp - sg(1)*sp;
B_eta(1,4) = cg(4)*cp - sg(1)*sp;

B_eta(2,2) = cp*sg(1) + cg(2)*sp; 
B_eta(2,3) = cp*sg(1) + cg(3)*sp; 
B_eta(2,4) = cp*sg(1) + cg(4)*sp; 

B_eta(3,1) = -Wh*cg(1) + Lh*sg(1); 
B_eta(3,2) =  Wh*cg(2) + Lh*sg(2); 
B_eta(3,3) =  Wh*cg(3) - Lh*sg(3); 
B_eta(3,4) = -Wh*cg(4) - Lh*sg(4); 
%--------------------------------------------

%Initial Fh0
feedfwd0 = 0;
feedback0 = pinv(B_eta)*M_eta*kp*error0; 
F_h0 = feedfwd0 - feedback0;