/*
  PASSifier! One Button Keyboard
  Copyright (C) 2014 Andrew J. Armstrong

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
  02111-1307  USA

  Author:
  Andrew J. Armstrong <androidarmstrong@gmail.com>
  
*/

/*
-------------------------------------------------------------------------------

NAME     - PASSifier One Button Keyboard

FUNCTION - This is a USB keyboard that can record a sequence of keystrokes (and
           other USB functions) and then play that sequence back at the press of
           a button.

FEATURES - 1. Single rotary knob is the only input.
           2. Can record and replay up to xxx keystrokes.
           3. Absolutely NO HOST DRIVERS required.

PIN USAGE -                      PIC18F25K50
                            .------------------.
             MCLR       --> | RE3  1    28 RB7 | <-- PGD            |
             LED        <-- | RA0  2    27 RB6 | <-- PGC            |
                        <-- | RA1  3    26 RB5 | <-- Rotary B       | PORTB
                        <-- | RA2  4    25 RB4 | <-- Rotary A       | Weak
                        <-- | RA3  5    24 RB3 | <-- Rotary  BUTTON | Pullups
                        <-- | RA4  6    23 RB2 | <--                | Enabled
                        <-- | RA5  7    22 RB1 | <--                |
             Ground     --- | VSS  8    21 RB0 | <--                |
             n/c        --- | RA7  9    20 VDD | --- +5V
                        <-- | RA6  10   19 VSS | --- Ground
                        <-- | RC0  11   18 RC7 | --> RX
                        <-- | RC1  12   17 RC6 | --> TX
                        <-- | RC2  13   16 RC5 | <-> USB D+
             220nF      --- | VUSB 14   15 RC4 | <-> USB D-
                            '------------------'

            Rotary Encoder (e.g. ALPS EC11K0924401):

                            .-------.
             Rotary A   <-- |A     D|--> Rotary button
             Ground     <-- |C  o / |
             Rotary B   <-- |B     E|--> Ground
                            '-------'

CONFIG    - The PIC18F25K50 configuration fuses should be set as follows (items
            marked with '*' are essential for this program to operate correctly):

            CONFIG1L 2B 00101011
                          1      = *LS48MHZ: System clock is expected at 48 MHz, FS/LS USB clock divide-by is set to 8
                           01    = *CPUDIV: CPU system clock (48 MHz) divided by 2 (= 24 MHZ)
                             0   =  Unimplemented
                              x  =  CFGPLLEN: (ignored, set dynamically)
                               x =  PLLSEL: (ignored, set dynamically)
            CONFIG1H 28 00101000
                        0        =  IESO: Oscillator Switchover mode disabled
                         0       =  FCMEN: Fail-Safe Clock Monitor disabled
                          1      = *PCLKEN: Primary Clock is always enabled
                           0     =  Unimplemented
                            1000 = *FOSC: Internal oscillator block
            CONFIG2L 5F 01011111
                        0        =  Unimplemented
                         1       =  LPBOR: Low-Power Brown-out Reset disabled
                          0      =  Unimplemented
                           11    =  BORV: VBOR set to 1.9V nominal
                             11  =  BOREN: Brown-out Reset enabled in hardware only (SBOREN is disabled)
                               1 =  PWRTEN: Power-up Timer disabled
            CONFIG2H 3C 00111100
                        00       =  Unimplemented
                          1111   =  WDTPS: Watchdog Timer Postscale Select = 1:32768
                              00 =  WDTEN: Watchdog Timer disabled in hardware, SWDTEN bit disabled
            CONFIG3L 00 00000000
                        00000000 =  Unimplemented
            CONFIG3H 53 01010000
                        0        = *MCLRE: RE3 input pin enabled; MCLR disabled
                         1       =  SDOMX: SDO is on RB3
                          0      =  Unimplemented
                           1     =  T3CMX: T3CKI is on RC0
                            00   =  Unimplemented
                              0  =  PDADEN: ANSELB<5:0> resets to 0, PORTB<4:0> pins are configured as digital I/O on Reset
                               0 =  CCP2MX: CCP2 input/output is multiplexed with RB3
            CONFIG4L 81 10000001 =  Default
            CONFIG5L 0F 00001111 =  Default
            CONFIG5H C0 11000000 =  Default
            CONFIG6L 0F 00001111 =  Default

OPERATION - The user interacts with the system via a digital encoder knob (with
            a built-in switch) and, when in programming mode, with a host-based
            text editor program than understands fairly standard editing control
            sequences.

            In run mode (the default mode) the user simply presses the rotary
            knob to cause the previously programmed sequence of keystrokes to be
            sent to the host over the connecting USB cable.

            If the user presses the knob down for more than about 3 seconds,
            then the device enters the programming mode. The user must already
            have started and given focus to the above-mentioned text editor
            host application because this will be the means of communication
            with the user.

            - Start a text editor program on the host computer and ensure it
              has keyboard focus.

            - Select the password number in some way? (TODO)

            - Press and hold the rotary encoder to enter "programming mode" for
              that password. The following prompt will be keyed into the text
              editor:

              PASSifier v.m
              0    Add Keystroke
              0000

            - Rotate the knob to choose between the available USB "usage pages"
              which are currently:
              Add Keystroke
              Add Consumer Device function
              Add System Control function
              Exit and save list
              Exit and discard list
              Add Delay in Seconds

            - Press the knob to select the desired "page".

            - Rotate the knob until the desired "usage" withing the selected
              "page" is displayed in the text editor program:

              0040 a

            - In "Add keystroke" page, you can choose a combination of keystroke
              modifiers (Ctl, Alt, Shift, or GUI) by pressing and holding the
              knob **while rotating it**:

              0240 SHIFT+A

            - Press the knob to append the displayed keystroke to the list of
              keystrokes (or functions) to be recorded.

            - If you wait for about 3 seconds without rotating or pressing the
              knob, then focus will return to the "page" selection step.

            - Select the "Exit and save list" page (or quit by selecting the "Exit
              and discard list" page). You are now back into the default "run"
              mode. When you press the knob, the recorded list of keystrokes
              will be replayed.







NOTES    - 1. The source code is written in MikroC Pro from mikroe.com

           2. Ensure that total RAM usage does not exceed 1k (4 x 256 byte banks).
              If this is not done, then the compiler may allocate variables to
              banks 4-7 which are also used by the USB module causing unpredictable
              results.

           3. Variable prefix convention:
              Prefix Short for       Width  Type
              ------ --------------- ------ ----------------
                b    boolean          1 bit flag
                n    number           8 bit unsigned integer
                w    word            16 bit unsigned integer
                i    int             16 bit signed integer
                l    long            32 bit signed integer
               ul    unsigned long   32 bit unsigned integer
                c    char             8 bit character
                s    string           8 bit character array


REFERENCE - USB Human Interface Device Usage Tables at:
            http://www.usb.org/developers/devclass_docs/Hut1_12v2.pdf

            USB Device Class Definition for Human Interface Devices (HID) at:
            http://www.usb.org/developers/devclass_docs/HID1_11.pdf

            USB 2.0 Specification at:
            http://www.usb.org/developers/docs/usb_20_101111.zip
            Specifically, the file in that zip archive called usb_20.pdf

AUTHORS  - Init Name                 Email
           ---- -------------------- ------------------------------------------
           AJA  Andrew J. Armstrong  androidarmstrong@gmail.com

CREDITS  - Rotary encoder logic based on code originally written by Ben Buxton.
           Copyright 2011 Ben Buxton. Licenced under the GNU GPL Version 3.
           http://www.buxtronix.net/2011/10/rotary-encoders-done-properly.html


HISTORY  - Date     Ver   By  Reason (most recent at the top please)
           -------- ----- --- -------------------------------------------------
           20140817 1.00  AJA Initial version

-------------------------------------------------------------------------------
*/
#include <stdint.h>
#include <built_in.h>
#include "USBdsc.h"
#include "passifier.h"


