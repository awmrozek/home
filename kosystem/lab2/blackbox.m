clear all;
rand('seed',0)%set random seed
lambda=7.5; %arrival rate
mu1=10; 	%service rate
mu2=12; 	%service rate
mu3=20; 	%service rate
mu4=9; 	%service rate
mu5=12; 	%service rate
alfa1=0.4;       %probability 
alfa2=0.4;       %probability
alfa3=0.4;       %probability
alfa4=0.4;       %probability
k=1;
endtime=500; %simulation length
t=0; %current time
tstep=1; %time between consecutive sample events
currcustomers1=0; %current nbr of customers in system 1
currcustomers2=0; %current nbr of customers in system 2
currcustomers3=0; %current nbr of customers in system 3
currcustomers4=0; %current nbr of customers in system 4
currcustomers5=0; %current nbr of customers in system 5
servtime1=0; %time in server in system 1
servtime2=0; %time in server in system 2
servtime3=0; %time in server in system 3
servtime4=0; %time in server in system 4
servtime5=0; %time in server in system 5
event=[1,7]; %time for next arrival system 1 (pos 1), next service completion
             %system 1 (pos 2), next service completion system 2
             %(pos 3), next service completion system 3 (pos 4),
             %next service completion system 4 (pos 5), next
             %service completion system 5 (pos 6) and next sample
             %event (pos 7)
event(1)=exprnd(1/lambda); %next arrival system 1
event(2)=inf; %no customer in system 1
event(3)=inf; %no customer in system 2
event(4)=inf; %no customer in system 3
event(5)=inf; %no customer in system 4
event(6)=inf; %no customer in system 5
event(7)=exprnd(tstep); %next sample event
nbrmeasurements1=0; %number of (nbr of customer) measurements made
nbrdeparted1=0; %nbr of departed customers from system 1
nbrmeasurements2=0; %number of (nbr of customer) measurements made
nbrdeparted2=0; %nbr of departed customers from system 2
nbrmeasurements3=0; %number of (nbr of customer) measurements made
nbrdeparted3=0; %nbr of departed customers from system 3
nbrmeasurements4=0; %number of (nbr of customer) measurements made
nbrdeparted4=0; %nbr of departed customers from system 4
nbrmeasurements5=0; %number of (nbr of customer) measurements made
nbrdeparted5=0; %nbr of departed customers from system 5
nbrarrivedtosystem=0;
nbrarrived1=0; %nbr of cust that have arrived throughout the 
                  %simulations to system 1
nbrarrived2=0; %nbr of cust that have arrived throughout the 
                  %simulations	to system 2
nbrarrived3=0; %nbr of cust that have arrived throughout the 
                  %simulations to system 3
nbrarrived4=0; %nbr of cust that have arrived throughout the 
                  %simulations	to system 4
nbrarrived5=0; %nbr of cust that have arrived throughout the 
                  %simulations	to system 5	  
