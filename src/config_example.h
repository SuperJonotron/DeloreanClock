#ifndef CONFIG_H
#define CONFIG_H

/**
 * Enter the WiFi network you want to connect to, for getting the current time and other information.
 * If for some reason your WiFi doesn't have a password, you can comment that out.
 */
// Safe stored password
// http://www.arduinoslovakia.eu/blog/2017/6/vlozenie-definicie-makra-do-programu-v-arduine?lang=en
#if defined(_PWD)
const char* ssid = _SSID;
const char* pass = _PWD;
#else
char ssid[] = "NetworkName";  //  your network SSID (name)
char pass[] = "NetworkPassword";       // your network password
#endif

const String jsonConfig = R"({
    "birthdays":[
        {"name":"Birthday1","timeUnix":473410800},
        {"name":"Birthday2","timeUnix":476611200},
        {"name":"Birthday3","timeUnix":489045600},
        {"name":"Birthday4","timeUnix":496994400}
    ],
    "anniversaries":[
        {"name":"B2F Release","timeUnix":489218400},
        {"name":"B2F2 2Release","timeUnix":627721200},
        {"name":"B2F3 Release","timeUnix":643615200}
    ]
})";

#endif // CONFIG_H
