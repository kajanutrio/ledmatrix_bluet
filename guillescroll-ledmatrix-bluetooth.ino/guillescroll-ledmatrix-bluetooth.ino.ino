#include <SoftwareSerial.h>

// Use the Parola library to scroll text on the display
//
// Demonstrates the use of the scrolling function to display text received
// from the serial interface
//
// User can enter text on the serial monitor and this will display as a
// scrolling message on the display.
// Speed for the display is controlled by a pot on SPEED_IN analog in.
// Scrolling direction is controlled by a switch on DIRECTION_SET digital in.
// Invert ON/OFF is set by a switch on INVERT_SET digital in.
//
// UISwitch library can be found at https://github.com/MajicDesigns/MD_UISwitch
//
// NOTE: MD_MAX72xx library must be installed and configured for the LED
// matrix type being used. Refer documentation included in the MD_MAX72xx
// library or see this link:
// https://majicdesigns.github.io/MD_MAX72XX/page_hardware.html
//

#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>


#include <SoftwareSerial.h>
SoftwareSerial SerialBLE(8, 12); // RX, TX





// Define the number of devices we have in the chain and the hardware interface
// NOTE: These pin numbers will probably not work with your hardware and may
// need to be adapted
#define MAX_DEVICES 12
#define CLK_PIN   13
#define DATA_PIN  11
#define CS_PIN    10

// HARDWARE SPI
MD_Parola P = MD_Parola(CS_PIN, MAX_DEVICES);


// Scrolling parameters
int Speed = 3;
int scrollSpeed = 25;    // default frame delay value
uint8_t fadeSpeed = 33;
textEffect_t scrollEffect = PA_SCROLL_LEFT;
textEffect_t fadeeffect = PA_FADE;
textEffect_t opncurseffect = PA_OPENING_CURSOR;
textPosition_t scrollAlign = PA_LEFT;
textPosition_t fadealign = PA_CENTER;
textPosition_t opencuralign = PA_CENTER;
int scrollPause = 1; // in milliseconds
uint16_t fadePause = 300; // in milliseconds
int dir ;

// Global message buffers shared by Serial and Scrolling functions
#define	BUF_SIZE	350
char curMessage[BUF_SIZE] = { "" };
char newMessage[BUF_SIZE] = { " automatismos, alarmas, sensores, arduino...  etc.  TFNO  677 789 490 " };
bool newMessageAvailable = true;

void setup()
{
  //Serial.begin(9600);
  SerialBLE.begin(9600); //inicializa bluetooth
  //Serial.print("\n[Parola Scrolling Display]\nType a message for the scrolling display\nEnd message line with a newline");
  P.begin();
  P.displayText(curMessage, scrollAlign, scrollSpeed, scrollPause, scrollEffect, scrollEffect);
  //P.displayText(curMessage, scrollAlign, scrollSpeed, fadePause, scrollEffect, opncurseffect);
}

void loop()
{
  if (P.displayAnimate())
  {
    if (newMessageAvailable)
    {
      //guilleadd
      String str(newMessage);
      str.replace("Ã±", "\244");//ñ
      str.replace("Ã", "\245");//Ñ
      str.replace("Ã¼", "\201");//ü
      str.replace("Â°", "\247"); //º grados  \247 \11
      str.replace("Ã¡", "\240"); //á  \240 \206
      str.replace("Âª", "\246"); //ª
      str.replace("Ã©", "\202"); //é
      str.replace("Ã­", "\241"); //í
      str.replace("Ã³", "\242"); //ó  \242
      str.replace("Ãº", "\243"); //ú   \243
      str.toCharArray(newMessage, BUF_SIZE);
      //                                              Serial.println( str);
      if (str.startsWith("kbd~:") )//si se envia texto ajusta direccion y velocidad
      {
        str.remove(0, 5);
        P.setTextEffect (PA_SCROLL_LEFT, PA_SCROLL_LEFT);
        str.toCharArray(newMessage, BUF_SIZE);
        P.setSpeed( 21);

      }


      if (str.startsWith("sldl:") )//lee slider brillo
      {
        str.remove(0, 5);
        int  intens = str.toInt();
        P.setIntensity( intens);
        strcpy(newMessage, curMessage);//conserva el ultimo mensaje y evita que salga el slider en el panel
      }
      else if (str.startsWith("slds:") )//lee slider speed
      {
        str.remove(0, 5);
        Speed = str.toInt();
        P.setSpeed( Speed);
        strcpy(newMessage, curMessage);//conserva el ultimo mensaje y evita que salga el slider en el panel
      }
      else  if (str.startsWith("butt:") )//lee direccion boton
      {
        str.remove(0, 5);
        dir = str.toInt();
        if (dir == 0) {
          P.setTextEffect (PA_SCROLL_LEFT, PA_SCROLL_LEFT);
        }
        else   if (dir >= 1) {
          P.setTextEffect (PA_SCROLL_RIGHT, PA_SCROLL_RIGHT);
        }
        strcpy(newMessage, curMessage);//conserva el ultimo mensaje y evita que salga el slider en el panel
      }
      else  if (str.startsWith("but2:") )//lee direccion boton
      {
        str.remove(0, 5);
        dir = str.toInt();
        if (dir == 0) {
          P.setTextEffect (PA_SCROLL_LEFT, PA_SCROLL_LEFT);
          str = ("<===     <==="); P.setSpeed( 8);
        }
        else   if (dir == 1) {
          P.setTextEffect (PA_SCROLL_LEFT, PA_SCROLL_LEFT);
          str = (" Peque\244os trabajos en torno y fresa, automatismos, alarmas, sensores, arduino...  etc.  TFNO  677 789 490 "); P.setSpeed(25);
        }
        else   if (dir == 2) {
          P.setTextEffect (PA_SCROLL_RIGHT, PA_SCROLL_RIGHT);
          str = ("===>     ===>"); P.setSpeed( 8);
        }
        str.toCharArray(newMessage, BUF_SIZE);
      }
      //guilleaddend
      strcpy(curMessage, newMessage);
      newMessageAvailable = false;


    }
    P.displayReset();
  }
  readBLE();

}

void readBLE(void)
{
  static  char *cp  = newMessage;
  if (SerialBLE.available())
  {
    //|| (cp - newMessage >= BUF_SIZE-2)

    *cp = (char )SerialBLE.read();
    if ((*cp == '\n') || ( newMessage >= BUF_SIZE - 2)) // end of message character or full buffer
    {
      *cp = '\0'; // end the string
      // restart the index for next filling spree and flag we have a message waiting
      cp = newMessage;
      newMessageAvailable = true;

    }
    else  // move char pointer to next position
      cp++;
  }
}
