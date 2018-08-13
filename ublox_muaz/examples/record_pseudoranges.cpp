#include "ublox/ublox.h"
#include <iostream>
#include <fstream>
#include "ublox/calc_position.h"
// #include "boost/filesystem.hpp"
#define BUF_SIZE 600
using namespace ublox;
using namespace std;


ofstream data_file_;  //!< file stream for logging gps data
std::string data_filename_; //!< file name for logging gps data
ofstream doppler_file_; //file stream for logging doppler and calculated doppler
std::string doppler_filename; //file name for logging doppler and calculated doppler
Position myPos = Position("hello");
bool StartDataLogging(std::string filename) {
    try {

        data_filename_ = filename;
        doppler_filename = filename + "_doppler";
        
        // open file and add header
        data_file_.open(data_filename_.c_str());
        doppler_file_.open(doppler_filename.c_str());
        // write header
        data_file_ << "%%RANGE  1) GPS Time(ms) 2) SVID  3) Pseudorange (m)  4) SVID  5) Pseudorange ..." << std::endl;
        data_file_ << "%%CLOCK  1) GPS Time(ms) 2) ClockBias(nsec) 3) ClkDrift(nsec/sec) 4) TimeAccuracyEstimate(nsec) 5) FreqAccuracyEstimate(ps/s)" << std::endl;
        doppler_file_ << "PRN | SNR | MEASURED | CALCULATED | DIFFERENCE | Y M D H M S" << std::endl;
    } catch (std::exception &e) {
        std::cout << "Error opening log file: " << e.what();
        if (data_file_.is_open())
            data_file_.close();
        if(doppler_file_.is_open())
            doppler_file_.close();
        return false;
    }
    std::cout << "Started Data Log" << std::endl;
    return true;
}

void PseudorangeData(ublox::RawMeas raw_meas, double time_stamp) {
    try {


         static double measBuffer[BUF_SIZE];
         static double prevMeas = 0, prevCalc = 0;
         double measDiff, calcDiff;
         vector <double> vecDop;
         static int dopNum = 0;
         double averageDop;

        
        data_file_ << fixed << "RANGE" << "\t" << (signed long)raw_meas.iTow;
        for(int ii=0;ii<raw_meas.numSV; ii++) {
            data_file_  << "\t" << (unsigned int)raw_meas.rawmeasreap[ii].svid
                        << "\t" << setprecision(3) << raw_meas.rawmeasreap[ii].psuedorange; // m

        }
        data_file_ << std::endl;
        data_file_ << fixed << "CARRIER" << "\t" << (signed long)raw_meas.iTow;
        for(int ii=0;ii<raw_meas.numSV; ii++) {
            data_file_  << "\t" << (unsigned int)raw_meas.rawmeasreap[ii].svid
            << "\t" << setprecision(3) << raw_meas.rawmeasreap[ii].carrier_phase; // m
        }
        data_file_ << std::endl;
        data_file_ << fixed << "DOPPLER" << "\t" << (double)raw_meas.iTow;

        vecDop.clear();
        for(int ii=0;ii<raw_meas.numSV; ii++) {
            data_file_  << "\t" << (unsigned int)raw_meas.rawmeasreap[ii].svid
            << "\t" << setprecision(3) << raw_meas.rawmeasreap[ii].doppler; // m
            if(!myPos.ephemerisExists(raw_meas.rawmeasreap[ii].svid))
            {
               cout << "  Ephemeris Does Not Exist" << endl;
            }
            else if (raw_meas.rawmeasreap[ii].gnssId)
            {
               cout << "  Non-GPS Constellation" << endl;
            }
            else
            {
                int svid = (unsigned int)raw_meas.rawmeasreap[ii].svid;

                double calcDoppler = myPos.calcDoppler(raw_meas.rawmeasreap[ii].svid, (double)raw_meas.iTow,  myPos);
                double measDoppler = raw_meas.rawmeasreap[ii].doppler;
               
               // Write to file
                doppler_file_ << setw(3) << svid << " ";
                doppler_file_ << setw(5) << (int)raw_meas.rawmeasreap[ii].cno << " ";
                doppler_file_ << setw(10) << measDoppler << " ";
                doppler_file_ << setw(12) << calcDoppler << " ";
                doppler_file_ << setw(12) << measDoppler - calcDoppler << " ";
                doppler_file_ << "0 0 0 0 0 " << raw_meas.iTow << endl;

               // Calculated moving average
               /*measBuffer[dopNum++] = measDoppler;
               dopNum %= BUF_SIZE;
               averageDop = 0;
               for (int m = 0; m < BUF_SIZE; m++)
               {
                  averageDop += measBuffer[m];
               }
               averageDop /= BUF_SIZE;

               measDiff = averageDop - prevMeas;
               calcDiff = calcDoppler - prevCalc;
               prevMeas = averageDop;
               prevCalc = calcDoppler;*/

               vecDop.push_back(calcDoppler - measDoppler);

               // Write to cout
                cout << "  Calc: " << setw(8) << calcDoppler << "  Meas: "<< setw(8) << measDoppler
                << "  Error: " << setw(8) << calcDoppler - measDoppler << endl;
                /*<< "  CalcDiff: " << setw(8) << calcDiff
                << "  MeasDiff: " << setw(8) << measDiff
                << "  DiffError: " << setw(8) << calcDiff - measDiff << endl;*/

                myPos.dopplers[svid] = measDoppler;
                myPos.calcDopplers[svid] = calcDoppler;
            }
        }
        data_file_ << std::endl;
        data_file_ << fixed << "CNO" << "\t" << (signed long)raw_meas.iTow;
        for(int ii=0;ii<raw_meas.numSV; ii++) {
            data_file_  << "\t" << (unsigned int)raw_meas.rawmeasreap[ii].svid
            << "\t" << setprecision(3) << (unsigned int) raw_meas.rawmeasreap[ii].cno; // m
        }
        data_file_ << std::endl;

    } catch (std::exception &e) {
        std::cout << "PseudorangeData() error";
    }    
}

