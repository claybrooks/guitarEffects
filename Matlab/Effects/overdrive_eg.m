% 
% Overdrive example simple call of DAFX symclip function

close all;
clear all;

filename='acoustic.wav';

% read the sample waveform
[x,Fs,bits] = wavread(filename);

y = symclip(x);

% write output
wavwrite(y,Fs,bits,'out_overdrive.wav');

figure(1);


hold on
plot(y,'r');

plot(x,'b');

title('Overdriven Signal');