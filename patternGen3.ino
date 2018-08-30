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
//#define DELIMITER  "***"

#define ROWS  24
#define COLS  32
byte mask[ROWS][COLS];
int values[] = {1,3,5,2};
int k=0;  // animation index
int DELAY = 1000;

void setup(void) { 
    #ifdef ENABLE_DEBUG
        Serial.begin(baud_rate); // to use the Rx pin for I/O, disable this line
    #endif    
    randomSeed(micros());
    //SERIAL_PRINTLN("\nHeatmap simulator starting...");
    /*    
    for (int i=0; i<COLS+3; i++) {
      Serial.print((i-2)%COLS); Serial.print(" ");      
      Serial.print((i-1)%COLS); Serial.print(" ");
      Serial.print(i%COLS); Serial.print(" ");
      Serial.print((i+1)%COLS); Serial.print(" ");      
      Serial.print((i+2)%COLS); Serial.println(" ");             
    }
    -2 -1 0 1 2 
    -1 0 1 2 3 
    0 1 2 3 4 
    1 2 3 4 5 
    2 3 4 5 6 
    3 4 5 6 7 
    4 5 6 7 0 
    5 6 7 0 1 
    6 7 0 1 2 
    7 0 1 2 3 
    0 1 2 3 4 
     */
     /*
    int x;
    for (int i=0; i<COLS+3; i++) {
      x = (i-2)%COLS; Serial.print(x < 0 ? x+COLS : x); Serial.print(" ");      
      x = (i-1)%COLS; Serial.print(x < 0 ? x+COLS : x); Serial.print(" ");
      x = i%COLS; Serial.print(x < 0 ? x+COLS : x); Serial.print(" ");
      x = (i+1)%COLS; Serial.print(x < 0 ? x+COLS : x); Serial.print(" ");      
      x = (i+2)%COLS; Serial.print(x < 0 ? x+COLS : x); Serial.println(" ");             
    }     
    6 7 0 1 2 
    7 0 1 2 3 
    0 1 2 3 4 
    1 2 3 4 5 
    2 3 4 5 6 
    3 4 5 6 7 
    4 5 6 7 0 
    5 6 7 0 1 
    6 7 0 1 2 
    7 0 1 2 3 
    0 1 2 3 4 
  */  
}

void loop(void){
    setup_data3();
    send_data();
    delay(DELAY);
} 

void setup_data1() {
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

void setup_data2() {
    int c;
    for (int i=0; i<ROWS; i++) 
        for (int j=0; j<COLS; j++)
          mask[i][j] = values[0];
    for (int i=1; i<4; i++) {
        c = (k-2)%COLS; if (c < 0) c += COLS; mask[i][c] = values[1]; 
        c = (k-1)%COLS; if (c < 0) c += COLS; mask[i][c] = values[1];   
        c = (k)%COLS;   if (c < 0) c += COLS; mask[i][c] = values[1];   
        c = (k+1)%COLS; if (c < 0) c += COLS; mask[i][c] = values[1];   
        c = (k+2)%COLS; if (c < 0) c += COLS; mask[i][c] = values[1];   
    }        
    k = (k+1)%(COLS+3);                   
}

void setup_data3() {
    int c;
    for (int i=0; i<ROWS; i++) 
        for (int j=0; j<COLS; j++)
          mask[i][j] = values[0];
    for (int i=3; i<6; i++) {
        c = (k-2)%COLS; if (c < 0) c += COLS; mask[i][c] = values[1]; 
        c = (k-1)%COLS; if (c < 0) c += COLS; mask[i][c] = values[1];   
        c = (k)%COLS;   if (c < 0) c += COLS; mask[i][c] = values[1];   
        c = (k+1)%COLS; if (c < 0) c += COLS; mask[i][c] = values[1];   
        c = (k+2)%COLS; if (c < 0) c += COLS; mask[i][c] = values[1];   
    }        
    for (int i=10; i<12; i++) {
        c = (k-9)%COLS;  if (c < 0) c += COLS; mask[i][c] = values[2];   
        c = (k-8)%COLS;  if (c < 0) c += COLS; mask[i][c] = values[2];   
        c = (k-7)%COLS;  if (c < 0) c += COLS; mask[i][c] = values[2];     
    }     
    for (int i=12; i<16; i++) {
        c = (k-10)%COLS; if (c < 0) c += COLS; mask[i][c] = values[2]; 
        c = (k-9)%COLS;  if (c < 0) c += COLS; mask[i][c] = values[2];   
        c = (k-8)%COLS;  if (c < 0) c += COLS; mask[i][c] = values[2];   
        c = (k-7)%COLS;  if (c < 0) c += COLS; mask[i][c] = values[2];   
        c = (k-6)%COLS;  if (c < 0) c += COLS; mask[i][c] = values[2];   
    }     
    for (int i=20; i<22; i++) {
        c = (k+10)%COLS; if (c < 0) c += COLS; mask[i][c] = values[3]; 
        c = (k+9)%COLS;  if (c < 0) c += COLS; mask[i][c] = values[3];   
        c = (k+8)%COLS;  if (c < 0) c += COLS; mask[i][c] = values[3];   
        c = (k+7)%COLS;  if (c < 0) c += COLS; mask[i][c] = values[3];   
        c = (k+6)%COLS;  if (c < 0) c += COLS; mask[i][c] = values[3];   
    }     
    k = (k+1)%(COLS+3);                   
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

