/*
  PUB! Programmable USB Button
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

NAME     - PUB! Programmable USB Button

FUNCTION - This is a programmable button on which you can record a sequence of
           keystrokes (and/or other USB functions) and then play that sequence
           back when you press it.

FEATURES - 1. A single rotary encoder knob (with push switch) is the only input.
           2. Can record and replay up to 127 keystrokes (or other "actions").
           3. Absolutely NO HOST DRIVERS required.

PIN USAGE -                     PIC18F25K50
                           .------------------.                       .--IOC
            MCLR       --> | RE3  1    28 RB7 | <-- PGD               ||
            LED        <-- | RA0  2    27 RB6 | <-- PGC, Rotary button||
                       <-- | RA1  3    26 RB5 | <-- Rotary B          || PORTB
                       <-- | RA2  4    25 RB4 | <-- Rotary A          || Weak
                       <-- | RA3  5    24 RB3 | <--                    | Pullups
                       <-- | RA4  6    23 RB2 | <--                    | Enabled
                       <-- | RA5  7    22 RB1 | <--                    |
            Ground     --- | VSS  8    21 RB0 | <--                    |
            n/c        --- | RA7  9    20 VDD | --- +5V
                       <-- | RA6  10   19 VSS | --- Ground
                       <-- | RC0  11   18 RC7 | --> RX
                       <-- | RC1  12   17 RC6 | --> TX
                       <-- | RC2  13   16 RC5 | <-> USB D+
            2 x 100 nF --- | VUSB 14   15 RC4 | <-> USB D-
                           '------------------'

            Rotary Encoder with switch (e.g. ALPS EC11K0924401):

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

OPERATION - The user interacts with the system via a digital encoder knob with
            a built-in switch and, when in PROGRAM mode, with a host-based text
            editor program than understands fairly standard editing control
            sequences.

            In RUN mode (the default mode when the device is plugged into a USB
            port) the user simply presses the rotary knob to cause the previously
            programmed sequence of keystrokes to be sent to the host over the
            connecting USB cable.

            If the user presses the knob down for more than about 3 seconds,
            then the device enters PROGRAM mode. The user must already
            have started and given focus to the above-mentioned text editor
            host application because this will be the means of communication
            with the user. If that is not done then the device will inject
            unwanted keystrokes into the application that currently has the
            keyboard focus.

            - Start a text editor program on the host computer and ensure it
              has keyboard focus.

            - Press and hold the rotary encoder to enter PROGRAM mode.
              The following prompt will be keyed into the text editor:

              PUB! Programmable USB Button vn.nn
              Main:   Turn=Select, Press=OK, Press+Turn=Set At, Press+Hold=Exit
                 0    Set Keystroke at 00

              At Code Action

            - Rotate the knob to choose between the available menu functions
              which are currently:

              Set Keystroke
              Set System Control function
              Set Consumer Device function
              Set Local Function (WAIT, GOTO etc)
              Save to EEPROM
              Redisplay
              Reload from EEPROM
              Delete Action

            - Press the knob to select the desired function. For example,
              Set Keystroke

            - Rotate the knob until the desired keystroke is displayed:

              00 0004 a

            - For "Set keystroke" you can choose a combination of keystroke
              modifiers (Ctl, Alt, Shift, or GUI) by pressing and holding the
              knob down while rotating it:

              00 0104 SHIFT+A

            - Press the knob to append the displayed keystroke to the list of
              keystrokes (or functions) to be recorded.

            - Select and append more keystrokes as required.

            - Press and hold the knob to return to the main menu.

            - Rotate the knob to choose the "Save to EEPROM" function.

            - Now when you press the knob the saved keystrokes will be replayed.





NOTES    - 1. The source code is written in MikroC Pro from mikroe.com


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
           http://www.buxtronix.net/2011/10/rotary-encoders-done-properly.html


HISTORY  - Date     Ver   By  Reason (most recent at the top please)
           -------- ----- --- -------------------------------------------------
           20161030 0.93  AJA Detected the Caps Lock state
           20141130 0.92  AJA Added arithmetic and conditional jump instructions
           20141108 0.91  AJA Fixed PCB to match code. Rotary button on RB6
           20141012 0.90  AJA First published version
           20140817 0.10  AJA Initial development

-------------------------------------------------------------------------------
*/
#include <stdint.h>
#include <built_in.h>
#include "USBdsc.h"
#include "pub.h"

// Copies a text string from ROM into a RAM buffer (saves RAM)
char * _TEXT(const char * p)
{
  static char sText[70];
  char * q;
  for (q=&sText; *q++ = *p++; );
  return &sText;
}

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


