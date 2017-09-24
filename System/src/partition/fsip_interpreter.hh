/**
 * @file fsip_interpreter.hh
 * @author Aljoscha Narr (alnarr@mail.uni-mannheim.de)
 * @brief An interpreter class for the Free Space Indicator Page
 * @bugs no bugs known
 * @todos 
 */

#ifndef FSIP_INTERPRETER_HH
#define FSIP_INTERPRETER_HH

#include "infra/types.hh"

class FSIPageInterpreter{

public:
  struct header{
    uint16_t _noRecords;
    uint16_t _nextFreePlace;
    uint16_t _freeSpace;
  }
 struct slot{
    uint16_t _offset;

}
public:
  FSIPageInterpreter();
  ~FSIPageInterpreter();

public:
  uint initNewPage(byte* aPP); 
  inline void attach(byte* aPP);
  void detach();
  
public:

  inline size_t get_page_size() {return _pageSize;}
  inline const size_t get_page_size() {return _pageSize;} 	

  inline const header* getHeader() const{return _header;}
  inline header* get_header() {return header;}

  inline uint get_free_space() const{return header()->_freeSpace;}
  inline uint get_no_records() const{return header()->_freeSpace;}

private:
  inline header* get_header_pointer(){return ((header*) (_pagePointer+_pageSize-sizeOf(header)));}
 inline header* get_slot_pointer(){return ((slot*) (_pagePointer+_pageSize-sizeof(header)-sizeof(slot)));}


private:
  header* _header;
  byte* _pagePointer;
  static size_t _pageSize;
  slot* _slots;
}

#endif
  
