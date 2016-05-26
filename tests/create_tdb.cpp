#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <cstdint>

#include "TrailDBConstructor.h"
#include "TrailDBcpp.h"


int main() {

  std::vector<std::string> fields{"field1", "field2"};
  TrailDBConstructor tdbCon("tdbtest",fields);
  tdbCon.Add("12345678123456781234567812345678", 1230000, {"Chrome","USA"});
  tdbCon.Add("12345678123456781234567812345678", 1400000, {"MacOSX","BRA"});
  tdbCon.Add("12345678123456781234567812345678", 1600000, {"MacOSX","BRA"});
  tdbCon.Finalize();

  TrailDB tdb("tdbtest.tdb");

  std::uint32_t num_events = tdb.GetNumberOfEvents();

  for(std::uint32_t num = 0; num < num_events; ++num) {

    EventListPtr eventList = tdb.LoadEvents(num);
    EventList::EventIterator evtIter;

    std::uint32_t evtSize = eventList->GetSize();

    for (evtIter = eventList->EventsBegin();
        evtIter != eventList->EventsEnd();
        ++evtIter) {

      std::uint32_t ts = evtIter->GetTimestamp();

      std::cout << tdb.GetHexUUIDByInd(num) <<" "
           <<ts<<" "<<evtIter->GetTrail("field1")<<" "
           <<evtIter->GetTrail("field2")<<std::endl;
      }

    }

}
