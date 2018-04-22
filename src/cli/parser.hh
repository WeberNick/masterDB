/**
 * @file   parser.hh
 * @author Nicolas Wipfler (nwipfler@mail.uni-mannheim.de)
 * @brief  Class implementing the parser for the command line interface
 * @bugs   TBD
 * @todos  TBD
 */
/*
 * table segment
 * 
 * segment manager hh, für insert tuples getSegment().insertTuples() (direkt auf segment)
 * partition manager hh
 * 
 create partition mit pfad, name, growth indicator (int)
 delete/remove partition mit name

 create segment mit name für segment, name von partitionname; 
 delete/remove segment mit name

 q/quit()

 insert tuples und weitere operationen auf segmenten
 pfad validieren

 locks, while true eigener thread
 */

#pragma once

#include "../infra/types.hh"

class CommandParser {
  protected:
    explicit CommandParser() = delete;
    explicit CommandParser(CB& aCB);

  public:

  private:
    CB* _cb;
}