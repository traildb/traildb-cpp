#include <iostream>
#include <vector>
#include <algorithm>
#include <string.h>
#include <stdint.h>

#include "TrailDBConstructor.h"
#include "TrailDB.h"

using namespace std;


int main() {

  vector<string> fields{"field1", "field2"};
  TrailDBConstructor tdbCon("tdbtest",fields);
  tdbCon.Add("12345678123456781234567812345678", 1230000, {"Chrome","USA"});
  tdbCon.Add("12345678123456781234567812345678", 1400000, {"MacOSX","BRA"});
  tdbCon.Add("12345678123456781234567812345678", 1600000, {"MacOSX","BRA"});
  tdbCon.Finalize();

  TrailDB tdb("tdbtest");

  uint32_t num_events = tdb.GetNumberOfEvents();

  for(uint32_t num = 0; num < num_events; ++num) {

    EventListPtr eventList = tdb.LoadEvents(num);
    EventList::EventIterator evtIter;

    uint32_t evtSize = eventList->GetSize();

    for (evtIter = eventList->EventsBegin();
        evtIter != eventList->EventsEnd();
        ++evtIter) {

      uint32_t ts = evtIter->GetTimestamp();

      cout << tdb.GetHexUUIDByInd(num) <<" "
           <<ts<<" "<<evtIter->GetTrail("field1")<<" "
           <<evtIter->GetTrail("field2")<<endl;
      }

    }

}
