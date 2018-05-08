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
    // getters
    
     int& year ();
     int &month();
     int &day();
     int &hour();
     int &minute();
     float &second();
    // setters
     void setYear(int);
     void setMonth(int);
     void setDay(int);
     void setHour(int);
     void setMinute(int);
     void setSecond(float);
     
     friend std::ostream& operator<< (std::ostream&, const Time&);
     friend std::istream& operator>>(std::istream&, Time&);
     Time& operator=(const Time&);
     friend float operator-(const Time&, const Time&);
 
};
