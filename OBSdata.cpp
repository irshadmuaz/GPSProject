/*
 * Find the doppler data in a rinex obs file and print to a text file
 * The doppler data is a frequency given in hertz
 */

#include <iostream>
#include <fstream>
#include <string.h>
#include <stdio.h>
#include <iomanip>
#include <cstring>
using namespace std;

void findHeader(ifstream&);
void findDoppler(ifstream&);

int main (int argc, char * argv[]){

// make sure there is an input file
   if(argc != 2){
     cout << "Error: enter a file" << endl;
     return(1);
   }
   
   ifstream inFile(argv[1]);
   ofstream outFile("Doppler.txt");
   
// make sure the files opened
   if(!(inFile && outFile)){
      cout << "Error: Could not open files" << endl;
      return(1);
    }
 
   findHeader(inFile);
  
   
// close the files
   inFile.close();
   outFile.close();
   
   return 0;
}

/*****************************************************************************
 *  Function find Header - positions the file to point to the beginning of the
 *                         obs data (finds and skips past the header)          
 * Parameters: ifstream& file - the file to find the header for
 * Return: void
 */
 void findHeader(ifstream& file){

// 'END OF HEADER' has 13 chars, so look for them in the file 
    int i = 0;   
    char endHeader[14];
    char searchForString[] = "END OF HEADER";
    
    while(strcmp(endHeader, searchForString) != 0){
       file.read(endHeader, 13);
       endHeader[13] = '\0';
       i++;
       file.seekg(i, file.beg);
    }
// position file to point to end of header
    file.seekg(i+12, file.beg);
    
 }
 
 
 
/*****************************************************************************
 * Function: findDopplar - finds the dopplar data in the obs file
 * Parameters: ifstream& file - the file to find the data for
 * Return: void
 */
void findDoppler(ifstream&){
     
       
}
