function f = pkMM1(lambda,mu);

%Ber�knar och plottar pk f�r ett M/M/1 system med obegr�nsad
%k�. Inparametrar �r lambda och mu. Det g�r att klicka i 
%diagrammet och p� s� s�tt f� sannolikheten att systemet
%befinner sig i ett visst tillst�nd. Klicka utanf�r diagrammet
%f�r att komma vidare.

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
