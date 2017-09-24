#include "fsip_interpreter.hh"

FSIPageInterpreter::FSIPageInterpreter() : _header(NULL),_pagePointer(NULL),slots(NULL);{}

uint
FSIPageInterpreter::initNewPage(const byte* aPagePointer){
  //......
}

void
FSIPageInterpreter::detach(){
  _pagePointer=0;
  _header=0;
  _slots=0;
}
