#include "Time.h"

// Constructors
Time::Time() { }
Time::Time(int year, int month, int day, int hour, int minute, float second) :
  YEAR(year), MONTH(month), DAY(day), HOUR(hour), MINUTE(minute), SECOND(second)
  {  }

// Destructors
Time::~Time() { }

// Getters
int& Time::year() { return this->YEAR; }
int& Time::month() { return this-> MONTH; }
int& Time::day() { return this-> DAY; }
int& Time::hour() { return this-> HOUR; }
int& Time::minute() { return this-> MINUTE; }
float& Time::second() { return this -> SECOND; }

// Setters
void Time::setYear(int year) {this->YEAR = year; }
void Time::setMonth(int month) {this->MONTH = month; }
void Time::setDay(int day) {this->DAY = day; }
void Time::setHour(int hour) {this->HOUR = hour; }
void Time::setMinute(int minute) {this->MINUTE = minute; }
void Time::setSecond(float second) {this->SECOND = second; }

// Overload = operator
Time& Time::operator=(const Time& rhs)
{
   this->YEAR = rhs.YEAR;
   this->MONTH = rhs.MONTH;
   this->DAY = rhs.DAY;
   this->HOUR = rhs.HOUR;
   this->MINUTE = rhs.MINUTE;
   this->SECOND = rhs.SECOND;
   return *this;
}

// Overload << operator
std::ostream& operator<< (std::ostream& out, const Time& time)
{
   Time copy = time;
   return out << "  " << copy.year() << " " << copy.month()
     << " " << copy.day() << " " << copy.hour() << " " << copy.minute()
     << " " << copy.second();
}

// Overload >> operator
std::istream& operator>> (std::istream& file, Time& time)
{
   int year, month, day, hour, minute;
   float second;
   
   file >> year;
   file >> month;
   file >> day;
   file >> hour;
   file >> minute;
   file >> second;
   
   Time newTime(year,month,day,hour,minute,second);
   time = newTime;
   
   
   return file;
} 

// Overload - operator
float operator-(const Time& time1, const Time& time2)
{
   Time time1copy;
   time1copy = time1;
   Time time2copy;
   time2copy = time2;
   
 //  int year = time1copy.year() - time2copy.year();
 //  int month = time1copy.month() - time2copy.month();
 //  int day = time1copy.day() - time2copy.day();
   int hour = time1copy.hour()*3600 - time2copy.hour()*3600;
   int minute = time1copy.minute()*60 - time2copy.minute()*60;
   float second = time1copy.second() - time2copy.second();
   
   float newSeconds = hour + minute + second;
   return newSeconds;
}


