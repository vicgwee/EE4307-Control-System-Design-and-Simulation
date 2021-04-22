Y = [16;48;82;98;126;160;195;235;300;350];
phi = ones([10 3]);
T = [0 .4 .8 1 1.2 1.6 1.8 2.2 2.6 3]
for i=1:10
    t = T(i);
    phi(i,2) = t;
    phi(i,3) = t^2/2;
end
theta = inv(phi.'*phi)*phi.'*Y