const char * getUsageDesc (t_action * pAction) // Note: Literals returned as const are in ROM
{
  switch (pAction->key.page)
  {
    case PAGE_KEYBOARD:
      if (pAction->key.mod & MODIFIER_LEFTSHIFT)
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

    case PAGE_DO:
      switch (pAction->key.mod)
      {
        case DO_DELETE:
          return "Delete action";
        case DO_LOAD:
          return "Load from EEPROM";
        case DO_SAVE:
          return "Save to EEPROM";
        case DO_REDISPLAY:
          return "Redisplay";
        default:
          return "";
      }

    case PAGE_EXECUTE:
      switch (pAction->key.mod)
      {
        case EXECUTE_SET:
          return "Let W = ";
        case EXECUTE_GET:
          return "Get W from R";
        case EXECUTE_PUT:
          return "Put W in R";
        case EXECUTE_COMPARE_IMMEDIATE:
          return "Compare W to ";
        case EXECUTE_COMPARE:
          return "Compare W to R";
        case EXECUTE_SAY:
          return "Say R";
        case EXECUTE_FORMAT:
          return "Say in ";
        case EXECUTE_ADD_IMMEDIATE:
          return "Let W = W + ";
        case EXECUTE_SUB_IMMEDIATE:
          return "Let W = W - ";
        case EXECUTE_CLEAR:
          return "Clear memory to ";
        case EXECUTE_ADD:
          return "Let W = W + R";
        case EXECUTE_SUB:
          return "Let W = W - R";
        case EXECUTE_MUL:
          return "Let W = W x R";
        case EXECUTE_DIV:
          return "Let W = W / R";
        case EXECUTE_WAIT_MS:
          return "Wait "; // milliseconds
        case EXECUTE_WAIT_SEC:
          return "Wait "; // seconds
        default:
          return "";
      }

    case PAGE_JUMP:
      switch (pAction->key.mod)
      {
        case JUMP_RELATIVE:
          return "Jump Relative";
        case JUMP_IF_CARRY:
          return "Jump if Carry";
        case JUMP_IF_HIGH:
          return "Jump if High";
        case JUMP_IF_HIGH_OR_CARRY:
          return "Jump if High or Carry";
        case JUMP_IF_LOW:
          return "Jump if Low";
        case JUMP_IF_LOW_OR_CARRY:
          return "Jump if Low or Carry";
        case JUMP_IF_NOT_ZERO_OR_CARRY:
          return "Jump if Not Zero or Carry";
        case JUMP_IF_NOT_ZERO:
          return "Jump if Not Zero";
        case JUMP_IF_ZERO:
          return "Jump if Zero";
        case JUMP_IF_ZERO_OR_CARRY:
          return "Jump if Zero or Carry";
        case JUMP_IF_NOT_LOW_OR_CARRY:
          return "Jump if Not Low or Carry";
        case JUMP_IF_NOT_LOW:
          return "Jump if Not Low";
        case JUMP_IF_ZERO_OR_LOW:
          return "Jump if Zero or Low";
        case JUMP_IF_NOT_HIGH:
          return "Jump if Not High";
        case JUMP_IF_NOT_CARRY:
          return "Jump if Not Carry";
        case JUMP:
          return "Jump";
        default:
          break;
      }

    default:
      return "";
  }
}

void toPrintableHex(uint8_t c, char * p)
{
  static const char HEX[] = "0123456789ABCDEF";
  *p++ = HEX[(c & 0xF0) >> 4];
  *p++ = HEX[(c & 0x0F)     ];
  *p   = '\0';
}

void adjustShiftModifier ()
{
  if (usbToHost[3] >= USB_KEY_A && usbToHost[3] <= USB_KEY_Z)  // Handle SHIFT for alphabetics
  {
    // If the user wants SHIFT+A to be sent to the host, then it is reasonable
    // to expect it to be interpreted by the host as an uppercase "A". But, if
    // the Caps Lock indicator is on, SHIFT+A would result in the host seeing
    // it as a lowercase "a"...UNLESS we can choose to set the SHIFT modifier
    // depending on the current state of the Caps Lock LED indicator.

    // Truth table:
    // input       input           output
    // modifiers   Caps Lock       modifiers
    // SHIFT bit   LED light       SHIFT bit   Result at host
    // ---------   ---------  -->  ---------   --------------
    //     0           0               0             a
    //     0           1               1             a
    //     1           0               1             A
    //     1           1               0             A
    // This is a classic Exclusive-OR truth table, from which we can code:

    if ((usbToHost[1] & SHIFT) != leds.bits.CapsLock) // Logical Exclusive-OR
      usbToHost[1] |= SHIFT;              // SHIFT modifier bit = 1
    else
      usbToHost[1] &= ~SHIFT;             // SHIFT modifier bit = 0
  }
}


void playSystemControlCommand(t_action * pAction)
{
  if (!bUSBReady) return;
  usbToHost[0] = REPORT_ID_SYSTEM_CONTROL;  // Report Id = System Control
  usbToHost[1] = pAction->sys.usage;        // Function requested
  while(!HID_Write(&usbToHost, 2));         // Copy to USB buffer and try to send
  usbToHost[1] = 0;                         // No function requested anymore
  while(!HID_Write(&usbToHost, 2));         // Copy to USB buffer and try to send
}

void playConsumerDeviceCommand(t_action * pAction)
{
  if (!bUSBReady) return;
  usbToHost[0] = REPORT_ID_CONSUMER_DEVICE; // Report Id = Consumer Device
  usbToHost[1] = pAction->cons.usage;       // Function requested (low byte)
  usbToHost[2] = pAction->cons.usage >> 8;  // Function requested (high byte)
  while(!HID_Write(&usbToHost, 3));         // Copy to USB buffer and try to send
  usbToHost[1] = 0;                         // Function requested low byte
  usbToHost[2] = 0;                         // Function requested high byte
  while(!HID_Write(&usbToHost, 3));         // Copy to USB buffer and try to send
}

void sayNoKeyPressed()
{
  usbToHost[0] = REPORT_ID_KEYBOARD;        // Report Id = Keyboard
  usbToHost[1] = 0;                         // No modifiers now
  usbToHost[3] = 0;                         // No key pressed now
  while(!HID_Write(&usbToHost, 4));         // Copy to USB buffer and try to send
}

