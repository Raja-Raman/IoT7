/***
Test storage of user data in RTC memory of ESP2866

RTC memory in 8266 is 768 bytes. Aligned as 4 byte chunks.
First 256 bytes (64 chunks, 0 to 63) are reserved for the system.
The next 512 bytes (128 chunks, 64   to 127) are available to users.
The memory survives a chip reset, but NOT a power outage.
***/

#include <Timer.h>  // https://github.com/JChristensen/Timer
extern "C" {
#include "user_interface.h" // needed for the RTC memory read/write functions
}
// ADC_MODE can only be set at compile time; and you can't call it from within a function
ADC_MODE(ADC_VCC);

//chunks 0-63 are for system
#define RTC_MEMORY_BASE     64
#define TOTAL_RTC_MEMORY    128
#define MAGIC_NUMER         0xaa55

typedef struct RtcDataChunk {
  uint16_t magic_number;      // to check if data is corrupted 
  uint16_t serial_number;     // increment this for every chunk
  uint16_t battery_voltage;   // milli Volts
  uint16_t user_data1;
  uint16_t user_data2;  
  uint16_t padding;           // the struct size must be a multiple of 4
};

int sl_no = 0;
RtcDataChunk  chunk;
boolean alignment_error = false;
int MAX_CHUNKS = 0;   
Timer T;

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("\n\nRTC memory tests starting...");
  Serial.print("size of int: ");  
  Serial.println(sizeof(int));           // 4
  Serial.print("size of uint16_t: ");  
  Serial.println(sizeof(uint16_t));      // 2  
  Serial.print("size of RtcDataChunk struct: ");
  Serial.println(sizeof(RtcDataChunk));  // 12
  MAX_CHUNKS = TOTAL_RTC_MEMORY*4/sizeof(RtcDataChunk);   
  Serial.print("Maximum chunks: ");
  Serial.println(MAX_CHUNKS);   
  float supply_voltage = ESP.getVcc()/1000.0;
  Serial.print("Supply voltage: ");  
  Serial.println(supply_voltage); 
  if (sizeof(RtcDataChunk) % 4 == 0) {
      Serial.println ("RTC data structure correctly aligned");
      alignment_error = false;
  }  else {
    Serial.println("Size of RTC data structure must be a multiple of 4 !");
    alignment_error = true;  
  }
  locate_data_blocks();
  Serial.println ("\nEnter two integers separated by space...");
  T.every(10000, read_data);
}

void loop() {
  T.update();
  if (alignment_error) return;
  if (Serial.available()) {
      int x = Serial.parseInt();
      int y = Serial.parseInt();     
      write_data(x,y); 
      Serial.println("Saved in RTC.");
      Serial.println ("\nEnter two integers separated by space...");      
  }
}

void locate_data_blocks() {
  sl_no = 0;
  for (int i=0; i<MAX_CHUNKS; i++) {
    system_rtc_mem_read(RTC_MEMORY_BASE+i*sizeof(RtcDataChunk), &chunk, sizeof(RtcDataChunk));  
    if (chunk.magic_number == MAGIC_NUMER) // occupied
      sl_no++;  
    else
      break;
  }
  Serial.print("Total chunks occupied: ");
  Serial.println(sl_no);
}

void print_data() {
  Serial.print("magic_number : ");
  Serial.println(chunk.magic_number, HEX);
  Serial.print("serial_number : ");
  Serial.println(chunk.serial_number);
  Serial.print("battery_voltage : ");
  Serial.println(chunk.battery_voltage/1000.0);
  Serial.print("user_data1 : ");
  Serial.println(chunk.user_data1);
  Serial.print("user_data2 : ");
  Serial.println(chunk.user_data2);
  Serial.print("padding :) ");
  Serial.println(chunk.padding);       
  Serial.println(" ");
}

void write_data(int x, int y) {
  chunk.magic_number = MAGIC_NUMER;
  chunk.serial_number = sl_no+1;  // 1-based
  chunk.battery_voltage = ESP.getVcc();
  chunk.user_data1 = x;
  chunk.user_data2 = y;
  chunk.padding = 12345;// additional magic number          
  system_rtc_mem_write(RTC_MEMORY_BASE+sl_no*sizeof(RtcDataChunk), &chunk, sizeof(RtcDataChunk)); 
  sl_no++; 
  if (sl_no > MAX_CHUNKS) {
    Serial.println ("RTC data storage is full ! Resetting to the beginning..");
    sl_no = 0;
  }
}

void read_data() {
  if (sl_no < 1) return; // no data yet
  for (int i=0; i<sl_no; i++) {
    system_rtc_mem_read(RTC_MEMORY_BASE+i*sizeof(RtcDataChunk), &chunk, sizeof(RtcDataChunk));  
    print_data();
  }
  Serial.println("------------------------------------ ");  
}
  
