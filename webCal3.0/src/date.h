#ifndef _WEBCAL_DATE_H_
#define _WEBCAL_DATE_H_

#include <istream>

namespace webcal
{
  //Exceptions for calutil
  class WebCalException : public std::exception
  {
  private:
    std::string msg;
  public:
    WebCalException(const std::string &msg) throw() : msg(msg) { }
    ~WebCalException() throw() { }
    virtual const char* what() const throw() { return msg.c_str(); }
  };

  
  class Date
  {
  private:
    int jday=-1;
    Date(int jd) : jday(jd) { }
  public:
    Date() {}
    Date(int month, int day, int year);
    int daysInRange(const Date& d2) const { return d2.jday - jday + 1; }
    Date forwardDays(int k) const { return Date(jday + k); } // NOTE: negative numbers allowed!
    Date forwardOneMonth() const;
    void getDMY(int &day, int &month, int &year) const;
    int getYear() const { int d,m,y; getDMY(d,m,y); return y; }
    std::string getMonthName();
    inline bool operator<(const Date& d2) const { return jday < d2.jday; }
    inline bool operator<=(const Date& d2) const { return jday <= d2.jday; }
    inline bool operator==(const Date& d2) const { return jday == d2.jday; }
    inline Date operator++(int) { Date d(*this); jday++; return d; } // NOTE: This is post-increment!
    int getDowIndex() const;
    char getDowLetter() const;
    int getDowLetter(const std::string& str);
  };

  std::string to_string(const Date &d);
  inline int daysInRange(const Date& d1, const Date& d2) { return d1.daysInRange(d2); }

  class DateContext
  {
  private:
    bool monthFirstInput = true;
    bool nominalRangeInitialized = false;
    Date d_first, d_last;
  public:
    DateContext() { }
    void setNominalRange(Date d_first, Date d_last);
    void setMonthFirstInput(bool val) { monthFirstInput = val; }
    void regDate(Date d);
    Date parseDate(std::istream &in);
    Date parseDate(const std::string &str);
    Date earliestDate() const { return d_first; }
    Date latestDate() const { return d_last; }
  };
  
}
#endif