const char * getKeyDescWithNoShift (uint8_t key) // Note: Literals returned as const are in ROM
{ // Keyboard key without SHIFT modifier key pressed
  if (key < ELEMENTS(UNSHIFTED_USB_DESC))
  {
    return UNSHIFTED_USB_DESC[key];
  }
  else
  {
    return "";
  }
}

const char * getKeyDescWithShift(uint8_t key) // Note: Literals returned as const are in ROM
{ // Keyboard key with SHIFT modifier key pressed
  if (key < ELEMENTS(SHIFTED_USB_DESC))
  {
    return SHIFTED_USB_DESC[key];
  }
  else
  {
    return "";
  }
}


uint16_t getNextConsumer (t_consumerDeviceAction * pAction)
{
  uint16_t i;
  for (i = pAction->usage+1; i < ELEMENTS(CONSUMER_DEVICE_DESC) && *CONSUMER_DEVICE_DESC[i] == '\0'; i++);
  if (i >= ELEMENTS(CONSUMER_DEVICE_DESC))
  {
    for (i = 0; i < pAction->usage && *CONSUMER_DEVICE_DESC[i] == '\0'; i++);
  }
  return i;
}

uint16_t getPrevConsumer (t_consumerDeviceAction * pAction)
{
  uint16_t i;
  for (i = pAction->usage-1; i > 0 && *CONSUMER_DEVICE_DESC[i] == '\0'; i--);
  if (i == 0)
  {
    for (i = ELEMENTS(CONSUMER_DEVICE_DESC); i > pAction->usage && *CONSUMER_DEVICE_DESC[i] == '\0'; i--);
  }
  return i;
}

