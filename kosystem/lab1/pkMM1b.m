function f = pkMM1b(lambda,mu);

%Beräknar och plottar pk för ett M/M/1 system med begränsad
%kö. Inparametrar är lambda och
%mu. Programmet frågar efter kölängden, L. Det går att klicka
%i diagrammet och på så sätt få sannolikheten för att systemet
%befinner sig i ett visst tillstånd. Klicka utanför diagrammet
%för att komma vidare.

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