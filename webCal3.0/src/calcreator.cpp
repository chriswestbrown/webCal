#include "calcreator.h"

namespace webcal
{

  CalCell& CalCreator::get(Date d)
  {
    int i = daysInRange(dcptr->earliestDate(),d) - 1;
    if (i < 0 || i >= cells.size())
      throw WebCalException("Error! Date " + to_string(d) + " is out of range in CalCreator::get!");
    return cells[i];
  }
  void CalCreator::setup()
  {
    Date df = dcptr->earliestDate(), dl = dcptr->latestDate();
      int N = daysInRange(df,dl);
    cells.resize(N);
    Date d = df;
    for(int i = 0; i < N; i++)
      cells[i].setDate(d++);    
  }

  void CalCreator::addToMask(Block* bptr)
  {
    for(CalCell& c : cells)
      bptr->addToMask(c);    
  }
  
  
  void CalCreator::dump(std::ostream& out)
  {
    for(auto& cell : cells)
      cell.dump(out);
  }

  using namespace std;

  static std::string cellStart() {
    //return "<td width=\"14%\" class=\"regcell\">";
    return "<td class=\"regcell\">";
  }
  static std::string cellEnd() { return "</td>"; }
  static std::string rowStart() { return "<tr class=\"reg\">"; }

  void beginMonth(ostream &fout, Date fom)
  {
    //the biginning of each month/table*********************************
    fout << "<a id=\"" << fom.getMonthName() << fom.getYear() << "\"  style=\"page-break-before: always\" ></a>";
    fout << "<table class=\"caltable\" cellpadding=\"0\" cellspacing=\"0\">" << endl
	 << "<tr valign=\"middle\">" << endl
	 << "\t<td class=\"month\" colspan=\"7\" valign=\"middle\">" 
	 << fom.getMonthName() << " " << fom.getYear() << "</td>" << endl
	 << "</tr>" << endl;

    //print out the names of the days
    fout << "<tr valign=\"middle\">"
	 << "\t<td>Sunday</td>" << endl
	 << "\t<td>Monday</td>" << endl
	 << "\t<td>Tuesday</td>" << endl
	 << "\t<td>Wednesday</td>" << endl
	 << "\t<td>Thursday</td>" << endl
	 << "\t<td>Friday</td>" << endl
	 << "\t<td>Saturday</td>" << endl
	 << "</tr>" << endl;   
    /*
    fout << "<tr valign=\"middle\">"
	 << "\t<td width=\"14%\">Sunday</td>" << endl
	 << "\t<td width=\"14%\">Monday</td>" << endl
	 << "\t<td width=\"14%\">Tuesday</td>" << endl
	 << "\t<td width=\"14%\">Wednesday</td>" << endl
	 << "\t<td width=\"14%\">Thursday</td>" << endl
	 << "\t<td width=\"14%\">Friday</td>" << endl
	 << "\t<td width=\"14%\">Saturday</td>" << endl
	 << "</tr>" << endl;   
    */
  }

  

  
  void CalCreator::generateHTMLMonth(ostream &fout, Date fom)
  {
    Date lom = fom.forwardOneMonth().forwardDays(-1);
    
    beginMonth(fout,fom);

    //begin the week properly
    fout << rowStart() << endl;

    //print blank days in front
    for(int i = 0; i < fom.getDowIndex(); i++)
      fout << '\t' << cellStart() << "&nbsp;" << cellEnd() << endl;

    //print actual days
    for(Date d = fom; d <= lom; d++)
    {
      if (fom < d && d.getDowIndex() == 0)
	fout << "</tr>\n" << rowStart() << endl;
      fout << '\t' << cellStart() << endl
	   << "\t   <div class=\"calheader\">" << daysInRange(fom,d) << "</div>" << endl;
      if (d < dcptr->earliestDate() || dcptr->latestDate() < d)
	fout << "&nbsp;";
      else
	fout << this->get(d).generateHTMLContent();
      fout << cellEnd();
    }

    //print blank days in front
    for(int i = lom.getDowIndex()+1; i < 7; i++)
      fout << '\t' << cellStart() << "&nbsp;" << cellEnd() << endl;

    //end month
    fout << "</tr>\n</table>" << endl << endl;

  }


  void CalCreator::generateHtml(ostream &fout)
  {
    //print top of html file
    fout << "<!DOCTYPE html>"
	 << "<html>" << endl << "<head>" << endl
	 << "<meta http-equiv=\"content-type\" content=\"text/html; charset=UTF-8\">" << endl
	 << "<LINK REL=\"STYLESHEET\" HREF=\"" << (dcptr->getStylesheet()) << "\">" << endl;
    fout << dcptr->getText("Head:") << "</head>" << endl << "<body>" << endl << endl;    
    fout << dcptr->getText("BodyBefore:");

    int d1,m1,y1;
    Date first_day = dcptr->earliestDate();
    Date last_day = dcptr->latestDate();
    first_day.getDMY(d1,m1,y1);
    dcptr->earliestDate().getDMY(d1,m1,y1);
    for(Date firstOfMonth = Date(m1,1,y1); firstOfMonth < last_day; firstOfMonth = firstOfMonth.forwardOneMonth())
    {
      generateHTMLMonth(fout,firstOfMonth);
      fout << "<p></p>" << endl;
    }

    //print bottom of html file
    fout << dcptr->getText("BodyAfter:");
    fout << "</body></html>" << endl << endl;
  }




}
