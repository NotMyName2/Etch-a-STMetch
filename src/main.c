/* Autor: Ivan Cermak, 21.5.2021
**  semestralni prace LPE
**  kresleni do terminalu pomoci ANSI sekvenci
**  ovladano 2 rotacnimi enkodery, 
**              2 prepinaci
**              3 potenciometry
**  podrobnejsi popis ve sprave nebo prezentaci
*/

#include "mbed.h"
#include "Timer.h"

AnalogIn pot_blue(PA_5);
AnalogIn pot_green(PA_6);
AnalogIn pot_red(PA_7);

Serial pc(PA_9,PA_10); // tx, rx

InterruptIn OE1_LEFT(PA_3); //rotary encoders
InterruptIn OE1_RIGHT(PA_2);
InterruptIn OE2_LEFT(PA_0);
InterruptIn OE2_RIGHT(PA_1);

DigitalIn erase_switch(PA_4); //left switch
DigitalIn editing_switch(PA_13); //right switch
//DigitalIn lastColor_pin(PA_14); //right button

#define ROWS 32
#define COLS 48 
volatile int OE1_RIGHT_FIRST = 0;
volatile int OE1_LEFT_FIRST = 0;
volatile int cursorRow = ROWS/2;
volatile int cursorCol = COLS/2;
volatile int nowColor = 0;
volatile int moveFlag = 0;

char colors[ROWS][COLS];

void resetColour(){
    pc.printf("\033[48;5;%dm", 0);
    }
    
void colorPixel(int newRow, int newCol, int newColor){
    colors[newRow][newCol] = newColor;
    }
    

void initImage(){
    //deletes image and sets the image to full black
    memset(colors,0, ROWS*COLS);
}

void printEmptyRows(int amount){
    for (int x = 0; x < amount; x++){
        pc.printf("\r\n");
    }
}

volatile int cursorColor = 68;
volatile int printColor = 68;

void changeBgColor(int newColor)
{
    if (newColor == printColor){
        return;
    }
    
    printColor= newColor;
    pc.printf("\033[48;5;%dm", printColor);
    return;
}


void display(){ //prints image to serial
    //printEmptyRows(5);   
    pc.printf("\033[1;1H");
    nowColor = 0;
    for (int nowRow = 0; nowRow < ROWS; nowRow++){
        for (int nowCol = 0; nowCol < COLS; nowCol++){
            if (nowColor != colors[nowRow][nowCol]){
                changeBgColor(colors[nowRow][nowCol]);
                nowColor = colors[nowRow][nowCol];
            }
            //if not cursor, print space
            if ((nowCol != cursorCol) && (nowRow != cursorRow)){
                pc.printf(" ");
                continue;
            }
            //if cursor center
            if ((nowCol == cursorCol) && (nowRow == cursorRow)){
                pc.printf("+");
                continue;
            }//if cursor column
            if (nowCol == cursorCol){
                    pc.printf("|");
                    continue;
            }//if cursor row
            if (nowRow == cursorRow){
                pc.printf("-");
                continue;
            }
        }//one row printed
        pc.printf("\r\n");
    }
    printEmptyRows(1);
}
    
//first encoder
void OE1_LEFT_IRQ_FALL(){
    if (OE1_RIGHT_FIRST == 1){
        OE1_LEFT_FIRST = 0;
        OE1_RIGHT_FIRST = 0;
        cursorCol -=1;
        if (cursorCol < 0){
            cursorCol = 0;
        }
        
        if(editing_switch){
            colorPixel(cursorRow, cursorCol, cursorColor);
        }
        if(erase_switch){
            colorPixel(cursorRow, cursorCol, 0);
        }
        moveFlag = 1;
    }
    else{
        if (OE1_RIGHT) //if right is high, left is truly first
        {
            OE1_LEFT_FIRST=1;
            OE1_RIGHT_FIRST=0;
        }
    }
}

void OE1_RIGHT_IRQ_FALL(){
    if (OE1_LEFT_FIRST == 1){
        OE1_RIGHT_FIRST = 0;
        OE1_LEFT_FIRST = 0;
        cursorCol +=1;
        if (cursorCol > COLS-1 ){
            cursorCol = COLS-1;
        }
        
        if(editing_switch){
            colorPixel(cursorRow, cursorCol, cursorColor);
        }
        
        if(erase_switch){
            colorPixel(cursorRow, cursorCol, 0);
        }
        moveFlag = 1;
    }
    else{
        if(OE1_LEFT)//if left is high, right is truly first
        {
            OE1_RIGHT_FIRST = 1;
            OE1_LEFT_FIRST=0;
        }
    }
}

void OE1_LEFT_IRQ_RISE(){ //debouncing
    OE1_RIGHT_FIRST = 0;
    OE1_LEFT_FIRST = 0;
}

void OE1_RIGHT_IRQ_RISE(){    //debouncing
    OE1_RIGHT_FIRST = 0;
    OE1_LEFT_FIRST = 0;
}


//second encoder
volatile int OE2_RIGHT_FIRST = 0;
volatile int OE2_LEFT_FIRST = 0;

