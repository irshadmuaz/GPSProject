// Header file for parsing the RINEX 2.10 Navigation file
#pragma once

/* ----- Includes, Usings, and Structs ----- */
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <math.h>

#define EARTH_GRAV	3.986008e14
#define EARTH_ROT	7.292115167e-5

using namespace std;

struct EphData
{
	// PRN / EPOCH / SV CLK
	short int	PRN;
	short int	year, month, day, hour, minute;
	float		second;
	double		svClkBias, svClkDrift, svClkDriftRate;

	// BROADCAST ORBIT - 1
	double	IODE, crs, dn, m0;

	// BROADCAST ORBIT - 2
	double	cuc, ecc, cus, sqrta;

	// BROADCAST ORBIT - 3
	double	t, cic, omega, cis;

	// BROADCAST ORBIT - 4
	double	inc, crc, w, omegaDot;

	// BROADCAST ORBIT - 5
	double	IDOT, channelCodes, gpsWeekNum, pDataFlag;

	// BROADCAST ORBIT - 6
	double	svAccuracy, svHealth, TGD, IODC;

	// BROADCAST ORBIT - 7
	double	timeOfMsg, fitInterval;
};

struct CalcData 
{
	short int hour, minute;
	float	second;		// Time of calculated data
	double	pos[3];		// GPS satellite position in ECEF
	double	vel[3];		// GPS satellite velocity in ECEF
	double	a;			// Semimajor axis
	double	dt;			// Time from ephemeris
	double	n;			// Mean motion
	double	M;			// Mean anomaly
	double	f;			// True anomaly
	double	E;			// Eccentric anomoly
	double	phi;		// Argument of latitude
	double	mu;			// Corrected argument of latitude
	double	rad;		// Corrected radius
	double	inc;		// Corrected inclination
	double	xOrb;		// X coordinate in orbit plane
	double	yOrb;		// Y coordinate in orbit plane
	double	omega;		// corrected longitude of ascending node
	double	EDot;		// Eccentric anomoly dot
	double	MDot;		// Mean anomaly dot
	double	phiDot;		// Argument of latitude dot
	double	muDot;		// Corrected Argument of latitude dot
	double	radDot;		// Corrected radius dot
	double	incDot;		// Corrected inclination dot
	double	xOrbDot;	// X coordinate in orbit plane dot
	double	yOrbDot;	// Y coordinate in orbit plane dot
	double	omegaDot;	// Corrected longitude of ascending node dot

};

/* ----- Prototypes and Variables ----- */
class NavParser
{
public:
	// Parses the data from fileLocation
	void	ReadData(string fileLocation);
	// Calculates data for each satellite at a given time
	void	EphCalc(short int hr, short int min, float sec, double pos[3]);

	// Header Data
	float	version;
	char	fileType;
	string	program;
	string	runBy;
	string	date;
	vector <string> comments;

	// Body Data
	vector <struct EphData> vecEph;

	// Calculated Data
	vector <struct CalcData> vecCalc;

private:
	ifstream inFile;		// Holds the navigation file
	string	 line;			// Holds the current line being read

	void	ReadHeader();		// Parses the data in the header
	void	ReadBody();			// Parses the data in the body

};
