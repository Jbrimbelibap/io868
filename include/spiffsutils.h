#include "FS.h"
#include "SPIFFS.h"

double default_freq = 868.3; //Change the default frequency here
double used_frequency; 

#define FORMAT_SPIFFS_IF_FAILED true

void listSPIFFS(const char * dirname, uint8_t levels){      //Function that lists all SPIFFS files, signals and saved frequencies. It is called on setup
    Serial.printf("Listing directory: %s\n", dirname);

    File root = SPIFFS.open(dirname);
    if(!root){
        Serial.printf("Open failed for %s\n",dirname);
        return;
    }
    if(!root.isDirectory()){
        Serial.printf("%s is not a directory\n",dirname);
        return;
    }

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            Serial.print(" DIR : ");
            Serial.println(file.name());
            if(levels){
                listSPIFFS(file.name(), levels -1);
            }
        } else {
            Serial.print(" FILE: ");
            Serial.print(file.name());
            Serial.print("\tSIZE: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }
}

void loadSPIFFS(const char * path, uint16_t *signal433_store, uint16_t size){       //Function that loads all 10 recorded signals. It is called on setup
    Serial.printf("Reading file: %s\n", path);
    File file = SPIFFS.open(path);
    if(!file || file.isDirectory()){
        Serial.printf("Load failed for %s\n",path);
        return;
    }
    int i = 0;
    while(file.available() && i < size){
       uint16_t value = ((unsigned char)file.read() << 8) | (unsigned char)file.read();
       signal433_store[i++] = value;
    }
}

void storeSPIFFS(const char * path, uint16_t *signal433_store, uint16_t size){      //Function that stores the recorded signals using SPIFFS
    Serial.printf("Writing file: %s\r\n", path);
    File file = SPIFFS.open(path, FILE_WRITE);
    if(!file){
       Serial.printf("Store failed for %s\n",path);
        return;
    }
   int i = 0;
   while(i < size){
       file.write((unsigned char)(signal433_store[i]>>8 & 0xff));
       file.write((unsigned char)(signal433_store[i] & 0xff));
       i++;
    }
}



void saveFrequency(String freqname, double frequency) {  //Function to save in storage the selected frequency for a specific page
  File freqFile = SPIFFS.open(freqname, "w");
  if (!SPIFFS.exists(freqname)) {
    Serial.println("Failed to open frequency file for writing");
    return;
  }
  freqFile.print(frequency);
  freqFile.close();
}


void readFrequency(String freqname) {       //Function that reads the saved frequency for a specific page
  File freqFile = SPIFFS.open(freqname, "r");
    if (!SPIFFS.exists(freqname)) {
        Serial.println("Creating file with default frequency");
        saveFrequency(freqname, default_freq);  //avoids having 0.00 by default when recording a single for the first time on a page
        return;
    }

  used_frequency = freqFile.readString().toDouble();
  freqFile.close();
}