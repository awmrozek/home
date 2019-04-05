function delta = delta (k, c, d, ak, bk, t)
    parnet = (2*pi*k*(t-c))/(d-c);
    delta = ak * cos(parnet) + bk * sin (parnet);
end