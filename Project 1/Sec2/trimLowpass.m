function [Y, W] = trimLowpass(X, Start, Final, w, N)
%TRIMLOWPASS Summary of this function goes here
%   Detailed explanation goes here

% Trim
Y = X(Start:Final,:);

% Subtract min
minVal = min(Y(:,3));

for i = Start:Final
    Y(i - Start + 1,3)=X(i,3) - minVal;
end

% Lowpass
W = idfilt(Y(:,3), N, w, 'noncausal');
end

