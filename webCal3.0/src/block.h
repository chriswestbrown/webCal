#ifndef _WEBCAL_BLOCK_H_
#define _WEBCAL_BLOCK_H_

#include "parse.h"
#include "date.h"
#include "context.h"
#include "calcreator.h"
#include "ctype.h"

namespace webcal
{

  class Options : public Block
  {
  public:
    Options(const std::string& bname, WebCalContext &dc) : Block(bname,&dc) { }
    virtual bool parseLine(const std::string& line)
    {
      std::istringstream sin(line);
      std::string sw;
      sin >> sw;
      if (sw == "day-first-dates")
	dcptr->setMonthFirstInput(false);
      else if (sw == "stylesheet")
      {
	std::string ss;
	sin >> ss;
	dcptr->setStylesheet(ss);
      }
      else if (sw == "outfile")
      {
	std::string ss;
	sin >> ss;
	dcptr->setOutfile(ss);
      }
      else
	throw WebCalException("Error!  Switch \"" + line + "\" not understood.");
      switches.push_back(sw);
      return true;
    }
    virtual Block* clone() { return new Options(*this); }
    virtual void dump(std::ostream& out)
    {
      out << getTag() << std::endl;
      for(auto p : switches)
	out << "  " << p << std::endl;
      out << getEndTag() << std::endl;	
    }
  private:
    std::vector<std::string> switches;
  };


  class Holidays : public Block
  {  
  public:
    Holidays(const std::string& bname, WebCalContext &dc) : Block(bname,&dc) { }
    virtual void process(const std::string& msg, Date d)
    {
      dcptr->regDate(d);
      datemsg.push_back( make_pair(d,msg) );
    }
    virtual bool parseLine(const std::string& line)
    {
      std::string msg, date;
      if (!parseMsgDate(line,msg,date))
	throw WebCalException("Error!  Could not parse line.");
      Date d = dcptr->parseDate(date);
      process(msg,d);
      return true;
    }
    int getPriorityLevel() { return 2; }
    virtual Block* clone() { return new Holidays(*this); }
    virtual void dump(std::ostream& out)
    {
      out << getTag() << std::endl;
      for(auto p : datemsg)
	out << "  " << p.second << ' ' << to_string(p.first) << std::endl;
      out << getEndTag() << std::endl;	
    }
    virtual void fill(CellBank* cbptr)
    {
      for(auto p : datemsg)
	cbptr->get(p.first).addMsg(p.second);
    }
    virtual void addToMask(CalCell& c)
    {
      if (getTag() == "Holidays(blocking):")
      {
	for(auto& p : datemsg)
	  if (c.getDate() == p.first)	  
	    c.addToMask("blocking",getPriorityLevel());
      }
    }
  private:
    std::vector< std::pair<Date,std::string> > datemsg;
  };

  class TextBlock : public Block
  {
  public:
    TextBlock(const std::string& bname, WebCalContext &dc) : Block(bname,&dc) { }
    virtual bool parseLine(const std::string& line)
    {
      dcptr->addText(getTag(),line);
      (sout += line) += "\n";
      return true;
    }
    virtual Block* clone() { return new TextBlock(*this); }
    virtual void dump(std::ostream& out)
    {
      out << getTag() << std::endl;
      out << sout;
      out << getEndTag() << std::endl;	
    }
  private:
    std::string sout;
  };

  class DateRange : public Block
  {
  private:
    Date d_first, d_second;
    int count;
  public:
    DateRange(const std::string& bname, WebCalContext &dc) : Block(bname,&dc) { count = 0; }
    virtual bool parseLine(const std::string& line)
    {
      Date d = dcptr->parseDate(line);
      ++count;
      if (count == 1)
	d_first = d;
      else if (count == 2)
	dcptr->setDateRange(d_first,(d_second = d));
      else
	throw WebCalException("Error! DateRange cannot contain more than two dates!");
      return true;
    }
    virtual void notifyFinish()
    {
      if (count != 2)
	throw WebCalException("Error! Date range requires two dates!");
    }
    virtual Block* clone() { return new DateRange(*this); }
    virtual void dump(std::ostream& out)
    {
      out << getTag() << std::endl;
      out << "  " << to_string(d_first) << '\n';
      out << "  " << to_string(d_second) << '\n';
      out << getEndTag() << std::endl;	      
    }
  };  

