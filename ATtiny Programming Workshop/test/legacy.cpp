#include <Arduino.h>

// put function declarations here:
void fade_to_next_frame(void);
void draw_frame(void);
void light_led(char led_num);
void off_led(char led_num);
void life(void);
void start(void);
void initial_config(void);
void generate_next_led(void);
void generate_next_led_reverse(void);
char current_equals_next();
void set_random_next_frame(void);
void led_blink(void);
void generate_next_generation(void);
void initialize_frame_log();
int next_equals_logged_frame();
void log_current_frame();
void led_sequential(void);



#define LINE_A 0 //Pin 5 (PB0) on ATtiny85
#define LINE_B 1 //Pin 6 (PB1) on ATtiny85
#define LINE_C 2 //Pin 7 (PB2) on ATtiny85
#define LINE_D 3 //Pin 2 (PB3) on ATtiny85
#define LINE_E 4 //Pin 3 (PB4) on ATtiny85

// arrays to set the brightness of each led (same order as on board 1-20)
char led_grid[20] = {
 000 , 000 , 000 , 000 , 000 ,
 000 , 000 , 000 , 000 , 000 ,
 000 , 000 , 000 , 000 , 000 ,
 000 , 000 , 000 , 000 , 000
};
// use this array to create the next frame
char led_grid_next[20] = {
 000 , 000 , 000 , 000 , 000 ,
 000 , 000 , 000 , 000 , 000 ,
 000 , 000 , 000 , 000 , 000 ,
 000 , 000 , 000 , 000 , 000
};

void start(void) {
 int f;
 initial_config();

 while(1){
   led_blink();
   
   for(int i=0; i<3; i++){
     led_sequential();
   }

   life();
 }
}

void led_blink(void){
 int f, led;
 
 //fade to all-on LED grid
 for ( led=0; led<=19; led++ ) { led_grid_next[led] = 100; }
 fade_to_next_frame();
 // keeps the current state on for a bit
 for( f=0 ; f<1000 ; f++ ){ draw_frame(); }

 //fade to all-off LED grid
 for ( led=0; led<=19; led++ ) { led_grid_next[led] = 0; }
 fade_to_next_frame();
 // keeps the current state on for a bit
 for( f=0 ; f<1000 ; f++ ){ draw_frame(); }
 
 return;
}

void led_sequential(void){
 char led;
 int f, counter = 0;
 bool state = true;

 // Turn the first LED ON at the start
 led_grid_next[0] = 100;
 fade_to_next_frame();

 // loop that keeps the LEDs lit and sequentially lights up or turns off the next LED in the outline
 while(1){

   // keeps the current state on for a bit
   for( f=0 ; f<100 ; f++ ){
     draw_frame();
   }

   // check if turning off or on LEDs
   if(state){
     generate_next_led(); // light up next LEDs in the outline
   }
   else{
     generate_next_led_reverse(); // turn off next LEDs in the outline
   }

   //check if animation is finished
   if( current_equals_next() == 1 ){
     //next generated frame is the same as the current frame... "still life" achieved
     for( f=0 ; f<1500 ; f++ ){ draw_frame(); } //stay on frame for awhile
     counter++;
     state = false; //switch from turning on LEDs to turning off
     if(counter > 1){
       return; //exit this function
     } 
   }
   
   fade_to_next_frame();
 }
}

void initial_config(void){
 int i;
 for(i=0; i<20; i++){
   led_grid_next[i] = 0;
 }
 return;
}

// Generate next LED frame - adding LEDs
void generate_next_led(void){
 int i, second_side = 19;
 for(i=1; i<11; i++){
   if(led_grid[i] == 0){
     led_grid_next[i] = 100;
     led_grid_next[second_side] = 100;
     return;
   }
   
   second_side--;
 }
 
 return;
} 

// Generate next LED frame - removing LEDs
void generate_next_led_reverse(void){
 int i, second_side = 10;
 for(i=10; i>0; i--){
   if(led_grid[i] == 100){
     led_grid_next[i] = 0;
     led_grid_next[second_side] = 0;
     return;
   }
   
   second_side++;
 }
 if(led_grid[0] == 100){
   led_grid_next[0] = 0;
 }
 
 return;
} 

// function that checks if the LED states changed
char current_equals_next(){
 char led;
 for(led=0;led<=19;led++){
   if( led_grid[led] != led_grid_next[led] ){ return 0; } //a difference was found, return 0 (for False)
 }
 return 1; //no differences found, return 1 (for True)
}

