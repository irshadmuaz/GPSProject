#include "RinexNav.h"

// Opens the navigation file and calls functions to parse the data
void NavParser::ReadData(const char *fileLocation)
{
	inFile.open(fileLocation);
   
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
			header.version = stof(line.substr(0, 9)); 
			header.fileType = line[20];
		}
		else if (label == "PGM / RUN BY / DATE ")
		{
			header.program = line.substr(0, 20);
			header.runBy = line.substr(20, 20);
			header.date = line.substr(40, 20);
		}
		else if (label == "COMMENT             ")
		{
			header.comments.push_back(line.substr(0, 59));
		}
		else if (label.substr(0, 3) == "END")
			break;
	}
}

// Reads the body lines; Should be called after ReadHeader()
void NavParser::ReadBody()
{
	struct EphData satEph;

	while (getline(inFile, line))
	{
		// PRN / EPOCH / SV CLK
		satEph.PRN =	   stoi(line.substr(0, 2));
		satEph.year =	   stoi(line.substr(3, 2));
		satEph.month =    stoi(line.substr(6, 2));
		satEph.day =	   stoi(line.substr(9, 2));
		satEph.hour =	   stoi(line.substr(12, 2));
		satEph.minute =	stoi(line.substr(15, 2));
		satEph.second =	stof(line.substr(17, 5));
		satEph.svClkBias =  stod(line.substr(22, 19));
		satEph.svClkDft =   stod(line.substr(41, 19));
		satEph.svClkDftRt = stod(line.substr(60, 19));

		// BROADCAST ORBIT - 1
		getline(inFile, line);
		satEph.IODE =	stod(line.substr(3, 19));
		satEph.crs =	stod(line.substr(22, 19));
		satEph.dn =		stod(line.substr(41, 19));
		satEph.m0 =		stod(line.substr(60, 19));

		// BROADCAST ORBIT - 2
		getline(inFile, line);
		satEph.cuc =	stod(line.substr(3, 19));
		satEph.ecc =	stod(line.substr(22, 19));
		satEph.cus =	stod(line.substr(41, 19));
		satEph.sqrta =	stod(line.substr(60, 19));

		// BROADCAST ORBIT - 3
		getline(inFile, line);
		satEph.t =		stod(line.substr(3, 19));
		satEph.cic =	stod(line.substr(22, 19));
		satEph.omega =	stod(line.substr(41, 19));
		satEph.cis =	stod(line.substr(60, 19));

		// BROADCAST ORBIT - 4
		getline(inFile, line);
		satEph.inc =		stod(line.substr(3, 19));
		satEph.crc =		stod(line.substr(22, 19));
		satEph.w =			stod(line.substr(41, 19));
		satEph.omegaDot =	stod(line.substr(60, 19));

		// BROADCAST ORBIT - 5
		getline(inFile, line);
		satEph.IDOT =			   stod(line.substr(3, 19));
		satEph.channelCodes =	stod(line.substr(22, 19));
		satEph.gpsWeekNum =	   stod(line.substr(41, 19));
		satEph.pDataFlag =		stod(line.substr(60, 19));

		// BROADCAST ORBIT - 6
		getline(inFile, line);
		satEph.svAccuracy =	stod(line.substr(3, 19));
		satEph.svHealth =	   stod(line.substr(22, 19));
		satEph.TGD =			stod(line.substr(41, 19));
		satEph.IODC =			stod(line.substr(60, 19));

		// BROADCAST ORBIT - 7
		getline(inFile, line);
		satEph.timeOfMsg =	stod(line.substr(3, 19));
		satEph.fitInterval =	stod(line.substr(22, 19));

		vecEph.push_back(satEph);
	}
   satCount = vecEph.size();
}

void NavParser::EphCalc(short int hr, short int min, float sec, double pos[3])
{
	struct CalcData calc;
	struct EphData eph;

	double   temp1;
	double   temp2;
	double   EOld;
	int	   count;

	vecCalc.clear();

	for (int i = 0; i < satCount; i++)
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
			calc.relPos[k] = -calc.pos[k] + pos[k];

		calc.relVel = (calc.relPos[0] * calc.vel[0] + calc.relPos[1] * calc.vel[1] + calc.relPos[2] * calc.vel[2]) /
			sqrt(calc.relPos[0] * calc.relPos[0] + calc.relPos[1] * calc.relPos[1] + calc.relPos[2] * calc.relPos[2]);

		calc.doppler = 1.57542e9 * calc.relVel / 299792458.0 - 224;

		vecCalc.push_back(calc);
	}
}

// Print the doppler to their respective 
void NavParser::CompDoppler(double pos[3])
{
   ifstream obsData;
   stringstream fileName;

   // Loop through the sattelites
   for (int i = 0; i < 32; i++)
   {
      fileName.str("");
      obsData.close();

      if (i < 10)
         fileName << "G0" << i << ".txt";
      else
         fileName << "G" << i << ".txt";

      cout << fileName.str() << endl;

      obsData.open(fileName.str().c_str());

      if (!obsData.is_open())
         continue;

      getline()
   }
}
