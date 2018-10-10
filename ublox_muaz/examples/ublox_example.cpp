#define BUF_SIZE 600
#include "ublox/ublox.h"
#include <iostream>
#include <fstream>
#include "ublox/calc_position.h"
#include <thread>
// #include "boost/filesystem.hpp"

using namespace ublox;
using namespace std;

ofstream data_file_;  //!< file stream for logging gps data
std::string data_filename_; //!< file name for logging gps data
ofstream doppler_file_; //file stream for logging doppler and calculated doppler
ofstream speed_file_;
std::string doppler_filename; //file name for logging doppler and calculated doppler
std::string speed_filename;
Position myPos = Position("hello");
bool StartDataLogging(std::string filename) {
    try {

        data_filename_ = filename;
        doppler_filename = filename + "_doppler";
        speed_filename = filename + "_speed";
        
        // open file and add header
        data_file_.open( data_filename_.c_str());
        doppler_file_.open(doppler_filename.c_str());
        speed_file_.open(speed_filename.c_str());
        // write header
        data_file_ << "%%RANGE  1) GPS Time(ms) 2) SVID  3) Pseudorange (m)  4) SVID  5) Pseudorange ..." << std::endl;
        data_file_ << "%%CLOCK  1) GPS Time(ms) 2) ClockBias(nsec) 3) ClkDrift(nsec/sec) 4) TimeAccuracyEstimate(nsec) 5) FreqAccuracyEstimate(ps/s)" << std::endl;
        doppler_file_ << "PRN | SNR | MEASURED | CALCULATED | DIFFERENCE | Y M D H M S" << std::endl;
        speed_file_ << "X Direction (m/s) | Y Direction (m/s) | Z Direction (m/s) | Heading (deg) | Time (sec) (XYZ in ecef coords)" << std::endl;
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


         static double measBuffer[32][BUF_SIZE];
         static double calcBuffer[32][BUF_SIZE];
         static double prevMeas[32];
	 static double prevCalc[32];
         double measDiff, calcDiff;
         static int dopNum[32];
         static int totNum[32];
         double averageDop;
         double averageCalc;
	 vector<double> vecDop;
         int clearSats;
         static double prevMedian = 0;

        
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

	// Calculate median prior
	vecDop.clear();
	for(int ii=0;ii<raw_meas.numSV; ii++) 
		if (myPos.ephemerisExists(raw_meas.rawmeasreap[ii].svid) && !raw_meas.rawmeasreap[ii].gnssId)
                	vecDop.push_back(myPos.calcDoppler(raw_meas.rawmeasreap[ii].svid, (double)raw_meas.iTow, myPos) - raw_meas.rawmeasreap[ii].doppler);
	sort(vecDop.begin(), vecDop.end());
	prevMedian = vecDop.at((int)(vecDop.size() / 2));
        
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

                double calcDoppler = myPos.calcDoppler(raw_meas.rawmeasreap[ii].svid, (double)raw_meas.iTow, myPos);
                double measDoppler = raw_meas.rawmeasreap[ii].doppler;
		measDoppler += prevMedian;

               // Write to file
                doppler_file_ << setw(3) << svid << " ";
                doppler_file_ << setw(5) << (int)raw_meas.rawmeasreap[ii].cno << " ";
                doppler_file_ << setw(10) << measDoppler << " ";
                doppler_file_ << setw(12) << calcDoppler << " ";
                doppler_file_ << setw(12) << measDoppler - calcDoppler << " ";
                doppler_file_ << "0 0 0 0 0 " << raw_meas.iTow << endl;

               // Calculated moving average
               if (totNum[svid - 1] < BUF_SIZE)
                  totNum[svid - 1]++;
               measBuffer[svid - 1][dopNum[svid - 1]] = measDoppler;
               calcBuffer[svid - 1][dopNum[svid - 1]] = calcDoppler;
               dopNum[svid - 1] = ++dopNum[svid - 1] % BUF_SIZE;
               averageDop = 0;
               averageCalc = 0;
               for (int m = 0; m < totNum[svid - 1]; m++)
               {
                  averageCalc += calcBuffer[svid - 1][m];
                  averageDop += measBuffer[svid - 1][m];
               }
               averageDop /= totNum[svid - 1];
               averageCalc /= totNum[svid - 1];

               measDiff = averageDop - prevMeas[svid - 1];
               calcDiff = averageCalc - prevCalc[svid - 1];

               //cout << "   " << calcBuffer[svid - 1][0] << "  " << calcBuffer[svid - 1][1];
               prevMeas[svid - 1] = averageDop;
               prevCalc[svid - 1] = averageCalc;


               // Write to cout
                cout << "  Calc: " << setw(8) << calcDoppler << "  Meas: "<< setw(8) << measDoppler
                << "  Error: " << setw(10) << std::setprecision( 6 ) << std::setfill( ' ' ) << calcDoppler - measDoppler;
                //<< "  CalcDiff: " << setw(12) << calcDiff
                //<< "  MeasDiff: " << setw(12) << measDiff
                //<< "  DiffError: " << setw(12) << calcDiff - measDiff
                //<< "  DataPoints: " << setw(5) << totNum[svid - 1];
                //if (calcDiff - measDiff <= 0.015 && calcDiff - measDiff >= -0.015) 
                //   cout << "  Clear!";
                
                cout << endl;
                myPos.dopplers[svid] = measDoppler;
                myPos.calcDopplers[svid] = calcDoppler;
            }
        }
	clearSats = 0;
	sort(vecDop.begin(), vecDop.end());
	cout << "MEDIAN ERROR: " << vecDop.at((int)(vecDop.size() / 2)) << endl;
    //prevMedian = vecDop.at((int)(vecDop.size() / 2));
	for (int r = 0; r < vecDop.size(); r++)
		if (vecDop.at(r) >= vecDop.at((int)(vecDop.size() / 2)) - 25 &&
		    vecDop.at(r) <= vecDop.at((int)(vecDop.size() / 2)) + 25 )
			clearSats += 1;
	cout << "CLEAR SATELLITES (MEDIAN METHOD): " << clearSats << endl;
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
   static double oldlat;
   static double oldlon;
    try{
        myPos.setCoords(nav_data.ecefX/100,nav_data.ecefY/100,nav_data.ecefZ/100);
        //cout<<"position defined"<<endl;
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
        //xx = nav_data.ecefVX/100.;
        //yy = nav_data.ecefVY/100.;
        //zz = nav_data.ecefVZ/100.;
        if (nav_data.gpsFix == 3 )
        {
           //speed_file_ << setw(12) << sqrt(nav_data.ecefVX * nav_data.ecefVX + nav_data.ecefVY * nav_data.ecefVY + nav_data.ecefVZ * nav_data.ecefVZ)/100. << " ";
           
           myPos.coords.ecefVX = nav_data.ecefVX;
           myPos.coords.ecefVY = nav_data.ecefVY;
           myPos.coords.ecefVZ = nav_data.ecefVZ;
           

           // Calculate ecef to lla
           double a = 6378137;
           double e = 8.1819190842622e-2;
           double x = nav_data.ecefX/100.;
           double y = nav_data.ecefY/100.;
           double z = nav_data.ecefZ/100.;
           double asq = a*a;
           double esq = e*e;
           double b = sqrt(asq*(1-esq));
           double bsq = b*b;
           double ep = sqrt((asq-bsq)/bsq);
           double p = sqrt(x*x+y*y);
           double th = atan2(a*z, b*p);
           double lon = atan2(y,x) * 57.2958;
           double lat = atan2(z+ep*ep*b*pow(sin(th), 3), (p - esq*a*pow(cos(th), 3))) * 57.2958;
           double direction = -(atan2(lat - oldlat, lon - oldlon) * 57.2958) + 90;
           //cout << endl << lat << " " << lon << endl; 
           //cout.precision(17);
           //cout << "Direction:" << direction << endl;
           oldlon = lon;
           oldlat = lat;
           speed_file_ << setw(17) << nav_data.ecefVX/100. << " " << setw(17) <<  nav_data.ecefVY/100. << 
           " " << setw(17) << nav_data.ecefVZ/100. << " " <<  setw(17)  << direction << " ";

           speed_file_ << setw(11) << nav_data.iTOW / 1000 << endl;
        }

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
int extra_receiver(std::string& port)
{
  //cout<<"Port2: "<<port<<endl;
  Ublox second_gps;
  if(second_gps.Connect(port,115200))
  {
    cout<<"Receiver 2 Successfully connected"<<endl;
  }
  else
  {
      cout<<"Failed to connect second receiver"<<endl;
      return -1;

  }
  second_gps.ConfigureNavigationParameters(4,2);
  second_gps.SbasOff();
  second_gps.set_nav_solution_callback(NavData);
  second_gps.ConfigureMessageRate(0x01,0x06,2);
  while(1);
  return 0;
}
int main(int argc, char **argv)
{
    Ublox my_gps;
    std:: string port2;
    if (argc > 3)
    {
      port2 = argv[3];
      std::thread t1(extra_receiver, std::ref(port2));
      t1.detach();
      usleep(50*100000);

    }
    else if(argc < 3) {
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
      //my_gps.ConfigureMessageRate(0x01,0x06,1);
    if(argc == 3)
    {
      my_gps.set_nav_solution_callback(NavData);
      my_gps.ConfigureMessageRate(0x01,0x06,1);
    }
    // loop forever
    while(1)
      usleep(50*1000); // sleep for 50 ms

    my_gps.Disconnect();
    StopLoggingData();
    return 0;
}























