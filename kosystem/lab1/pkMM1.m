function f = pkMM1(lambda,mu);

%Beräknar och plottar pk för ett M/M/1 system med obegränsad
%kö. Inparametrar är lambda och mu. Det går att klicka i 
%diagrammet och på så sätt få sannolikheten att systemet
%befinner sig i ett visst tillstånd. Klicka utanför diagrammet
%för att komma vidare.

  rho=lambda/mu;
  maxk=input('Maximum k-value: ');
  k=[0:maxk];

  pk=rho.^k*(1-rho);

  plot(k,pk);
  scales=[0 maxk 0 0.5];
  axis(scales);
  grid on;
  xlabel('Number of packets in system, k');
  ylabel('P(k)');
  xxxInspect(k,pk,scales,'p','%g','%e');
