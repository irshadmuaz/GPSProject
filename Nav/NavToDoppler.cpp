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
	
   string reportName = "Report.txt";
	double rec[3] = {654771, -5209937, 3608422}; 

   if (argc < 2)
   {
      cout << "Error: Must provide a .nav file" << endl;
      return 0;
   }

   if (argc == 5)
   {
      rec[0] = stod(argv[2]);
      rec[1] = stod(argv[3]);
      rec[2] = stod(argv[4]);
   }

	NavMsg.ReadData(argv[1]);

   NavMsg.createReport(reportName, rec);
   
   return 0;
}
