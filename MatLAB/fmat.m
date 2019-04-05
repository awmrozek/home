% Fourier matrix
% Basic, no omega optimization
function fmat = fmat(n)
fmat = [];
for k = 0:n-1
    line = [];
    for j = 0:n-1
        line = [line, omega(j*k, n)];
    end
    fmat = [fmat; line];
end
end