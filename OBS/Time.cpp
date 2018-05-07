#include "Time.h"

// Constructors
Time::Time() { }
Time::Time(int year, int month, int day, int hour, int minute, float second) :
  YEAR(year), MONTH(month), DAY(day), HOUR(hour), MINUTE(minute), SECOND(second)
  {  }

// Destructors
Time::~Time() { }

// Getters and setters
int& Time::year() { return this->YEAR; }
int& Time::month() { return this-> MONTH; }
int& Time::day() { return this-> DAY; }
int& Time::hour() { return this-> HOUR; }
int& Time::minute() { return this-> MINUTE; }
float& Time::second() { return this -> SECOND; }

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
