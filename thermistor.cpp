
#include "thermistor.h"

float Thermistor(int RawADC,int Tt,float R0)
{
	double Temp;

	Temp = log(R0 * 1024/RawADC - R0);
	Temp = 1/(At+(Bt * Temp) + (Ct * Temp * Temp * Temp));

	switch(Tt)
	{
		case TK:	// Return Kelvin
			break;
		case TC:	// Return Celcius
			Temp = Temp - 273.15;
			break;
		case TF:	// Return Fahrenheit
			Temp = (Temp * 9.0) / 5.0 + 32.0;
			break;
	}

	return Temp;
}