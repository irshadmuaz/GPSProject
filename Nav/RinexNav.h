// Header file for parsing the RINEX 2.10 Navigation file
#ifndef RINEX_H
#define RINEX_H

/* ----- Includes, Usings, and Structs ----- */
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <math.h>
#include <sstream>
#include "Time.h"
#include <iomanip>

#define EARTH_GRAV   3.986008e14
#define EARTH_ROT    7.292115167e-5

using namespace std;

// Holds the ephemris data as defined in RINEX 2.10
struct EphData
{
   // PRN / EPOCH / SV CLK
   short int   PRN;
   Time        time;
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

// Holds the header data for RINEX 2.10
struct HeaderData
{
   float    version;
   char     fileType;
   string   program;
   string   runBy;
   string   date;
   vector <string> comments;
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

/* ----- Prototypes and Variables ----- */
class NavParser
{
public:
   // Parses the data from fileLocation
   void ReadData(string fileLocation);
   // Calculates data for each satellite at a given time and returns the calculated values
   struct CalcData EphCalc(short int prn, Time time, double pos[3]);
   // Prints the doppler for each satellite to text files
   void createReport(string reportName, double pos[3]);

   // Header Data
   struct HeaderData header;

   // Body Data
   struct EphData satEph[32];

private:
   ifstream    inFile;     // Holds the navigation file
   string      line;       // Holds the current line being read

   void   ReadHeader();    // Parses the data in the header
   void   ReadBody();      // Parses the data in the body
};

#endif