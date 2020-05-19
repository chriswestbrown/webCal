/*
 * Calendar Utilities
 * Conventions:
 * 1. day of month starts (as usual) at 1
 * 2. month number starts (as usual) at 1 = January
 * 3. day-of-week starts at 0, i.e. Sunday = 0
 * 4. only valid for dates after 1752 (when the
 *    Gregorian calendar was adopted)
 */
#ifndef _CALUTIL_H_
#define _CALUTIL_H_
#include <string>

namespace calutil
{

//Exceptions for calutil
class CalUtilException : public std::exception
{
private:
  std::string msg;
public:
  CalUtilException(const std::string &msg) throw() : msg(msg) { }
  ~CalUtilException() throw() { }
  virtual const char* what() const throw() { return msg.c_str(); }
};

  static char dowLetterArray[] = { 'U','M','T','W','R','F','S' };
  inline char dowLetter(int i_dow) { return dowLetterArray[i_dow]; }
  static std::string dowNameArray[] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };
  inline int dowNameToIndex(const std::string& name)
  {
    int i = 0;
    while(i < 7 && name != dowNameArray[i])
      i++;
    return i;
  }
  
//Determines if year is a leap year
bool isLeapYear (int year); 

//Get the day name from a numeric day
std::string getDayName (int day);

//Get the month name from a numeric month 
std::string getMonthName (int month);

//Determines the day of the week for a given date
//where Sunday is 0, Saturday is 6
int dayOfWeek (int day, int month, int year);

//Determines the day of the week for the first of the month
//where Sunday is 0, Saturday is 6
int startOfMonth (int month, int year);

//Determines the number of days in the month
int daysInMonth(int month, int year);

//Determines the number of weeks to display for a month
int weeksInMonth(int month, int year);

//Converts a Julian Calendar day (assumed to be noon) to
//Gregorian Calendar day
void julieToGreg(int jday, int &day, int &month, int &year);

//Converts a Gregorian Calendar day to 
//Julian Calendar day 
void gregToJulie(int &jday, int day, int month, int year);

//Writes textual output (and possibly padding for complete weeks
// based on days in the prior and following months)to standard output.
void generateOutput(int month, int year, bool pad);
}
#endif
