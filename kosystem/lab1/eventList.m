classdef eventList < handle
    properties
        t = inf(1,100)
        e = zeros(1,100)
    end
    methods 
        function schedule(obj, event, eventtime)
            [~, place] = max(obj.t);
            obj.t(place) = eventtime;
            obj.e(place) = event;
        end
        function [ev, time] = getEvent(obj)
            [time, place] = min(obj.t);
            ev = obj.e(place);
            obj.e(place) = 0;
            obj.t(place) = inf;
        end 
    end
end