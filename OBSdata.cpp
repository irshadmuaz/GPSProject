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
#include <cmath>
using namespace std;

void findHeader(ifstream&);
void getDoppler(ifstream&, ofstream&);

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
   getDoppler(inFile, outFile);
   
  
   
// close the files
   inFile.close();
   outFile.close();
   
   return 0;
}

/*****************************************************************************
 *  Function findHeader - positions the file to point to the beginning of the
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
 * Function: getDopplar - finds the dopplar data in the obs file and prints
 *                        the data to a text file
 * Parameters: ifstream& inFile - the file to find the data for
 *             ofstream& outFile - the file to write the dopplar data to
 * Return: void
 */
void getDoppler(ifstream& inFile, ofstream& outFile){    

// the dopplar frequency should never go above 10,000 Hz
     double dopplar = 0;
     string lineHold;
     unsigned int i = 0;
     int spaceCount = 0;
 
     while(!inFile.eof()){
       getline(inFile, lineHold);
       spaceCount = 0;
       for(i = 0; i < lineHold.length(); i++){
          if(isspace(lineHold[i]))
                spaceCount++;
       }
        
        if (lineHold.length() > 15){
           dopplar = stod(lineHold.substr(30,39));
           if(abs(dopplar) > 25)
              outFile << setprecision(3) << fixed << dopplar << endl;
          }
         
     }
    
 }
     



     

