IRK! Infrared Remote USB Keyboard
=================================
Control your computer with an IR remote
---------------------------------------

IRK! is an open-hardware Infrared Remote Controlled USB keyboard, without keys, implemented using a PIC micro-controller and MikroC. It enables you to use your *learning* remote control to control any system that you can plug a USB keyboard into (Windows, Linux, Mac). Great for controlling a home theatre system such as MythTV, XBMC or Media Center. No drivers are required.

Note: Your IR remote *must* be able to *learn* infrared control signals. You *cannot* use a pre-programmed remote control with IRK!.

The original intention was to allow a user to control MythTV home theatre software using an infrared remote control. As it is seen by a PC as just another USB keyboard it can be used to control other PC functions as well.

What makes this unit different from the rest is that it can *teach* a learning remote control the IR code associated with a particular USB keystroke. As a result, there are *no* pre-programmed IR codes and IRK! does *not* have to support all the different types of infrared remotes on the market - it *teaches* the remote IRK!'s own infrared commands - which can be anything! Currently, IRK! uses Pulse Width Modulation (PWM) to encode/decode the infrared signals.

You can program -any- USB keyboard function (with or without Control, Alt, Shift and GUI modifiers) to a button on your infrared remote control and when you press that button IRK! will send the corresponding USB keystroke to the host computer.

You can program -any- USB Consumer Device control code too - for example, Play, Pause, Mute, Volume up, Volume down, and many others.


Features
--------
- Teaches your LEARNING remote control IR codes
- Lets you map IR codes to USB keystrokes - for example, Shift+Enter
- Sends the USB keystroke to your PC when you press a button on your infrared remote control
- Can send USB System Control codes (Power off, sleep, wake) to your PC
- Can can have one of 256 addresses - so you can have multiple IRK!'s
- Handles broadcasts - so multiple IRK!'s can respond to a single IR button
- Can remotely press the POWER and RESET buttons on your PC (if wired)
- Supports USB Consumer Device functions (e.g. Mute, Play, Pause, Stop, etc.)
- Has a programmable LCD backlight delay (and backlight ON/OFF commands)
- Requires NO drivers for Windows/Linux etc
- The Printed Circuit Board (PCB) comes in two flavours: 
  - Surface Mount Technology (SMT) and 
  - Through Hole Technology (THT)

