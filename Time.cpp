#include "Time.h"

// Constructors
Time::Time() { }
Time::Time(int year, int month, int day, int hour, int minute, float second) :
  YEAR(year), MONTH(month), DAY(day), HOUR(hour), MINUTE(minute), SECOND(second)
  {  }

// Destructors
Time::~Time() { }

// Setters
int& Time::year() { return this->YEAR; }
int& Time::month() { return this-> MONTH; }
int& Time::day() { return this-> DAY; }
int& Time::hour() { return this-> HOUR; }
int& Time::minute() { return this-> MINUTE; }
float& Time::second() { return this -> SECOND; }

// Gettes
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
