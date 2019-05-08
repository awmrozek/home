function f = pkMMmloss(rho,m);

%Calculates the steady state probabilities an M/M/m* loss system
%without queue,%Erlangfallet. The parameters required are offered
%traffic (rho) and
%number of servers (m).
prob=ones(m+1,1);
pk=zeros(m+1,1);
for i=2:m+1
  prob(i)=prob(i-1)*rho/(i-1);
end;
prob;
pk=prob/sum(prob)
pk(m+1);
  
