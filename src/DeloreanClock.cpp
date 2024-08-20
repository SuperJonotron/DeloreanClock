#include <DeloreanClock.h>

//Define the night mode start and end date
// time_t night_start = 
// time_t night_end = 

/**
* Default constructor for a DeloreanClock object                                             
*/
DeloreanClock::DeloreanClock(){
  preferences.begin(APP_NAME,false);
}

Mode DeloreanClock::getMode(){
  return mode;
}

void DeloreanClock::refresh(){
  updateAll(time_red,time_green,time_yellow);
}

void DeloreanClock::start(){  
    segCtrl.start();
    rtc.begin(DateTime(F(__DATE__), F(__TIME__)));
    DateTime now(2020,12,30,0,0,0);
    setGreenTime(rtc.now().unixtime());
    rtc.adjust(now.unixtime());
    //Past Time Travled to in B2F - 1955-11-05T6:00:000
    //time_yellow.setTime(6,0,0,5,11,1955);
    ntp.start();
    initPins();
 }

 void DeloreanClock::initPins(){
  pinMode(PIN_YELLOW_AM,OUTPUT);
  pinMode(PIN_YELLOW_PM,OUTPUT);
  pinMode(PIN_RED_AM,OUTPUT);
  pinMode(PIN_RED_PM,OUTPUT);
  pinMode(PIN_GREEN_AM,OUTPUT);
  pinMode(PIN_GREEN_PM,OUTPUT);
}

void DeloreanClock::setMode(Mode m){
  mode = m;
}

Mode DeloreanClock::strToMode(String m){
  m.trim();
  m.toLowerCase();
  if(m == "birthday"){
    return birthday;
  }
  else if(m == "anniversary"){
    return anniversary;
  }
  else if(m == "holiday"){
    return holiday;
  }
  else if(m == "event"){
    return event;
  }
  else if(m == "night"){
    return night;
  }
  else if(m == "demo"){
    return demo;
  }
  else{
    return unknown;
  }
}

void DeloreanClock::setTimezone(Timezone timezone){
  tz = timezone;
}

Timezone DeloreanClock::getTimezone(){
  return tz;
}

void DeloreanClock::setRedTime(time_t time_set){
  Serial.print("Setting Top Time: ");
  printTime(time_set,getTimezone());
  
  time_red = time_set;

  Serial.print("Getting Set Top Time: ");
  printTime(time_red,getTimezone());
}

void DeloreanClock::setGreenTime(time_t time_set){
  time_green = time_set;
}

void DeloreanClock::setYellowTime(time_t time_set){
  time_yellow = time_set;
}

bool DeloreanClock::syncRequired(){
  return rtc.now().unixtime()>nextSync.unixtime();
}

bool DeloreanClock::sync(){
  time_t utc = ntp.getTime();
  if(utc>0){
      rtc.adjust(DateTime(year(utc), month(utc), day(utc), hour(utc), minute(utc), second(utc)));
      DateTime future (rtc.now().unixtime() + 7 *  3600L);//7 Hours
      nextSync=future;
      return true;
   }
   return false;
}

void DeloreanClock::updateCurrentTime(){
  TimeChangeRule *tcr;
  time_t now = rtc.now().unixtime();
  Serial.print("Now Unix: ");
  Serial.println(now);

  now = tz.toLocal(now, &tcr);

  Serial.print("Now Local: ");
  printTime(now,getTimezone());
  segCtrl.updateGreen(now,true,true);
}

void DeloreanClock::updateCurrentTimeOnly(){
  TimeChangeRule *tcr;
  time_t now = rtc.now().unixtime();
  now = tz.toLocal(now, &tcr);
  segCtrl.updateGreen(now,false,true);
}

void DeloreanClock::updateAll(time_t red, time_t green, time_t yellow){
  segCtrl.updateAll(red,green,yellow);
}

time_t DeloreanClock::now(){
  return rtc.now().unixtime();
}

/**
* Calling update will process one of the following modes
* based on which mode is set. NTP syncing is always checked
* and performed if required before the current mode is
* processed.
*
* Default Mode: 
*  - Static Past and Present Dates
*  - Updates current time with ntp syncing
* 
* Birthday Mode:
*  - Show the last persons birthdate in the past and 
*  the next persons birthdate in the future 
* 
* Anniversary Mode:
*  - Show the last anniversary date in the past and 
*  the next anniversary date in the future
*
* Holiday Mode:  
*  - Show the last holiday date in the past and the 
*  next holiday date in the future
*
* Event Mode:
*  - Show the last event date in the past and the 
*  next event date in the future. Events are either a
*  birthday or anniversary
*
* Night Mode: 
*  - Show only the current time and turn off all 
*  other displays
*/
void DeloreanClock::update() {
  if (syncRequired()) {
    sync();
  }
  switch (mode) {
    case birthday:
      birthdayMode();
      break;
    case anniversary:
      anniversaryMode();
      break;
    case holiday://Holiday Mode
      //ToDo:
      break;
    case event://Event Mode
      eventMode();
      break;
    case night://Night Mode
      nightMode();
      break;
    case demo:
      demoMode();
      break;
    default:
      defaultMode();
  }
}

