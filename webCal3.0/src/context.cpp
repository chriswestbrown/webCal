#include "context.h"
#include "calcreator.h"
#include <fstream>

namespace webcal
{

  
  WebCalContext::WebCalContext()
  {
    stylesheet = "calstyle.css";
    outfile = "calendar.html";
    ccptr = new CalCreator(*this);
  }
  
  WebCalContext::~WebCalContext()
  {
    for(auto p : blocks)
      delete p;
    blocks.clear();
    delete ccptr;
  }


  void WebCalContext::fill()
  {
    ccptr->setup();

    for(auto bptr : blocks)
      bptr->calMod(ccptr);

    for(auto bptr : blocks)
      ccptr->addToMask(bptr);

    // actually dole out topics and messages
    for(auto bptr : blocks)
    {
      bptr->fill(ccptr);
    }    
  }

  void WebCalContext::writeCalendar(const std::string& fname)
  {
    std::ofstream fout(fname);
    if (!fout)
      throw WebCalException("Error! Could not create file \"" + fname + "\".");
    ccptr->generateHtml(fout);
  }

  void WebCalContext::dump()
  {
    using namespace std;
    for(Block* bptr : blocks)
      bptr->dump(cout);
    cout << "# Dates from " << to_string(earliestDate())
	 << " to " << to_string(latestDate()) << endl;
    ccptr->dump(cout);
  }


}