void playKeystroke(t_action * pAction)
{
  if (!bUSBReady) return;
  if (usbToHost[0] == REPORT_ID_KEYBOARD && // If new keystroke is the same as the last one
      usbToHost[1] == pAction->key.mod   &&
      usbToHost[3] == pAction->key.usage)
  {
    sayNoKeyPressed();                      // Release the key before sending it again
  }
  usbToHost[0] = REPORT_ID_KEYBOARD;        // Report Id = Keyboard
  usbToHost[1] = pAction->key.mod;          // Ctrl/Alt/Shift modifiers
  usbToHost[2] = 0;                         // Reserved for OEM
  usbToHost[3] = pAction->key.usage;        // Key pressed
  adjustShiftModifier();                    // Adjust SHIFT for alphabetics
  while(!HID_Write(&usbToHost, 4));         // Copy to USB buffer and try to send
  usbToHost[1] = pAction->key.mod;          // Reset modifiers to what was requested
}

void say(uint8_t * p)
{
  uint8_t c;
  t_action action;

  while (*p)
  {
    if (*p < sizeof(ASCII_to_USB))
    {
      c = ASCII_to_USB[*p]; // Top bit on means capitalise with Left Shift
    }
    else
    {
      c = SPACE; // Replace invalid ASCII character with a space
    }
    if (c & 0b10000000) // If SHIFT key needed
    {
      action.key.mod = MODIFIER_LEFTSHIFT;
      action.key.usage = c & 0b01111111;
    }
    else
    {
      action.key.mod = 0;
      action.key.usage = c;
    }
    playKeystroke(&action);
    p++;
  }
  sayNoKeyPressed(); // Release key otherwise the host will think the last key is still being pressed
}

void sayConst(const uint8_t * p)
{
  say(_TEXT(p));
}

void sayModifiers(t_keyboardAction * pAction)
{
  if (pAction->mod & MODIFIER_LEFTGUI)     sayConst("GUI+");
  if (pAction->mod & MODIFIER_LEFTCTL)     sayConst("CTL+");
  if (pAction->mod & MODIFIER_LEFTALT)     sayConst("ALT+");
  if (pAction->mod & MODIFIER_LEFTSHIFT)   sayConst("SHIFT+");
}

void sayChar(uint8_t c)
{
  char sString[2];
  sString[0] = c;
  sString[1] = '\0';
  say(sString);
}

void sayHex(uint8_t c)
{
  char xString[3];
  toPrintableHex(c, xString);
  say(xString);
}

void sayDec(uint8_t c)
{
  char * p;
  char sString[4]; // "nnn"
  ByteToStr(c, &sString);
  for (p=&sString; *p == ' '; p++);  // Find first non-blank
  say(p);
}

void saySignedDec(int8_t c)
{
  char * p;
  char sString[4]; // "nnn"
  ShortToStr(c, &sString);
  for (p=&sString; *p == ' '; p++);  // Find first non-blank
  say(p);
}

void sayKey(uint8_t modifiers, uint8_t key)
{
  if (!bUSBReady) return;
  usbToHost[0] = REPORT_ID_KEYBOARD;      // Report Id = Keyboard
  usbToHost[1] = modifiers;               // Ctrl/Alt/Shift modifiers
  usbToHost[2] = 0;                       // Reserved for OEM
  usbToHost[3] = key;                     // Key pressed
  adjustShiftModifier();                  // Adjust SHIFT for alphabetics
  while(!HID_Write(&usbToHost, 4));       // Copy to USB buffer and try to send
  sayNoKeyPressed();                      // Release key
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
      Delay_ms(100);
      ACTIVITY_LED = ON;                         // LED will be turned off by the next timer interrupt
      bUSBReady = HID_Write(&usbToHost, 4) != 0; // Copy to USB buffer and try to send
    }
    if (!bUSBReady)
    {
      HID_Disable();
      Delay_ms(5000);
    }
  }
  Delay_ms(250);
}

void disableUSB()
{
  HID_Disable();
  bUSBReady = FALSE;
}

void wait()
{
  Delay_ms(20);
}

void saveInEEPROM()
{
  t_action * p;
  uint8_t addr;
  uint8_t  i;

  EEPROM_Write(0, nActionFocus);   // Save currently focussed action in byte 0
  EEPROM_Write(1, nAction);        // Save number of actions
  p = &aAction[0];
  addr = 2;
  for (i = 0; i < nAction; i++, p++)
  {
    EEPROM_Write(addr++, Hi(p->action));
    EEPROM_Write(addr++, Lo(p->action));
  }
}

void loadFromEEPROM()
{
  t_action * p;
  uint8_t addr;
  uint8_t  i;

  // Read any actions present in the EEPROM
  nActionFocus = EEPROM_Read(0);          // Read currently focussed action
  if (nActionFocus > ELEMENTS(aAction))   // If EEPROM empty, or number of actions invalid
    nActionFocus = 0;                     // Assume content of EEPROM is NBG
  nAction = EEPROM_Read(1);               // Read number of actions
  if (nAction > ELEMENTS(aAction))        // If EEPROM empty, or number of actions invalid
    nAction = 0;                          // Assume content of EEPROM is NBG

  p = &aAction[0];          // Point to the first element of the actions array
  for (addr = 2, i = 0; i < nAction; i++)
  {
    Hi(p->action) = EEPROM_Read(addr++);
    Lo(p->action) = EEPROM_Read(addr++);
    p++;
  }
  // Clear any unused actions to zero
  for (; i < ELEMENTS(aAction); i++, p++)
  {
    p->action = 0;
  }

}





