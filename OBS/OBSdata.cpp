
/*
 * Find the doppler data in a rinex obs file and print to a text file
 * The doppler data is a frequency given in hertz
 *
 * Compatible with RINEX v 2.10
 * Compiled using C++11
 *
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
void storeDopplar(ifstream&, vector<vector<double>> &, vector<string> &, 
     vector<vector<Time>> &, vector<vector<double>> &);
void storeSection(ifstream&, string, int, vector<string>&, 
     vector<vector<double>>&, vector<vector<Time>> &, string, 
     vector<vector<double>> &);
void writeToFiles(vector<vector<double>> &, vector<string> &, 
     vector<vector<Time>> &, vector<vector<double>> &);
Time storeTime(string);


int main (int argc, char * argv[])
{
   vector<vector<double>> dopplarData;
   vector<string> satNames;
   vector<vector<Time>> dopplarTime;
   vector<vector<double>> CNdata;
   
// make sure there is an input file
   if(argc != 2)
   {
     cout << "Error: Please input a file" << endl;
     return(1);
   }
   
   ifstream inFile(argv[1]);
   
// make sure the files opened
   if(!(inFile))
    {
      cout << "Error: Could not open files" << endl;
      return(1);
    }
 
   findHeader(inFile);
   storeDopplar(inFile, dopplarData, satNames, dopplarTime, CNdata);
   writeToFiles(dopplarData, satNames, dopplarTime, CNdata);
   
// close the files
   inFile.close();
   
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
 *              2D vector CNdata - the 2D vector with the CNo data, each      *
 *              column for a specific satellite                               *
 * Return - void                                                              *
 *****************************************************************************/
void storeDopplar(ifstream& inFile, vector<vector<double>>& dopplarData, 
     vector<string> & satNames, vector<vector<Time>> & dopplarTime, 
     vector<vector<double>>& CNdata)
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

 // simple constraint to ensure this line is significant (line header or data)
       if(lineHold.length() > 30)
       {  
      // gathers list of satellite names and time into strings
         time_Head = lineHold.substr(0, 30);
         satel_Head = lineHold.substr(30, lineHold.length() -30);

         if(isspace(satel_Head[0])) {satel_Head[0] = '0';}
 
         satNum = stoi(satel_Head.substr(0,2));

         if(satNum > 12)
         { 
            std::string s2; 
            getline(inFile, s2);
            satel_Head = satel_Head + s2; 
         }
         
      // Stores the section into the 2D vector. A section is the data
      // between data readings (between satellite headers)
         storeSection(inFile, satel_Head, satNum, satNames, dopplarData,
           dopplarTime, time_Head, CNdata);
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
 *              2D vector CNdata - the 2D vector with the CNo data, each      *
 *              column for a specific satellite                               *
 * Return - void                                                              *
 *****************************************************************************/
void storeSection(ifstream& dataFile, string sat_Head, int satNum,
     vector<string> & satellite_Names, vector<vector<double>> & dopplarData,
     vector<vector<Time>> & dopplarTime, string time_Head, vector<
     vector<double>>& CNdata)
{
    string sat_Name;
    string lineHold;
    string lineHold2;
    string lineHold3;
    bool isNew = true;
    double dopplar;
    double CN;
    vector<double> dopplarHold;
    vector<double> CNHold;
    unsigned int index = 0;
    int i = 0;
    unsigned int j = 0;
    int k, kcount = 0;
    Time thisTime;
    vector<Time> timeHold;
    thisTime = storeTime(time_Head);


// if the number of satellites is over 12 the header will take up two lines, erase the spaces
    if (satNum > 12) { sat_Head.erase(38,32); }
 
    for(i = 0; i < satNum; i++)
    {
      timeHold.clear();
      kcount = 0;
      index = 0;
      dopplarHold.clear();
      CNHold.clear();
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
           lineHold3 = lineHold.substr(55, 10);
          // check to make sure its not all spaces
           for(k = 0; k < 13; k++)
           {
              if(isspace(lineHold2[k]))
                  kcount++;
           }
           
           if (kcount < 12) {  dopplar = stod(lineHold2); }

         // the flag for if the dop wasn't extracted
           else {dopplar = -1000000.00; }
          
        // test case to ensure the extraced dopplar is legitimate
           if(abs(dopplar) > 15)
           {
             CN = stod(lineHold3);
             CNHold.push_back(CN);
             CNdata.push_back(CNHold);
             dopplarHold.push_back(dopplar);
             dopplarData.push_back(dopplarHold);
           }
           
         } // closes if (lineHold.length() > 15)

      } // closes if (isNew)
        
      else
      {
         dopplarTime.at(index).push_back(thisTime);
         getline(dataFile, lineHold);
         if (lineHold.length() > 15)
         {
            lineHold2 = lineHold.substr(35,13);
            lineHold3 = lineHold.substr(55,10);
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
              CN = stod(lineHold3);
              dopplarData.at(index).push_back(dopplar);
              CNdata.at(index).push_back(CN);
            }

         } // closes if (lineHold.length() > 15)

      } // closes else to if (isNew)
 
   } // closes for i < satNum
  
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
        satNames, vector<vector<Time>> & dopplarTime, 
        vector<vector<double>> & CNdata)
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
         out << "     ";
         out << CNdata.at(i).at(j);
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
 
 
 
 
      
