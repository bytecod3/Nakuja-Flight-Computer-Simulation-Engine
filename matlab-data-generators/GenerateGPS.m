%{
This code generates GPS Cordinates(LAT, LONG)
Formated to 2 d.p

%}

% first half
n1 = 50;
base_lat = 37.05;
base_long = -1.5;
lat1 = base_lat * ones(n1, 1);
longt1 = base_long * ones(n1, 1);

% second half - latitude
a=1;
b=1.5;
r = a + (b-a).*rand(n1,1);
r = base_lat * r;

% second half - longitude
c=1;
d=-1.5;
u = c + (d-c).*rand(100,1);
u = base_long * u;

% join data 
lat_final = [lat1; r];
long_final = [longt1; u];
plot(lat_final);
plot(long_final);