// This sets up two pins as outputs to light one LED
//DDRB direction config for each LED (1 = output)
const char led_dir[20] = {
 ( 1<<LINE_A | 1<<LINE_E ), //LED 0
 ( 1<<LINE_A | 1<<LINE_D ), //LED 1
 ( 1<<LINE_A | 1<<LINE_C ), //LED 2
 ( 1<<LINE_A | 1<<LINE_B ), //LED 3
 
 ( 1<<LINE_B | 1<<LINE_E ), //LED 4
 ( 1<<LINE_B | 1<<LINE_D ), //LED 5
 ( 1<<LINE_B | 1<<LINE_C ), //LED 6
 ( 1<<LINE_B | 1<<LINE_A ), //LED 7
 
 ( 1<<LINE_C | 1<<LINE_E ), //LED 8
 ( 1<<LINE_C | 1<<LINE_D ), //LED 9
 ( 1<<LINE_C | 1<<LINE_B ), //LED 10
 ( 1<<LINE_C | 1<<LINE_A ), //LED 11
 
 ( 1<<LINE_D | 1<<LINE_E ), //LED 12
 ( 1<<LINE_D | 1<<LINE_C ), //LED 13
 ( 1<<LINE_D | 1<<LINE_B ), //LED 14
 ( 1<<LINE_D | 1<<LINE_A ), //LED 15
 
 ( 1<<LINE_E | 1<<LINE_D ), //LED 16
 ( 1<<LINE_E | 1<<LINE_C ), //LED 17
 ( 1<<LINE_E | 1<<LINE_B ), //LED 18
 ( 1<<LINE_E | 1<<LINE_A )  //LED 19
};

const char led_dir_stop[20] = {
 ( 0<<LINE_A | 0<<LINE_E ), //LED 0
 ( 0<<LINE_A | 0<<LINE_D ), //LED 1
 ( 0<<LINE_A | 0<<LINE_C ), //LED 2
 ( 0<<LINE_A | 0<<LINE_B ), //LED 3
 
 ( 0<<LINE_B | 0<<LINE_E ), //LED 4
 ( 0<<LINE_B | 0<<LINE_D ), //LED 5
 ( 0<<LINE_B | 0<<LINE_C ), //LED 6
 ( 0<<LINE_B | 0<<LINE_A ), //LED 7
 
 ( 0<<LINE_C | 0<<LINE_E ), //LED 8
 ( 0<<LINE_C | 0<<LINE_D ), //LED 9
 ( 0<<LINE_C | 0<<LINE_B ), //LED 10
 ( 0<<LINE_C | 0<<LINE_A ), //LED 11
 
 ( 0<<LINE_D | 0<<LINE_E ), //LED 12
 ( 0<<LINE_D | 0<<LINE_C ), //LED 13
 ( 0<<LINE_D | 0<<LINE_B ), //LED 14
 ( 0<<LINE_D | 0<<LINE_A ), //LED 15
 
 ( 0<<LINE_E | 0<<LINE_D ), //LED 16
 ( 0<<LINE_E | 0<<LINE_C ), //LED 17
 ( 0<<LINE_E | 0<<LINE_B ), //LED 18
 ( 0<<LINE_E | 0<<LINE_A )  //LED 19
};

// This Defines which pin should be high to light an LED
//PORTB output config for each LED (1 = High, 0 = Low)
const char led_out[20] = {
 ( 1<<LINE_A ), //LED 0
 ( 1<<LINE_A ), //LED 1
 ( 1<<LINE_A ), //LED 2
 ( 1<<LINE_A ), //LED 3
 
 ( 1<<LINE_B ), //LED 4
 ( 1<<LINE_B ), //LED 5
 ( 1<<LINE_B ), //LED 6
 ( 1<<LINE_B ), //LED 7
 
 ( 1<<LINE_C ), //LED 8
 ( 1<<LINE_C ), //LED 9
 ( 1<<LINE_C ), //LED 10
 ( 1<<LINE_C ), //LED 11
 
 ( 1<<LINE_D ), //LED 12
 ( 1<<LINE_D ), //LED 13
 ( 1<<LINE_D ), //LED 14
 ( 1<<LINE_D ), //LED 15
 
 ( 1<<LINE_E ), //LED 16
 ( 1<<LINE_E ), //LED 17
 ( 1<<LINE_E ), //LED 18
 ( 1<<LINE_E )  //LED 19
};

