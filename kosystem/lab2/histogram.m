%Ritar upp ett histogram över mätvärden i en vektor. Programmet 
%frågar efter namnet på vektorn. 

Vxx=[]; Vxx=input('Vector to make a histogram of: ');

while ~isempty(Vxx)

  maxX=max(Vxx)+5;
  xarr=[0:maxX];
  hold off;
  subplot(1,1,1)
    
  [Nxx,Xxx]=hist(Vxx,xarr);
  bar(Xxx,Nxx);
  axis([0,maxX,0,max(Nxx)+50]);
  grid on;

  slask=input('Press enter to normalize the histogram','s');

  Nxx=Nxx/sum(Nxx);
  bar(Xxx,Nxx);
  axis([0,maxX,0,max(Nxx)+0.1]);
  grid on;

  Vxx=input('Vector to make a histogram of (press enter to quit program): ');
end;
  
   
    
    
    