void Prolog()
{
  ANSELA = 0b00000000;    // Configure all PORTA bits as digital
  ANSELB = 0b00000000;    // Configure all PORTB bits as digital
  ANSELC = 0b00000000;    // Configure all PORTC bits as digital

  LATA = 0;
  LATB = 0;
  LATC = 0;

  //        76543210
  TRISA = 0b00000000;     // 1=Input, 0=Output
  TRISB = 0b01111000;
  TRISC = 0b00110000;


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

// Timer0 is used for LED blinking (Timer0 is always enabled)
  T0CON   = 0b10010010;
//            x              1  = TMR0ON: Timer0 on (1 = on, 0 = off)
//             x             0  = T08BIT: Timer0 8-bit mode (1 = 8-BIT, 0 = 16-bit)
//              x            0  = T0CS:   Timer0 clock source (1 = T0CKI, 0 = FOSC/4)
//               x           1  = T0SE:   Timer0 Source Edge select T0CKI (1 = H-to-L, 0 = L-to-H)
//                x          0  = PSA:    Pre Scaler Assigned (1 = Not assigned, 0 = Assigned)
//                 xxx       010= T0PS:   Timer0 Pre Scaler (010 = 1:8)
// Timer0 tick rate = 48 MHz FOSC/4/8 = 1.5 MHz
// Timer0 interrupt rate = 1.5 MHz / 65536 = 22.9 times per second (once every 43.7 ms)


// Timer3 is used for human-scale delays (Timer3 is not always enabled)
  T3CON   = 0b00110010;
//            xx             00 = TMR3CS: Timer3 clock source is instruction clock (Fosc/4)
//              xx           11 = TMR3PS: Timer3 prescale value is 1:8
//                x          0  = SOSCEN: Secondary Oscillator disabled
//                 x         0  = T3SYNC: Ignored because TMR3CS = 0x
//                  x        1  = RD16:   Enables register read/write of Timer3 in one 16-bit operation
//                   x       0  = TMR3ON: Timer3 is off
// Timer3 tick rate = 48 MHz FOSC/4/8 = 1.5 MHz (667 ns)
// Timer3 interrupt rate = 1.5 MHz / 65536 = 22.9 times per second

  bUserInterrupt = FALSE;
  ACTIVITY_LED = OFF;

  loadFromEEPROM();       // Load any existing script from the EEPROM at power up

  action.key.page = PAGE_KEYBOARD;
  action.key.usage = USB_KEY_A;
  action.key.mod = 0;
  rotation = 0;  // Indicate rotary event handled

//----------------------------------------------------------------------------
// Let the interrupts begin
//----------------------------------------------------------------------------

  TMR0IE_bit = 1;         // Enable Timer0 interrupts (to turn off LED)
  TMR3IE_bit = 1;         // Enable Timer3 interrupts (for long press delay)
  IOCIE_bit = 1;          // Enable PORTB/C Interrupt On Change interrupts
  PEIE_bit = 1;           // Enable peripheral interrupts
  GIE_bit = 1;            // Enable global interrupts

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
    case PAGE_SYSTEM_CONTROL:   return "Set System Control Command";
    case PAGE_CONSUMER_DEVICE:  return "Set Consumer Device Command";
    case PAGE_DO:               return "Do Local Function";
    case PAGE_EXECUTE:          return "Execute Instruction";
    case PAGE_JUMP:             return "Jump On Condition";
    default:                    return "";
  }
}

void sayPage()
{
  char xWork[3];

  selectLine(SELECTION_LINE);
  // sayHex(nActionFocus);
  sayConst("   ");
  toPrintableHex(action.key.page, &xWork);
  say(&xWork[1]);
  sayConst("    ");
  sayConst(getPageDesc(action.key.page));
  switch (action.key.page)
  {
    case PAGE_KEYBOARD:
    case PAGE_SYSTEM_CONTROL:
    case PAGE_CONSUMER_DEVICE:
    case PAGE_DO:
    case PAGE_EXECUTE:
    case PAGE_JUMP:
      sayConst(" at ");
      sayHex(nActionFocus);
      break;

    default:
      break;
  }
  sayKey(SHIFT, HOME);   // Highlight this selection
}

