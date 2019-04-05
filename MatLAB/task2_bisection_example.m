mid = 100;
pl = 0;
pr = 1;
while abs(cos(mid) - sin(mid)) > 10^-6
   mid = (pl + pr)/2;
   if(cos(mid) - sin(mid) > 0)
       pl = mid;
   else
       pr = mid;
   end
end

mid