  class VirtualDays : public Block
  {
  private:
    std::vector< std::pair<Date,std::string> > vdays;
  public:
    VirtualDays(const std::string& bname, WebCalContext &dc) : Block(bname,&dc) { }
    virtual bool parseLine(const std::string& line)
    {
      std::istringstream sin(line);
      std::string name, tmp;
      Date d;
      if (sin >> name && (d = dcptr->parseDate(sin), !(sin >> tmp)))
	vdays.push_back(make_pair(d,name));
      else
	throw WebCalException("Error! Invalid Virtual_Days line.");
      return true;
    }
    virtual Block* clone() { return new VirtualDays(*this); }
    virtual void calMod(CalCreator* ccptr)
    {
      for(auto& p : vdays)
      {
	CalCell& cell = ccptr->get(p.first);
	cell.setEffectiveDayOfWeek(p.second);
      }
    }
    virtual void dump(std::ostream& out)      
    {
      out << getTag() << std::endl;
      for(auto& p : vdays)
	out << "  " << p.second << " " << to_string(p.first) << std::endl;
      out << getEndTag() << std::endl;	            
    }
  };

  class Topic
  {
  private:
    Topic() { }
  public:    
    static Topic parse(const std::string& line)
    {
      bool star = (line[0] == '*');
      std::string path;
      int i = star;
      if (line.length() < 2) { throw WebCalException("Error! Topic line without content."); }
      if (line[i] == '[')
      {
	int j = i + 1;
	while(j < line.length() && line[j] != ']')
	  j++;
	if (j >= line.length()) { throw WebCalException("Error! Invalid path expression in Topic line."); }
	path = line.substr(i+1, j-i-1);
	i = j+1;
      }
      while(i < line.length() && isspace(line[i]))
	i++;
      int j = line.length() - 1;
      while(j > i && isspace(line[j]))
	j--;
      if (j < i) { throw WebCalException("Error! Topic line without content."); }
      Topic T;
      T.msg = line.substr(i,j-i+1);
      T.numbered = !star;
      T.id = ""; // TODO: Need to add id's for Topic objects later!
      T.path = path;
      return T;
    }
    void dump(std::ostream& out)
    {
      if (!numbered) { out << '*'; }
      if (path != "") { out << '[' << path << ']'; }
      out << ' ' << msg << std::endl;
    }

    std::string fillLinkTemplate(const std::string& lt, int count)
    {
      std::ostringstream sout;
      int state = 0, dollarcount = 0;
      // account for ""s on outside
      for(int s = 1, f = lt.size() - 2; s <= f; s++)
      {
	char next = lt[s];
	switch(state)
	{
	case 2:
	  if (next == '$') { dollarcount++; break; }
	  {
	    state = 0;
	    std::string n = std::to_string(count), pad;
	    while(dollarcount > n.length() + pad.length())
	      pad += "0";
	    sout << pad << n;
	    dollarcount = 0;
	  }
	  // falling into case 0 on purpose!
	case 0:
	  if (next == '\\') { state = 1; break; }
	  if (next == '$') { state = 2; dollarcount = 1; break; }
	  sout << next;
	  break;
	case 1:
	  state = 0;
	  sout << next;
	  break;	  
	}	
      }
      return sout.str();
    }    
    
    std::string getMsg(std::string& linkTemplate, int& count)
    {
      std::string res;
      if (numbered)
      {
	res += fillLinkTemplate(linkTemplate,count);
	res += " ";
	++count;
      }
      res += msg;
      return res;
    }
    
  private:
    std::string msg;
    bool numbered;
    std::string id;
    std::string path;
  };
  
