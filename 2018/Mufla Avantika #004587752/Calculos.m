%% Calculos Mufla
%% JUDAPIES 2018
clc,clear
TempeInicial=20;
Temperatura1=100;
Tiempo=3600;
Pendiente=(Temperatura1-TempeInicial)/Tiempo;
PendienteMinutos=Pendiente*60;
PotenciaEntregada=(PendienteMinutos*100)/10;
