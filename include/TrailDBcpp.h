#ifndef CPP_TRAILDB_H_INCLUDED
#define CPP_TRAILDB_H_INCLUDED


#include <iostream>
#include <iomanip>
#include <vector>
#include <map>
#include <exception>
#include <algorithm>
#include <functional>
#include <sstream>
#include <string.h>
#include <stdint.h>
#include <fstream>

#include "TrailDBException.h"
#include "ByteManipulations.h"
#include "PointerTypedefs.h"

extern "C" {
#include <traildb.h>
}

static const uint32_t BUFFER_SIZE = 1000000;

using namespace std;


class Event {

  public:

    Event(uint32_t timestamp=0xFFFFFFFFULL):
      timestamp_(timestamp) { }

    ~Event() { }

    uint32_t GetTimestamp() const {return timestamp_;}
    void SetTimestamp(const uint32_t timestamp) {timestamp_ = timestamp;}

    void ResetTrail(std::string item) {
      trail_[item] = "";
    }

    void AddTrail(std::string item, std::string value) {
      if (trail_.find(item) == trail_.end()) {
        ResetTrail(item);
      }
      trail_[item] = value;
    }

    std::string GetTrail(std::string item) {
      if (trail_.find(item) == trail_.end()) {
        std::cerr <<"Requested item "<< item<<" not found"<<std::endl;
        for(std::map<std::string,std::string>::iterator it = trail_.begin();
            it != trail_.end(); ++it){
          std::cerr <<it->first<<"  "<<it->second<<std::endl;
        }
        ResetTrail(item);
      }
      return trail_[item];
    }

    bool IsEventType(std::string type){
      return GetTrail("type") == type;
    }

    std::string GetEventType() {
      return GetTrail("type");
    }

    // Define time sorting of events
    bool operator<(const Event& e) const {return timestamp_ < e.timestamp_;}
    bool operator>(const Event& e) const {return timestamp_ > e.timestamp_;}

  private:
    uint32_t timestamp_;
    std::map<std::string, std::string> trail_;

};

SHARED_POINTER_TYPEDEFS(Event);

class EventTypeMatch : public std::binary_function<Event, std::string, bool> {
  public:
    bool operator()(Event& e, const std::string type) const {
      return e.GetTrail("type") == type;
    }
};

class EventList {

  public:

    EventList() { }

    EventList(int nReserve) {
      events_.reserve(nReserve);
    }

    ~EventList() { }

    void AddEvent(const Event& e) {events_.push_back(e);}

    uint32_t GetSize() const {return (uint32_t)events_.size();}

    void Clear() {events_.clear();}

    typedef std::vector<Event>::iterator EventIterator;
    typedef std::vector<Event>::reverse_iterator EventRIterator;
    typedef std::vector<Event>::const_iterator ConstEventIterator;

    EventIterator EventsBegin() {return events_.begin();}
    EventIterator EventsEnd() {return events_.end();}

    EventRIterator EventsRBegin() {return events_.rbegin();}
    EventRIterator EventsREnd() {return events_.rend();}

    ConstEventIterator EventsBegin() const {return events_.begin();}
    ConstEventIterator EventsEnd() const {return events_.end();}

    bool HasEventType(std::string type) {
      return find_if(EventsBegin(), EventsEnd(),
          bind2nd(EventTypeMatch(), type)) != EventsEnd();
    }


    uint32_t GetCountEventType(std::string type) {
      uint32_t count = (uint32_t)count_if(EventsBegin(), EventsEnd(),
          bind2nd(EventTypeMatch(),type));
      return count;
    }

  private:

    std::vector<Event> events_;
};

SHARED_POINTER_TYPEDEFS(EventList);

class TrailDB;
SHARED_POINTER_TYPEDEFS(TrailDB);


class TrailDB {

  public:

    TrailDB(std::string dbpath);

    virtual ~TrailDB();

    void LoadFieldNames();

    uint64_t GetNumberOfUUIDs() const { return numUUIDs_; }

    uint64_t GetNumberOfEvents() const { return numEvents_; }

    uint32_t GetMaxTimestamp() const { return maxTimestamp_; }

    uint32_t GetMinTimestamp() const { return minTimestamp_; }

    uint32_t GetNumberOfFields() const { return numFields_; }

    tdb* GetDB() const {return db_;}

    std::vector<std::string> GetDimNames() const {
      return fieldNames_;
    }

    void GetUUIDByIdx(uint64_t ind, std::vector<uint8_t> &res);

    uint64_t GetUUIDIdx(const uint8_t * uuid_id);

    const std::string GetHexUUIDByInd(uint64_t ind);

    uint32_t GetLexiconSize(std::string field);

    uint32_t GetFieldSize(std::string field);

    bool GetFieldIdx(std::string field, uint32_t &id);

    std::string GetFieldName(uint32_t fieldIdx);

    EventListPtr LoadEvents(uint64_t i);

    uint32_t GetNumberOfEvents(uint64_t uuid_index);

    std::vector<uint32_t> GetTimestampVector(uint64_t uuid_index);


  //private:

    std::string dbPath_;
    tdb* db_;
    uint64_t numUUIDs_;
    uint64_t numEvents_;
    uint32_t numFields_;
    uint32_t minTimestamp_;
    uint32_t maxTimestamp_;
    uint32_t eventTypeInd_;

    std::vector<std::string> fieldNames_;

    uint32_t buff_[BUFFER_SIZE];
    char res_[BUFFER_SIZE];

    static const unsigned TDB_KEY_SIZE_BYTES = 16;
};


