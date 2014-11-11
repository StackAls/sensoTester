#ifndef THERMISTOR_H
#define THERMISTOR_H
#include <math.h>

#define Rb 9830	// Balanse Resistor Ohm

// Temperature
#define TK 0 //Kelvin
#define TC 1 //Celcius
#define TF 2 //Farenheit

// http://en.wikipedia.org/wiki/Thermistor
// This is for R0=10kOhm
#define At 0.001129148	
#define Bt 0.000234125
#define	Ct 0.0000000876741

float Thermistor(int ,int =TC,float =Rb);

#endif