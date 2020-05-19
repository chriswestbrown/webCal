#ifndef _WEBCAL_CALLCELL_H_
#define _WEBCAL_CALLCELL_H_

#include <vector>
#include <sstream>
#include <string>
#include "date.h"

namespace webcal
{
  class CalMsg
  {
  public:
    virtual std::string formMsg() = 0;
  };

  
  class CalCell
  {
  public:
    void setEffectiveDayOfWeek(char dow) { effDay = dow; }
    void setEffectiveDayOfWeek(const std::string& str) { effDay = mydate.getDowLetter(str); }
    Date getDate() const { return mydate; }
    void setDate(Date d) { mydate = d; }
    char getEffectiveDayOfWeek() const { return (effDay == 0 ? mydate.getDowLetter() : effDay); }
    void addToMask(const std::string& id, int priority)
    {
      if (priority > currentPriorityLevel)
      {
	currentPriorityLevel = priority;
	mtgMask.clear();
      }
      if (priority == currentPriorityLevel)
	mtgMask.push_back(id);
    }
    void dump(std::ostream& out)
    {
      out << to_string(mydate) << ' ' << getEffectiveDayOfWeek();
      for(auto& id : mtgMask)
	out << ' ' << id;
      out << std::endl;
      for(int i = 0; i < mtgMask.size(); ++i)
	out << "  " << mtgMask[i] << " " << (i < topics.size() ? topics[i] : "???") << std::endl;
      for(auto& msg : messages)
	out << "  " << msg << std::endl;
    }

    // return true if topic message added, false if not
    bool addTopicLine(const std::string& id, const std::string& msg)
    {
      const int N = mtgMask.size();
      if (N != topics.size())
	topics.resize(N);
      int i = 0;
      while(i < N && (mtgMask[i] != id || topics[i] != ""))
	i++;
      if (i == N)
	return false;
      topics[i] = msg;
      return true;
    }

    void addMsg(const std::string& msg)
    {
      messages.push_back(msg);
    }    

    std::string generateHTMLContent()
    {
      using namespace std;
      std::ostringstream sout;
      sout << "<br>" << std::endl;
      sout << "<div class=\"calcellwrap\">" << std::endl;
      sout << "<div class=\"calcellmid\">" << std::endl;
      int topcount = 0;
      for(int i = 0; i < topics.size(); i++)
	if (topics[i] != "")
	  (sout << "<div>" << topics[i] << "</div>" << endl), ++topcount;
      if (topcount == 0)
	sout << "&nbsp;" << endl;	    
      sout << "</div>" << std::endl;
      
      sout << "<div class=\"calcellfoot\">" << std::endl;
      for(int i = messages.size() - 1; i >= 0; i--)
	sout << "<div>" << messages[i] << "</div>" << endl;
      sout << "</div>" << std::endl;
      
      sout << "</div>" << std::endl;
      return sout.str();
    }
    
  private:
    Date mydate;
    char effDay = 0;
    int currentPriorityLevel = 0;
    std::vector<std::string> mtgMask; // something like [lec,lab]

    std::vector<std::string> topics;
    std::vector<std::string> messages;
  };

  class CellBank
  {
  public:
    virtual CalCell& get(Date d) = 0;
    virtual std::vector<CalCell>::iterator begin() = 0;
    virtual std::vector<CalCell>::iterator end() = 0;
  };

}
#endif
