function dummy =xxxInspect(x,y,s,t,fx,fy)
%
% Inspektion av diagram; Åke Arvidsson 1992-07-28
% 
% x  är en vektor som innehåller x-värdena
% y  är en vektor som innehåller y-värdena
% s  är en vektor som innehåller första och sista värde för x- och y axlarna
% t  är en textsträng som innehåller y-variablens namn
% fx är en textsträng som innehåller formatet som x-index skrivs ut med
% fy är en textsträng som innehåller formatet som y-index skrivs ut med
%
[xinput,yinput]=ginput(1);
while ((xinput>=s(1))&(xinput<=s(2))&(yinput>=s(3))&(yinput<=s(4)))
   [mindist,minind]=min(abs(x-xinput));
   fprintf(['\n',t,'(',fx,') = ',fy],x(minind),y(minind))
   fprintf('\n')
   [xinput,yinput]=ginput(1);
end
return