// PACK(
//     struct NavClock{
//         UbloxHeader header;
//         uint32_t iTOW;
//         int32_t clkbias;    // clock bias in nanoseconds
//         int32_t clkdrift;   // clock drift in ns/s
//         uint32_t tacc;      // time accuracy estimate (ns)
//         uint32_t facc;      // frequency accuracy estimate (ps/s)
//         uint8_t checksum[2];
// });

void ClockData(ublox::NavClock nav_clock, double time_stamp) {
    try {

        data_file_  << fixed << "CLOCK" << "\t" << (signed long)nav_clock.iTOW;
        data_file_  << "\t" << (signed long) nav_clock.clkbias  // ns
                    << "\t" << (signed long) nav_clock.clkdrift // ns/s
                    << "\t" << (unsigned long) nav_clock.tacc  // ns
                    << "\t" << (unsigned long) nav_clock.facc;  // ps/s

        data_file_ << std::endl;

    } catch (std::exception &e) {
        std::cout << "ClockData() error";
    }    
}

void ParsedEphems(ublox::ParsedEphemData parsed_ephem_data, double time_stamp) {
    try{
        myPos.addEphemeris(parsed_ephem_data);
        data_file_  << fixed << "Ephemerides" << "\t" << (signed long)parsed_ephem_data.tow;
        data_file_  << setprecision(15) << "\t" << parsed_ephem_data.prn
                    << "\t" << parsed_ephem_data.tgd
                    << "\t" << parsed_ephem_data.toc
                    << "\t" << (double) parsed_ephem_data.af0
                    << "\t" << (double) parsed_ephem_data.af1
                    << "\t" << (double) parsed_ephem_data.af2
                    << "\t" << parsed_ephem_data.anrtime
                    << "\t" << (double) parsed_ephem_data.dN
                    << "\t" << parsed_ephem_data.ecc
                    << "\t" << parsed_ephem_data.majaxis
                    << "\t" << (double) parsed_ephem_data.wo
                    << "\t" << parsed_ephem_data.ia
                    << "\t" << parsed_ephem_data.omega
                    << "\t" << parsed_ephem_data.dwo
                    << "\t" << parsed_ephem_data.dia
                    << "\t" << parsed_ephem_data.cuc
                    << "\t" << parsed_ephem_data.cus
                    << "\t" << parsed_ephem_data.crc
                    << "\t" << parsed_ephem_data.crs
                    << "\t" << parsed_ephem_data.cis
                    << "\t" << parsed_ephem_data.cic
                    << "\t" << parsed_ephem_data.toe;
        data_file_ << std::endl;
    } catch (std::exception &e) {
        std::cout << "ParsedEphems() error";
    }
    
}

