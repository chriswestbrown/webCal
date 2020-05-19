#include "date.h"
#include "calutil.h"
#include <sstream>
#include <iostream>

using namespace std;

namespace webcal
{

  Date::Date(int month, int day, int year)
  {
    calutil::gregToJulie(jday,day,month,year);
  }

  void Date::getDMY(int &day, int &month, int &year) const
  {
    calutil::julieToGreg(jday,day,month,year);
  }

  Date Date::forwardOneMonth() const
  {
    int d,m,y;
    this->getDMY(d,m,y);
    int k = calutil::daysInMonth(m,y);
    return this->forwardDays(k);
  }

  
  char Date::getDowLetter() const
  {
    int d,m,y;
    getDMY(d,m,y);
    return calutil::dowLetter(calutil::dayOfWeek(d,m,y));
  }

  int Date::getDowIndex() const
  {
    int d,m,y;
    getDMY(d,m,y);
    return calutil::dayOfWeek(d,m,y);
  }

  int Date::getDowLetter(const std::string& str) { return calutil::dowLetter(calutil::dowNameToIndex(str)); }

  std::string Date::getMonthName()
  {
    int d,m,y;
    getDMY(d,m,y);
    return calutil::getMonthName(m);
  }

  
  string to_string(const Date &D)
  {
    int d, m, y;
    D.getDMY(d,m,y);
    ostringstream sout;
    sout << m << '/' << d << '/' << y;
    return sout.str();
  }
  
  void DateContext::setNominalRange(Date d_first, Date d_last)
  {
    this->d_first = d_first;
    this->d_last = d_last;
    nominalRangeInitialized = true;
  }

  void DateContext::regDate(Date d)
  {
    if (!nominalRangeInitialized)
      setNominalRange(d,d);
    else
      setNominalRange(min(d_first,d),max(d_last,d));
  }

  
  static int yearExpand(int y)
  {
    return y < 100 ? 2000 + y : y;
  }

  Date DateContext::parseDate(const std::string &str)
  {
    istringstream sin(str);
    return parseDate(sin);
  }

  Date DateContext::parseDate(std::istream &in)
  {
    try {
    int f1, f2, f3;
    char delim = 0;
    if (in >> f1 && in >> delim && delim == '/' && in >> f2)
    {
      if (monthFirstInput)
	swap(f1,f2);
      if (in.peek() != '/')
      {
	// abbreviated date
	int y1 = d_first.getYear();
	int y2 = d_last.getYear();
	Date a(f2,f1,y1);
	Date b(f2,f1,y1 + 1);
	if (d_first <= a && a <= d_last) return a;
	if (d_first <= b && b <= d_last) return b;
	if (a < d_first)
	{
	  int da = daysInRange(a,d_first);
	  int db = daysInRange(d_last,b);
	  return da <= db ? a : b;
	}
	else // d_last < a
	{
	  Date c(f2,f1,y1 - 1);
	  int dc = daysInRange(c,d_first);
	  int da = daysInRange(d_last,a);
	  return da <= dc ? a : c;
	}	  
      }
      else if (in >> delim >> f3)
      {
	return Date(f2,f1,yearExpand(f3));
      }	
      else
	throw WebCalException("Error! Invalid date format in parseDate.");
    }
    else
      throw WebCalException("Error! Invalid date format in parseDate.");
    }
    catch(calutil::CalUtilException& e)
    {
      throw WebCalException(e.what());
    }
  }
}
