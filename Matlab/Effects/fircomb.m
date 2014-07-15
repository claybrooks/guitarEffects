x=zeros(100,1);x(1)=1; % unit impulse signal of length 100
g=0.5;
Delayline=zeros(10,1); % memory allocation for length 10
for n=1:length(x);
	y(n)=x(n)+g*Delayline(10);
	Delayline=[x(n);Delayline(1:10-1)];
end;