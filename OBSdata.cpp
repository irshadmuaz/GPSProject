/*
 * Find the doppler data in a rinex obs file and print to a text file
 * The doppler data is a frequency given in hertz
 *
 * Compiled using C++11
 */

#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <iomanip>
#include <cstring>
#include <cmath>
#include <vector>
#include <algorithm>
#include <sstream>
using namespace std;

void findHeader(ifstream&);
void getDoppler(ifstream&, ofstream&);
void storeDopplar(ifstream&);
void storeSection(ifstream&, string, int, vector<string>&,
     vector<vector<double>>& dataFile);

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
   storeDopplar(inFile);
   
  
   
// close the files
   inFile.close();
   outFile.close();
   
   return 0;
}

/******************************************************************************
 *  Function findHeader - positions the file to point to the beginning of the *
 *                         obs data (finds and skips past the header)         *
 * Parameters: ifstream& file - the file to find the header for               *
 * Return: void                                                               *
 *****************************************************************************/
 void findHeader(ifstream& file)
 {
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
 * Function: getDopplar - finds the dopplar data in the obs file and prints  *
 *                        the data to a text file                            *
 * Parameters: ifstream& inFile - the file to find the data for              *
 *             ofstream& outFile - the file to write the dopplar data to     *
 * Return: void                                                              *
 *****************************************************************************/
void getDoppler(ifstream& inFile, ofstream& outFile)
{    
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
 
 
/******************************************************************************
 * function storeDopplar - We're going to store this data in a 2D vector. So  *
 *                         the dopplar data for each satellite is going in a  *
 *                         seperate column with a vector of satellite names in*
 *                         corresponding indexes                              *
 * Parameters - ifstream& inFile - the file with the data                     *
 * Return - void                                                              *
 *****************************************************************************/
void storeDopplar(ifstream& inFile)
{

   int spaceCount = 0;
   string lineHold;
   string satel_Head;
   unsigned int i, j;
   vector<double> dopplarHold;
   int satNum;
   vector<string> SatNames;
   vector<vector<double>> dopplarData;
   
   while(!inFile.eof())
     { 
       getline(inFile, lineHold);
       spaceCount = 0;
       for(i = 0; i < lineHold.length(); i++)
       {
          if(isspace(lineHold[i]))
                spaceCount++;
       }
     
    // conditions signify this is not a data line
       if(lineHold.length() > 30 && spaceCount < 18)
       {
      // gathers satellite name
         satel_Head = lineHold.substr(30, lineHold.length() - 1);
         if(isspace(satel_Head[0]))
              satel_Head = satel_Head.substr(1, satel_Head.length()-1);
        
         satNum = stoi(satel_Head.substr(0,2));
        // cout << satNum << endl;
      //   cout << satel_Head << endl;
         storeSection(inFile, satel_Head, satNum, SatNames, dopplarData);
       }
      
   } // closes eof  
   
 // lets print out the data
     for(i=0; i < dopplarData.size(); i++)
     {
       cout << SatNames.at(i) << endl;
       for(j = 0; j < dopplarData.at(i).size(); j++)
       {
         cout << setprecision(3) << fixed << dopplarData.at(i).at(j) << endl;
       }
     }  
   
}


/******************************************************************************
 * function storeSection - stores the dopplar data for a given section in the *
 *                         2D vector.                                         *
 * Parameters - ifstream& dataFile - the file with all the data               *
 *              string sat_Name - the header giving which satellites the      *
 *                dopplar data is for                                         *
 *              int satNum - the number of satellites to store the dopplar for*
 *              vector<string> satellite_Names & - a vector of all the        *
 *                satellites we have found                                    *
 *              vector<vector<double>>& dopplarData - the 2D vector we are    *
 *                writing the data to                                         *
 * Return - void                                                              *
 *****************************************************************************/
void storeSection(ifstream& dataFile, string sat_Head, int satNum, 
     vector<string> & satellite_Names, vector<vector<double>> & dopplarData)
{
    string sat_Name;
    string lineHold;
    bool isNew = true;
    double dopplar;
    vector<double> dopplarHold;
    unsigned int index = 0;

    for(int i = 0; i < satNum; i++)
    {
      sat_Name = sat_Head.substr(1+(3*i),3);
         
   // Check to see if each satellite has been found before or not
      for(unsigned j = 0; j < satellite_Names.size(); j++)
      {
          if (sat_Name.compare(satellite_Names.at(j)) == 0)
          {
              isNew = false;
              index = j;
          }
      }  
      
      if(isNew)
      {
         satellite_Names.push_back(sat_Name);
    // find the dopplar in the dataFile, save to a new column in the 2D vector
         getline(dataFile, lineHold);
         if (lineHold.length() > 15)
         {
           dopplar = stod(lineHold.substr(30,39));
           dopplarHold.push_back(dopplar);
           dopplarData.push_back(dopplarHold);
          // cout << dopplarData.at(dopplarData.size()-1).at(0) << endl;
       //    cout << setprecision(3) << fixed << dopplar << endl;
         }
      }
        
      else
      {
         getline(dataFile, lineHold);
         if (lineHold.length() > 15)
         {
            dopplar = stod(lineHold.substr(30,39));
            dopplarData.at(index).push_back(dopplar);
         }
      }
   }
}