void NavData(ublox::NavSol nav_data, double time_stamp) {
    try{
        myPos.setCoords(nav_data.ecefX/100,nav_data.ecefY/100,nav_data.ecefZ/100);
        cout<<"position defined"<<endl;
        data_file_  << fixed << "Position" << "\t" << (signed long) nav_data.iTOW;
        data_file_  << setprecision(12) << "\t" << nav_data.ecefX/100.
                    << "\t" << nav_data.ecefY/100.
                    << "\t" << nav_data.ecefZ/100.
                    << "\t" << nav_data.pAcc
                    << "\t" << (unsigned int)nav_data.pDop
                    << "\t" << (unsigned int)nav_data.numSV;
        data_file_ << std::endl;
        data_file_  << fixed << "Velocity" << "\t" << (signed long) nav_data.iTOW
                    << "\t" << nav_data.ecefVX/100.
                    << "\t" << nav_data.ecefVY/100.
                    << "\t" << nav_data.ecefVZ/100.
                    << "\t" << nav_data.sAcc
                    << "\t" << (unsigned int)nav_data.pDop
                    << "\t" << (unsigned int)nav_data.numSV;
        data_file_ << std::endl;

    } catch (std::exception &e) {
        std::cout << "Navigation Solution Error";
    }
    
}

void StopLoggingData() {
    if (data_file_.is_open())
        data_file_.close();
    if (doppler_file_.is_open())
        doppler_file_.close();
}

int main(int argc, char **argv)
{
    Ublox my_gps;

   
    if(argc < 3) {
        std::cerr << "Usage: ublox_example <serial port address> <baud rate>" << std::endl;
        return 0;
    }
    
    std::string port(argv[1]);
    int baudrate=115200;
    istringstream(argv[2]) >> baudrate;

    // Connect to Receiver
    bool result = my_gps.Connect(port,baudrate);
        if (result) {
            cout << "Successfully connected." << endl;
        }
        else {
            cout << "Failed to connect." << endl;
            return -1;
        }
    //! Start Data Logging
    bool logging_on = StartDataLogging("range_data.log");
    
    my_gps.ConfigureNavigationParameters(4,2);
    my_gps.SbasOff();

    // Set Callback for pseudorange data
    my_gps.set_rxm_raw_callback(PseudorangeData);
    //my_gps.set_nav_clock_callback(ClockData);
    my_gps.set_parsed_ephem_callback(ParsedEphems);
    //my_gps.set_nav_solution_callback(NavData);

    //! Configure ublox
    // request pseudorange data
    my_gps.ConfigureMessageRate(0x02,0x10,1);
    // nav clock data
        //my_gps.ConfigureMessageRate(0x01,0x22,1);
    // nav clock data
    my_gps.ConfigureMessageRate(0x0B,0x31,1);
    // nav solution data
    my_gps.ConfigureMessageRate(0x01,0x06,1);
    
    // loop forever
    while(1)
      usleep(50*1000); // sleep for 50 ms

    my_gps.Disconnect();
    StopLoggingData();
    return 0;
}
























