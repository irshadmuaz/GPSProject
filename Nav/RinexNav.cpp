// Function definitions for RinexNav
#include "RinexNav.h"

// Opens the navigation file and calls functions to parse the data
void NavParser::ReadData(const char *fileLocation)
{
	inFile.open("/home/david/GPSdopplar/Nav/COM4_180405_123531(apr5_binary2).nav");
   
	// Return exception if the file isn't opening
	if (!inFile.is_open())
		throw "Invalid file input.";

	ReadHeader();
	ReadBody();

	inFile.close();
}

// Reads the header lines; Should be called before ReadBody()
void NavParser::ReadHeader()
{
	string label;	// Columns 61 to 80 of line

	// Iterate through each line
	while (getline(inFile, line))
	{ 
		label = line.substr(60);

		if (label == "RINEX VERSION / TYPE")
		{ 
			version = stof(line.substr(0, 9)); 
			fileType = line[20];
		}
		else if (label == "PGM / RUN BY / DATE ")
		{
			program = line.substr(0, 20);
			runBy = line.substr(20, 20);
			date = line.substr(40, 20);
		}
		else if (label == "COMMENT             ")
		{
			comments.push_back(line.substr(0, 59));
		}
		else if (label.substr(0, 3) == "END")
			break;
	}
}

// Reads the body lines; Should be called after ReadHeader()
void NavParser::ReadBody()
{
	struct EphData satData;

	while (getline(inFile, line))
	{
		// PRN / EPOCH / SV CLK
		satData.PRN =	stoi(line.substr(0, 2));
		satData.year =	stoi(line.substr(3, 2));
		satData.month = stoi(line.substr(6, 2));
		satData.day =	stoi(line.substr(9, 2));
		satData.hour =	stoi(line.substr(12, 2));
		satData.minute =	stoi(line.substr(15, 2));
		satData.second =	stof(line.substr(17, 5));
		satData.svClkBias = stod(line.substr(22, 19));
		satData.svClkDrift =		stod(line.substr(41, 19));
		satData.svClkDriftRate =	stod(line.substr(60, 19));

		// BROADCAST ORBIT - 1
		getline(inFile, line);
		satData.IODE =		stod(line.substr(3, 19));
		satData.crs =		stod(line.substr(22, 19));
		satData.dn =		stod(line.substr(41, 19)); // 3.14159265358979323846;
		satData.m0 =		stod(line.substr(60, 19)); // 3.14159265358979323846;

		// BROADCAST ORBIT - 2
		getline(inFile, line);
		satData.cuc =			stod(line.substr(3, 19));
		satData.ecc =			stod(line.substr(22, 19));
		satData.cus =			stod(line.substr(41, 19));
		satData.sqrta =			stod(line.substr(60, 19));

		// BROADCAST ORBIT - 3
		getline(inFile, line);
		satData.t =			stod(line.substr(3, 19));
		satData.cic =		stod(line.substr(22, 19));
		satData.omega =		stod(line.substr(41, 19)); // 3.14159265358979323846;
		satData.cis =		stod(line.substr(60, 19));

		// BROADCAST ORBIT - 4
		getline(inFile, line);
		satData.inc =		stod(line.substr(3, 19)); // 3.14159265358979323846;
		satData.crc =		stod(line.substr(22, 19));
		satData.w =			stod(line.substr(41, 19)); // 3.14159265358979323846;
		satData.omegaDot =	stod(line.substr(60, 19)); // 3.14159265358979323846;

		// BROADCAST ORBIT - 5
		getline(inFile, line);
		satData.IDOT =			stod(line.substr(3, 19)); // 3.14159265358979323846;
		satData.channelCodes =	stod(line.substr(22, 19));
		satData.gpsWeekNum =	stod(line.substr(41, 19));
		satData.pDataFlag =		stod(line.substr(60, 19));

		// BROADCAST ORBIT - 6
		getline(inFile, line);
		satData.svAccuracy =	stod(line.substr(3, 19));
		satData.svHealth =		stod(line.substr(22, 19));
		satData.TGD =			stod(line.substr(41, 19));
		satData.IODC =			stod(line.substr(60, 19));

		// BROADCAST ORBIT - 7
		getline(inFile, line);
		satData.timeOfMsg =		stod(line.substr(3, 19));
		satData.fitInterval =	stod(line.substr(22, 19));

		
		vecEph.push_back(satData);
		
	}
}