while t<endtime
   [t,nextevent]=min(event);
   if nextevent==1
       event(1)=exprnd(1/lambda)+t;
       if currcustomers1==0
	  servtime1=exprnd(1/mu1);
          event(2)=servtime1+t;
       end
       currcustomers1=currcustomers1+1;
       nbrarrivedtosystem=nbrarrivedtosystem+1;
       nbrarrived1=nbrarrived1+1; %one more customer has arrived to 
                                  %system 1 through the simulations
       arrivedtime1(nbrarrived1)=t; %the new customer arrived at time t
   elseif nextevent==2
         currcustomers1=currcustomers1-1;
	 timeinsystem1=t-arrivedtime1(nbrarrived1-currcustomers1);
         nbrdeparted1=nbrdeparted1+1; %one more customer has departed from 
                         %system 1 through the simulations
         T1(nbrdeparted1)=timeinsystem1;
	 T1s(nbrdeparted1)=servtime1;
         if currcustomers1>0
	      servtime1=exprnd(1/mu1);
              event(2)=servtime1+t;
         else
            event(2)=inf;
         end
	 if rand(1)<alfa1
	   if currcustomers2==0
	   servtime2=exprnd(1/mu2);
           event(3)=servtime2+t;
	   end
           currcustomers2=currcustomers2+1;
           nbrarrived2=nbrarrived2+1; %one more customer has arrived to 
                                   %system 2 through the simulations
           arrivedtime2(nbrarrived2)=t; %the new customer arrived
                                        %at time t
	 else
	   if currcustomers3==0
	   servtime3=exprnd(1/mu3);
           event(4)=servtime3+t;
	   end
           currcustomers3=currcustomers3+1;
           nbrarrived3=nbrarrived3+1; %one more customer has arrived to 
                                   %system 3 through the simulations
           arrivedtime3(nbrarrived3)=t; %the new customer arrived
                                        %at time t
	 end
    elseif nextevent==3		    
	 currcustomers2=currcustomers2-1;
	 timeinsystem2=t-arrivedtime2(nbrarrived2-currcustomers2);
         nbrdeparted2=nbrdeparted2+1; %one more customer has departed from 
                         %system 1 through the simulations
         T2(nbrdeparted2)=timeinsystem2;
	 T2s(nbrdeparted2)=servtime2;
         if currcustomers2>0
	      servtime2=exprnd(1/mu2);
              event(3)=servtime2+t;
         else
            event(3)=inf;
         end
	 if rand(1)<alfa2
	   if currcustomers1==0
	   servtime1=exprnd(1/mu1);
           event(2)=servtime1+t;
	   end
           currcustomers1=currcustomers1+1;
           nbrarrived1=nbrarrived1+1; %one more customer has arrived to 
                                   %system 1 through the simulations
           arrivedtime1(nbrarrived1)=t; %the new customer arrived
                                        %at time t
	 else
	   if currcustomers3==0
	   servtime3=exprnd(1/mu3);
           event(4)=servtime3+t;
	   end
           currcustomers3=currcustomers3+1;
           nbrarrived3=nbrarrived3+1; %one more customer has arrived to 
                                   %system 3 through the simulations
           arrivedtime3(nbrarrived3)=t; %the new customer arrived
                                        %at time t
	 end
    elseif nextevent==4			    
	 currcustomers3=currcustomers3-1;
	 timeinsystem3=t-arrivedtime3(nbrarrived3-currcustomers3);
         nbrdeparted3=nbrdeparted3+1; %one more customer has departed from 
                         %system 3 through the simulations
         T3(nbrdeparted3)=timeinsystem3;
	 T3s(nbrdeparted3)=servtime3;
         if currcustomers3>0
	      servtime3=exprnd(1/mu3);
              event(4)=servtime3+t;
         else
            event(4)=inf;
         end
	 if rand(1)<alfa3
	   if currcustomers4==0
	   servtime4=exprnd(1/mu4);
           event(5)=servtime4+t;
	   end
           currcustomers4=currcustomers4+1;
           nbrarrived4=nbrarrived4+1; %one more customer has arrived to 
                                   %system 4 through the simulations
           arrivedtime4(nbrarrived4)=t; %the new customer arrived
                                        %at time t
	 else
	   if currcustomers5==0
	   servtime5=exprnd(1/mu5);
           event(6)=servtime5+t;
	   end
           currcustomers5=currcustomers5+1;
           nbrarrived5=nbrarrived5+1; %one more customer has arrived to 
                                   %system 5 through the simulations
           arrivedtime5(nbrarrived5)=t; %the new customer arrived
                                        %at time t
	 end
    elseif nextevent==5			    
	currcustomers4=currcustomers4-1;
	 timeinsystem4=t-arrivedtime4(nbrarrived4-currcustomers4);
         nbrdeparted4=nbrdeparted4+1; %one more customer has departed from 
                         %system 4 through the simulations
         T4(nbrdeparted4)=timeinsystem4;
	 T4s(nbrdeparted4)=servtime4;
         if currcustomers4>0
	      servtime4=exprnd(1/mu4);
              event(5)=servtime4+t;
         else
            event(5)=inf;
         end
	 if rand(1)<alfa4
	   if currcustomers3==0
	   servtime3=exprnd(1/mu3);
           event(4)=servtime3+t;
	   end
           currcustomers3=currcustomers3+1;
           nbrarrived3=nbrarrived3+1; %one more customer has arrived to 
                                   %system 3 through the simulations
           arrivedtime3(nbrarrived3)=t; %the new customer arrived
                                        %at time t
	 else
	   if currcustomers5==0
	   servtime5=exprnd(1/mu5);
           event(6)=servtime5+t;
	   end
           currcustomers5=currcustomers5+1;
           nbrarrived5=nbrarrived5+1; %one more customer has arrived to 
                                   %system 5 through the simulations
           arrivedtime5(nbrarrived5)=t; %the new customer arrived
                                        %at time t
	 end
    elseif nextevent==6			    
	currcustomers5=currcustomers5-1;
     timeinsystem5=t-arrivedtime5(nbrarrived5-currcustomers5);
     nbrdeparted5=nbrdeparted5+1; %one more customer has departed from 
                         %system 5 through the simulations
     T5(nbrdeparted5)=timeinsystem5;
     T5s(nbrdeparted5)=servtime5;
         if currcustomers5>0
	   servtime5=exprnd(1/mu5);
           event(6)=servtime5+t;
         else
           event(6)=inf;
	 end
	 
   else
       nbrmeasurements1=nbrmeasurements1+1; %one more (nbr of customer) measurement made
       N1(nbrmeasurements1)=currcustomers1;
       if currcustomers1>0
         N1q(nbrmeasurements1)=currcustomers1-1;
       else
	 N1q(nbrmeasurements1)=currcustomers1;
       end
       nbrmeasurements2=nbrmeasurements2+1; %one more (nbr of customer) measurement made
       N2(nbrmeasurements2)=currcustomers2;
       if currcustomers2>0
         N2q(nbrmeasurements2)=currcustomers2-1;
       else
	 N2q(nbrmeasurements2)=currcustomers2;
       end
       nbrmeasurements3=nbrmeasurements3+1; %one more (nbr of customer) measurement made
       N3(nbrmeasurements3)=currcustomers3;
       if currcustomers3>0
         N3q(nbrmeasurements3)=currcustomers3-1;
       else
	 N3q(nbrmeasurements3)=currcustomers3;
       end
       nbrmeasurements4=nbrmeasurements4+1; %one more (nbr of customer) measurement made
       N4(nbrmeasurements4)=currcustomers4;
       if currcustomers4>0
         N4q(nbrmeasurements4)=currcustomers4-1;
       else
	 N4q(nbrmeasurements4)=currcustomers4;
       end
       nbrmeasurements5=nbrmeasurements5+1; %one more (nbr of customer) measurement made
       N5(nbrmeasurements5)=currcustomers5;
       if currcustomers5>0
         N5q(nbrmeasurements5)=currcustomers5-1;
       else
	 N5q(nbrmeasurements5)=currcustomers5;
       end 			    
       Ttot(k)=(N1(nbrmeasurements1)+N2(nbrmeasurements2)+N3(nbrmeasurements3)+N4(nbrmeasurements4)+N5(nbrmeasurements5))/lambda;
       k=k+1;

 event(7)=event(7)+exprnd(tstep);
   end
 end
		
