function [c,ceq]=mycon1(u)
global Ty M Izz lf lr dt xx vx N X % global variables 
u_lim=0.2;
c=ones(2*N,1);   % inequality eqution. Limit u to between -u_lim and u_lim
for k=1:N
    c(k)=u(k)-u_lim;
    c(k+N)=-u(k)-u_lim;
end
ceq=zeros(4,1);  % equality eqution, no constraint  so 0 = 0 

end

