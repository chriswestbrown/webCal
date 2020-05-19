#ifndef _WEBCAL_CALCREATOR_H_
#define _WEBCAL_CALCREATOR_H_
#include "context.h"
#include "calcell.h"

namespace webcal
{

  class CalCreator : public CellBank
{
 private:
  WebCalContext* dcptr;
  std::vector<CalCell> cells;
 public:
  CalCreator(WebCalContext& C) : dcptr(&C) { }
  void setup();
  void addToMask(Block* bptr);
  void dump(std::ostream& out);
  void generateHtml(std::ostream& out);
  CalCell& get(Date d);
  std::vector<CalCell>::iterator begin() { return cells.begin(); }
  std::vector<CalCell>::iterator end() { return cells.end(); }
 private:
  void generateHTMLMonth(Date fom);
  void generateHTMLMonth(std::ostream &fout, Date fom);
};

}

#endif