const char * getUsageDesc (t_action * pAction) // Note: Literals returned as const are in ROM
{
  uint16_t i;
  switch (pAction->key.page)
  {
    case PAGE_KEYBOARD:
      if (pAction->key.mod.bits.LeftShift)
      {
        const char * pKeyDescWithShift = getKeyDescWithShift(pAction->key.usage);
        if (*pKeyDescWithShift)
          return pKeyDescWithShift;
      }
      return getKeyDescWithNoShift(pAction->key.usage);

    case PAGE_SYSTEM_CONTROL:
      if (pAction->sys.usage < ELEMENTS(SYSTEM_CONTROL_DESC))
      {
        return SYSTEM_CONTROL_DESC[pAction->sys.usage];
      }
      else
        return "";

    case PAGE_CONSUMER_DEVICE:
      if (pAction->cons.usage < ELEMENTS(CONSUMER_DEVICE_DESC))
      {
        return CONSUMER_DEVICE_DESC[pAction->cons.usage];
      }
      else
        return "";
//  case 0x30:     // Reserved
//  case 0x40:     // Reserved
//  case 0x50:     // Reserved
//  case 0x60:     // Reserved
//  case 0x70:     // Reserved
//  case 0x90:     // Reserved
//  case 0x90:     // Reserved
//  case 0xA0:     // Reserved
//  case 0xB0:     // Reserved
//  case 0xC0:     // Reserved
//  case 0xD0:     // Reserved
//  case 0xE0:     // Reserved
    default: return "";
  }
}

void toPrintableHex(uint8_t c, char * p)
{
  static const char HEX[] = "0123456789ABCDEF";
  *p++ = HEX[(c & 0xF0) >> 4];
  *p++ = HEX[(c & 0x0F)     ];
  *p   = '\0';
}


void sendUSBSystemControlCommand()
{
  if (!bUSBReady) return;
  usbToHost[0] = REPORT_ID_SYSTEM_CONTROL;  // Report Id = System Control
  usbToHost[1] = usbData.s.yy;              // Function requested
  while(!HID_Write(&usbToHost, 2));         // Copy to USB buffer and try to send
  usbToHost[1] = 0;                         // No function requested anymore
  while(!HID_Write(&usbToHost, 2));         // Copy to USB buffer and try to send
}

void sendUSBConsumerDeviceCommand()
{
  if (!bUSBReady) return;
  usbToHost[0] = REPORT_ID_CONSUMER_DEVICE; // Report Id = Consumer Device
  usbToHost[1] = usbData.s.yy;              // Function requested (low byte)
  usbToHost[2] = usbData.s.xx.byte & 0x0F;  // Function requested (high byte)
  while(!HID_Write(&usbToHost, 3));         // Copy to USB buffer and try to send
  usbToHost[1] = 0;                         // Function requested low byte
  usbToHost[2] = 0;                         // Function requested high byte
  while(!HID_Write(&usbToHost, 3));         // Copy to USB buffer and try to send
}

void sendUSBKeystroke()
{
  if (!bUSBReady) return;
  usbToHost[0] = REPORT_ID_KEYBOARD;        // Report Id = Keyboard
  usbToHost[1] = usbData.s.xx.byte;         // Ctrl/Alt/Shift modifiers
  usbToHost[2] = 0;                         // Reserved for OEM
  usbToHost[3] = usbData.s.yy;              // Key pressed
  while(!HID_Write(&usbToHost, 4));         // Copy to USB buffer and try to send
  usbToHost[1] = 0;                         // No modifiers now
  usbToHost[3] = 0;                         // No key pressed now
  while(!HID_Write(&usbToHost, 4));         // Copy to USB buffer and try to send
}

void say(uint8_t * p)
{
  uint8_t c;
  while (*p)
  {
    usbData.s.xx.byte = 0;
    if (*p < sizeof(ASCII_to_USB))
    {
      c = ASCII_to_USB[*p];
      if (c)
      {
        if (c & 0b10000000) // If SHIFT key needed
        {
          usbData.s.xx.bits.LeftShift = 1;
          usbData.s.yy = c & 0b01111111;
        }
        else
        {
          usbData.s.yy = c;
        }
      }
      else
      {
        usbData.s.yy = 0;
      }
    }
    else
    {
      c = *p;
    }
    sendUSBKeystroke();
    p++;
  }
}

void sayConst(const uint8_t * p)
{
  say(_TEXT(p));
}

