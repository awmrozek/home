function y=h2rnd(alfa1,m1,m2)
  if rand(1)<alfa1
    y=exprnd(1/m1);
  else
    y=exprnd(1/m2);
  end