const char led_out_stop[20] = {
 ( 0<<LINE_A ), //LED 0
 ( 0<<LINE_A ), //LED 1
 ( 0<<LINE_A ), //LED 2
 ( 0<<LINE_A ), //LED 3
 
 ( 0<<LINE_B ), //LED 4
 ( 0<<LINE_B ), //LED 5
 ( 0<<LINE_B ), //LED 6
 ( 0<<LINE_B ), //LED 7
 
 ( 0<<LINE_C ), //LED 8
 ( 0<<LINE_C ), //LED 9
 ( 0<<LINE_C ), //LED 10
 ( 0<<LINE_C ), //LED 11
 
 ( 0<<LINE_D ), //LED 12
 ( 0<<LINE_D ), //LED 13
 ( 0<<LINE_D ), //LED 14
 ( 0<<LINE_D ), //LED 15
 
 ( 0<<LINE_E ), //LED 16
 ( 0<<LINE_E ), //LED 17
 ( 0<<LINE_E ), //LED 18
 ( 0<<LINE_E )  //LED 19
};

void light_led(char led_num) { //led_num must be from 0 to 19
 DDRB = led_dir[led_num];
 PORTB = led_out[led_num];
}

void off_led(char led_num){ //led_num must be from 0 to 19
 PORTB = led_out_stop[led_num];
 DDRB = led_dir_stop[led_num];
}

void leds_off() { // turns all pins off
 DDRB = 0;
 PORTB = 0;  
}

void draw_frame(void){ // displays the current frame once
 char led, bright_val, b;
 for ( led=0; led<=19; led++ ) {
   //software PWM
   bright_val = led_grid[led];
   for( b=0 ; b < bright_val ; b+=4 ) { light_led(led); } //delay while on
   off_led(led);
   for( b=bright_val ; b<100 ; b+=4 ) { leds_off(); } //delay while off
 }
}

void fade_to_next_frame(void){ //gradually adjusts the brightness of the LEDs to their new values
 char led, changes;  
 
 while(1){
   changes = 0;
   for ( led=0; led<=19; led++ ) {
     if( led_grid[led] < led_grid_next[led] ){ led_grid[led] = (led_grid[led]+1); changes++; }
     if( led_grid[led] > led_grid_next[led] ){ led_grid[led] = (led_grid[led]-1); changes++; }
     
   }
   draw_frame();
   if( changes == 0 ){break;}
 }
}

char current_living_cells(){
 char led, cells;
 for ( led=0; led<=19; led++ ) {
   if( led_grid[led] > 0 ){ cells++; }
 }
 return cells;
}

// this function creates a "random" output
void life(void){

 char led;
 int counter = 0;

 while(1){
   int f, frame_number;
   frame_number = 0;
   
   initialize_frame_log();
   
   //fade to all-on LED grid
   for ( led=0; led<=19; led++ ) { led_grid_next[led] = 100; }
   fade_to_next_frame();
   for( f=0 ; f<300 ; f++ ){ draw_frame(); } //display this frame for awhile
   
   //fade to random start frame
   set_random_next_frame();
   fade_to_next_frame();
   counter++;
   
   while(1){
     
     if( frame_number==0 ){ log_current_frame(); } //log every 20th frame, compare future frames against it to check for oscillators
     
     for( f=0 ; f<300 ; f++ ){
       draw_frame();
     }
     
     generate_next_generation();    
     
     if( current_equals_next() == 1 ){
       //next generated frame is the same as the current frame... "still life" achieved
       for( f=0 ; f<1500 ; f++ ){ draw_frame(); } //stay on frame for awhile
       break; //exit this loop & start over
     }
     //next frame is not the same as the last
     
     if( next_equals_logged_frame() == 1 ){
       //next frame matches one we've had before, we have a repeating pattern
       //show the next frame
       fade_to_next_frame();
       for( f=0 ; f<300 ; f++ ){ draw_frame(); }
       break; //exit this loop & start over
     }
     
     fade_to_next_frame();
     frame_number++;
     if( frame_number==20 ){ frame_number = 0; }
     
     if( current_living_cells() == 0 ){
       for( f=0 ; f<1500 ; f++ ){ draw_frame(); } //stay on dead frame for awhile
       break; //all cells dead, exit this loop & start over
     }
     
   }

   if(counter > 15){
     return;
   }
   
 }

}

//----- Frame log -----

char frame_log[20];

void initialize_frame_log(){
 char cell;
 for( cell=0 ; cell<20 ; cell++ ){
   frame_log[cell] = -1;
 }
}

