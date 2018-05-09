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
   int i; 

   for (int j = 0; j < 32; j++)
      satEph[j].PRN = 0;

	while (getline(inFile, line))
	{
      i = stoi(line.substr(0, 2)) - 1;

		// PRN / EPOCH / SV CLK
		satEph[i].PRN =	      stoi(line.substr(0, 2));
		satEph[i].time.setYear(stoi(line.substr(3, 2)));
		satEph[i].time.setMonth(stoi(line.substr(6, 2)));
		satEph[i].time.setDay(stoi(line.substr(9, 2)));
		satEph[i].time.setHour(stoi(line.substr(12, 2)));
		satEph[i].time.setMinute(stoi(line.substr(15, 2)));
		satEph[i].time.setSecond(stof(line.substr(17, 5)));
		satEph[i].svClkBias =   stod(line.substr(22, 19));
		satEph[i].svClkDft =    stod(line.substr(41, 19));
		satEph[i].svClkDftRt =  stod(line.substr(60, 19));

		// BROADCAST ORBIT - 1
		getline(inFile, line);
		satEph[i].IODE =	stod(line.substr(3, 19));
		satEph[i].crs =	stod(line.substr(22, 19));
		satEph[i].dn =		stod(line.substr(41, 19));
		satEph[i].m0 =		stod(line.substr(60, 19));

		// BROADCAST ORBIT - 2
		getline(inFile, line);
		satEph[i].cuc =	stod(line.substr(3, 19));
		satEph[i].ecc =	stod(line.substr(22, 19));
		satEph[i].cus =	stod(line.substr(41, 19));
		satEph[i].sqrta =	stod(line.substr(60, 19));

		// BROADCAST ORBIT - 3
		getline(inFile, line);
		satEph[i].t =		stod(line.substr(3, 19));
		satEph[i].cic =	stod(line.substr(22, 19));
		satEph[i].omega =	stod(line.substr(41, 19));
		satEph[i].cis =	stod(line.substr(60, 19));

		// BROADCAST ORBIT - 4
		getline(inFile, line);
		satEph[i].inc =		stod(line.substr(3, 19));
		satEph[i].crc =		stod(line.substr(22, 19));
		satEph[i].w =			stod(line.substr(41, 19));
		satEph[i].omegaDot =	stod(line.substr(60, 19));

		// BROADCAST ORBIT - 5
		getline(inFile, line);
		satEph[i].IDOT =			   stod(line.substr(3, 19));
		satEph[i].channelCodes =	stod(line.substr(22, 19));
		satEph[i].gpsWeekNum =	   stod(line.substr(41, 19));
		satEph[i].pDataFlag =		stod(line.substr(60, 19));

		// BROADCAST ORBIT - 6
		getline(inFile, line);
		satEph[i].svAccuracy =	stod(line.substr(3, 19));
		satEph[i].svHealth =	   stod(line.substr(22, 19));
		satEph[i].TGD =			stod(line.substr(41, 19));
		satEph[i].IODC =			stod(line.substr(60, 19));

		// BROADCAST ORBIT - 7
		getline(inFile, line);
		satEph[i].timeOfMsg =	stod(line.substr(3, 19));
		satEph[i].fitInterval =	stod(line.substr(22, 19));
	}
}

// Calculates data for a satellite at a given time and reciever position
struct CalcData NavParser::EphCalc(short int prn, Time time, double pos[3])
{
	struct CalcData calc;
	struct EphData eph;

	double   temp1;
	double   temp2;
	double   EOld;
	int	   count;

   // Set the ephemris data
   eph = satEph[prn - 1];

   // Semimajor axis
   calc.a = eph.sqrta * eph.sqrta;

   // Time difference
   calc.dt = time - eph.time;

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

   calc.doppler = 1.57542e9 * calc.relVel / 299792458.0; // - 210

   return calc;
}

// Print the doppler to their respective 
void NavParser::createReport(char *reportName, double pos[3])
{
   ifstream obsData;
   ofstream report;

   Time timeOfData;
   double measDoppler;
   double calcDoppler;
   double refDoppler; // Accounts for drift errors

   stringstream fileName;
   string line;

   report.open(reportName);

   report << "PRN | MEASURED | CALCULATED | DIFFERENCE | Y M D H M S" << endl;

   refDoppler = 0;

   // Loop through the sattelites
   for (int i = 1; i <= 32; i++)
   {
      fileName.str("");
      obsData.close();

      // Create fileName to open
      if (i < 10)
         fileName << "G0" << i << ".txt";
      else
         fileName << "G" << i << ".txt";
      
      obsData.open(fileName.str().c_str());

      // Ignore if satellite info is missing
      if (!obsData.is_open() || satEph[i - 1].PRN == 0)
         continue;

      // Iterate through lines
      while (!obsData.eof())
      {
         obsData >> measDoppler;
         if (!(obsData >> timeOfData)) break;

         calcDoppler = EphCalc(i, timeOfData, pos).doppler;

         // Set drift error on first pass
         if (refDoppler == 0)
         {
            refDoppler = measDoppler - calcDoppler;
         }

         calcDoppler += refDoppler;
         
         report << setw(3) << i << " ";
         report << setw(10) << measDoppler << " ";
         report << setw(12) << calcDoppler << " ";
         report << setw(12) << measDoppler - calcDoppler;
         report << timeOfData << endl;
      }

      
   }

   report.close();
}
