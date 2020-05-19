#include "calutil.h"
#include <iostream>

using namespace std;

namespace calutil
{
bool isLeapYear(int year)
{
  bool leap = false;
  if (year % 4 == 0)
    leap = true;
  if (year % 100 == 0)
    leap = false;
  if (year % 400 == 0)
    leap = true;
  return leap;
}

string getDayName(int day)
{
  if ((day < 0) || (day > 6))
    throw CalUtilException("Unexpected day input!");
  else if (day == 0)
    return "Sunday";
  else if (day == 1)
    return "Monday";
  else if (day == 2)
    return "Tuesday";
  else if (day == 3)
    return "Wednesday";
  else if (day == 4)
    return "Thursday";
  else if (day == 5)
    return "Friday";
  else 
    return "Saturday";
}

string getMonthName(int month)
{
  string out;

  if (month == 1)
    out="January";
  else if (month == 2)
    out="February";
  else if (month == 3)
    out="March";
  else if (month == 4)
    out="April";
  else if (month == 5)
    out="May";
  else if (month == 6)
    out="June";
  else if (month == 7)
    out="July";
  else if (month == 8)
    out="August";
  else if (month == 9)
    out="September";
  else if (month == 10)
    out="October";
  else if (month == 11)
    out="November";
  else if (month == 12)
    out="December";
  else    
    throw CalUtilException("Unexpected month input!");
  return out;
}

int dayOfWeek (int day, int month, int year)
{
  //returns 0 for Sunday, 1 for Monday, ... 6 for Saturday
  //This function only works if you input dates beginning 1 October 1752.
  //Dates before this had weirdness because the Gregorian calendar hadn't
  //yet been adopted.

  if (month < 3)
    {
      month = month + 12;
      year = year - 1;
    }
  return (
	  day
	  + (2 * month)
	  + int(6 * (month + 1) / 10)
	  + year
	  + int(year / 4)
	  - int(year / 100)
	  + int(year / 400)
	  + 1 //account for Gregorian calendar
	  ) % 7; //Beatles' song notwithstanding...
}
  
int startOfMonth (int month, int year)
{
  return dayOfWeek(1,month,year);
}

int daysInMonth(int month, int year)
{
  //A lot easier than using your knuckles...

  if ((month < 1) || (month > 12))
    throw CalUtilException("Illegal month specified!");
  else if (year < 1752)
    throw CalUtilException("Illegal year specified!");
  else if ((year == 1752) && (month < 9))
    throw CalUtilException("Illegal date specified!");
  else if ((year == 1752) && (month == 9))
    return 19; //Note: This is accurate; there was no 3SEP1752 to 13SEP1752
               //done checking weird Gregorian dates
  else if ((month == 1) ||
	   (month == 3) ||
	   (month == 5) ||
	   (month == 7) ||
	   (month == 8) ||
	   (month == 10)||
	   (month == 12))
    return 31;
  else if ((month == 4) ||
	   (month == 6) ||
	   (month == 9) ||
	   (month == 11))
    return 30;
  else if (month == 2)
    {
      if (isLeapYear(year))
	return 29;
      else
	return 28;
    }
  else
    throw CalUtilException("Unexpected input specified!");
}


int weeksInMonth(int month, int year)
{
  int start = startOfMonth(month,year);
 
  int days  = daysInMonth(month,year);
  int remaining = days-(7-start);

  return 1 + remaining / 7 + (remaining % 7 == 0 ? 0 : 1);
  
}

//Reference: Fliegel, H. F. and van Flandern, T. C. (1968).
//Communications of the ACM, Vol. 11, No. 10 (October, 1968).
void julieToGreg(int jday, int &day, int &month, int &year)
{
  int I;
  int J;
  int K;
  int L;
  int N;
  int JD = jday;

  L= JD+68569;
  N= 4*L/146097;
  L= L-(146097*N+3)/4;
  I= 4000*(L+1)/1461001;
  L= L-1461*I/4+31;
  J= 80*L/2447;
  K= L-2447*J/80;
  L= J/11;
  J= J+2-12*L;
  I= 100*(N-49)+I+L;

  year= I;
  month= J;
  day= K;
}

//Reference: Fliegel, H. F. and van Flandern, T. C. (1968).
//Communications of the ACM, Vol. 11, No. 10 (October, 1968).
void gregToJulie(int &jday, int day, int month, int year)
{
  if (!(1 <= month && month <= 12 && 1752 < year && 1 <= day && day <= daysInMonth(month,year)))
  {
    std::string s; s += to_string(month) + "/" + to_string(day) + "/" + to_string(year);
    throw CalUtilException("Error!  Invalid date '" + s + "'.");
  }

  int JD;
  int I = year;
  int J = month;
  int K = day;
 
  JD = K-32075+1461*(I+4800+(J-14)/12)/4+367*(J-2-(J-14)/12*12)
    /12-3*((I+4900+(J-14)/12)/100)/4;

  jday = JD;
}

void generateOutput(int month, int year, bool pad)
{
  //prepare for start of month dummy days
  int prevMonth;
  int prevYear;
  int nextMonth;
  int nextYear;

  if (month == 1)
    {
      prevMonth = 12;
      prevYear = year - 1;
      nextMonth = 2;
      nextYear = year;
    }
  else if (month == 12)
    {
      prevMonth = 11;
      prevYear = year;
      nextMonth = 1;
      nextYear = year;
    }
  else
    {
      prevMonth = month - 1;
      prevYear = year;
      nextMonth = month + 1;
      nextYear = year;
    }

  int day = daysInMonth(prevMonth,prevYear)-startOfMonth(month,year)+1;

  //Make month/year header
  cout<<getMonthName(month)<<" "<<year<<"\n\n";
 
  //Make start of month dummy days
  if(pad)
    {
      for (day; day <= daysInMonth(prevMonth,prevYear); day++)
	{
	  cout<<getDayName(dayOfWeek(day,prevMonth,prevYear))<<" "<<day
	      <<" "<<getMonthName(prevMonth)<<" "<<prevYear<<"\n";
	}
      cout<<endl;
    }
  
  //Make month's days
  for (day=1; day<=daysInMonth(month,year); day++)
    {
      cout<<getDayName(dayOfWeek(day,month,year))<<" "<<day<<" "<<getMonthName(month)<<" "<<year<<"\n";
    }

  cout<<endl;

  //prepare for end of month dummy days
  int i= dayOfWeek(day,month,year);
  day = 1;

  //Make end of month dummy days
  if (pad)
    {
      for (i;i<=6;i++)
	{
	  cout<<getDayName(i)<<" "<<day<<" "<<getMonthName(nextMonth)
	      <<" "<<nextYear<<"\n";
	  day++;
	}
    }
}

}