void sayModifiers(t_keyboardAction * pAction)
{
  if (pAction->mod.bits.LeftGUI)     sayConst("GUI+");
  if (pAction->mod.bits.LeftControl) sayConst("CTL+");
  if (pAction->mod.bits.LeftAlt)     sayConst("ALT+");
  if (pAction->mod.bits.LeftShift)   sayConst("SHIFT+");
}

void sayHex(uint8_t c)
{
  char xString[3];
  toPrintableHex(c, xString);
  say(xString);
}


void sayKey(uint8_t modifiers, uint8_t key)
{
  if (!bUSBReady) return;
  usbToHost[0] = REPORT_ID_KEYBOARD;      // Report Id = Keyboard
  usbToHost[1] = modifiers;               // Ctrl/Alt/Shift modifiers
  usbToHost[2] = 0;                       // Reserved for OEM
  usbToHost[3] = key;                     // Key pressed
  while(!HID_Write(&usbToHost, 4));       // Copy to USB buffer and try to send
  usbToHost[1] = 0;                       // No modifiers now
  usbToHost[3] = 0;                       // No key pressed now
  while(!HID_Write(&usbToHost, 4));       // Copy to USB buffer and try to send
}

void resetKeyboardState() // ...this doesn't really work (with Ubuntu anyway)
{
  t_ledIndicators leds;
  if (!bUSBReady) return;
  sayKey(0, CAPS_LOCK); // Elicit LED indicator response
  // Read the state of the CapsLock, NumLock and ScrollLock LEDs
  if (HID_Read())
  {
    if (usbFromHost[0] == REPORT_ID_KEYBOARD)
    {
      leds.byte = usbFromHost[1];
      sayConst("L");
      sayHex(leds.byte);
      if (!leds.bits.NumLock)   sayKey(0, NUM_LOCK);      // Turn on num lock
      if (leds.bits.CapsLock)
      {
        sayKey(0, CAPS_LOCK);     // Turn off caps lock
        sayConst("z\n");
      }
      if (leds.bits.ScrollLock) sayKey(0, SCROLL_LOCK);   // Turn off scroll lock
    }
  }
}

void enableUSB()
{
  uint8_t i;

  usbToHost[0] = REPORT_ID_KEYBOARD;     // Report Id = Keyboard
  usbToHost[1] = 0;                      // No modifiers
  usbToHost[2] = 0;                      // Reserved for OEM
  usbToHost[3] = 0;                      // No key pressed
  bUSBReady = FALSE;
  while (!bUSBReady)
  {
    HID_Enable(&usbFromHost, &usbToHost);
    for (i=0; !bUSBReady && i < 50; i++) // Keep trying for up to 50 x 100 ms = 5 seconds
    {
      Delay_ms(50);
      LED = ON;
      Delay_ms(50);
      LED = OFF;
      bUSBReady = HID_Write(&usbToHost, 4) != 0; // Copy to USB buffer and try to send
    }
    if (!bUSBReady)
    {
      HID_Disable();
      Delay_ms(5000);
    }
  }
  Delay_ms(250);
  //resetKeyboardState();
}

void disableUSB()
{
  HID_Disable();
  bUSBReady = FALSE;
}








