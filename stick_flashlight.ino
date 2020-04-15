/*===========================================================
 * Tiny Stick Flashlight
 *  
 * Test code for the tiny-stick board.  
 * Uses button to change LED color.
 * Uses Potentiometer to change brightness.
 */

#include <Adafruit_NeoPixel.h>

// Which pin to use for DualRingLED control
#define LED_PIN    3

#define BUTTON_PIN 0      // Tiny
//#define BUTTON_PIN 8    // Uno

#define NUMPIXELS  8


#define POT_PIN    2   // Tiny
//#define POT_PIN    A0    // Uno

int pixelState[NUMPIXELS];
int cylonSpeed = 1;
int cylonIndex = 0;

#define CYLONSIZE 3

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, LED_PIN, NEO_GRB+NEO_KHZ800);

#define COLOR_RED     0xFF0000
#define COLOR_GREEN   0x00FF00
#define COLOR_BLUE    0x0000FF
#define COLOR_MAGENTA 0xFF00FF
#define COLOR_YELLOW  0xFFFF00
#define COLOR_CYAN    0x00FFFF
#define COLOR_BLACK   0
#define COLOR_WHITE   0xFFFFFF



/*================================================================================
 * fillAll
 */
void fillAll( uint32_t color )
{
  int i;

  for (i=0; i<NUMPIXELS; i++)
  {
    pixels.setPixelColor(i, color);
  }
}




/*================================================
 * Debounce function.
 * 
 * Only count a button press on release, and only if it's been down for a sufficient 
 * amount of time.
 *===============================================*/ 
#define DEBOUNCE_TIME_MS 50
bool buttonPressed( void )
{
  static int last_state=HIGH;
  int current_state;
  static unsigned long down_start_time=0;
  unsigned long current_time;

  current_state = digitalRead(BUTTON_PIN);

  /* Look for high-to-low transistions */
  if (last_state == HIGH)
  {
    if (current_state == LOW)
    {
      last_state = current_state;
      down_start_time = millis();
    }

    return(false);
  }
  
  else
  {
    /* look for the release with "enough time" to count a button press. */
    if (current_state == HIGH)
    {
      /* button went from low to high.  Was it down long enough? */
      current_time = millis();
      if (current_time - down_start_time > DEBOUNCE_TIME_MS)
      {
        last_state = current_state;
        return(true);
      }
      else
      {
        /* went up too quick...this is a bounce.  */
        last_state = current_state;
        return(false);
      }
    }
    else
    {
      /* We're still "low".  Waiting for release.  */
      return(false);
    }
  }  
}

void setup()
{
    int i;
    uint32_t led_color=COLOR_BLUE;

    #if 0
    // Uno debug 
    Serial.begin(9600);
    pinMode(BUTTON_PIN,INPUT_PULLUP);
    #endif
    
    pixels.begin();

    for (int i=0; i < NUMPIXELS; i++)
    {
      if (i < CYLONSIZE)
      {
         pixelState[i] = 1;
         pixels.setPixelColor(i,led_color);
      }
      else
      {
         pixelState[i] = 0; 
         pixels.setPixelColor(i,0);
      }    
     // delay(50);
      pixels.show();
    }
}

uint32_t colors[] = 
{ 
  COLOR_RED,
  COLOR_BLUE,
  COLOR_GREEN,
  COLOR_WHITE
};

void cylonShift(){
  int tempFirstPixel, tempLastPixel;
  cylonIndex = cylonIndex + cylonSpeed;
  if (cylonSpeed == 1)
  {
  for (int i = NUMPIXELS-1; i > 0; i--)
    { 
      tempLastPixel = pixelState[NUMPIXELS-1];
      pixelState[i] = pixelState[i-1];
    }
    pixelState[0] = tempLastPixel;   
  }
  else
  {
  for (int i = 0; i < NUMPIXELS-1; i++)
    { 
      tempFirstPixel = pixelState[0];
      pixelState[i]=pixelState[i+1];
    }
    pixelState[NUMPIXELS-1]=tempFirstPixel;
   }
  if ((cylonIndex >= NUMPIXELS - 1 - CYLONSIZE) || (cylonIndex <= 0))
    cylonSpeed = cylonSpeed * -1;
}

void showLEDs(){
   uint32_t led_color=COLOR_BLUE;
   for (int i=0; i < NUMPIXELS; i++)
    {
    if (pixelState[i] == 1)
      pixels.setPixelColor(i,led_color);
    else
      pixels.setPixelColor(i,0);
    }
    pixels.show();
}

void loop()
{
  cylonShift();
  showLEDs();
  delay(200);
}
