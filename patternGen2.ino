// Pattern generator to simulate IR sensor array

#define ENABLE_DEBUG
// LED connected to Rx will work only if debug is not enabled

#ifdef ENABLE_DEBUG
  #define  SERIAL_PRINT(x) Serial.print(x)
  #define  SERIAL_PRINTLN(x) Serial.println(x)
  #define  SERIAL_PRINTF(x,y)  Serial.printf(x,y) 
#else
  #define  SERIAL_PRINT(x)
  #define  SERIAL_PRINTLN(x)
  #define  SERIAL_PRINTF(x,y)
#endif
 
long baud_rate = 115200L;
#define DELIMITER  "***"

#define ROWS  8
#define COLS  8
byte mask[ROWS][COLS];
int values[] = {1,3,5,2};

void setup(void) { 
    #ifdef ENABLE_DEBUG
        Serial.begin(baud_rate); // to use the Rx pin for I/O, disable this line
    #endif    
    randomSeed(micros());
    //SERIAL_PRINTLN("\nHeatmap simulator starting...");
}

void loop(void){
    setup_data();
    send_data();
    delay(5000);
} 

int k=0;
void setup_data() {
    for (int i=0; i<ROWS; i++) 
        for (int j=0; j<3; j++)
          mask[i][j] = values[k];
    k = (k+1)%4;
    for (int i=0; i<ROWS; i++) 
        for (int j=3; j<5; j++)   
            mask[i][j] = values[k];    
    k = (k+1)%4;
    for (int i=0; i<ROWS; i++) 
        for (int j=5; j<8; j++)   
            mask[i][j] = values[k];      
    k = (k+1)%4;                   
}

void send_data () {
    //SERIAL_PRINTLN(DELIMITER);
    for (int i=0; i<ROWS; i++) {
        for (int j=0; j<COLS; j++) {
            SERIAL_PRINT (mask[i][j]);
            SERIAL_PRINT (" ");
        }
    }
    SERIAL_PRINTLN("");  // it sends  '\r\n'
}