void OE2_LEFT_IRQ_FALL(){
    if (OE2_RIGHT_FIRST == 1){
        OE2_LEFT_FIRST = 0;
        OE2_RIGHT_FIRST = 0;
        cursorRow -=1;
        if (cursorRow < 0){
            cursorRow = 0;
        }
        
        if(editing_switch){
            colorPixel(cursorRow, cursorCol, cursorColor);
        }
        
        if(erase_switch){
            colorPixel(cursorRow, cursorCol, 0);
        }
        moveFlag = 1;
    }
    else{
        if (OE2_RIGHT) //if right is high, left is truly first
        {
            OE2_LEFT_FIRST=1;
            OE2_RIGHT_FIRST=0;
        }
    }
}

void OE2_RIGHT_IRQ_FALL(){
    if (OE2_LEFT_FIRST == 1){
        OE2_RIGHT_FIRST = 0;
        OE2_LEFT_FIRST = 0;
        cursorRow +=1;
        if (cursorRow > ROWS-1 ){
            cursorRow = ROWS-1;
        }
        
        if(editing_switch){
            colorPixel(cursorRow, cursorCol, cursorColor);
        }
        
        if(erase_switch){
            colorPixel(cursorRow, cursorCol, 0);
        }
        moveFlag = 1;
    }
    else{
        if(OE2_LEFT)//if left pin is high, right is truly first
        {
            OE2_RIGHT_FIRST = 1;
            OE2_LEFT_FIRST=0;
        }
    }
}

void OE2_LEFT_IRQ_RISE(){ //debouncing
    OE2_RIGHT_FIRST = 0;
    OE2_LEFT_FIRST = 0;
}

void OE2_RIGHT_IRQ_RISE(){    //debouncing
    OE2_RIGHT_FIRST = 0;
    OE2_LEFT_FIRST = 0;
}

/* //usable characters
char fullBlock = 219;
char low = 176; 
char med = 177;
char high = 178;
char chars[4] = {fullBlock, low, med, high};
*/

void randomiseColors(){
    int tempColor;
    for (int nowRow = 0; nowRow < ROWS; nowRow++){
        for (int nowCol = 0; nowCol < COLS; nowCol++){
            tempColor = (rand() % 216)+16;
            colors[nowRow][nowCol] = tempColor;
        }
    }
        
}


int computeColor(int R, int G, int B){
    return 36*R+6*G+B+16; //standart ANSI calculation, find on wikipedia
}
    
uint16_t aRed;
uint16_t aGreen;
uint16_t aBlue;

void initDisplay(){
    pc.printf("\033[?25l");
    }
    
int main(){
     int splashFlag = 1;
     srand(time(NULL));
     pc.baud(460800);
     initImage();
     initDisplay();
     OE1_LEFT.fall(&OE1_LEFT_IRQ_FALL); 
     OE1_RIGHT.fall(&OE1_RIGHT_IRQ_FALL);
     OE1_LEFT.rise(&OE1_LEFT_IRQ_RISE); 
     OE1_RIGHT.rise(&OE1_RIGHT_IRQ_RISE);
     
     OE2_LEFT.fall(&OE2_LEFT_IRQ_FALL); 
     OE2_RIGHT.fall(&OE2_RIGHT_IRQ_FALL);
     OE2_LEFT.rise(&OE2_LEFT_IRQ_RISE); 
     OE2_RIGHT.rise(&OE2_RIGHT_IRQ_RISE);
     
     pc.printf("\033[38;5;%dm", 15); //high intensity white foreground
     pc.printf("Info: I work!\r\n"); 
     pc.printf("Info: MCU restarted!\r\n"); 
     
     if (splashFlag){
        pc.printf("\033[?17;0;255c");
        pc.printf("Nejlepe funguji na rozliseni 34 radku x 48 sloupcu!\r\n");  
        pc.printf("Pokud nejsem nastaven na 34x48, prosim, nastav\r\n");  
        pc.printf("Muzes zacit kreslit otocenim sedych knofliku!\r\n"); 
        pc.printf("Intenzitu slozek barvy menis otocenim cernych knofliku!\r\n"); 
     }
     
     int rRed = pot_red.read_u16(); //read raw 
     int rGreen = pot_green.read_u16();
     int rBlue = pot_blue.read_u16(); 
     
     int aRed = rRed/11000;
     int aGreen = rGreen/11000;
     int aBlue = rBlue/11000; //rough calculation to 0-5
        
     while(1){     //infinite main program loop
        if (moveFlag){
            display();
            moveFlag = 0;
            splashFlag = 0;
        }
        changeBgColor(0);
        rRed = pot_red.read_u16(); //read raw
        rGreen = pot_green.read_u16();
        rBlue = pot_blue.read_u16(); 
        
        aRed = rRed/11000;
        aGreen = rGreen/11000;
        aBlue = rBlue/11000; //rough calculation to 0-5
        cursorColor = computeColor(aRed, aGreen, aBlue);
        if(erase_switch){
            pc.printf("Mazani     ");
        }
        else{
            if(editing_switch){
                pc.printf("Kresleni   ");
            }
            else{
                pc.printf("Pohyb      ");
            }
        }
        pc.printf("  Barva");
        changeBgColor(cursorColor);
        pc.printf("              \r");
        changeBgColor(0);
     
        
        wait(0.2f);
        //pc.printf("color %d\r\n",currColor);
        //pc.printf("row %d col %d R %d G %d B %d \r\n", cursorRow, cursorCol, aRed, aBlue, aGreen); 
     }
}



