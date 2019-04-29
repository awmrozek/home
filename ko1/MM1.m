clear %clears the workspace

%The events that can take place
ARRIVAL = 1;
DEPARTURE = 2;
MEASURE = 3;

%Parameters are given values
lambda = 20;
mu = 10;
endTime = 1000;
meanMeasureTime = 1;
queueMaxLength = 7;

rand('seed', 1); %the random number generator is started
time = 0;

%The start values of the variables that describe the system and are used
%for measurements
noCustomers = 0;
noMeasurements = 0;
noDeparted = 0;
noArrived = 0;
queueFullCount = 0;

%the eventList (a class with two methods) is created, two events are placed
%in it
ev = eventList;
ev.schedule(ARRIVAL, time + exprnd(1/lambda));
ev.schedule(MEASURE, time + exprnd(meanMeasureTime));

%The simulation loop begins:
while time < endTime
    [nextEvent, time] = ev.getEvent();
    switch nextEvent
        case ARRIVAL
            if (noCustomers < queueMaxLength)
                if noCustomers == 0
                    ev.schedule(DEPARTURE, time + exprnd(1/mu));
                end
                noCustomers = noCustomers + 1;
                noArrived = noArrived + 1;
                arrivedTime(noArrived) = time;
            else
                queueFullCount = queueFullCount + 1;
            end
            ev.schedule(ARRIVAL, time + exprnd(1/lambda));
        case DEPARTURE
            noCustomers = noCustomers - 1;
            timeInSystem = time - arrivedTime(noArrived - noCustomers);
            noDeparted = noDeparted + 1;
            T(noDeparted) = timeInSystem;
            if noCustomers > 0
                ev.schedule(DEPARTURE, time + exprnd(1/mu));
            end
        case MEASURE
            noMeasurements = noMeasurements + 1;
            N(noMeasurements) = noCustomers;
            ev.schedule(MEASURE, time + exprnd(meanMeasureTime));
    end
end