void Prolog()
{
  uint8_t i;
  t_action * p;

  ANSELA = 0b00000000;    // Configure all PORTA bits as digital
  ANSELB = 0b00000000;    // Configure all PORTB bits as digital
  ANSELC = 0b00000000;    // Configure all PORTC bits as digital

  //        76543210
  TRISA = 0b10111110;     // 1=Input, 0=Output
  TRISB = 0b11111111;
  TRISC = 0b00000111;

  LATA = 0;
  LATB = 0;
  LATC = 0;

  NOT_RBPU_bit = 0;       // Enable PORTB weak pull-ups (PIC 18F25K50)

  OSCCON = 0b01110000;
//           x              0  = IDLEN: Sleep on SLEEP instruction
//            xxx           111= IRCF: HFINTOSC (16 MHz)
//               x          0  = OSTS: Status bit
//                x         0  = HFIOFS: Status bit
//                 xx       00 = SCS: Primary clock (determined by FOSC in CONFIG1H)
  while (!HFIOFS_bit);    // Wait for HFINTOSC to stabilise
  while (!OSTS_bit);      // Wait until FOSC<3:0> is being used

// You can enable the PLL and set the PLL multipler two ways:
// 1. At compile time - setting configuration fuses: CFGPLLEN and PLLSEL
//    ...but only if you use an External Clock (EC).
// 2. At run time - setting OSCCON2.PLLEN and OSCTUNE.SPLLMULT
//    ...but only if you use a High Speed crystal (HS), or
//       the High Frequency Internal Oscillator (HFINTOSC).
// This project uses the HFINTOSC set to run at 16 MHz and the only valid
// PLL multiplier for USB is x3.
  PLLEN_bit = 0;          // OSCCON2.PLLEN = 0:    Disable the PLL, then...
  SPLLMULT_bit = 1;       // OSCTUNE.SPLLMULT = 1: Set the PLL multiplier to x3
  PLLEN_bit = 1;          // OSCCON2.PLLEN = 1:    Now re-enable the PLL
  while(!PLLRDY_bit);     // Wait for PLL to lock

  ACTEN_bit = 0;          // Disable Active Clock Tuning, then...
  ACTSRC_bit = 1;         // Tune HFINTOSC to match USB host clock
  ACTEN_bit = 1;          // Enable Active Clock Tuning
//  while (!ACTLOCK_bit);   // Wait until HFINTOSC is successfully tuned

  cFlags = 0;             // Reset all flags
  usbData.xxyy = 0;

  // Set up USB
  UPUEN_bit = 1;          // USB On-chip pull-up enable
  FSEN_bit = 1;           // 1 = USB Full Speed enabled (requires 48 MHz USB clock)
                          // 0 = USB Full Speed disabled (requires 6 MHz USB clock)

  // Enable Interrupt On Change interrupts for the rotary encoder
  IOCIF_bit = 0;          // Clear IOC interrupt flag
  IOCB6_bit = 1;          // Enable IOC interrupts for rotary switch button
  IOCB5_bit = 1;          // Enable IOC interrupts for rotary switch pin A
  IOCB4_bit = 1;          // Enable IOC interrupts for rotary switch pin B

// Timer3 is used for human-scale delays
  T3CON   = 0b00110010;
//            xx             00 = TMR3CS: Timer3 clock source is instruction clock (Fosc/4)
//              xx           11 = TMR3PS: Timer3 prescale value is 1:8
//                x          0  = SOSCEN: Secondary Oscillator disabled
//                 x         0  = T3SYNC: Ignored because TMR3CS = 0x
//                  x        1  = RD16:   Enables register read/write of Timer3 in one 16-bit operation
//                   x       0  = TMR3ON: Timer3 is off
// Timer3 tick rate = 48 MHz crystal/4/8 = 1.5 MHz (667 ns)

//----------------------------------------------------------------------------
// Let the interrupts begin
//----------------------------------------------------------------------------

  PIE2.TMR3IE = 1;        // Enable Timer3 interrupts
  IOCIE_bit = 1;          // Enable PORTB/C Interrupt On Change interrupts
  PEIE_bit = 1;           // Enable peripheral interrupts
  GIE_bit = 1;            // Enable global interrupts

  button = 0;
  LED = OFF;

  nAction = 0; // Number of entries
  for (i = 0, p = &aAction[0]; i < ELEMENTS(aAction); i++, p++)
  {
    p->action = 0;
  }
  action.key.page = PAGE_KEYBOARD;
  action.key.usage = USB_KEY_A;
  action.key.mod.value = 0;
  rotation = 0;  // Indicate rotary event handled
  nActionFocus = 0; // Entry with the current focus

  enableUSB();            // Enable USB interface
}



/*
       .----- 00 -----.
       |              |
       |              |
      10      BA      01
       |              |
       |              |
       '----- 11 -----'
*/
#define EVENT_MASK 0b11000000
#define STATE_MASK 0b00111111
#define DIR_CW     0b01000000 // Positive = Clockwise step
#define DIR_AC     0b10000000 // Negative = Anti-clockwise step

typedef enum {BEG = 0, CW1, CW2, CW3, AC1, AC2, AC3} t_encoderState;

volatile t_encoderState state = BEG;

const t_encoderState stateArray[] =
{
/* 00,  01,  10,  11 */
  BEG,  AC1, CW1, BEG,         // BEG Initial state (direction unknown)

  CW2,  BEG, CW1, BEG,         // CW1 First transition clockwise
  CW2,  CW3, CW2, BEG,         // CW2 Second transition clockwise
  CW2,  CW3, BEG, BEG|DIR_CW,  // CW3 Third transition clockwise

  AC2,  AC1, BEG, BEG,         // AC1 First transition anticlockwise
  AC2,  AC1, AC3, BEG,         // AC2 Second transition anticlockwise
  AC2,  BEG, AC3, BEG|DIR_AC,  // AC3 Third transition anticlockwise
};


void selectAll()
{
  sayKey(CTL, HOME);
  sayKey(CTL|SHIFT, END);
}

void selectLine(uint8_t line)
{
  sayKey(CTL, HOME);
  while (--line)
  {
    sayKey(NONE, DOWN);
  }
  sayKey(SHIFT, END);
}