void log_current_frame(){
 char cell;
 for( cell=0 ; cell<20 ; cell++ ){
   frame_log[cell] = led_grid[cell];
 }
}

int next_equals_logged_frame(){
 char cell, diffs;
 diffs = 0;
 for( cell=0 ; cell<=19 ; cell++ ){
   if( led_grid_next[cell] != frame_log[cell] ){ return 0; } //a difference was found, return 0 (for False)
 }
 return 1; //no differences found, return 1 (for True)
}

//----- end frame log -----


void set_led_next_xy( char col, char row, char value ){
 //5 columns, 4 rows, zero-indexed
 led_grid_next[((5*row)+col)] = value;
}

char get_led_xy( char col, char row ){
 //5 columns, 4 rows, zero-indexed
 if( col<0 | col>4 ){return 0;}
 if( row<0 | row>3 ){return 0;}
 return led_grid[((5*row)+col)];
}


/*
* pseudorandom - From Instructables: http://www.instructables.com/id/Flickering-LED-Candle/
* return the next pseudo-random number (PRN) using a standard maximum
* length xor-feedback 16-bit shift register.
* This returns the number from 1 to 65535 in a fixed but apparently
* random sequence.  No one number repeats.
*/
uint16_t randreg = 222; //seed value

static uint16_t pseudorandom16 (void)
{
   uint16_t newbit = 0;

   if (randreg == 0) {
       randreg = 1;
   }
   if (randreg & 0x8000) newbit = 1;
   if (randreg & 0x4000) newbit ^= 1;
   if (randreg & 0x1000) newbit ^= 1;
   if (randreg & 0x0008) newbit ^= 1;
   randreg = (randreg << 1) + newbit;
   return randreg;
} 


void set_random_next_frame(void){
 char i;
 
 //clear LED Grid
 for ( i=0; i<=19; i++ ) {
   led_grid_next[i] = 0;
 }
 
 char total, cell;
 total = ((pseudorandom16() % 16)+5); //yield random from 5 to 20
 
 //set random cells
 for( i=0 ; i<total ; i++ ) {
   cell = (pseudorandom16() % 20);
   led_grid_next[cell] = 100;
 }
}


void generate_next_generation(void){  //looks at current generation, writes to next generation array
 char row, col, neighbors;
 for ( row=0; row<=3; row++ ) {
   for ( col=0; col<=4; col++ ) {
     //look at cell (col,row) in current generation
     
     //count the number of current neighbors
     neighbors = 0;
     if( get_led_xy((col-1),(row-1)) > 0 ) { neighbors++; } //NW
     if( get_led_xy(( col ),(row-1)) > 0 ) { neighbors++; } //N
     if( get_led_xy((col+1),(row-1)) > 0 ) { neighbors++; } //NE
     if( get_led_xy((col-1),( row )) > 0 ) { neighbors++; } //W
     if( get_led_xy((col+1),( row )) > 0 ) { neighbors++; } //E
     if( get_led_xy((col-1),(row+1)) > 0 ) { neighbors++; } //SW
     if( get_led_xy(( col ),(row+1)) > 0 ) { neighbors++; } //S
     if( get_led_xy((col+1),(row+1)) > 0 ) { neighbors++; } //SE
     
     if( get_led_xy(col,row) > 0 ){
       
       //current cell is alive
       if( neighbors < 2 ){
         //Any live cell with fewer than two live neighbours dies, as if caused by under-population.
         set_led_next_xy( col , row , 0 );
       }
       if( (neighbors == 2) || (neighbors == 3) ){
         //Any live cell with two or three live neighbours lives on to the next generation.
         set_led_next_xy( col , row , 100 );
       }
       if( neighbors > 3 ){
         //Any live cell with more than three live neighbours dies, as if by overcrowding.
         set_led_next_xy( col , row , 0 );
       }
       
     } else {
       
       //current cell is dead
       if( neighbors == 3 ){
         // Any dead cell with exactly three live neighbours becomes a live cell, as if by reproduction.
         set_led_next_xy( col , row , 100 );
       } else {
         //stay dead for next generation
         set_led_next_xy( col , row , 0 );
       }
       
     }
   }
 }
}
void setup() {
  // put your setup code here, to run once:
  cli(); // Disable interrupts
  CLKPR = (1<<CLKPCE); // Prescaler enable
  CLKPR = 0x00; // Clock division factor
  sei(); // Enable interrupts
  start();
}

void loop() {
 // put your main code here, to run repeatedly:
}

