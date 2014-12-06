PUB! Programmable USB Button
============================
Program a button to send keystrokes to your computer


PUB! is a Programmable USB Button on which you can program a sequence of keystrokes. When you press the button those keystrokes are "replayed" over a USB connection.

The button (really a rotary encoder with a built-in push button switch) is programmed using an innovative and slightly quirky interface that does not require any custom software to be installed on your computer. All you need is a text editor like Notepad or gedit.



Features
--------
- One-button design (a rotary encoder with a built in switch).
- Programmed by using an ordinary text editor as a display (for example, gedit on Linux, or Notepad on Windows).
- Up to 127 keystrokes can be recorded and played back.
- Support for conditional logic. For example, Compare to value, Jump on zero, etc.
- Support for 256 x 8-bit "registers" to record state.
- Support for basic arithmetic. Add, subtract, etc.
- Can send USB System Control codes (Power off, sleep, wake) to your PC
- Can send USB Consumer Device functions (e.g. Mute, Play, Pause, Stop, etc.)
- Requires NO drivers (or custom software) for Windows/Linux etc


Futures
-------
The possibilities are fairly broad including:
- Using a Bluetooth module to detect proximity of the button owner's mobile phone so that the USB button is only enabled when its owner is near.
- Addition of an external EEPROM for extra keystroke capacity.
- The one-button user interface will be improved to make it easier to use.

Construction
------------

See http://www.instructables.com/id/PUB-Programmable-USB-Button/

How To Use
----------
### You want to program your password onto the PUB! button
  - Open a text editing program (for example, gedit or Notepad) and make sure it has keyboard focus
  - Plug PUB! into a host USB port
  - By default it is in RUN mode, so press and hold the rotary encoder knob for more than 1 second to switch to PROGRAM mode. It will display in your text editor:

          PUB! Programmable USB Button v0.92
          Main:   Turn=Select, Press=OK, Press+Turn=Set At, Press+Hold=Exit
             0    Set Keystroke at 00
          At Code Action

  - Press the rotary knob to set a keystroke. It will display:

          PUB! Programmable USB Button v0.92
          Key:    Turn=Select, Press+Turn=Modify, Press=OK, Press+Hold=Return
          00 0004 a
          At Code Action

  - Turn the rotary knob to choose the first letter of your password (say c) and then press the knob to add it to the list of actions:

          PUB! Programmable USB Button v0.92
          Key:    Turn=Select, Press+Turn=Modify, Press=OK, Press+Hold=Return
          01 0006 c
          At Code Action
          00 0006 c

  - Continue choosing and adding the remaining letters (say, a and t):

          PUB! Programmable USB Button v0.92
          Key:    Turn=Select, Press+Turn=Modify, Press=OK, Press+Hold=Return
          03 0017 t
          At Code Action
          00 0006 c
          01 0004 a
          02 0017 t

  - Press and hold the rotary knob to Return to the main menu:

          PUB! Programmable USB Button v0.92
          Main:   Turn=Select, Press=OK, Press+Turn=Set At, Press+Hold=Exit
             0    Set Keystroke at 03
          At Code Action
          00 0006 c
          01 0004 a
          02 0017 t

  - Rotate the knob until "Do Local Function" is displayed:

          PUB! Programmable USB Button v0.92
          Main:   Turn=Select, Press=OK, Press+Turn=Set At, Press+Hold=Exit
             0    Do Local Function at 03
          At Code Action
          00 0006 c
          01 0004 a
          02 0017 t

  - Press the knob to enter the "Do" menu:

            PUB! Programmable USB Button v0.92
            Do:     Turn=Modify, Press=OK, Press+Turn=Set At, Press+Hold=Exit
               0    Delete action at 00
            At Code Action
            00 0006 c
            01 0004 a
            02 0017 t

  - Press-and-rotate the knob anticlockwise until "Save to EEPROM" is displayed:

          PUB! Programmable USB Button v0.92
          Do:     Turn=Modify, Press+Turn=Select, Press=OK, Press+Hold=Return
                  Save to EEPROM
          At Code Action
          00 0006 c
          01 0004 a
          02 0017 t

  - Press the knob to save "cat" to the EEPROM

          Saved in EEPROM

  - The device is now in RUN mode. Press the knob to "play" the recorded sequence:

          cat

### An example of some conditional logic

  - Enter the following program into the PUB! device:

          At Code Action
          00 E005 Let W = 05
          01 E200 Put W in R00
          02 0006 c
          03 0004 a
          04 0017 t
          05 002C Space
          06 E500 Say R00
          07 0028 Enter
          08 EF01 Wait 1 sec
          09 E801 Let W = W - 01
          0A F701 Jump if Not Zero to 01

  - Now when you press the knob in run mode you should see the following displayed (1 second wait between lines):

          cat 05
          cat 04
          cat 03
          cat 02
          cat 01