void gotoLine(uint8_t line)
{
  sayKey(CTL, HOME);
  while (--line)
  {
    sayKey(NONE, DOWN);
  }
  sayKey(NONE, END);
}

void deleteLastLine()
{
  sayKey(CTL, END);
  sayKey(SHIFT, HOME);
  sayKey(NONE, DELETE);
  sayKey(NONE, BACKSPACE);
}

const char * getPageDesc (uint8_t page)
{
  switch (action.key.page)
  {
    case PAGE_KEYBOARD:         return "Set Keystroke";
    case PAGE_CONSUMER_DEVICE:  return "Set Consumer Device Command";
    case PAGE_SYSTEM_CONTROL:   return "Set System Control Command";
    case PAGE_CANCEL:           return "Cancel and Exit";
    case PAGE_REDISPLAY:        return "Redisplay";
    case PAGE_EXIT:             return "Save and Exit";
    case PAGE_LOCAL_FUNCTION:   return "Set Local Function";
    default:                    return "";
  }
}

void sayPage()
{
  char xWork[3];

  selectLine(SELECTION_LINE);
  sayHex(nActionFocus);
  sayConst(" ");
  toPrintableHex(action.key.page, &xWork);
  say(&xWork[1]);
  sayConst("xxx ");
  sayConst(getPageDesc(action.key.page));
  sayKey(SHIFT, HOME);   // Highlight this selection
}

void sayUsage(uint8_t nAction, t_action * pAction)
{
  char * p;
  char sWork[4]; // "nnn"

  sayHex(nAction);
  sayKey(NONE, SPACE);
  sayHex(pAction->key.page<<4 | pAction->key.mod.value);
  sayHex(pAction->key.usage);
  sayKey(NONE, SPACE);
  switch (pAction->key.page)
  {
    case PAGE_KEYBOARD:
      sayModifiers((t_keyboardAction *)pAction);
      sayConst(getUsageDesc(pAction));
      break;
    case PAGE_LOCAL_FUNCTION:
      switch (pAction->key.mod.value)
      {
        case LOCAL_FUNCTION_WAIT_SEC:
          sayConst("Wait ");
          ByteToStr(pAction->key.usage, &sWork);
          for (p=&sWork; *p == ' '; p++);  // Find first non-blank
          say(p);
          sayConst(" sec");
          break;
        case LOCAL_FUNCTION_WAIT_MS:
          sayConst("Wait ");
          ByteToStr(pAction->key.usage, &sWork);
          for (p=&sWork; *p == ' '; p++);  // Find first non-blank
          say(p);
          sayConst(" ms");
          break;
        case LOCAL_FUNCTION_GOTO:
          sayConst("Goto ");
          sayHex(pAction->key.usage);
          break;
        default:
          break;
      }
      break;
    default:
      sayConst(getUsageDesc(pAction));
      break;
  }
}

void newLine()
{
  sayKey(NONE, ENTER);
  sayKey(NONE, HOME);
}


void sayAction(uint8_t n)
{
  sayUsage(n, &aAction[n]);
}

void sayActions()
{
  uint8_t i;
  t_action * pAction;

  sayKey(CTL, END);
  for (i = 0; i < nAction; i++)
  {
    newLine();
    sayAction(i);
  }
}

void nextKnownPage(int8_t rotation)
{
  if (rotation)
  {
    do
    {
      action.key.page = 0x0F & (rotation > 0 ? action.key.page+1 : action.key.page-1);
    }
    while (!*getPageDesc(action.key.page));
  }
}


void nextKnownUsage(int8_t rotation)
{
  if (action.key.page == PAGE_CONSUMER_DEVICE)
  {
    if (rotation > 0)
      action.cons.usage = getNextConsumer(&action.cons);    // 12-bit consumer device usage
    else
      action.cons.usage = getPrevConsumer(&action.cons);    // 12-bit consumer device usage
  }
  else
  {
    rotation > 0 ? action.key.usage++ : action.key.usage--; // 8-bit keyboard usage
  }
}

void clearAll()
{
  selectAll();
  sayKey(NONE, DELETE);
}

void setInactivityTimeOut()
{
  nTimerDelay = DELAY_IN_SECONDS(3); // Switch focus back to MODE after inactivity
  TMR3ON_bit = 1;
}

void setFocus(uint8_t newFocus)
{
  focus = newFocus;
  selectLine(INFO_LINE);
  if (newFocus == FOCUS_ON_PAGE)
  {
    sayConst("Menu:   Turn=Sel, Press+Turn=Set Address, Press=OK");
  }
  else
  {
    sayConst("Action: Turn=Sel, Press+Turn=Modify, Press=OK, Press+Hold=Delete");
  }
  selectLine(SELECTION_LINE);
}

