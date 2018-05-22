#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
#include <time.h>
#define bPI                 3.1415926535898
#define bGM84               3.986005e14
#define bOMEGAE84           7.2921151467e-5
#include <iostream>
#include <string>
#include <map>
#include "ublox_structures.h"
using namespace std;
using namespace ublox;
class Position{
private:
	string name;
	map <uint32_t,ParsedEphemData> ephemeris;
	ecef coords;
public:
	Position(string name);
	string getName();
	void printInfo();
	void addEphemeris(ParsedEphemData ephemeris);
	bool ephemerisExists(uint32_t id);
	ParsedEphemData getEphemeris(uint32_t id);
	ecef calcPosition(int id,double _time);
	double calcDoppler(int id, double _time);
	double calcDistance(ecef satPos);
	void setCoords(double x, double y, double z);
	double dopplers[50];
	double calcDopplers[50];
};