void sayUsage(uint8_t nAction, t_action * pAction)
{

  if (pAction->key.page == PAGE_DO)
  {
    sayConst("        ");
  }
  else //say "aa pxxx "  where aa = Number of actions, p = page, xxx = usage
  {
    sayHex(nAction);
    sayKey(NONE, SPACE);
    sayHex(pAction->key.page<<4 | pAction->key.mod);
    sayHex(pAction->key.usage);
    sayKey(NONE, SPACE);
  }

  switch (pAction->key.page)
  {
    case PAGE_KEYBOARD:
      sayModifiers((t_keyboardAction *)pAction);
      sayConst(getUsageDesc(pAction));
      break;

    case PAGE_JUMP:
      sayConst(getUsageDesc(pAction));
      if (pAction->key.mod == JUMP_RELATIVE)
      {
        sayConst(" by ");
        saySignedDec(pAction->key.usage);
      }
      else
      {
        sayConst(" to ");
        sayHex(pAction->key.usage);
      }
      break;

    case PAGE_EXECUTE:
      sayConst(getUsageDesc(pAction));
      switch (pAction->key.mod)
      {
        case EXECUTE_CLEAR:
          sayHex(pAction->key.usage);
          break;
        case EXECUTE_WAIT_SEC:
          sayDec(pAction->key.usage);
          sayConst(" sec");
          break;
        case EXECUTE_WAIT_MS:
          sayDec(pAction->key.usage);
          sayConst(" ms");
          break;
        case EXECUTE_FORMAT:
          switch (pAction->key.usage)
          {
            case FORMAT_CHAR:
              sayConst("Char");
              break;
            case FORMAT_DEC:
              sayConst("Decimal");
              break;
            case FORMAT_HEX:
            default:
              sayConst("Hex");
              break;
          }
          break;
        default:
          sayHex(pAction->key.usage);
          break;
      }
      break;

    case PAGE_DO:
      sayConst(getUsageDesc(pAction));
      if (pAction->key.mod == DO_DELETE)
      {
        if (nAction)  // If we have actions to delete
        {
          if (pAction->key.usage >= nAction)
          {
            pAction->key.usage = nAction-1;
          }
          sayConst(" at ");
          sayHex(pAction->key.usage);
        }
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
  for (i = pAction->usage-1; i > 0 && i < ELEMENTS(CONSUMER_DEVICE_DESC) && *CONSUMER_DEVICE_DESC[i] == '\0'; i--);
  if (i == 0 || i >= ELEMENTS(CONSUMER_DEVICE_DESC))
  {
    for (i = ELEMENTS(CONSUMER_DEVICE_DESC)-1; i > pAction->usage && *CONSUMER_DEVICE_DESC[i] == '\0'; i--);
  }
  return i;
}

uint8_t getNextSys (t_systemControlAction * pAction)
{
  uint8_t i;
  for (i = pAction->usage+1; i < ELEMENTS(SYSTEM_CONTROL_DESC) && *SYSTEM_CONTROL_DESC[i] == '\0'; i++);
  if (i >= ELEMENTS(SYSTEM_CONTROL_DESC))
  {
    for (i = 0; i < pAction->usage && *SYSTEM_CONTROL_DESC[i] == '\0'; i++);
  }
  return i;
}

uint8_t getPrevSys (t_systemControlAction * pAction)
{
  uint8_t i;
  for (i = pAction->usage-1; i > 0 && i < ELEMENTS(SYSTEM_CONTROL_DESC) && *SYSTEM_CONTROL_DESC[i] == '\0'; i--);
  if (i == 0 || i >= ELEMENTS(SYSTEM_CONTROL_DESC))
  {
    for (i = ELEMENTS(SYSTEM_CONTROL_DESC)-1; i > pAction->usage && *SYSTEM_CONTROL_DESC[i] == '\0'; i--);
  }
  return i;
}


void nextKnownUsage(int8_t rotation)
{
  switch (action.key.page)
  {
    case PAGE_CONSUMER_DEVICE:
      action.cons.usage = rotation > 0 ? getNextConsumer(&action.cons) : getPrevConsumer(&action.cons);
      break;
    case PAGE_SYSTEM_CONTROL:
      action.sys.usage = rotation > 0 ? getNextSys(&action.sys) : getPrevSys(&action.sys);
      break;
    default:
      rotation > 0 ? action.key.usage++ : action.key.usage--; // 8-bit keyboard usage
      break;
  }
}

void clearDisplay()
{
  selectAll();
  sayKey(NONE, DELETE);
}

void setFocus(uint8_t newFocus)
{
  focus = newFocus;
  selectLine(INFO_LINE);
  if (newFocus == FOCUS_ON_PAGE)
  {
    sayConst("Main:   Turn=Select, Press=OK, Press+Turn=Set At, Press+Hold=Exit");
  }
  else
  {
    switch (action.key.page)
    {
      case PAGE_KEYBOARD:
        sayConst("Key:    Turn=Select, Press+Turn=Modify"); // , Press=OK, Press+Hold=Return
        break;
      case PAGE_SYSTEM_CONTROL:
        sayConst("System: Turn=Select"); // , Press=OK, Press+Hold=Return
        break;
      case PAGE_CONSUMER_DEVICE:
        sayConst("Cons:   Turn=Select"); // , Press=OK, Press+Hold=Return
        break;
      case PAGE_DO:
        sayConst("Do:     Turn=Modify, Press+Turn=Select"); // , Press=OK, Press+Hold=Return
        break;
      case PAGE_EXECUTE:
        sayConst("Exec:   Turn=Modify, Press+Turn=Select"); // , Press=OK, Press+Hold=Return
        break;
      case PAGE_JUMP:
        sayConst("Jump:   Turn=Modify, Press+Turn=Select"); // , Press=OK, Press+Hold=Return
        break;
      default:
        break;
    }
    sayConst(", Press=OK, Press+Hold=Return");
  }
  selectLine(SELECTION_LINE);
}

void deleteAction(uint8_t n)
{
  // TODO: Ideally this should also adjust any GOTO actions etc
  uint8_t i;

  if (nAction) // If anything to delete
  {
    if (n == (nAction-1)) // If it is the action on the end
    {
      nAction--; // Delete the last action (effectively)
      nActionFocus = nAction;
      deleteLastLine(); // Delete last action from the display
    }
    else  // Delete an action which is not at the end
    {
      for (i = n; i < nAction; i++)
      {
        aAction[i] = aAction[i+1];
      }
      nAction--; // We now have one less action in the array
      nActionFocus = nAction;
      gotoLine(START_ACTIONS_LINE+n);
      sayKey(CTL|SHIFT, END); // Select all actions from there onwards
      sayKey(NONE, DELETE);   // Delete them
      sayKey(SHIFT, LEFT);    // Select previous line's new line sequence
      for (i = n; i < nAction; i++)
      {
        newLine();
        sayAction(i);
      }
    }
    selectLine(SELECTION_LINE);
  }
}

void selectFirstUsage()
{
  switch (action.key.page)
  {
    case PAGE_KEYBOARD:
      action.key.mod = 0;            // No CTL/ALT/SHIFT/GUI modifiers
      action.key.usage = USB_KEY_A;  // Start at the key 'A'
      break;
    case PAGE_CONSUMER_DEVICE:
      action.cons.usage = 0x09D;     // Start at "Ch+"
      break;
    case PAGE_SYSTEM_CONTROL:
      action.sys.usage = 0x01;       // Start at "Power Down"
      break;
    default:
      action.cons.usage = 0;         // Start at zero
      break;
  }

}


void changePage()
{
  while (ROTARY_BUTTON_PRESSED)
  {
    if (rotation) // If the rotary knob is being turned while pressed
    {
      TMR3ON_bit = 0;   // Disable long-press timer
      if (rotation > 0) // Clockwise rotation
      {
        if (nActionFocus < nAction)
          nActionFocus++;
      }
      else              // Anticlockwise rotation
      {
        if (nActionFocus > 0)
          nActionFocus--;
        else if (nActionFocus == 0 && nAction)
          nActionFocus = nAction;
      }
      sayPage();
      rotation = 0;  // Indicate rotary event handled
    }
    else // The rotary knob is being pressed but not turned
    {
    }
  }
  selectFirstUsage();
  setFocus(FOCUS_ON_USAGE);         // We are now adjusting the usage
  sayUsage(nActionFocus, &action);  // Overwrite it with a usage from the new page
}

void displayProgrammingMenu()
{
  clearDisplay();
  sayConst("PUB! Programmable USB Button v" VERSION);
  newLine();
  newLine();
  newLine();
  newLine();
  sayConst("At Code Action");
  sayActions();
  action.key.page = PAGE_KEYBOARD;
  action.key.usage = USB_KEY_A;
  action.key.mod = 0;
  sayPage();
  setFocus(FOCUS_ON_PAGE);
}

void changeUsage()
{
  uint8_t bAppendAction;

  bAppendAction = TRUE;
  nRemainingTimerTicks = INTERVAL_IN_SECONDS(1);
  TMR3ON_bit = 1;   // Enable long-press timer
  while (ROTARY_BUTTON_PRESSED)
  {
    if (rotation) // If the rotary knob is being turned while pressed
    {
      TMR3ON_bit = 0;   // Disable long-press timer
      bAppendAction = FALSE;
      selectLine(SELECTION_LINE);
      switch (action.key.page)
      {
        case PAGE_KEYBOARD:         // Push+turn adjusts the key modifier (ALT, SHIFT etc)
        case PAGE_DO:               // Push+turn adjusts the local function
        case PAGE_EXECUTE:          // Push+turn adjusts the instruction
        case PAGE_JUMP:             // Push+turn adjusts the jump condition
          rotation > 0 ? action.key.mod++ : action.key.mod--;
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
    }
  }
  // Rotary button released...
  TMR3ON_bit = 0;   // Disable long-press timer
  if (bAppendAction)
  {
    if (action.key.page == PAGE_DO) // If we are doing a local function
    {
      switch (action.key.mod)
      {
        case DO_DELETE:                     // Delete action
          deleteAction(action.key.usage);
          break;

        case DO_SAVE:
          saveInEEPROM();
          clearDisplay();
          sayConst("Saved in EEPROM");      // Save actions in EEPROM
          sayKey(SHIFT, HOME);              // Highlight it
          bProgramMode = FALSE;
          break;

        case DO_LOAD:
          loadFromEEPROM();
          clearDisplay();
          sayConst("Reloaded from EEPROM"); // Re-instate actions from EEPROM
          sayKey(SHIFT, HOME);              // Highlight it
          bProgramMode = FALSE;
          break;

        case DO_REDISPLAY:
          displayProgrammingMenu();
          break;
      }
    }
    else  // we are appending or updating an action
    {
      if (nActionFocus >= nAction) // If we are appending a new action
      {
        if (nActionFocus < ELEMENTS(aAction))  // If room to add an action
        {
          sayKey(CTL, END);
          newLine();

          aAction[nActionFocus] = action;
          sayAction(nActionFocus);
          nAction++;        // Set new high water mark
        }
      }
      else  // We are updating an existing action
      {
        aAction[nActionFocus] = action;
        selectLine(START_ACTIONS_LINE+nActionFocus);
        sayAction(nActionFocus);
      }
      nActionFocus++;   // Automatically focus on the following action
      selectLine(SELECTION_LINE);
      sayUsage(nActionFocus, &action);
      selectLine(SELECTION_LINE);
    }
  }
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
        sayKey(SHIFT, HOME);    // Highlight action
        break;

      default:
        break;
    }
    rotation = 0;  // Indicate rotary event handled
  }
  else if (ROTARY_BUTTON_PRESSED)
  {
    bUserInterrupt = FALSE;
    Delay_ms(5);  // Cheap debounce
    if (ROTARY_BUTTON_PRESSED) // If still pressed
    {
      nRemainingTimerTicks = INTERVAL_IN_SECONDS(1);  // Long press timer
      bLongPress = FALSE;
      TMR3ON_bit = 1;   // Enable long-press timer
      while (ROTARY_BUTTON_PRESSED && !rotation && !bLongPress) // pressed but not rotated
      {
        if (TMR3ON_bit && nRemainingTimerTicks == 0) // If it is a long press
        {
          TMR3ON_bit = 0; // Stop the long press timer
          bLongPress = TRUE;
        }
      }
      TMR3ON_bit = 0; // Stop the long press timer
      switch (focus)
      {
        case FOCUS_ON_PAGE:
          if (bLongPress)
          {
            clearDisplay();
            sayConst("Not saved in EEPROM");    // Leave actions in EEPROM unchanged
            sayKey(SHIFT, HOME);                // Highlight it
            bProgramMode = FALSE;
          }
          else
          {
            changePage();
          }
          break;

        case FOCUS_ON_USAGE:
          if (bLongPress)
          {
            setFocus(FOCUS_ON_PAGE);
            sayPage();
          }
          else
          {
            changeUsage();
          }
          break;

        default:
          break;
      }
      while (ROTARY_BUTTON_PRESSED);  // Wait until button is released
      Delay_ms(5);  // Cheap debounce
    }
    rotation = 0;  // Ignore rotary while button pressed
  }
  else // rotary button is not currently being pressed or rotated
  {
  }
}

uint8_t getMemory(uint8_t addr)
{
  return MEMORY[addr];
}

void setMemory(uint8_t addr, uint8_t value)
{
  MEMORY[addr] = value;
}

void setConditionCode(int8_t n)
{
  if (n == 0)
    CC = CC_Z;
  else if (n > 0)
    CC = CC_P;
  else
    CC = CC_M;
}

void playInstruction(t_action * pAction)
{
  uint8_t i;
  uint16_t nIntervals;
  switch (pAction->inst.opcode)
  {
    case EXECUTE_SET:                 // W = xx    (load constant xx)
      WRK = pAction->inst.operand;
      break;

    case EXECUTE_GET:                 // W <- [xx] (load contents of "memory" at index xx)
      WRK = getMemory(pAction->inst.operand);
      break;

    case EXECUTE_PUT:                 // W -> [xx]
      setMemory(pAction->inst.operand, WRK);
      break;

    case EXECUTE_COMPARE_IMMEDIATE:   // W : xx
      setConditionCode(WRK - pAction->inst.operand);
      break;

    case EXECUTE_COMPARE:             // W : [xx]
      setConditionCode(WRK - getMemory(pAction->inst.operand));
      break;

    case EXECUTE_SAY:                 // SAY f(xx,format)
      switch (FORMAT)
      {
        case FORMAT_CHAR:
          sayChar(getMemory(pAction->inst.operand)); // For example: A
          break;
        case FORMAT_DEC:
          sayDec(getMemory(pAction->inst.operand));  // For example: 65
          break;
        case FORMAT_HEX:
        default:
          sayHex(getMemory(pAction->inst.operand));  // For example: 41
          break;
      }
      break;

    case EXECUTE_FORMAT:              // FORMAT = char|hex|dec
      FORMAT = pAction->inst.operand;
      break;

    case EXECUTE_ADD_IMMEDIATE:       // W = W + xx
      WRK = WRK + pAction->inst.operand;
      setConditionCode(WRK);
      break;

    case EXECUTE_SUB_IMMEDIATE:       // W = W - xx
      WRK = WRK - pAction->inst.operand;
      setConditionCode(WRK);
      break;

    case EXECUTE_CLEAR:               // xx -> [00-FF]
      for (i = 0; i < sizeof(MEMORY); i++)
      {
        setMemory(i, pAction->inst.operand);
      }
      setMemory(sizeof(MEMORY), pAction->inst.operand);
      break;

    case EXECUTE_ADD:                 // W = W + [xx]
      WRK = WRK + getMemory(pAction->inst.operand);
      setConditionCode(WRK);
      break;

    case EXECUTE_SUB:                 // W = W - [xx]
      WRK = WRK - getMemory(pAction->inst.operand);
      setConditionCode(WRK);
      break;

    case EXECUTE_MUL:                 // W = W x [xx]
      WRK = WRK * getMemory(pAction->inst.operand);
      setConditionCode(WRK);
      break;

    case EXECUTE_DIV:                 // W = W / [xx]
      WRK = WRK / getMemory(pAction->inst.operand);
      setConditionCode(WRK);
      break;

    case EXECUTE_WAIT_SEC:            // Wait 0 to 255 seconds (approximately)
      sayNoKeyPressed();  // Release key (otherwise host will do a "key repeat")
      nIntervals = 200 * pAction->inst.operand;  // Number of 5 ms intervals to wait
      while (nIntervals-- && !bUserInterrupt)    // Long waits can be interrupted
      {
        Delay_ms(5);
      }
      break;

    case EXECUTE_WAIT_MS:             // Wait 0 to 255 milliseconds (approximately)
      nIntervals = pAction->inst.operand; // Number of 1 ms intervals to wait
      while (nIntervals--)
      {
        Delay_ms(1);
      }
      break;

    default:
      break;
  }
}


void play()
{
  uint8_t pc;             // Program Counter (instruction address)
  t_action * pAction = &aAction;
  bUserInterrupt = FALSE; // The user can interrupt playback by pressing the button
  for (pc = 0; pc < nAction && !bUserInterrupt; pc++, pAction++)
  {
    ACTIVITY_LED = ON;         // The LED will be turned off by the next timer interrupt
    switch (pAction->key.page)
    {
      case PAGE_KEYBOARD:
        playKeystroke(pAction);
        break;

      case PAGE_SYSTEM_CONTROL:
        playSystemControlCommand(pAction);
        break;

      case PAGE_CONSUMER_DEVICE:
        playConsumerDeviceCommand(pAction);
        break;

      case PAGE_EXECUTE:
        playInstruction(pAction);
        break;

      case PAGE_JUMP:
        switch (pAction->jump.mask)
        { //                       Condition Code: ZLHC (Side note: This is similar to IBM mainframes)
          case JUMP_RELATIVE:                   // 0000 (...not this though. IBM mainframes use mask 0 as a No-op)
            if ((uint8_t)(pc + (int8_t) pAction->jump.addr) < nAction)
            {
              pc += (int8_t) pAction->jump.addr;
              pAction = &aAction[pc];
              pAction--;
              pc--;
            }
            break;
                                                // ELH- <- Comparison
                                                // ZMPO <- Arithmetic
          case JUMP_IF_CARRY:                   // 0001
          case JUMP_IF_HIGH:                    // 0010
          case JUMP_IF_HIGH_OR_CARRY:           // 0011
          case JUMP_IF_LOW:                     // 0100
          case JUMP_IF_LOW_OR_CARRY:            // 0101
          case JUMP_IF_NOT_ZERO_OR_CARRY:       // 0110
          case JUMP_IF_NOT_ZERO:                // 0111
          case JUMP_IF_ZERO:                    // 1000
          case JUMP_IF_ZERO_OR_CARRY:           // 1001
          case JUMP_IF_NOT_LOW_OR_CARRY:        // 1010
          case JUMP_IF_NOT_LOW:                 // 1011
          case JUMP_IF_ZERO_OR_LOW:             // 1100
          case JUMP_IF_NOT_HIGH:                // 1101
          case JUMP_IF_NOT_CARRY:               // 1110
            if (!(pAction->jump.mask & CC))     // If none of the required mask bits are on in the current Condition Code
              break;                            // Then the condition is not met, so ignore the jump instruction
            // Else fall through to...
          case JUMP:                            // 1111
            if (pAction->jump.addr < nAction)
            {
              pc = pAction->jump.addr;          // Set the new Program Counter location
              pAction = &aAction[pc];           // Point to the action at that location
              pAction--;                        // Adjust down by 1 because the outer loop
              pc--;                             // will increment it
            }
            // Else ignore jumps to destinations outside the current program size
            break;

          default:
            break;
        }
        break;

      default:
        break;
    }
  }
  sayNoKeyPressed();
  if (bUserInterrupt)
  {
    Delay_ms(5); // Wait for button press bouncing to subside
    while (ROTARY_BUTTON_PRESSED); // Wait for user to release button
    Delay_ms(5); // Wait for button release bouncing to subside
  }
}

void runMode()
{
  if (ROTARY_BUTTON_PRESSED)
  {
    Delay_ms(5);  // Cheap debounce
    if (ROTARY_BUTTON_PRESSED)
    {
      nRemainingTimerTicks = INTERVAL_IN_SECONDS(1);
      TMR3ON_bit = 1; // Enable long-press timer
      while (ROTARY_BUTTON_PRESSED)
      {
        if (nRemainingTimerTicks == 0) // If it is a long press
        {
          bProgramMode = TRUE;
          TMR3ON_bit = 0;   // Disable long-press timer
          displayProgrammingMenu();
          break;
        }
      }
      TMR3ON_bit = 0;   // Disable long-press timer
      Delay_ms(5);  // Cheap debounce
      if (!bProgramMode)
      {
        play();
      }
    }
  }
}

void main()
{
  uint8_t nRead;
  Prolog();
  while (1)
  {
    if (HID_Read() == 2 && usbFromHost[0] == REPORT_ID_KEYBOARD)   // If a host LED indication response is available
    {
      leds.byte = usbFromHost[1];   // Remember the most recent LED status change
    }
    bProgramMode ? programMode() : runMode();
  }
}

void interrupt()               // High priority interrupt service routine
{
  USB_Interrupt_Proc();        // Always give the USB module first opportunity to process

  if (IOCIF_bit)               // Interrupt On Change interrupt?
  {
    if (ROTARY_BUTTON_PRESSED)
    {
      bUserInterrupt = TRUE;
    }
    state = *(stateArray + ((state & STATE_MASK) << 2 | (ROTARY_B << 1 | ROTARY_A)));
    rotation |= state & EVENT_MASK; // Extract rotary event from encoder state
    IOCIF_bit = 0;             // Clear Interrupt On Change flag
  }
  if (TMR0IF_bit)              // Timer0 interrupt? (22.9 times/second)
  {
    ACTIVITY_LED = OFF;        // Always turn the LED off after at most 44 ms
    TMR0IF_bit = 0;            // Clear the Timer0 interrupt flag
  }
  if (TMR3IF_bit)              // Timer3 interrupt? (22.9 times/second)
  {
    nRemainingTimerTicks--;    // Decrement delay count
    TMR3IF_bit = 0;            // Clear the Timer3 interrupt flag
  }

}