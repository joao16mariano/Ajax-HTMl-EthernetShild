#include <SPI.h>
#include <Ethernet.h>
#include <SD.h>

#define REQ_BUF_SZ   60


byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 00, 27);   //ip 
EthernetServer server(8788);   //porta configurar no rotiador    
File webFile;                     
char HTTP_req[REQ_BUF_SZ] = {0}; 
char req_index = 0;               
unsigned char LED_state[3] = {0}; 


void setup()
{  
 int i;
    
    pinMode(10, OUTPUT);
    digitalWrite(10, HIGH);
    
    Serial.begin(9600);     
        
    Serial.println("Initializing SD card...");
    if (!SD.begin(4)) {
        Serial.println("ERROR - SD card initialization failed!");
        return;   
    }
    Serial.println("SUCCESS - SD card initialized.");    
    if (!SD.exists("index.htm")) {
        Serial.println("ERROR - Can't find index.htm file!");
        return;  
    }
    Serial.println("SUCCESS - Found index.htm file.");   
    pinMode(2, INPUT);
    pinMode(3, INPUT);
    

    for (i = 26; i <= 49; i++) {
        pinMode(i, OUTPUT);   
        digitalWrite(i, LOW);  
    }
   
    
    Ethernet.begin(mac, ip); 
    server.begin();          
}

void loop()
{
    EthernetClient client = server.available();  

    if (client) {  // got client?
        boolean currentLineIsBlank = true;
        while (client.connected()) {
            if (client.available()) {  
                char c = client.read();                
                if (req_index < (REQ_BUF_SZ - 1)) {
                    HTTP_req[req_index] = c;         
                    req_index++;
                }


  
if (c == '\n' && currentLineIsBlank) {
                 if (StrContains(HTTP_req, "GET / ")
                                 || StrContains(HTTP_req, "GET /index.htm")) {
                        client.println("HTTP/1.1 200 OK");
                        client.println("Content-Type: text/html");
                        client.println("Connnection: close");
                        client.println();
                        webFile = SD.open("index.htm");        // open web page file
                    }

                        else if (StrContains(HTTP_req, "GET /page2.htm")) {
                        client.println("HTTP/1.1 200 OK");
                        client.println("Content-Type: text/html");
                        client.println("Connnection: close");
                        client.println();
                        webFile = SD.open("page2.htm");        // open web page file
                        }
                        else if (StrContains(HTTP_req, "GET /page3.htm")) {
                        client.println("HTTP/1.1 200 OK");
                        client.println("Content-Type: text/html");
                        client.println("Connnection: close");
                        client.println();
                        webFile = SD.open("page3.htm");        // open web page file
                        }
                        else if (StrContains(HTTP_req, "GET /page4.htm")) {
                        client.println("HTTP/1.1 200 OK");
                        client.println("Content-Type: text/html");
                        client.println("Connnection: close");
                        client.println();
                        webFile = SD.open("page4.htm");        // open web page file
                        }
                   

                     
}
                
                
                if (c == '\n' && currentLineIsBlank) {                   
                    client.println("HTTP/1.1 200 OK");                    
                    if (StrContains(HTTP_req, "ajax_inputs")) {                       
                        client.println("Content-Type: text/xml");
                        client.println("Connection: keep-alive");
                        client.println();
                        SetLEDs();                        
                        XML_response(client);
                        

                        
                    
       }           
                       



                   
                    if (webFile) {
                        while(webFile.available()) {
                            client.write(webFile.read()); // send web page to client
                        }
                        webFile.close();
                    }
                    // reset buffer index and all buffer elements to 0
                    Serial.print(HTTP_req);
                    req_index = 0;
                    StrClear(HTTP_req, REQ_BUF_SZ);
                    break;
                }


                               
                if (c == '\n') {                  
                  
                    currentLineIsBlank = true;
                } 
                else if (c != '\r') {                   
                    currentLineIsBlank = false;
                }
            }
        } 
        delay(1);   
        client.stop(); 
    }
}

                                                                                                                                           

void SetLEDs(void)
{
    char str_on[12] = {0};
    char str_off[12] = {0};
    unsigned char i;
    unsigned int  j;
    int LED_num = 1;
    
    for (i = 0; i < 3; i++) {
        for (j = 1; j <= 0x80; j <<= 1) {
            sprintf(str_on,  "LED%d=%d", LED_num, 1);
            sprintf(str_off, "LED%d=%d", LED_num, 0);
            if (StrContains(HTTP_req, str_on)) {
                LED_state[i] |= (unsigned char)j;  // save LED state
                digitalWrite(LED_num + 25, HIGH);
                Serial.println("ON");
            }
            else if (StrContains(HTTP_req, str_off)) {
                LED_state[i] &= (unsigned char)(~j);  // save LED state
                digitalWrite(LED_num + 25, LOW);
            }
            LED_num++;
        }
    }
}


void XML_response(EthernetClient cl)
{
    unsigned char i;
    unsigned int  j;

    int analog_val;           
    int count;                
    int sw_arr[] = {2, 3, 5};  
    
    cl.print("<?xml version = \"1.0\" ?>");
    cl.print("<inputs>");

    for (count = 2; count <= 5; count++) { // A2 to A5
        analog_val = analogRead(count);
        cl.print("<analog>");
        cl.print(analog_val);
        cl.println("</analog>");
    }

for (count = 0; count < 3; count++) {
        cl.print("<switch>");
        if (digitalRead(sw_arr[count])) {
            cl.print("ON-LINE");
        }
        else {
            cl.print("OFF");
        }
        cl.println("</switch>");
    }

    
    
    for (i = 0; i < 3; i++) {
        for (j = 1; j <= 0x80; j <<= 1) {
            cl.print("<LED>");
            if ((unsigned char)LED_state[i] & j) {
                cl.print("checked");
                //Serial.println("ON");
            }
            else {
                cl.print("unchecked");
            }
            cl.println("</LED>");
        }
    }
    cl.print("</inputs>");
}


void StrClear(char *str, char length)
{
    for (int i = 0; i < length; i++) {
        str[i] = 0;
    }
}


char StrContains(char *str, char *sfind)
{
    char found = 0;
    char index = 0;
    char len;

    len = strlen(str);
    
    if (strlen(sfind) > len) {
        return 0;
    }
    while (index < len) {
        if (str[index] == sfind[found]) {
            found++;
            if (strlen(sfind) == found) {
                return 1;
            }
        }
        else {
            found = 0;
        }
        index++;
    }

    return 0;
}
