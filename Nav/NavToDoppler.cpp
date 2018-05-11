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
	double rec[3] = {654572, -5210045, 3608339}; 

   if (argc < 2)
   {
      cout << "Error: Must provide a .nav file" << endl;
      //return 0;
   }

   if (argc == 5)
   {
      rec[0] = stoi(argv[2]);
      rec[1] = stoi(argv[3]);
      rec[2] = stoi(argv[4]);
   }

	NavMsg.ReadData("2018-05-08.nav");

   NavMsg.createReport(reportName, rec);
   
   return 0;
}
