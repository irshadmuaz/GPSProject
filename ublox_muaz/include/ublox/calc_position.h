#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <vector>
#include <math.h>
#include <time.h>
#define bPI                 3.1415926535898
#define bGM84               3.986005e14
#define bOMEGAE84           7.2921151467e-5
#define EARTH_GRAV   3.986008e14
#define EARTH_ROT    7.292115167e-5
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

public:
	Position(string name);
	string getName();
	void printInfo();
	void addEphemeris(ParsedEphemData ephemeris);
	bool ephemerisExists(uint32_t id);
	ParsedEphemData getEphemeris(uint32_t id);
	ecef calcPosition(int id,double _time);
	double calcDoppler(int id, double _time, Position myPos);
	double calcDistance(ecef satPos);
	void setCoords(double x, double y, double z);
	double dopplers[50];
	double calcDopplers[50];
   ecef coords;
	 double speed_diff;
};

// Holds the ephemris data as defined in RINEX 2.10
struct EphData
{
   // PRN / EPOCH / SV CLK
   short int   PRN;

   double      svClkBias, svClkDft, svClkDftRt;

   // BROADCAST ORBIT - 1
   double   IODE, crs, dn, m0;

   // BROADCAST ORBIT - 2
   double   cuc, ecc, cus, sqrta;

   // BROADCAST ORBIT - 3
   double   t, cic, omega, cis;

   // BROADCAST ORBIT - 4
   double   inc, crc, w, omegaDot;

   // BROADCAST ORBIT - 5
   double   IDOT, channelCodes, gpsWeekNum, pDataFlag;

   // BROADCAST ORBIT - 6
   double   svAccuracy, svHealth, TGD, IODC;

   // BROADCAST ORBIT - 7
   double   timeOfMsg, fitInterval;
};

// Holds data that is calculated from the satellite ephemris and reciever data
struct CalcData
{
   double   pos[3];     // GPS satellite position in ECEF
   double   vel[3];     // GPS satellite velocity in ECEF
   double   a;          // Semimajor axis
   double   dt;         // Time from ephemeris
   double   n;          // Mean motion
   double   M;          // Mean anomaly
   double   f;          // True anomaly
   double   E;          // Eccentric anomoly
   double   phi;        // Argument of latitude
   double   mu;         // Corrected argument of latitude
   double   rad;        // Corrected radius
   double   inc;        // Corrected inclination
   double   xOrb;       // X coordinate in orbit plane
   double   yOrb;       // Y coordinate in orbit plane
   double   omega;      // corrected longitude of ascending node
   double   EDot;       // Eccentric anomoly dot
   double   MDot;       // Mean anomaly dot
   double   phiDot;     // Argument of latitude dot
   double   muDot;      // Corrected Argument of latitude dot
   double   radDot;     // Corrected radius dot
   double   incDot;     // Corrected inclination dot
   double   xOrbDot;    // X coordinate in orbit plane dot
   double   yOrbDot;    // Y coordinate in orbit plane dot
   double   omegaDot;   // Corrected longitude of ascending node dot
   double   relVel;     // Relative velocity compared to reciever
	double   relPos[3];  // Relative position compared to reciever
	double   doppler;    // Doppler in Hz
};
