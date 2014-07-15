
clear all;
close all;

infile = 'acoustic.wav';

% read in wav sample
[ x, Fs, N ] = wavread(infile);


%set Parameters for vibrato
% Change these to experiment with vibrato

Modfreq = 10; %10 Khz
Width = 0.0008; % 0.8 Milliseconds

% Do vibrato

yvib = vibrato(x, Fs, Modfreq, Width);

% write output wav files
wavwrite(yvib, Fs,  'out_vibrato.wav');

% plot the original and equalised waveforms

figure(1)
hold on
plot(x(1:500),'r');
plot(yvib(1:500),'b');
title('Vibrato First 500 Samples');