/*----------------------------------------------------------------------*
 * Update the clock event configuration                                 *
 * output                                                               *
 * Input - config - JSONObject that contains all the birthdays and      *
 *                  anniversary data as arrays                          *
 *----------------------------------------------------------------------*/
void DeloreanClock::updateConfig(String config){
  Serial.println("Updating Config...");
  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, config);
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    return;
  }
  Serial.print("Config size: ");
  Serial.println(doc.size());
  serializeJsonPretty(doc, Serial);

  for (JsonObject elem : doc["birthdays"].as<JsonArray>()) {
    const char* name = elem["name"];
    Serial.print("Adding Birthday: ");
    Serial.println(name);
    time_t timeUnix = elem["timeUnix"];
    birthdays.push_back(timeUnix);
  }

  for (JsonObject elem : doc["anniversaries"].as<JsonArray>()) {
    const char* name = elem["name"];
    Serial.print("Adding Anniversary: ");
    Serial.println(name);
    long timeUnix = elem["timeUnix"];
    anniversaries.push_back(timeUnix);
  }
  Serial.println("Config Updated");
}


/*----------------------------------------------------------------------*
 * Read the configuration file stored in a config file in SPIFFS and    *
 * return the contents of the file read as a String                     *                    *
 *----------------------------------------------------------------------*/
String DeloreanClock::readConfig(){
  return preferences.getString(K_CONFIG,"{}");
  // if (SPIFFS.exists("/config.json")){
  //   File f = SPIFFS.open("/config.json", "r");
  //   if (f && f.size()) {
  //       Serial.println("Reading config file");
  //       String config;
  //       while (f.available()){
  //         config += char(f.read());
  //       }
  //       f.close();
  //       Serial.println("=====================================");
  //       Serial.println(config);
  //       Serial.println("=====================================");
  //       return config;
  //   }
  // }
  // else {
  //   Serial.println("No config.json file found");
  //   return "{}";
  // }
}

void DeloreanClock::anniversaryMode() {
  Serial.println("Anniversary Mode:");
  time_t last = getLastDate(now(),getTimezone(),anniversaries, 0);
  time_t next = getNextDate(now(),getTimezone(),anniversaries, 0);
  Serial.print("Last Anniversary:");
  printTime(last,getTimezone());
  Serial.print("Next Anniversary:");
  printTime(next,getTimezone());
  updateCurrentTime();
  segCtrl.updateRed(next);
  segCtrl.updateYellow(last);
  setRedTime(next);
  setYellowTime(last);
}


void DeloreanClock::birthdayMode() {
  // Serial.println("\nBirthday Mode:");
  time_t last = getLastDate(now(),getTimezone(),birthdays, 0);
  time_t next = getNextDate(now(),getTimezone(),birthdays, 0);
  // Serial.print("Last Birthday: ");
  // printTime(last,getTimezone());
  // Serial.print("Next Birthday: " );
  // printTime(next,getTimezone());
  updateCurrentTime();
  segCtrl.updateRed(next);
  segCtrl.updateYellow(last);
  setRedTime(next);
  setYellowTime(last);
  //Update the LED's
  setLEDs(false,false,red);
  setLEDs(false,false,yellow);

  TimeChangeRule *tcr;
  time_t now = rtc.now().unixtime();
  now = tz.toLocal(now, &tcr);

  setLEDs(hour(now)<12,hour(now)>12,green);

}

void DeloreanClock::eventMode(){
  Serial.println("\nEvent Mode:");
  //Resolve birthday data
  time_t lastBirthday = getLastDate(now(),getTimezone(),birthdays, 0);
  time_t nextBirthday = getNextDate(now(),getTimezone(),birthdays, 0);
  //Resolve anniversary data
  time_t lastAnniversary = getLastDate(now(),getTimezone(),anniversaries, 0);
  time_t nextAnniversary = getNextDate(now(),getTimezone(),anniversaries, 0);
  //Combine resolved event dates into array for sorting
  std::vector<time_t> lastEvents = {lastBirthday, lastAnniversary};
  std::vector<time_t> nextEvents = {nextBirthday, nextAnniversary};
  //Resolve final events
  time_t last = getLastDate(now(),getTimezone(),lastEvents,0);
  time_t next = getNextDate(now(),getTimezone(),nextEvents,0);
  Serial.print("Last Event: ");
  printTime(last,getTimezone());
  Serial.print("Next Event: " );
  printTime(next,getTimezone());
  updateCurrentTime();
  Serial.println("Cur Updated");
  segCtrl.updateRed(next);
  Serial.println("Red Updated");
  segCtrl.updateYellow(last);
  Serial.println("Yellow Updated");
  //setRedTime(next);
  //setYellowTime(last);
}