  class Topics : public Block
  {
  public:
    Topics(const std::string& bname, WebCalContext &dc) : Block(bname,&dc) { numbered = false; }
    virtual bool parseLine(const std::string& line)
    {
      topicList.push_back(Topic::parse(line));
      return true;
    }
    virtual Block* clone() { return new Topics(*this); }
    virtual void processRest(std::string& rest)
    {
      Tokenizer tok(rest);
      for(auto next = tok.nextToken(); next.first != 0; next = tok.nextToken())
      {
	if (next.first == 4)
	  linkTemplate = next.second;
	else if (next.second == "Numbered_Topics")
	  numbered = true;
	else if (isPat(next.second))
	  pat = next.second;
	else
	  id = next.second;
      }
      if (pat == "") throw WebCalException("Error! Topics require a pattern of days.");
      if (id == "")
	id = dcptr->nextDefaultId();
    }
    int getPriorityLevel() { return 1; }
    virtual void addToMask(CalCell& cell)
    {
      char dow = cell.getEffectiveDayOfWeek();
      for(int i = 0; i < pat.length(); i++)
	if (dow == pat[i])
	  cell.addToMask(id,getPriorityLevel());
    }
    virtual void dump(std::ostream& out)
    {
      out << getTag()
	  << (id == "" ? " " : " " + id + " ")
	  << (numbered ? "Numbered_Topics " : "")
	  << (linkTemplate == "" ? "" : linkTemplate + " ")
	  << pat
	  << std::endl;
      for(auto& T : topicList)
	T.dump(out);	  
      out << getEndTag() << std::endl;	      
    }
    virtual void fill(CellBank* cbptr)
    {
      int i = 0, nextCount = 1;
      for(auto itr = cbptr->begin(); itr != cbptr->end() && i < topicList.size(); ++itr)
      {
	bool added = false;
	do {
	  int newCount = nextCount;
	  std::string msg = topicList[i].getMsg(linkTemplate,newCount);
	  added = itr->addTopicLine(id,msg);
	  if (added) { i++; nextCount = newCount; }
	} while(added && i < topicList.size());
      }
    }
  private:
    bool isPat(const std::string& s) {
      int i = 0;
      while(i < s.length() && (s[i] == 'U' || s[i] == 'M' || s[i] == 'T' ||
			       s[i] == 'W' || s[i] == 'R' || s[i] == 'F' || s[i] == 'S'))
	i++;
      return i == s.length();
    }
    bool numbered = false;
    std::string linkTemplate;
    std::string pat;
    std::string id;
    std::vector<Topic> topicList;
  };

  /*
   * A new tag, Modifications:, that allows you to specify a behavior for a date
   * that takes precedence over the usual patterns.  This requires that the Topics
   * involved have been given ids.  For example, suppose you have Topics with id's
   * lec and lab, and suppose 2/22/2020 is normally a lab day.  You could specify
   *    2/22/2020 lec lec
   * to say that you wanted two lectures on that day rather than a lab, or
   *    2/22/2020 lec lab
   * to say that you wanted a lecture then a lab. 
   */
  class Mod
  {
  public:
    virtual bool parseLine(const std::string& line, DateContext* dcptr)
    {
      std::istringstream sin(line);
      d = dcptr->parseDate(sin);
      std::string str;
      while(sin >> str)
	ids.push_back(str);
      return true;
    }
    Date getDate() { return d; }
    int getNumIds() { return ids.size(); }
    const std::string& getId(int k) { return ids[k]; }
    void addToMask(CalCreator* ccptr, int priorityLevel)
    {
      CalCell& cell = ccptr->get(this->getDate());     
      for(auto& str : ids)
	cell.addToMask(str,priorityLevel);
    }    
  private:
    Date d;
    std::vector<std::string> ids;
  };
  
  class Modifications : public Block
  {
  public:
    Modifications(const std::string& bname, WebCalContext &dc) : Block(bname,&dc) { }
    virtual bool parseLine(const std::string& line)
    {
      mods.push_back(Mod());
      mods.back().parseLine(line,dcptr);
    }
    virtual Block* clone() { return new Modifications(*this); }
    int getPriorityLevel() { return 2; }
    virtual void calMod(CalCreator* ccptr)
    {
      for(Mod& m : mods)
	m.addToMask(ccptr,getPriorityLevel());
    }
    virtual void dump(std::ostream& out)
    {
      out << getTag() << '\n';
      for(Mod& m : mods)
      {
	out << "  " << to_string(m.getDate());
	for(int i = 0; i < m.getNumIds(); i++)
	  out << ' ' << m.getId(i);
	out << std::endl;
      }
      out << getEndTag() << std::endl;	      
    }
  private:
    std::vector<Mod> mods;
  };
  
}
#endif

