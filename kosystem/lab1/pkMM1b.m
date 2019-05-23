function f = pkMM1b(lambda,mu);

%Ber�knar och plottar pk f�r ett M/M/1 system med begr�nsad
%k�. Inparametrar �r lambda och
%mu. Programmet fr�gar efter k�l�ngden, L. Det g�r att klicka
%i diagrammet och p� s� s�tt f� sannolikheten f�r att systemet
%befinner sig i ett visst tillst�nd. Klicka utanf�r diagrammet
%f�r att komma vidare.

rho=lambda/mu;

L=0;
L=input('Buffer size: ');
while L>0
  k=[0:L+1];
  pk=rho.^k*(1-rho)/(1-rho^(L+2));
  Nxx=rho/(1-rho)-(L+2)*rho^(L+2)/(1-rho^(L+2));
  leff=lambda*(1-pk(L+2)); %index starts from 1

  subplot(1,1,1);
  plot(k,pk);
  grid on;
  scales=[0 L+5 0 0.5];
  axis(scales);
  xlabel('Number of packets in system, k');
  ylabel('P(k)');
  xxxInspect(k,pk,scales,'p','%g','%e');
  L=0;
  L=input('Buffer size (press enter to quit program): ');

end