void DeloreanClock::defaultMode() {
  //Future Time Travled to in B2F2 - 2015-10-21T16:29:00
  segCtrl.updateRed(2015, 10, 21, 4, 29);
  updateCurrentTime();
  //Past Time Travled to in B2F - 1955-11-05T6:00:00
  segCtrl.updateYellow(1955, 11, 05, 6, 00);
}

void DeloreanClock::nightMode() {
  segCtrl.clearRed(true, true, true);
  segCtrl.clearYellow(true, true, true);
  segCtrl.clearGreen(true, true, false);
  updateCurrentTimeOnly();
}

void DeloreanClock::demoMode() {
  Serial.print("Current Time: ");
  printTime(now(),getTimezone());

  //Birthday Mode
  birthdayMode();
  delay(5000);

  //Anniversary Mode
  anniversaryMode();
  delay(5000);

  //Event Mode
  eventMode();
  delay(5000);

  //Night Mode
  nightMode();
  delay(5000);

  //Default Mode:
  defaultMode();
  delay(5000);
}

String DeloreanClock::getClockState() {
  Serial.println("\n\nGetting Clock State...");
  // TimeChangeRule *tcr;
  // time_t cur = tz.toLocal(now(), &tcr);
 //  // Allocate the JSON document
 //  //
 //  // Inside the brackets, 200 is the RAM allocated to this document.
 //  // Don't forget to change this value to match your requirement.
 //  // Use arduinojson.org/v6/assistant to compute the capacity.
 //  const size_t capacity = JSON_OBJECT_SIZE(2) + 2*JSON_OBJECT_SIZE(5);
 //  DynamicJsonDocument doc(capacity);

 //  JsonObject curTime = doc.createNestedObject("curTime");
 //  curTime["month"] = month(cur);
 //  curTime["day"] = day(cur);
 //  curTime["year"] = year(cur);
 //  curTime["hour"] = hour(cur);
 //  curTime["min"] = minute(cur);
 //  String output;
 //  serializeJson(doc,output);
 //  return String(output);
  // JsonObject top = getTopTime();
  // JsonObject middle = getMiddleTime();

  JsonDocument doc;
  JsonObject root = doc.to<JsonObject>(); // clear and replace with a new JsonObject
  // JsonObject topObj = root.createNestedObject("topTime");
  // JsonObject curObj = root.createNestedObject("curTime");
  // topObj.set(top);
  // curObj.set(middle);

  update();
  root["topTime"].to<JsonObject>();
  root["midleTime"].to<JsonObject>();
  root["topTime"]=getTopTime();
  root["middleTime"]=getMiddleTime();

  String output;
  serializeJson(doc,output);
  Serial.println("Clock State Finished\n");
  return String(output);
}

JsonObject DeloreanClock::getTopTime(){
  Serial.print("Getting Top Time: ");
  printTime(time_red,getTimezone());
  return timeToJson(time_red,true);
}

JsonObject DeloreanClock::getMiddleTime(){
  return timeToJson(now(),true);
}

JsonObject DeloreanClock::timeToJson(time_t time,bool toLocal){
  TimeChangeRule *tcr;
  if(toLocal){
    time = tz.toLocal(time, &tcr);
  }
  //const size_t capacity = JSON_OBJECT_SIZE(1) + JSON_OBJECT_SIZE(5);
  JsonDocument  doc;

  JsonObject obj = doc.to<JsonObject>(); // clear and replace with a new JsonObject
  obj["month"] = month(time);
  obj["day"] = day(time);
  obj["year"] = year(time);
  obj["hour"] = hour(time);
  obj["min"] = minute(time);
  return obj;
}

/* Control the AM/PM LED's for the specified color row
 * Wiring is designed so the pins are power sinking which
 * means the control logic is inverse 
 *
 * true -> output low, false -> output high
 * output low -> on, output high -> off
*/
void DeloreanClock::setLEDs(bool am, bool pm, LEDColor color){
  Serial.print("Updating LEDs: ");
  Serial.println(color);
  Serial.print("AM:");
  Serial.println(am);
  Serial.print("PM:");
  Serial.println(pm);
  switch(color){
    case red:
      digitalWrite(PIN_RED_AM,!am);
      digitalWrite(PIN_RED_PM,!pm);
    break;
    case green:
      digitalWrite(PIN_GREEN_AM,!am);
      digitalWrite(PIN_GREEN_PM,!pm);
    break;
    case yellow:
      digitalWrite(PIN_YELLOW_AM,!am);
      digitalWrite(PIN_YELLOW_PM,!pm);
    break;
  }
}