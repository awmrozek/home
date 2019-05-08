%Beräknar anropsspärr och tidsspärr för ett M/M/m system med 
%begränsat antal kunder (M st). Det plottar spärren för olika 
%värden på M. Programmet frågar efter medel-
%betjäningstid, anropsintensitet för en kund samt det maximala
%värdet på M. 


%serv=input('Mean service time, x (min): ');
%b=input('Arrival intensity for a customer, beta (1/min): ');
MaxM=0; MaxM=input('Maximum value of M: ');
serv=1;
b=0.2;

while MaxM>0 

  mu=1/serv;
  M=[1:MaxM];
  P=ones([size(M,2),4]);

  P(:,2)=M'.*b./mu; 
  P(:,3)=(M-1)'.*b./(2*mu).*P(:,2);
  P(:,4)=(M-2)'.*b./(3*mu).*P(:,3);

  p0=zeros(size(M'));
  p0=1./sum(P');

  E=zeros(size(M'));
  B=zeros(size(M'));

  E=P(:,4).*p0';
  B=(M-3)'.*P(:,4)./(M'.*P(:,1)+(M-1)'.*P(:,2)+(M-2)'.*P(:,3)+(M-3)'.*P(:,4));

  hold off;
  plot(M,E,'-');
  hold on;
  plot(M,B,'x');
  hold off;
  grid on;
  xlabel('Number of customers, M');
  ylabel('Loss probability and probability of full system');

  MaxM=0; MaxM=input('Maximum value of M (press enter to quit program): ');
end;
