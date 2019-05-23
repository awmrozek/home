function dummy =xxxInspect(x,y,s,t,fx,fy)
%
% Inspektion av diagram; �ke Arvidsson 1992-07-28
% 
% x  �r en vektor som inneh�ller x-v�rdena
% y  �r en vektor som inneh�ller y-v�rdena
% s  �r en vektor som inneh�ller f�rsta och sista v�rde f�r x- och y axlarna
% t  �r en textstr�ng som inneh�ller y-variablens namn
% fx �r en textstr�ng som inneh�ller formatet som x-index skrivs ut med
% fy �r en textstr�ng som inneh�ller formatet som y-index skrivs ut med
%
[xinput,yinput]=ginput(1);
while ((xinput>=s(1))&(xinput<=s(2))&(yinput>=s(3))&(yinput<=s(4)))
   [mindist,minind]=min(abs(x-xinput));
   fprintf(['\n',t,'(',fx,') = ',fy],x(minind),y(minind))
   fprintf('\n')
   [xinput,yinput]=ginput(1);
end
return
