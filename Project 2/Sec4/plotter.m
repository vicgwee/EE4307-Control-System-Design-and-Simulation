subplot(2,2,1);
plot(out.error);
title("Error");

subplot(2,2,2);
plot(out.X_p);
title("X_p");

%Plot gains
subplot(2,2,3);
plot(out.theta_r);
title("theta_r");

subplot(2,2,4);
plot(out.theta_x);
title("theta_x"); 
