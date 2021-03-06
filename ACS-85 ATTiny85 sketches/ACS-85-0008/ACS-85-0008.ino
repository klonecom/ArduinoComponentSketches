
/**
   ACS-85-0008
   ATTiny85  Squarewave with Vibrato

   External pin 1       = Reset (not used)
   External pin 2 (PB3) = input 0 freq 1
   External pin 3 (PB4) = none
   External pin 4       = GND
   External pin 5 (PB0) = output 0 output ramps down
   External pin 6 (PB1) = Output 1 output ramps up
   External pin 7 (PB2) = Output 2 output ramps up and down
   External pin 8       = Vcc

   V 1.0  -  First Version

   Note: This sketch has been written specifically for ATTINY85, but should
   work on Arduino uno with little if any changes.

   Rob Stave (Rob the fiddler) ccby 2015
*/

//  ATTiny overview
//                      +-\/-+
//               Reset 1|    |8  VCC
// (pin3) in 0 A3  PB3 2|    |7  PB2 (pin2) out 2
// (pin4) in 1 A2  PB4 3|    |6  PB1 (pin1) out 1
//                 GND 4|    |5  PB0 (pin0) out 0
//                      ------


#define VCO1_HIGH 50
#define VCO1_LOW 200


//counters for the frequencies
volatile uint8_t oscFreq1 = 200;
volatile uint8_t maxTrem = 0;

volatile uint8_t  oscCounter1 = 0;
volatile uint8_t  oscCounter2 = 0;
volatile uint8_t  oscCounter3 = 0;

volatile uint8_t counter1 = 1;
volatile uint8_t counter2 = 1;
volatile uint8_t counter3 = 1;

volatile boolean state = HIGH;



// the setup function runs once when you press reset or power the board
void setup() {

  DDRB = B00000111;  //set output bits

  // initialize timer1
  noInterrupts();           // disable all interrupts

  TCCR0A = 0;
  TCCR0B = 0;

  TCCR0A |= (1 << WGM01); //Start timer 1 in CTC mode Table 11.5

  OCR0A = 79; //CTC Compare value

  TCCR0B |= (1 << CS00); // Prescaler =8 Table 11.6

  // TCCR0A |=(1<<COM0A1); //Timer0 in toggle mode Table 11.2
  TIMSK |= (1 << OCIE0A); //Enable CTC interrupt see 13.3.6
  interrupts();             // enable all interrupts
}


ISR(TIMER0_COMPA_vect)          // timer compare interrupt service routine
{

  //Count up and toggle portB bits
  if (oscCounter1 <= 0) {
    oscCounter1 = oscFreq1 + (counter1 >> 3);

    counter1++;
    if (counter1 > maxTrem) {
      counter1 = 0;
    }

    PORTB ^= (_BV(PB0));
  }
  oscCounter1--;


  if (oscCounter2 <= 0) {
    oscCounter2 = oscFreq1 - (counter2 >> 3);

    counter2++;
    if (counter2 > maxTrem) {
      counter2 = 0;
    }

    PORTB ^= (_BV(PB1));
  }
  oscCounter2--;


  if (oscCounter3 <= 0) {
    oscCounter3 = oscFreq1 +  (counter3 >> 3);;

    if (state == HIGH) {
      counter3++;
      if (counter3 >= maxTrem) {
        state = LOW;
      }
    } else {
      counter3--;
      if (counter3 <= 0) {
        state = HIGH;
        counter3 = 0;
      }
    }
    PORTB ^= (_BV(PB2));
  }
  oscCounter3--;

}


void loop() {

  byte loopCount = 0;

  while (true) {
    //Read freq
    int osc1_t = analogRead(A3);
    oscFreq1 = map(osc1_t, 0, 1023, VCO1_LOW,  VCO1_HIGH);

    //Read freq...but not as often
    if (loopCount > 10) {
      loopCount = 0;
      int trem_t = analogRead(A2);
      maxTrem = map(trem_t, 0, 1023, 0,  253);
    }
    loopCount++;
  }


}
