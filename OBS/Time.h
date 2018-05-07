#include <ostream>
#include <istream>
#include <algorithm>
#include <tuple>
#include <sstream>

using namespace std;

class Time
{
   private:
     int YEAR;
     int MONTH;
     int DAY;
     int HOUR;
     int MINUTE;
     float SECOND;
     
   public:
     Time();
     Time(int, int, int, int, int, float);
     ~Time();
     int& year ();
     int &month();
     int &day();
     int &hour();
     int &minute();
     float &second();
     friend std::ostream& operator<< (std::ostream&, const Time&);
     Time& operator=(const Time&);
 
};
