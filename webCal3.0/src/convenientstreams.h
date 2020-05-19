/***************************************************************
 * Conveneient stream 1.1
 *
 * This file defines two very convenient stream classes.  They
 * are wrappers for other input streams.  For example, if you
 * define  "cacIstream in(cin);" and use "in" instead of "cin"
 * for the rest of your program, your program will run just as
 * usual, except that #'s will be treated as defining comments
 * (in the usual way), and \\n will be treated as line 
 * continuation (in the usual way). NOTE: You can choose a
 * different comment character if you like. Also, \# gives you
 * a literal "#".
 *
 *
 * cacIstream  - Comments and continuations istream,
 *               i.e. # & \\n mean what you think
 *
 * slwcistream - Single-line buffered, respecting #'s for 
 *               comments and \\n for continuations
 * Chris Brown, 11 April, 2007
 ***************************************************************/
#ifndef _CONVENIENT_STREAMS_
#define _CONVENIENT_STREAMS_
#include <string.h>
#include <string>
#include <sstream>
using namespace std;

/***************************************************************
 * slwcistream - Single-line buffered, respecting #'s for
 * comments and \\n for continuations
 ***************************************************************/
class slwcistream : public istringstream
{
public:
  enum option {none = 0, skipleadingws = 1};
  slwcistream(istream& in, option opt = none, char commentChar='#')
  {
    string s = "";
    char c = in.get(); 
    if (opt == skipleadingws) 
      while(c != EOF && (isspace(c) || c == '\\' && isspace(in.peek()))) c = in.get();
    // States  : 0 = normal, 1 = in comment, 2 = just read a backslash
    int state = 0;
    do {
      if (c == EOF) break;
      if (state == 2 && c == '\n') { state = 0; continue; }
      if (c == '\n') break;
      if (state == 1 && c == '\\' && in.peek() == '\n') { in.get(); state = 0; continue; }
      if (state == 1) continue;
      if (state == 0 && c == commentChar) { state = 1; continue; }
      if (state == 0 && c == '\\') { state = 2; continue; }
      if (state == 2) { s += '\\'; }
      s += c;
      state = 0;
    }while(c = in.get());
    str(s);
  }
};


/***************************************************************
 * cacIstream - Comments and continuations istream, 
 * i.e. # & \\n mean what you think
 ***************************************************************/
static const int buffSize = 10;
static const int extra = 4;

class cacInBuff : public streambuf
{
protected:
  char buff[buffSize];
  char commentChar;
  istream *trueIn;
  int newlinesSeen, lastchar;
public:
  cacInBuff(istream &_in, char _cc) 
  { trueIn = &_in; commentChar = _cc; char *tmp = buff+extra; setg(tmp,tmp,tmp); newlinesSeen = lastchar = 0; }
  inline virtual int_type underflow();
  inline int getCurrentLine() { return newlinesSeen + (lastchar != '\n'); }
};

class cacIstream : public istream
{
protected:
  cacInBuff buff;
public:
  cacIstream(istream &_in, char _commentChar='#') : buff(_in,_commentChar), istream(&buff) { }
  inline int getCurrentLine() { return buff.getCurrentLine(); }
};

/***************************************************************
 * Implementation of realineInBuff member functions
 ***************************************************************/
cacInBuff::int_type cacInBuff::underflow()
{
  if (gptr() >= egptr())
  {
    int leftover = min(extra, int(gptr() - eback()));
    memmove(buff + (extra-leftover), gptr() - leftover, leftover);

    char *s = buff + extra;     // the string I'm reading in
    int ls = 0;                 // the actual size of s

    // States  : 0 = normal, 1 = in comment
    istream & in = *trueIn;
    int state = 0;
    char c;
    while(ls < buffSize - extra && (c = in.get()))
    {
      lastchar = c;
      if (c == EOF) break;
      else if (c == '\\' && in.peek() == '\n') { in.get(); newlinesSeen++; lastchar = '\n'; state = 0; continue; }
      else if (state == 1 && c != '\n') { continue; }
      else if (state == 0 && c == commentChar) { state = 1; continue; }
      else if (state == 0 && c == '\\' && in.peek() =='#')  { c = in.get(); lastchar = '#'; }
      if (c == '\n') { newlinesSeen++; }
      s[ls++] = c;
      state = 0;
    }
    if (ls == 0) return EOF;
    setg(buff + (extra-leftover), buff + extra, buff + extra + ls);
  }
  return *gptr();
}
#endif