void changePage()
{
  while (ROTARY_BUTTON_PRESSED)
  {
    if (rotation) // If the rotary knob is being turned while pressed
    {
      TMR3ON_bit = 0;
      if (rotation > 0) // Clockwise rotation
      {
        if (nActionFocus < nAction)
          nActionFocus++;
      }
      else              // Anticlockwise rotation
      {
        if (nActionFocus > 0)
          nActionFocus--;
      }
      sayPage();
      rotation = 0;  // Indicate rotary event handled
    }
    else // The rotary knob is being pressed but not turned
    {
    }
  }
  if (action.key.page == PAGE_KEYBOARD)
  {
    action.key.mod.value = 0; // No CTL/ALT/SHIFT/GUI modifiers
    action.key.usage = USB_KEY_A;  // Start at the key 'A'
  }
  else
  {
    action.cons.usage = 0;          // Reset usage to zero
  }
  setFocus(FOCUS_ON_USAGE);         // We are now adjusting the usage
  sayUsage(nActionFocus, &action);  // Overwrite it with a usage from the new page
}

void changeUsage()
{
  uint8_t bAppendAction;

  bAppendAction = TRUE;
  nTimerDelay = DELAY_IN_SECONDS(1);
  TMR3ON_bit = 1;
  while (ROTARY_BUTTON_PRESSED)
  {
    if (rotation) // If the rotary knob is being turned while pressed
    {
      TMR3ON_bit = 0;
      bAppendAction = FALSE;
      selectLine(SELECTION_LINE);
      switch (action.key.page)
      {
        case PAGE_KEYBOARD:         // Push+turn adjusts the key modifier (ALT, SHIFT etc)
        case PAGE_LOCAL_FUNCTION:   // Push+turn adjusts the local function (GOTO, EXIT, DELAY etc)
          rotation > 0 ? action.key.mod.value++ : action.key.mod.value--;
          break;
        case PAGE_CONSUMER_DEVICE:  // Push+turn adjusts the 12-bit usage directly
          rotation > 0 ? action.cons.usage++ : action.cons.usage--;
          break;
        default:                    // Push+turn adjusts the 8-bit usage directly
          rotation > 0 ? action.key.usage++  : action.key.usage--;
          break;
      }
      sayUsage(nActionFocus, &action);
      rotation = 0;  // Indicate rotary event handled
    }
    else // The rotary knob is being pressed but not turned
    {
      if (nTimerDelay == 0) // If it is a long press, delete the last entry
      {
        bAppendAction = FALSE;
        if (nAction) // If anything to delete
        {
          nAction--; // Delete the last action (effectively)
          nActionFocus = nAction;
          deleteLastLine(); // Delete last action from the display
          selectLine(SELECTION_LINE);
        }
        nTimerDelay = DELAY_IN_SECONDS(0.5);
      }
      else
      {
      }
    }
  }
  // Rotary button released...
  TMR3ON_bit = 0;
  if (bAppendAction)
  {
    if (nActionFocus >= nAction) // If we are appending a new action
    {
      if (nActionFocus < ELEMENTS(aAction))  // If room to add an action
      {
        sayKey(CTL, END);
        newLine();

        aAction[nActionFocus] = action;
        sayAction(nActionFocus);
        nActionFocus++;
        nAction = nActionFocus; // Set new high water mark
      }
    }
    else  // We are updating an existing action
    {
      aAction[nActionFocus] = action;
      selectLine(START_ACTIONS_LINE+nActionFocus);
      sayAction(nActionFocus);
      nActionFocus++; // Assume we will want to update the following action
    }
    selectLine(SELECTION_LINE);
    sayUsage(nActionFocus, &action);
    selectLine(SELECTION_LINE);
  }
}

void displayProgrammingMenu()
{
  clearAll();
  sayConst("PASSifier 0.90");
  newLine();
  newLine();
  newLine();
  newLine();
  sayConst("Ad Code Action");
  sayActions();
  action.key.page = PAGE_KEYBOARD;
  action.key.usage = USB_KEY_A;
  action.key.mod.value = 0;
  sayPage();
  setFocus(FOCUS_ON_PAGE);
}

