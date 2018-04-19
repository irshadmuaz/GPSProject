/*
 * Find the doppler data in a rinex obs file and print to a text file
 * The doppler data is a frequency given in hertz
 *
 * Compiled using C++11
 */

#include <iostream>
#include <fstream>
#include <string.h>
#include <stdio.h>
#include <iomanip>
#include <cstring>
#include <cmath>
#include <vector>
#include <algorithm>
using namespace std;

void findHeader(ifstream&);
void getDoppler(ifstream&, ofstream&);
vector<vector<double>> storeDopplar(ifstream&);
void getDopplar(vector<double> &, ifstream&);


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
   //getDoppler(inFile, outFile);
   vector<vector<double>> dopplarData = storeDopplar(inFile);
   
  
   
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
 
 
/****************************************************************************
 * function storeDopplar - saves the dopplar data in a 2D vector, each column
 *                        stores the data of a different satellite
 * Parameters -
 * Return - 2D double vector with all the dopplar data for each satellite
 */
vector<vector<double>> storeDopplar(ifstream& inFile){
 
 // vector to hold the dopplar frequencies  
   vector<vector<double>> data;
 // vector to store the names of each satellite. Index will coorespond to
 // that satellite's data in the data vector
   vector<string> sat_Names;
   int spaceCount = 0;
   string lineHold;
   string satel_Hold;
   unsigned int i, j;
   vector<double> dopplarHold;
   bool isNew = true;
   int satelliteColumn;
   
   while(!inFile.eof()){
       getline(inFile, lineHold);
       spaceCount = 0;
       for(i = 0; i < lineHold.length(); i++){
          if(isspace(lineHold[i]))
                spaceCount++;
       }
     
    // conditions signify this is not a data line
       if(lineHold.length() > 30 && spaceCount < 18){
      // gathers satellite name
          satel_Hold = lineHold.substr(30, lineHold.length() - 1);
          if(isspace(satel_Hold[0]))
               satel_Hold = satel_Hold.substr(1, satel_Hold.length()-1);
        
      // get the dopplar data for this satellite
          getDopplar(dopplarHold, inFile);
          
       // check to see if this is a new satellite
       for(i = 0; i < sat_Names.size(); i++){
          if(satel_Hold.compare(sat_Names.at(i)) == 0){
             isNew = false;
             satelliteColumn = i;
          }
          else
             isNew = true;
        }
         
        if(isNew){
        // store the dopplar data for this satellite in a new column and add
        // the satellite name to the names vector
          sat_Names.push_back(satel_Hold);
          data.push_back(dopplarHold);   
        }
        
          
        else {  
       // store the dopplar for this satellite in a new column
           for(i = 0; i < dopplarHold.size(); i++){
              data.at(satelliteColumn).push_back(i);
           }   
         }
  
       
     }
      
   }
   
    
 // lets print out the 2D vector
  /* for(i = 0; i < data.size(); i++){
     cout << sat_Names.at(i) << endl;
     for(j = 0; j < data.at(i).size(); j++){
       cout << data.at(i).at(j) << endl;
       }
   }*/
           
   
   return data;

}


/*****************************************************************************
 * function getDopplar - gets the dopplar data for the current satellite
 * Parameters - vector<double> & to save the data to
 * Return - void
 */
void getDopplar(vector<double> & data, ifstream& inFile){
   
   string lineHold;
   unsigned int i = 0;
   double dopplar = 0.00;
   data.clear();
   
 // do while the data in the file is for this satellite
 // spaceCount > 18 signifies header for next satellite
 
 // let spaceCount start above 18
   int spaceCount = 19;
   while (spaceCount > 18){
      getline(inFile, lineHold);
      spaceCount = 0;
      for(i = 0; i < lineHold.length(); i++){
          if(isspace(lineHold[i])) { spaceCount++; }
       }
      
     
      
   // ensure this a line with data
      if (lineHold.length() > 15){
           dopplar = stod(lineHold.substr(30,39));
         // ensure this is the dopplar data
           if(abs(dopplar) > 25) { 
              cout << fixed << setprecision(3) << dopplar << endl;
              data.push_back(dopplar);
          }
       }
              
   }


}


