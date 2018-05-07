
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
#include "Time.h"
using namespace std;

void findHeader(ifstream&);
void getDoppler(ifstream&, ofstream&);
void storeDopplar(ifstream&, vector<vector<double>> &, vector<string> &, 
     vector<vector<Time>> &);
void storeSection(ifstream&, string, int, vector<string>&, 
     vector<vector<double>>&, vector<vector<Time>> &, string);
void writeToFiles(vector<vector<double>> &, vector<string> &, 
     vector<vector<Time>> &);
Time storeTime(string);


int main (int argc, char * argv[])
{
   vector<vector<double>> dopplarData;
   vector<string> satNames;
   vector<vector<Time>> dopplarTime;
   
// make sure there is an input file
   if(argc != 2){
     cout << "Error: Please input a file" << endl;
     return(1);
   }
   
   ifstream inFile(argv[1]);
   ofstream outFile("Doppler.txt"); //needed if want to use getDoppler
   
// make sure the files opened
   if(!(inFile && outFile)){
      cout << "Error: Could not open files" << endl;
      return(1);
    }
 
   findHeader(inFile);
   storeDopplar(inFile, dopplarData, satNames, dopplarTime);
   writeToFiles(dopplarData, satNames, dopplarTime);
   
  
   
// close the files
   inFile.close();
   outFile.close();
   
   return 0;
}

/******************************************************************************
 * Function findHeader - positions the file to point to the beginning of the  *
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
 *                        the data to a text file (not organized by          *
 *                        satellite name)                                    *
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
 * Function storeDopplar - We're going to store this data in a 2D vector. So  *
 *                         the dopplar data for each satellite is going in a  *
 *                         seperate column with a vector of satellite names   *
 *                         in corresponding indexes. Time will be saved the   *
 *                         same way as the dopplar.                           *          
 * Parameters - ifstream& inFile - the file with the data                     *
 *              2D vector dopplarData - the 2D vector which will hold the     *
 *              dopplar data.                                                 *
 *              string vector satNames - a vector which saves each found      *
 *              satellite name in the order which it was found                *
 *              2D vector dopplarTime - the 2D vector which will store the    *
 *              time data, each column representing data for a specific       *
 *              satellite                                                     *
 * Return - void                                                              *
 *****************************************************************************/
void storeDopplar(ifstream& inFile, vector<vector<double>>& dopplarData, 
     vector<string> & satNames, vector<vector<Time>> & dopplarTime)
{
   int spaceCount = 0;
   string lineHold;
   string satel_Head;
   unsigned int i;
   vector<double> dopplarHold;
   int satNum;
   string time_Head;
   
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
      // gathers list of satellite names and time into strings
         satel_Head = lineHold.substr(30, lineHold.length() -30);
         time_Head = lineHold.substr(0, 30);
     
         if(isspace(satel_Head[0]))
             satel_Head[0] = '0'; 
         satNum = stoi(satel_Head.substr(0,2));
         
      // Stores the section into the 2D vector. A section is the data
      // between data readings (between satellite headers)
         storeSection(inFile, satel_Head, satNum, satNames, dopplarData,
          dopplarTime, time_Head);
       }
  
  } // closes while loop
       
   
}


/******************************************************************************
 * Function storeSection - stores the dopplar data and time for a given       * 
 *                         section in 2D vectors.                             *
 * Parameters - ifstream& dataFile - the file with all the data               *
 *              string sat_Head - the header giving which satellites the      *
 *              dopplar data is for                                           *
 *              int satNum - the number of satellites in this section         *
 *              vector<string> & satellite_Names - a vector of all the        *
 *              satellites we have found                                      *
 *              2D vector dopplarData - the 2D vector we are writing the data * 
 *              to                                                            *
 *              2D vector dopplarTime - the 2D vector we are writing the time *
 *              data to                                                       *
 *              string time_Head - a string reading of the time, found in the *
 *              header for the section                                        *
 * Return - void                                                              *
 *****************************************************************************/
