 #ifndef DELOREANCLOCK_H
 #define DELOREANCLOCK_H 

/*Import all third party libraries required*/
#include <Preferences.h> //https://github.com/vshymanskyy/Preferences
#include <ArduinoJson.h>
#include <Timezone.h>
#include "RTClib.h"

/*Import all clock related libraries*/
#include <NTPClient.h>
#include <FluxCapacitor.h>
/*If Preferences is imported before Delorean7SegmentControl, 
* it cause issues with the min macro that is fixed in this
* library so we'll just always make sure to import it last
*/
#include <Delorean7SegmentControl.h> 

// Key values used for preferences
#define APP_NAME "delorean_clock"
#define K_CONFIG "config"
#define K_MODE "mode"

//Central European Time (Frankfurt, Paris)
const TimeChangeRule CEST = {"CEST", Last, Sun, Mar, 2, 120};    //Central European Summer Time
const TimeChangeRule CET = {"CET", Last, Sun, Oct, 3, 60};       //Central European Standard Time
const Timezone CE(CEST, CET);

//Australia Eastern Time Zone (Sydney, Melbourne)
const TimeChangeRule aEDT = {"AEDT", First, Sun, Oct, 2, 660};    //UTC + 11 hours
const TimeChangeRule aEST = {"AEST", First, Sun, Apr, 3, 600};    //UTC + 10 hours
const Timezone ausET(aEDT, aEST);

//US Eastern Time Zone (New York, Detroit)
const TimeChangeRule usEDT = {"EDT", Second, Sun, Mar, 2, -240};  //Eastern Daylight Time = UTC - 4 hours
const TimeChangeRule usEST = {"EST", First, Sun, Nov, 2, -300};   //Eastern Standard Time = UTC - 5 hours
const Timezone usET(usEDT, usEST);

//US Pacifig Time Zone
const TimeChangeRule usPDT = {"PDT", Second, Sun, Mar, 2, -420};  //Pacific Daylight Time = UTC - 7 hours
const TimeChangeRule usPST = {"PST", First, Sun, Nov, 2, -480};   //Pacific Standard Time = UTC - 8 hours
const Timezone usPT(usPDT, usPST);

//US Mountain Time Zone
const TimeChangeRule usMDT = {"MDT", Second, Sun, Mar, 2, -360};  //Pacific Daylight Time = UTC - 6 hours
const TimeChangeRule usMST = {"MST", First, Sun, Nov, 2, -420};   //Pacific Standard Time = UTC - 7 hours
const Timezone usMT(usMDT, usMST);

enum Mode{
  unknown,
  birthday,
  anniversary,
  holiday,
  event,
  night,
  demo
};

enum LEDColor{
    red,
    green,
    yellow
};

#if defined(ESP8266)
    //ToDo:  Figure out what pins I should use for esp8266
    // use zero just as a place holder for compiling
    //define the pins for the AM/PM LEDs
    const byte PIN_YELLOW_AM = 0;
    const byte PIN_YELLOW_PM = 0;
    const byte PIN_GREEN_AM = 0;
    const byte PIN_GREEN_PM = 0;
    const byte PIN_RED_AM = 0;
    const byte PIN_RED_PM = 0;

#elif defined(ESP32) //This pinout is not confirmed just placed here to compile

    //ToDo:  Figure out what pins I should use for esp32
    // use zero just as a place holder for compiling
    //define the pins for the AM/PM LEDs
    const byte PIN_YELLOW_AM = 0;
    const byte PIN_YELLOW_PM = 0;
    const byte PIN_GREEN_AM = 0;
    const byte PIN_GREEN_PM = 0;
    const byte PIN_RED_AM = 0;
    const byte PIN_RED_PM = 0;

#elif defined(ARDUINO_ARCH_MBED_RP2040) || defined(ARDUINO_ARCH_RP2040)
    //define the pins for the AM/PM LEDs
    const byte PIN_YELLOW_AM = 10;
    const byte PIN_YELLOW_PM = 11;
    const byte PIN_GREEN_AM = 12;
    const byte PIN_GREEN_PM = 13;
    const byte PIN_RED_AM = 14;
    const byte PIN_RED_PM = 15;

#endif


/**
 * DeloreanClock object used to control the I/O to maintain the time 
 * and display output  on the 7-Segment displays                                                             
 */
class DeloreanClock{
    public:
        DeloreanClock();
        Mode getMode();
        void start();
        void setTimezone(Timezone tz);
        Timezone getTimezone();
        String readConfig();
        void refresh();
        void setMode(Mode m);
        Mode strToMode(String m);
        void setRedTime(time_t time);
        void setGreenTime(time_t time);
        void setYellowTime(time_t time);
        bool sync();
        bool syncRequired();
        void updateAll(time_t red, time_t green, time_t yellow);
        void updateCurrentTime();
        void updateCurrentTimeOnly();
        time_t now();
        void demoMode();
        void update();
        void updateConfig(String config);
        //ToDo: Update these implementations once renaming
        //to top,middle,bottom conventions has been done
        String getClockState();
        JsonObject getTopTime();
        JsonObject getMiddleTime();

        //Possibly move this to flux capacitor
        JsonObject timeToJson(time_t time,bool toLocal);

        //Make these methods private when full migration
        //is complete
        void anniversaryMode();
        void birthdayMode();
        void eventMode();
        void defaultMode();
        void nightMode();
        void initPins();
        void setLEDs(bool am, bool pm, LEDColor);
        std::vector<time_t> birthdays;
        std::vector<time_t> anniversaries;
    private:
        NTPClient ntp = NTPClient("time.nist.gov", 2990);
        bool syncReq = true;
        Timezone tz = usMT;
        DateTime nextSync;
        RTC_Millis rtc;
        Delorean7SegmentControl segCtrl;
        //Future Time Travled to in B2F2 - 2015-10-21T16:29:00
        time_t time_red = 1445385600;
        time_t time_green = 0;
        //Past Time Travled to in B2F - 1955-11-05T6:00:00
        time_t time_yellow = -446774400;
        Mode mode = demo;
        Preferences preferences;
};

#endif