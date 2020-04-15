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

    for (i=0; i<NUMPIXELS; i++)
    {
      pixels.setPixelColor(i,led_color);
      delay(50);
      pixels.show();
    }
    delay(100);
    
    pixels.show();
    delay(1000);
}

uint32_t colors[] = 
{ 
  COLOR_RED,
  COLOR_BLUE,
  COLOR_GREEN,
  COLOR_WHITE
};

void loop()
{
  static int color_index = 0;
  int pot_val;
  uint32_t brightness;
  uint32_t color;

  pot_val = analogRead(POT_PIN);

  /* Brightness:  we've got a 24-bit composite RGB value.
   * shift the 0-1023 from the pot to be a 1 byte value , then
   * shift and mask into the right color.  Note for white, we'll build
   * a composite.  
   */
  brightness = pot_val >> 2;

  //Serial.print("bright = ");
  //Serial.println(brightness, HEX);

  if (buttonPressed())
  {
    color_index++;
    if (color_index == 4) color_index = 0;
  }

  /* there's probably a more elegant way of doing this... */
  switch (color_index)
  {
    case 1:  // Blue
      /* Blue is the LSByte...no transform needed */
      color = brightness;

      //Serial.println("blue");
      
    break;

    case 2: // Green
      /* green is shifted up 2 bytes */
      color = brightness << 8;
      
      //Serial.println("green");
      
    break;

    case 0:  // RED
      /* red is shifed up 4 bytes */
      color = brightness << 16;

      // Serial.println("red");
      
    break;

    case 3: // White
      /* white is fun...each RGB needs to be equal */
      color = brightness;        // blue byte
      color |= brightness << 8;  //green byte
      color |= brightness << 16; //red byte

      // Serial.println("white");
    break;

    //default:
      // Serial.print("unknown color index: ");
      // Serial.println(color_index);
  }

  // Serial.print("Color = ");
  // Serial.println(color, HEX);
  
  fillAll(color);
  pixels.show();

  delay(1);
}
