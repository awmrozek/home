function omega = omega(k, n)
    k = mod(k, n);
    omega = exp(-2*i*pi*k/n);
end