void storeSection(ifstream& dataFile, string sat_Head, int satNum,
     vector<string> & satellite_Names, vector<vector<double>> & dopplarData,
     vector<vector<Time>> & dopplarTime, string time_Head)
{
    string sat_Name;
    string lineHold;
    string lineHold2;
    bool isNew = true;
    double dopplar;
    vector<double> dopplarHold;
    unsigned int index = 0;
    int i = 0;
    unsigned int j = 0;
    int k, kcount = 0;
    Time thisTime;
    thisTime = storeTime(time_Head);
    vector<Time> timeHold;
 
    for(i = 0; i < satNum; i++)
    {
      kcount = 0;
      index = 0;
      dopplarHold.clear();
      isNew = true;
      sat_Name = sat_Head.substr(2+(3*i),3);
      for(j = 0; j < 3; j++)
      {
          if(isspace(sat_Name[j]))
              sat_Name[j] = '0';
      }
    
         
   // Check to see if each satellite has been found before or not
      for(j = 0; j < satellite_Names.size(); j++)
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
         timeHold.push_back(thisTime);
         dopplarTime.push_back(timeHold);
    // find the dopplar in the dataFile, save to a new column in the 2D vector
         getline(dataFile, lineHold); 
         if (lineHold.length() > 15)
         {
           lineHold2 = lineHold.substr(35,13);
       
          // check to make sure its not all spaces
           for(k = 0; k < 13; k++)
           {
              if(isspace(lineHold2[k]))
                  kcount++;
           }
           
           if (kcount < 12) {dopplar = stod(lineHold2); }
         // the flag for if the dopwasn't extracted
           else {dopplar = -1000000.00; }
          
           {
             dopplar = stod(lineHold2);
             if(abs(dopplar) > 15)
             {
               dopplarHold.push_back(dopplar);
               dopplarData.push_back(dopplarHold);
             }
           }
         }
      }
        
      else
      {
         dopplarTime.at(index).push_back(thisTime);
         getline(dataFile, lineHold);
         if (lineHold.length() > 15)
         {
            lineHold2 = lineHold.substr(35,13);
           // check to make sure its not all spaces
           for(k = 0; k < 13; k++)
           {
              if(isspace(lineHold2[k]))
                  kcount++;
           }
           
           if (kcount  < 12) {dopplar = stod(lineHold2); }
           else { dopplar = -1000000.00; }
            
           if(abs(dopplar) > 25)
            {
              dopplarData.at(index).push_back(dopplar);
            }
         }
      }


    // cout << setprecision(3) << fixed << dopplar << endl;

   }
  
}


/******************************************************************************
 * Function writeToFiles - writes the dopplar data in the 2D array to seperate*
 *                         text files                                         *
 * Parameters - vector<vector<double>> dopplarData - the 2D vector with the   *
 *              dopplar data we have stored                                   *
 *              vector<string> satNames - a vector with all the found         *
 *              satellite names                                               *
 *              2D vector dopplarTime - the 2D vector with the time data      *
 * Return: void                                                               *
 *****************************************************************************/
 void writeToFiles(vector<vector<double>>& dopplarData, vector<string>&
        satNames, vector<vector<Time>> & dopplarTime)
 {
    std::stringstream ss;
 
    for(unsigned i = 0; i < dopplarData.size(); i++)
    {
       ss.str(std::string());
       ss << satNames.at(i) << ".txt";
       ofstream out(ss.str());
       for(unsigned int j = 0; j < dopplarData.at(i).size(); j++)
       {
         out << setprecision(3) << fixed << setw(9)<< dopplarData.at(i).at(j);
         out << dopplarTime.at(i).at(j) << endl;
       } 
     out.close();
    }
 }
 
/******************************************************************************
 * Function: storeTime - finds and returns the time of the dopplar reading    *
 * Parameters: string time_Head - the line in the rinex file that contains    *
 *             the time data before each reading                              *
 * Return: the time reading saved into a Time object                          *
 *****************************************************************************/
Time storeTime(string time_Head)
{
    int year, month, day, hour, minute;
    float seconds; 
    
    year = stoi(time_Head.substr(1,2));
    month = stoi(time_Head.substr(4,2));
    day = stoi(time_Head.substr(7,2));
    hour = stoi(time_Head.substr(10,2));
    minute = stoi(time_Head.substr(13,2));
    seconds = stod(time_Head.substr(16, 6));
    
    Time currentTime(year, month, day, hour, minute, seconds);
    
    return currentTime;
    
}
 
 
 
 
      