void NavParser::EphCalc(short int hr, short int min, float sec, double pos[3])
{
	struct CalcData calc;
	struct EphData eph;

	double temp1;
	double temp2;
	double EOld;
	int	count;

	double relVel;
	double relPos[3];
	double doppler;

	vecCalc.clear();

	for (int i = 0; i < vecEph.size(); i++)
	{
		// Set the ephemris data
		eph = vecEph.at(i);

		// Semimajor axis
		calc.a = eph.sqrta * eph.sqrta;

		// Time difference
		calc.dt = 3600 * (hr - eph.hour) + 60 * (min - eph.minute) + sec - eph.second;

		// Calculate mean motion
		calc.n = sqrt(EARTH_GRAV / (calc.a * calc.a * calc.a)) + eph.dn;

		// Calculate mean anomaly
		calc.M = eph.m0 + calc.n * calc.dt;

		// Calculate eccentric anomaly using  Newton-Raphson
		calc.E = calc.M;
		count = 0;

		do {
			EOld = calc.E;
			temp1 = 1.0 - eph.ecc * cos(EOld);
			calc.E = calc.E + (calc.M - EOld + eph.ecc * sin(EOld)) / temp1;
			count++;
			if (count > 5)
				break;
		} while (fabs(calc.E - EOld) > 1.0E-14);

		calc.EDot = calc.n / temp1;

		// Begin calc for True anomaly and Argument of Latitude
		temp2 = sqrt(1.0 - eph.ecc * eph.ecc);
		calc.phi = atan2(temp2 * sin(calc.E), cos(calc.E) - eph.ecc) + eph.w;
		calc.phiDot = temp2 * calc.EDot / temp1;

		// Calculate corrected argument of latitude based on position
		calc.mu = calc.phi + eph.cus * sin(2.0 * calc.phi) + eph.cuc * cos(2.0 * calc.phi);
		calc.muDot = calc.phiDot * (1.0 + 2.0 * (eph.cus * cos(2.0 * calc.phi) - eph.cuc * sin(2.0 * calc.phi)));

		// Calculate corrected radius based on argument of latitude
		calc.rad = calc.a * temp1 + eph.crc * cos(2.0 * calc.phi) + eph.crs * sin(2.0 * calc.phi);
		calc.radDot = calc.a * eph.ecc * sin(calc.E) * calc.EDot +
			2.0 * calc.phiDot * (eph.crs * cos(2.0 * calc.phi) - eph.crc * sin(2.0 * calc.phi));

		// Calculate inclination based on argument of latitude
		calc.inc = eph.inc + eph.IDOT * calc.dt + eph.cic * cos(2.0 * calc.phi) +
			eph.cis * sin(2.0 * calc.phi);
		calc.incDot = eph.IDOT + 2 * calc.phiDot * (eph.cis * cos(2.0 * calc.phi) -
			eph.cic * sin(2.0 * calc.phi));

		// Calculate position and velocity in orbital plane
		calc.xOrb = calc.rad * cos(calc.mu);
		calc.yOrb = calc.rad * sin(calc.mu);
		calc.xOrbDot = calc.radDot * cos(calc.mu) - calc.yOrb * calc.muDot;
		calc.yOrbDot = calc.radDot * sin(calc.mu) + calc.xOrb * calc.muDot;

		// Corrected longitude of ascending node
		calc.omegaDot = eph.omegaDot - EARTH_ROT;
		calc.omega = eph.omega + calc.dt * calc.omegaDot - EARTH_ROT * eph.t;

		// Calculate coordinates
		calc.pos[0] = calc.xOrb * cos(calc.omega) - calc.yOrb * cos(calc.inc) * sin(calc.omega);
		calc.pos[1] = calc.xOrb * sin(calc.omega) + calc.yOrb * cos(calc.inc) * cos(calc.omega);
		calc.pos[2] = calc.yOrb * sin(calc.inc);

		// Calculate velocity
		temp1 = calc.yOrbDot * cos(calc.inc) - calc.yOrb * sin(calc.inc) * calc.incDot;
		calc.vel[0] = -calc.omegaDot * calc.pos[1] + calc.xOrbDot * cos(calc.omega) - temp1 * sin(calc.omega);
		calc.vel[1] = calc.omegaDot * calc.pos[0] + calc.xOrbDot * sin(calc.omega) + temp1 * cos(calc.omega);
		calc.vel[2] = calc.yOrb * cos(calc.inc) * calc.incDot + calc.yOrbDot * sin(calc.inc);

		// Doppler
		for (int k = 0; k < 3; k++)
			relPos[k] = -calc.pos[k] + pos[k];

		relVel = (relPos[0] * calc.vel[0] + relPos[1] * calc.vel[1] + relPos[2] * calc.vel[2]) /
			sqrt(relPos[0] * relPos[0] + relPos[1] * relPos[1] + relPos[2] * relPos[2]);

		doppler = 1.57542e9 * relVel / 299792458.0 - 224;

		cout << eph.PRN << " doppler: " << doppler << endl;

		vecCalc.push_back(calc);
	}
}