void programMode()
{
  if (rotation)
  {
    switch (focus)
    {
      case FOCUS_ON_PAGE:   // User is adjusting the page
        nextKnownPage(rotation);
        sayPage();
        break;

      case FOCUS_ON_USAGE:  // User is adjusting the usage within a page
        nextKnownUsage(rotation);
        selectLine(SELECTION_LINE);
        sayUsage(nActionFocus, &action);
        sayKey(SHIFT, HOME);
        setInactivityTimeOut();
        break;

      default:
        break;
    }
    rotation = 0;  // Indicate rotary event handled
  }
  else if (ROTARY_BUTTON_PRESSED)
  {
    button = 0;
    LED = OFF;
    Delay_ms(5);  // Cheap debounce
    if (ROTARY_BUTTON_PRESSED) // If still pressed
    {
      switch (focus)
      {
        case FOCUS_ON_PAGE:
          switch (action.key.page)
          {
            case PAGE_EXIT:
              clearAll();
              sayConst("Saved");
              bProgramMode = FALSE;
              break;
            case PAGE_CANCEL:
              clearAll();
              sayConst("Cancelled");
              bProgramMode = FALSE;
              break;
            case PAGE_REDISPLAY:
              displayProgrammingMenu();
              break;
            default:
              changePage();
              setInactivityTimeOut();
              break;
          }
          break;
        case FOCUS_ON_USAGE:
          changeUsage();
          setInactivityTimeOut();
          break;
        default:
          break;
      }
    }
    rotation = 0;  // Ignore rotary while button pressed
  }
  else // rotary button is not currently being pressed or rotated
  {
    if (TMR3ON_bit && nTimerDelay == 0) // If no rotary actions for n seconds
    {
      TMR3ON_bit = 0;
      setFocus(FOCUS_ON_PAGE);
      sayPage();
    }
  }
}



void play()
{
  uint8_t i;
  uint8_t nCount;
  t_action * pAction = &aAction;
  button = 0;
  LED = OFF;
  for (i = 0; i < nAction && !button; i++, pAction++)
  {
    switch (pAction->key.page)
    {
      case PAGE_KEYBOARD:
        usbData.s.xx.byte = pAction->key.mod.value;
        usbData.s.yy = pAction->key.usage;
        sendUSBKeystroke();
        break;
      case PAGE_SYSTEM_CONTROL:
        usbData.s.yy = pAction->sys.usage;
        sendUSBSystemControlCommand();
        break;
      case PAGE_CONSUMER_DEVICE:
        usbData.s.xx.byte = pAction->cons.usage >> 8;
        usbData.s.yy = pAction->cons.usage;
        sendUSBConsumerDeviceCommand();
        break;
      case PAGE_LOCAL_FUNCTION:
        switch (pAction->key.mod.value)
        {
          case LOCAL_FUNCTION_WAIT_SEC:
            nCount = pAction->key.usage;
            while (!button && nCount--)
            {
              Delay_ms(1000);
            }
            break;
          case LOCAL_FUNCTION_WAIT_MS:
            nCount = pAction->key.usage;
            while (!button & nCount--)
            {
              Delay_ms(1);
            }
            break;
          case LOCAL_FUNCTION_GOTO:
            if (pAction->key.usage < nAction)
            {
              i = pAction->key.usage;
              pAction = &aAction[i];
              pAction--;
              i--;
            }
            break;
          default:
            break;
        }
        break;
      default:
        break;
    }
  }
  button = 0;
  LED = OFF;
}

void runMode()
{
  if (ROTARY_BUTTON_PRESSED)
  {
    button = 0;
    LED = OFF;
    Delay_ms(5);  // Cheap debounce
    if (ROTARY_BUTTON_PRESSED)
    {
      nTimerDelay = DELAY_IN_SECONDS(1);
      TMR3ON_bit = 1;
      while (ROTARY_BUTTON_PRESSED)
      {
        if (nTimerDelay == 0) // If it is a long press
        {
          bProgramMode = TRUE;
          TMR3ON_bit = 0;
          displayProgrammingMenu();
          break;
        }
      }
      TMR3ON_bit = 0;
      while (ROTARY_BUTTON_PRESSED); // Wait for rotary button to be released
      if (!bProgramMode)
      {
        play();
      }
    }
  }
}

void main()
{
  Prolog();
  while (1)
  {
    bProgramMode ? programMode() : runMode();
    button = 0;
    LED = OFF;
  }
}

void interrupt()               // High priority interrupt service routine
{
  USB_Interrupt_Proc();        // Always give the USB module first opportunity to process

  if (IOCIF_bit)               // Interrupt On Change interrupt?
  {
    button |= ROTARY_BUTTON_PRESSED;
    LED = button;
    state = *(stateArray + ((state & STATE_MASK) << 2 | (ROTARY_B << 1 | ROTARY_A)));
    rotation |= state & EVENT_MASK; // Extract rotary event from encoder state
    IOCIF_bit = 0;             // Clear Interrupt On Change flag
  }

  if (PIR2.TMR3IF)             // Timer3 interrupt? (22.9 times/second)
  {
    nTimerDelay--;             // Decrement delay count
    PIR2.TMR3IF = 0;           // Clear the Timer3 interrupt flag
  }

}