TrailDB::TrailDB(std::string dbpath):
  dbPath_(dbpath){

    db_ = tdb_init();
    int ret = tdb_open(db_, dbPath_.c_str());
    numUUIDs_ = tdb_num_trails(db_);
    numFields_ = tdb_num_fields(db_);
    numEvents_ = tdb_num_events(db_);
    minTimestamp_ = tdb_min_timestamp(db_);
    maxTimestamp_ = tdb_max_timestamp(db_);

    LoadFieldNames();

  };

TrailDB::~TrailDB() {
  tdb_close(db_);
};

//Return vector of 16 bytes for uuid id
void TrailDB::GetUUIDByIdx(uint64_t ind, std::vector<uint8_t> &res) {
  const uint8_t *p_uuid = (uint8_t *)tdb_get_uuid(db_, ind);
  res.clear();
  res.resize(0);
  if (p_uuid != NULL) {
    for (int i = 0; i < TDB_KEY_SIZE_BYTES; ++i) {
      uint8_t uuid_byte;
      p_uuid = ParseValue(&(*p_uuid), uuid_byte, 1);
      res.push_back(uuid_byte);
    }
  }
};



//Return the Hex representation of the UUID in a string
const std::string TrailDB:: GetHexUUIDByInd(uint64_t ind) {
  std::vector<uint8_t> res;
  GetUUIDByIdx(ind,res);
  std::stringstream ss;
  if (res.size() == TDB_KEY_SIZE_BYTES) {
    for (auto &i : res) {
      ss << std::hex << std::setfill('0') << std::setw(2) << (uint)i;
    }
  }
  const std::string s = ss.str();
  return s;
};

//Given uuid key (16 bytes) return corresponding index
uint64_t TrailDB::GetUUIDIdx(const uint8_t *uuid) {
  uint64_t uuid_id;
  tdb_get_trail_id(db_, uuid, &uuid_id);
  if (uuid_id < numUUIDs_) {
    return uuid_id;
  }
  else { 
    throw TrailDBException();
  }
};

//Given an index, return the number of events
uint32_t TrailDB::GetNumberOfEvents(uint64_t uuid_idx) {
  tdb_cursor* cursor = tdb_cursor_new(db_);

  if (tdb_get_trail(cursor, uuid_idx)) 
    throw TrailDBException();
  uint32_t num_events = tdb_get_trail_length(cursor);
  tdb_cursor_free(cursor);
  return num_events;
};


uint32_t TrailDB::GetFieldSize(std::string field) {
  uint32_t res = 0;
  uint32_t pos;
  if(GetFieldIdx(field, pos)) {
    res = tdb_lexicon_size(db_, (uint8_t)pos) - 1;
  }
  return res;
};


std::string TrailDB::GetFieldName(uint32_t fieldIdx) {
  std::string res(tdb_get_field_name(db_, fieldIdx));
  return res;
}


//Given a cookie, load all corresponding events
EventListPtr TrailDB::LoadEvents(uint64_t uuid_idx) {

  tdb_cursor* cursor = tdb_cursor_new(db_);

  //TODO: Need better error handling
  //if (tdb_get_trail(cursor, uuid_idx)) 
  //  throw TrailDBException();

  tdb_get_trail(cursor, uuid_idx);
  EventListPtr eventList = boost::make_shared<EventList>();

  tdb_event* tdbevent;
  while((tdbevent = (tdb_event*)tdb_cursor_next(cursor)) != NULL) {
    Event e;
    e.SetTimestamp(tdbevent->timestamp);

    uint64_t num_items = tdbevent->num_items;
    size_t count = 0;
    while (count < num_items) {

      uint64_t vlength;
      auto val = tdb_get_item_value(db_, tdbevent->items[count], &vlength);
      auto fieldname = GetFieldName(tdb_item_field(tdbevent->items[count]));

      std::string value(val, vlength);
      e.AddTrail(fieldname, value);

      count++;
    }

    eventList->AddEvent(e);
  }

  tdb_cursor_free(cursor);

  return eventList;

};


std::vector<uint32_t> TrailDB::GetTimestampVector(uint64_t uuid_idx) {

  tdb_cursor* cursor = tdb_cursor_new(db_);

  if (tdb_get_trail(cursor, uuid_idx)) 
    throw TrailDBException();

  std::vector<uint32_t> vts;

  tdb_event* tdbevent;
  while((tdbevent = (tdb_event*)tdb_cursor_next(cursor)) != NULL) {
    vts.push_back(tdbevent->timestamp);
  }

  tdb_cursor_free(cursor);

  return vts;
}


uint32_t TrailDB::GetLexiconSize(std::string field) {
  uint32_t field_id;
  if (GetFieldIdx(field, field_id)) {
    uint32_t lex_size = tdb_lexicon_size(db_, field_id);
    return lex_size;
  }
  else {
    throw TrailDBException();
  }
}

//Given field (a string), return corresponding idx
bool TrailDB::GetFieldIdx(std::string field, uint32_t &id) {
  auto it = std::find(fieldNames_.begin(), fieldNames_.end(), field);
  if (it != fieldNames_.end()) {
    auto f_pos = std::distance(fieldNames_.begin(), it);
    id = (uint32_t)f_pos; 
    return true;
  }
  return false;
};


void TrailDB::LoadFieldNames() {
  for (size_t i = 0; i < numFields_; ++i) {
    fieldNames_.push_back(tdb_get_field_name(db_,i));
  }
};


#endif  //CPP_TRAILDB_H_INCLUDED
