// This application uses the NavParser class to read in the data from a 
// RINEX 2.10 .nav file into usable data. The standards for this type of
// file can be found at ftp://igs.org/pub/data/format/rinex210.txt
// David Lynge 10-April-2018

#include <iostream>
#include <string>
#include "RinexNav.h"

using namespace std;

int main(int argc, char *argv[])
{
	NavParser NavMsg;
	
	double rec[3] = {654572, -5210045, 3608339}; 

   if (argc != 2)
   {
      cout << "Error: Must provide a single nav file argument" << endl;
      return 0;
   }
   
   cout << 1 + ".txt" << endl;
	NavMsg.ReadData(argv[1]);

	NavMsg.EphCalc(13, 1, 56.996, rec);

	for (int i = 0; i < NavMsg.satCount; i++)
		cout << NavMsg.vecEph.at(i).PRN << " Doppler: " << 
         NavMsg.vecCalc.at(i).doppler << endl;

   NavMsg.CompDoppler(rec);

	cin.get();   
   return 0;
}
