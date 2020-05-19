#include "parse.h"
#include "block.h"
#include <iostream>
#include <algorithm>
#include <map>
#include <vector>
#include <fstream>
#include "convenientstreams.h"
#include "context.h"
using namespace std;
using namespace webcal;

const char* version = "webCal version 3.0";
string processArguments(int argc, char** argv, WebCalContext& dc);

int main(int argc, char** argv)
{
  // SETUP
  WebCalContext dc;
  string fname = processArguments(argc,argv,dc);
  ifstream fin(fname);
  if (!fin) { cerr << "File \"" + fname + "\" could not be opened." << endl; exit(2); }
  cacIstream cacin(fin);
  vector<Block*> blocks = {
    new Options("Options:",dc),
    new Holidays("Holidays(blocking):",dc),
    new Holidays("Holidays(non-blocking):",dc),
    new TextBlock("Head:",dc),
    new TextBlock("BodyBefore:",dc),
    new TextBlock("BodyAfter:",dc),
    new DateRange("DateRange:",dc),
    new VirtualDays("Virtual_Days:",dc),
    new Topics("Topics:",dc),
    new Modifications("Modifications:",dc)
  };
  map<string,Block*> supported;
  for(auto p : blocks)
    supported[p->getTag()] = p;
  
  // PARSE
  try {
    string tag, rest, endtag;
    while(readStartTag(cacin,tag,rest,endtag))
    {
      auto itr = supported.find(tag);
      if (itr == supported.end())
	throw WebCalException("Error! tag '" + tag + "' not supported!");
      Block* bptr = itr->second->clone();
      dc.push_block(bptr);

      bptr->processRest(rest);
      string next;
      while((next = nextDataLine(cacin,endtag)) != "")
      {
	if (!bptr->parseLine(next))
	  throw WebCalException("Error! line could not be parsed!");
      }
      bptr->notifyFinish();
    }
  }
  catch(WebCalException& e)
  {
    cerr << e.what() << endl;
    cerr << "near line " << cacin.getCurrentLine() << endl;
  }
  fin.close();
  
  // CREATE CALENDAR
  dc.fill();
  dc.writeCalendar(dc.getOutfile());
  
  
  // SHUTDOWN
  for(auto p : blocks)
    delete p;
  return 0;
}

string processArguments(int argc, char** argv, WebCalContext& dc)
{
  string fname;
  for(int i = 1; i < argc; i++)
  {
    if (argv[i] == string("-v") || argv[i] == string("--version"))
    {
      cout << version << endl;
      exit(0);
    }
    else if (argv[i] == string("-h") || argv[i] == string("--help"))
    {
      cout << "usage: webCal <inputfile>" << endl;
      exit(0);
    }
    else if (fname == "")
    {
      fname = argv[i];
    }
    else
    {
      cerr << "Error!  Unknown argument \"" << argv[i]
	   << "\". Note: using \"" << fname << "\" as inpur file." << endl;
      exit(1);
    }      
  }
  if (fname == "") { cerr << "Error! No input file specified!" << endl; exit(3); }
  return fname;
}
