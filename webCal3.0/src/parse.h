#ifndef _WEBCAL_PARSE_H_
#define _WEBCAL_PARSE_H_

#include <iostream>
#include <string>
#include "date.h"
#include "calcell.h"

namespace webcal
{
  class WebCalContext;
  class CalCreator;

  class Block
  {
  protected:
    std::string tagname;
    WebCalContext* dcptr;
  public:
    Block(const std::string& tn, WebCalContext* dc) : tagname(tn), dcptr(dc) { }
    virtual ~Block() { }
    virtual const std::string& getTag() const { return tagname; }
    virtual const std::string getEndTag() const { return ":end" + tagname; }
    // return true if parse succssful, false otherwise
    virtual bool parseLine(const std::string& line) { return false; }
    virtual void notifyFinish() { }
    virtual Block* clone() = 0;
    virtual void processRest(std::string& rest) {
      if (rest != "")
	throw WebCalException("Error! '" + getTag() + "' blocks do not allow attributes!");
    }
    virtual void dump(std::ostream& out) = 0;
    virtual void addToMask(CalCell& cell) { }
    virtual void calMod(CalCreator* ccptr) { }
    virtual void fill(CellBank* cbptr) { }
  };

  void parse(std::istream& in);
  bool parseMsgDate(const std::string & str, std::string &msg, std::string &date);
  bool readStartTag(std::istream &in, std::string &tag, std::string &rest, std::string &endtag);
  std::string nextDataLine(std::istream& in, const std::string &endtag);

  class Tokenizer
  {
    std::string line;
    int i;
  public:
    Tokenizer(const std::string& str) : line(str) { i = 0; }
    std::pair<int,std::string> nextToken();
  };

}

#endif
