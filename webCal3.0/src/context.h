#ifndef _WEBCAL_CONTEXT_H_
#define _WEBCAL_CONTEXT_H_
#include "parse.h"
#include <iostream>
#include <algorithm>
#include <map>
#include <vector>
#include <sstream>
#include "date.h"

namespace webcal
{

  class CalCreator;
  
  class WebCalContext : public DateContext
  {
    CalCreator* ccptr;
  public:
    WebCalContext();
    ~WebCalContext();
  
    void setDateRange(Date d_first, Date d_last) {
      if (d_last < d_first)
	throw WebCalException("Error! Invalid range of dates '" +
			      to_string(d_first) + " .. " +
			      to_string(d_last) + "'.");
      this->d_first = d_first;
      this->d_last = d_last;
      regDate(d_first);
      regDate(d_last);
    }
    void addText(const std::string& tag, const std::string& text)
    {
      if (tag != "Head:" && tag != "BodyBefore:" && tag != "BodyAfter:")
	throw WebCalException("Error! Unknown text block tag '" + tag + "'.");
      std::ostringstream &out = textBlocks[tag];
      out << text << "\n";
    }

    std::string getText(const std::string& tag)
    {
      return textBlocks[tag].str();
    }
  
    void addVirtualDay(Date d, int dayOfWeek) // zero indexed!
    {
      virtualDays.push_back(std::make_pair(d,dayOfWeek));
    }
    void push_block(Block *ptr) { blocks.push_back(ptr); }

    void fill();
  
    void dump();
    std::string nextDefaultId()
    {
      std::string id = "top";
      id += nextDefaultLetter++;
      return id;
    }
    Date firstDate() { return d_first; }
    Date lastDate() { return d_last; }

    std::string getStylesheet() { return stylesheet; }
    void setStylesheet(const std::string& ss) { stylesheet = ss; }

    std::string getOutfile() { return outfile; }
    void setOutfile(const std::string& of) { outfile = of; }
  
    void writeCalendar(const std::string& fname);
  
  private:
    Date d_first, d_last;
    std::string stylesheet, outfile;
    std::map<std::string,std::ostringstream> textBlocks;
    std::vector<std::pair<Date,int> > virtualDays;
    std::vector<Block*> blocks;
    char nextDefaultLetter = 'A';
  };
}
#endif
