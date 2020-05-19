#include <iostream>
#include <regex>
#include "convenientstreams.h"
#include "parse.h"
using namespace std;

namespace webcal
{

  /**
   * Splits input str into msg and date strings.
   * Returns false if format is bad.
   */
  bool parseMsgDate(const string & str, string &msg, string &date)
  {
    string date_regex = "^\\s*(.*\\S)\\s+((\\d+)/(\\d+)|(\\d+)/(\\d+)/(\\d+))\\s*$";
    regex reg(date_regex);
    smatch sm;
    regex_match(str,sm,reg);
    if (sm.size() == 0)
      return false;
    msg = sm[1].str();
    date = sm[2].str();
    return true;
  }

  /**
   * Returns true if a start tag is discovered.
   * Returns false if it gets to EOF without findind a start tag
   * Throws exception if something other than a valid start tag is found
   */
  bool readStartTag(istream &in, string &tag, string &rest, string &endtag)
  {
    regex blank("^\\s*$");
    regex tagline("^\\s*(\\S+[:])(\\s+(.*\\S))?\\s*$");
    string next;
    while(getline(in,next) && in)
    {
      if (regex_search(next,blank)) continue;
      smatch sm;
      regex_match(next,sm,tagline);
      if (sm.size() == 0)
	throw WebCalException("Error!  Expecting a start tag, but found \"" + next + "\"");
      tag = sm[1].str();
      rest = sm[3].str();
      endtag = ":end"+tag;
      return true;
    }
    return false;
  }

  /**
   * Returns the next non-empty input line, with whitespace trimmed.
   * Returns "" if the next line is the endtag.
   * Throws an error if EOF is encountered.
   */
  string nextDataLine(istream& in, const string &endtag)
  {
    string next;
    while(getline(in,next))
    {
      regex reg("^\\s*(\\S+)?\\s*$");
      smatch sm;
      regex_match(next,sm,reg);
      if (sm.size() == 0)
	return next;
      if (sm[1].length() == 0) continue;
      if (sm[1].str() == endtag)
	return "";
      return next;
    }
    throw exception();
  }

  void parse(istream& in)
  {
    cacIstream cacin(in);
    string tag, rest, endtag;
    while(readStartTag(cacin,tag,rest,endtag))
    {
      cout << "start tag [" << tag << "] rest [" << rest << "]" << endl;
      cerr << "endtag = '" << endtag << "'" << endl;
      for(string next = nextDataLine(cacin,endtag); next != ""; next = nextDataLine(cacin,endtag))
      {
	string msg, date;      
	if (parseMsgDate(next,msg,date))
	  cout << "[" << msg << "]" << " [" << date << "]" << endl;
	else
	  cout << "invalid format" << endl;      
      }
    }
  }

  /*
    Should skip whitespace, understand ""-delimited strings, 
    and understand backslash escapes of spaces and ""s inside of strings
    0 : end
    1 : string
    2 : other
   */#include <iostream>
#include <regex>
#include "convenientstreams.h"
#include "parse.h"
using namespace std;

namespace webcal
{

/**
 * Splits input str into msg and date strings.
 * Returns false if format is bad.
 */
bool parseMsgDate(const string & str, string &msg, string &date)
{
  string date_regex = "^\\s*(.*\\S)\\s+((\\d+)/(\\d+)|(\\d+)/(\\d+)/(\\d+))\\s*$";
  regex reg(date_regex);
  smatch sm;
  regex_match(str,sm,reg);
  if (sm.size() == 0)
    return false;
  msg = sm[1].str();
  date = sm[2].str();
  return true;
}

/**
 * Returns true if a start tag is discovered.
 * Returns false if it gets to EOF without findind a start tag
 * Throws exception if something other than a valid start tag is found
 */
bool readStartTag(istream &in, string &tag, string &rest, string &endtag)
{
  regex blank("^\\s*$");
  regex tagline("^\\s*(\\S+[:])(\\s+(.*\\S))?\\s*$");
  string next;
  while(getline(in,next) && in)
  {
    if (regex_search(next,blank)) continue;
    smatch sm;
    regex_match(next,sm,tagline);
    if (sm.size() == 0)
      throw exception();
    tag = sm[1].str();
    rest = sm[3].str();
    endtag = ":end"+tag;
    return true;
  }
  return false;
}

/**
 * Returns the next non-empty input line, with whitespace trimmed.
 * Returns "" if the next line is the endtag.
 * Throws an error if EOF is encountered.
 */
string nextDataLine(istream& in, const string &endtag)
{
  string next;
  while(getline(in,next))
  {
    regex reg("^\\s*(\\S+)?\\s*$");
    smatch sm;
    regex_match(next,sm,reg);
    if (sm.size() == 0)
      return next;
    if (sm[1].length() == 0) continue;
    if (sm[1].str() == endtag)
      return "";
    return next;
  }
  throw exception();
}

void parse(istream& in)
{
  cacIstream cacin(in);
  string tag, rest, endtag;
  while(readStartTag(cacin,tag,rest,endtag))
  {
    cout << "start tag [" << tag << "] rest [" << rest << "]" << endl;
    cerr << "endtag = '" << endtag << "'" << endl;
    for(string next = nextDataLine(cacin,endtag); next != ""; next = nextDataLine(cacin,endtag))
    {
      string msg, date;      
      if (parseMsgDate(next,msg,date))
	cout << "[" << msg << "]" << " [" << date << "]" << endl;
      else
	cout << "invalid format" << endl;
      
    }
  }
}

}


  std::pair<int,std::string> Tokenizer::nextToken()
  {
    int j = i-1, state = 0, done = 0;
    char nc = 42;
    while(!done && nc != -1)
    {
      ++j;
      nc = (j < line.length() ? line[j] : -1);
      switch (state)
      {
      case 0:
	if (nc == -1)
	  done = 1;
	else if (isspace(nc))
	  i = j+1;
	else
	  state = (nc != '"' ? 1 : 2);
	break;
      case 1:
	if (nc == -1 || isspace(nc)) {
	  --j;
	  done = 1; }
	break;
      case 2:
	if (nc == '"') { state = 4; done = 1; }
	else if (nc == '\\') { state = 3; }
	break;
      case 3:
	state = 2;
	break;	  
      }
    }
    if (state == 0) return make_pair(0,"");
    if (state == 1 || state == 4)
    {
      int iold = i;
      i = j + 1;
      return make_pair(state,line.substr(iold,j-iold+1));
    }      
    throw WebCalException("Error! Error parsing line: " + line);
  }  
}