Construction
------------
Go to the [Instructables](http://www.instructables.com/id/IRK-Infrared-Remote-Controlled-USB-Keyboard/) web site for detailed information on how to build your own unit.

Front Panel Buttons
-------------------

The front panel buttons comprise:

- UP - Press to increment the displayed function code. Press and hold to repeatedly increment the function code.
- DOWN - Press to decrement the displayed function code. Press and hold to repeatedly decrement the function code.
- OK - Press to accept an entry, or to cause the selected function to be executed.
- TEACH - Press to transmit a code representing the currently selected function to your IR remote control so that it can "learn" it.
- CTL - Press to toggle the keyboard "Ctrl" key. Press *and hold* to toggle the keyboard "GUI" key. The "GUI" key is a generic name for the "Windows" key on Microsoft keyboards, the "Apple" key on Apple keyboard, or the "Meta" or "Super" key on other keyboards. 
- ALT - Press to toggle the keyboard "Alt" key 
- SHIFT - Press to toggle the keyboard "Shift" key. Press *and hold* to select a different "usage page" (currently you can choose from Keyboard, Consumer Device, System Control, and local IRK! functions); then press UP/DOWN and OK to select the "usage page".


Front Panel Display 
-------------------

### Front Panel Display
The user is presented with an LCD 2 x 16 display that looks like:

<table style="border-collapse: collapse; width: 300px; text-align: left; background-color: #ccff00;">
  <tbody style="font-family: Courier New, courier, monospace; font-size: 20px; font-weight: bold; border: 10px solid #000000;">
    <tr><td style="padding-left:10px;width: 40px;">ux</td><td>line1</td></tr>
    <tr><td style="padding-left:10px;width: 40px;">yy</td><td>line2</td></tr>
  </tbody>
</table>

The codes, ux and yy, are hexadecimal values (that is, each digit can be from 0 to 9 and A to F).

-  u = The USB Usage Page index (i.e. used to assign meaning to the
      following x and yy codes). Press and hold the SHIFT button and
      then press the UP/DOWN buttons to cycle through the usage page
      indexes (0 to F and back to 0). At the moment, IRK! uses only
      the following usage page indexes:
    - 0 = Keyboard (A, B, C etc)
    - 1 = System Control (Power off, Sleep, Wake etc)
    - 2 = Consumer Devices (Mute, Vol+, Vol-, Play/Pause etc)
    - F = IRK! Local Functions (LCD backlight on/off etc)
- x = Either:
    - The key modifiers [Ctl, Alt, Shift, GUI] for Keyboard usage (see below), or else
    - The first nybble of xyy (the function to be performed) for all
          other usage pages.
- line1 = Text that describes the ux code
- yy = See below. Press and/or hold the UP and DOWN buttons to change
           the value of xyy.
- line2 = Text that describes the yy (or xyy) code.


The meanings id u, x and yy are as follows:

- When u = 0, xyy represents a Keyboard function:
    - 0x yy
         -    0 = Keyboard device
         -    x = Keyboard modifier (CTRL/ALT/SHIFT/GUI) flag bits (see below)
         -    yy = USB keyboard function code
    - 00 yy   Key yy pressed with no keyboard modifier keys
    - 01 yy   CTRL key pressed along with key yy
    - 02 yy   SHIFT key presed along with key yy
    - 04 yy   ALT key pressed along with key yy
    - 08 yy   GUI key pressed along with key yy
    
    Examples:
    - 00 05   b key pressed (05 is the code for "b")
    - 02 05   SHIFT+b key pressed (i.e. upper case B)
    - 06 05   ALT+SHIFT+b key pressed: 04(ALT) + 02(SHIFT) = 06(ALT+SHIFT)
    - 0C 05   GUI+SHIFT+b key pressed: 04(ALT) + 08(GUI)   = 0C(ALT+GUI)  

- When u = 1, xyy represents a System Control function:
    - 1x yy
         -    1 = System Control
         -    xyy = USB System Control function
    
    Examples:
    - 10 81   Power off
    - 10 82   Sleep
    - 10 83   Wake

- When u = 2, xyy represents a Consumer Device function:
    - 2x yy
         -   2 = Consumer Device
         -   xyy = USB Consumer Device function
     
    Examples:
    - 20 E2   Mute
    - 20 E9   Vol+
    - 20 EA   Vol-
    - 21 95   Internet Browser


- When u = F, xyy represents a local IRK! function (i.e. not USB):
    - Fx yy
         -   F = IRK! device
         -   xyy = Local IRK! device function
    - F0 00   Set Address ... the next LCD display will be:
        -    aa <-- Address
        -    00 Set Address
        -    You then press up/down to cycle the address (aa) then
             press OK to select it. The default address is AA.
    - F0 01   Power Switch
        - The two header pins marked PWR will become low impedance for about
          250 ms.  Note: this is not meant to switch large currents. The 4066
          quad analog switch can pass an absolute maximum of only +/- 25 mA.
    - F0 02   Reset Switch
        - The two header pins marked RST will become low impedance for about
          250 ms.
    - F0 03   Init USB
        - Causes the unit to re-register itself as a USB device. It is
          almost the equivalent of unplugging and replugging the device
          into a USB port.
    - F0 04   Back light off
        - Turns the LCD back light off
    - F0 05   Back light on
        - Turns the LCD back light off
    - F0 06   Set light delay  
        - Lets you set how many seconds you would like the LCD
          backlight to remain on after you have pressed a panel 
          button or issued a valid infrared command.
        - The next LCD display will be one of the following:
            -   xx <-- nnn secs (where nnn is xx in decimal)
            -   FF <-- ON       (keep back light always on)
            -   00 <-- OFF      (keep back light always off)
        -   You then press up/down to choose the delay value (xx) then
            press OK to select it.
    - F0 07   Debug on 
        - Displays debug information on the LCD when an IR code is received
    - F0 08   Debug off 
        - Returns the unit to Normal mode (displays no debug information on the LCD)
    - F0 09   Auxiliary Switch
        - The two header pins marked AUX will become low impedance for about
          250 ms.
    - F0 0A   Power Switch On
        - The two header pins marked PWR will become low impedance until a
          subsequent Power Switch Off command is received.
    - F0 0B   Reset Switch On
        - The two header pins marked RST will become low impedance until a
          subsequent Power Switch Off command is received.
    - F0 0C   Auxiliary Switch On
        - The two header pins marked AUX will become low impedance until a
          subsequent Power Switch Off command is received.
    - F0 0D   Power Switch Off
        - The two header pins marked PWR will become high impedance.
    - F0 0E   Reset Switch Off
        - The two header pins marked RST will become high impedance.
    - F0 0F   Auxiliary Switch Off
        - The two header pins marked AUX will become high impedance.

- Other values (u = 3 to F) are currently reserved for future use.
            
Examples
--------            
1. The user wants to program the Enter key on the OK button of a
   learning remote control. The steps to follow are:
    - Plug IRK! into a host USB port
    - The default usage mode is Keyboard, which is displayed as
    "00 Keyboard" in the first line of the LCD, so just
    press and hold the UP button until the second line of the LCD
    shows "28 Enter".
    - Switch your learning remote control into "Learn" mode and
    select the remote's button to be programmed.
    - Press IRK!'s TEACH button. This will send an IR code that
    means "Enter" to your learning remote.
    - Repeat until the learning remote has learned that IR code.
    - Press the programmed button on the learning remote control.
    - Verify that IRK! has received the IR code (it will flash
    its LED when it has recognised a valid IR code). Verify 
    that the "Enter" USB Keyboard function has been performed
    at the host.

2. The user wants to program the "Mute" Consumer Devices function
   on the "Mute" button of a learning remote control. The default
   usage mode of IRK! is "Keyboard", so you need to switch IRK! into
   "Consumer Dev" mode first:
    - Press and hold IRK!'s SHIFT button until the second line of
    the LCD shows "^^ Select Usage".
    - Press IRK!'s UP button until "20 Consumer Dev" is displayed in
    the first line of the LCD.
    - Press IRK!'s OK button to enter "Consumer Dev" usage mode.
    - Press and hold IRK!'s UP button until "E2 Mute" is displayed
    on the second line of the LCD. If you overshoot then press the
    DOWN button repeatedly until you reach "E2 Mute".
    - Press OK to verify that the Mute function is accepted by the
    host.
    - Put your learning remote control into "Learn" mode and choose
    one of the remote's buttons to be programmed as "Mute".
    - Position your learning remote's IR receiver to within 5 cm of
    IRK!'s IR transmitter LED.
    - Press IRK!'s TEACH button. This will send an IR code that 
    means "Mute" to your learning remote.
    - Keep pressing TEACH until the learning remote has learned that
    IR code.
    - Press the programmed button on the learning remote control.
    - Verify that IRK! has received the IR code (it will flash
    its LED when it has recognised a valid IR code). Verify 
    that the "Mute" USB consumer device function has been performed
    at the host.