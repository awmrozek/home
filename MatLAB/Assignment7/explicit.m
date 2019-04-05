clf(1);
figure(1);
h=0.1; % step's size
N=10; % number of steps

for lambda = -5:6
%for lambda = 1:1
    clear y;
    clear x;
    y(1)=1;
    for n=1:N
        y(n+1)= y(n)+h*(lambda*y(n));
        x(n+1)=n*h;
    end
    hold on;
    plot(x,y);
    legend(lambda);
end
