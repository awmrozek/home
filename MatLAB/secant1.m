sprintf ('============================================================')
f = @(x) (2*x + 2)^(1/3) - x;
x0 = 1;
x1 = 2;
x2 = 0;

linConvgCtr = 0;
quadConvgCtr = 0;

plot(xv, f);
for i = 1:7
    x2 = x1 - f(x1)*(x1-x0)/(f(x1)-f(x0));
    x0 = x1;
    
    sprintf ('--------------------------------------------------')
    linConvg = abs(x2/x1)
    sprintf('Linear Convergernce Error %f', linConvg)
    if linConvg < 1
        linConvgCtr = linConvgCtr + 1;
    end
    
    quadConvg = abs(x2/((x1)^2))
    sprintf ('Quadratic convergence error: %f', quadConvg)
    if quadConvg < 1
        quadConvgCtr = quadConvgCtr + 1;
    end
    x1 = x2;
end

x2