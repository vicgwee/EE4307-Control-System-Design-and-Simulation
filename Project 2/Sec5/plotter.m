subplot(2,2,1);
plot(out.error);
title("Error");

eta = squeeze(out.eta.data);
subplot(2,2,2);
plot(eta(:,1),eta(:,2));
title("X-Y Graph");

subplot(2,2,3);
plot(out.eta.Time,eta(:,3));
title("Psi");

subplot(2,2,4);
plot(out.F_h);
axis([0 60 -15 15]);
title("Control Force Vector"); 
hold off;