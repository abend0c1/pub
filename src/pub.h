#define VERSION "0.92"

#define OUTPUT        0
#define INPUT         1

#define ON            1
#define OFF           0

#define HIGH          1
#define LOW           0

#define ENABLE        1
#define DISABLE       0

#define TRUE  (1==1)
#define FALSE !TRUE

#define ELEMENTS(array) (sizeof(array)/sizeof(array[0]))
#define CLOCK_FREQUENCY       (__FOSC__ * 1000)
#define TIMER3_PRESCALER      8
#define INTERVAL_IN_SECONDS(n) (CLOCK_FREQUENCY / 4 / TIMER3_PRESCALER / 65536 * n)

#define INFO_LINE           2
#define SELECTION_LINE      3
#define START_ACTIONS_LINE  6

#define NONE  0b00000000
#define SHIFT 0b00000001
#define CTL   0b00000010
#define ALT   0b00000100
#define GUI   0b00001000

#define ROTATED     0b00000010
#define NOT_ROTATED 0b00000000
#define PRESSED     0b00000001
#define NOT_PRESSED 0b00000000

/*
#define USE_LAT

#ifdef USE_LAT
  #define ASSIGN_PIN(name,b) \
  sbit name               at LAT ## b ## _bit; \
  sbit name ## _Direction at TRIS ## b ## _bit;
#else
  #define ASSIGN_PIN(name,b) \
  sbit name               at R ## b ## _bit; \
  sbit name ## _Direction at TRIS ## b ## _bit;
#endif


ASSIGN_PIN(LCD_EN, B0)
ASSIGN_PIN(LCD_RS, B1)
ASSIGN_PIN(LCD_D4, B2)
ASSIGN_PIN(LCD_D5, B3)
ASSIGN_PIN(LCD_D6, B4)
ASSIGN_PIN(LCD_D7, B5)
*/

uint8_t MEMORY[256];   // Memory

uint8_t FORMAT;   // Format for SAY instruction
#define FORMAT_HEX  0
#define FORMAT_DEC  1
#define FORMAT_CHAR 2

uint8_t WRK;      // Working register

uint8_t CC;       // Condition Code
#define CC_Z 0x8
#define CC_M 0x4
#define CC_P 0x2
#define CC_O 0x1



typedef struct
{
  union
  {
    uint8_t byte;
    struct
    {
      LeftShift:1;      // .......1
      LeftControl:1;    // ......1.
      LeftAlt:1;        // .....1..
      LeftGUI:1;        // ....1...
      RightShift:1;     // ...1....
      RightControl:1;   // ..1.....
      RightAlt:1;       // .1......
      RightGUI:1;       // 1.......
    } bits;
  };
} t_keyModifiers;


typedef struct
{ // Keyboard:      // 0000mmmmuuuuuuuu
  uint8_t usage;    //         uuuuuuuu
  uint8_t mod:4;    //     mmmm
#define MODIFIER_LEFTSHIFT   0b0001
#define MODIFIER_LEFTCTL     0b0010
#define MODIFIER_LEFTALT     0b0100
#define MODIFIER_LEFTGUI     0b1000
  uint8_t page:4;   // 0000
} t_keyboardAction;


typedef struct
{ // Consumer Device: // 0001uuuuuuuuuuuu
  uint8_t usage:12;   //     uuuuuuuuuuuu
  uint8_t page:4;     // 0001
} t_consumerDeviceAction;

typedef struct
{ // System Control:  // 0010....uuuuuuuu
  uint8_t usage;      //         uuuuuuuu
  uint8_t :4;         //     ....
  uint8_t page:4;     // 0010
} t_systemControlAction;

typedef struct
{ // Instruction:   // 1110ccccoooooooo
  uint8_t operand;  //         oooooooo
  uint8_t opcode:4; //     cccc
  uint8_t page:4;   // 1110
} t_instAction;

typedef struct
{ // Jump:          // 1111mmmmaaaaaaaa
  uint8_t addr;     //         aaaaaaaa
  uint8_t mask:4;   //     mmmm
  uint8_t page:4;   // 1111
} t_jumpAction;

typedef union
{
  t_keyboardAction       key;   // 0000mmmmuuuuuuuu = 0x0muu
  t_consumerDeviceAction cons;  // 0001uuuuuuuuuuuu = 0x1uuu
  t_systemControlAction  sys;   // 0010....uuuuuuuu = 0x2.uu
  t_instAction           inst;  // 1110ccccoooooooo = 0xEcoo
  t_jumpAction           jump;  // 1111mmmmaaaaaaaa = 0xFmaa
  uint16_t               action;// ................ = 0x....
} t_action;

uint8_t nAction;          // Number of actions
uint8_t nActionFocus;   // Action with the current focus
t_action action;
t_action aAction[127];    // 127 x 2-byte actions + 2-byte header fills the onboard EEPROM

#define FOCUS_ON_PAGE  0
#define FOCUS_ON_USAGE 1
uint8_t focus = FOCUS_ON_PAGE;


volatile bit bUserInterrupt;
volatile int8_t rotation;  // 0 = no rotary event, +n = clockwise, -n = anticlockwise
volatile uint8_t nRemainingTimerTicks;


/*
    .---------------------------------------.
    |          REPORT_ID_KEYBOARD           | OUT: Report Id
    |---------------------------------------|
    |    |    |    |    |    |SCRL|CAPL|NUML| OUT: NumLock,CapsLock,ScrollLock - and 5 unused pad bits
    '---------------------------------------'
*/
typedef union
{
  uint8_t byte;
  struct
  {
    NumLock:1;      // xxxxxxx1
    CapsLock:1;     // xxxxxx1x
    ScrollLock:1;   // xxxxx1xx
    :5;             // Pad
  } bits;
} t_ledIndicators;


// Buttons (weak pullups are enabled so they read high when not pressed)

sbit ROTARY_A                   at RB5_bit;
sbit ROTARY_B                   at RB4_bit;
sbit ROTARY_BUTTON              at RB6_bit;
#define ROTARY_BUTTON_PRESSED   !ROTARY_BUTTON

#define ACTIVITY_LED          LATA0_bit

volatile uint8_t             cFlags;
#define bUSBReady            cFlags.B0
#define bProgramMode         cFlags.B1
#define bLongPress           cFlags.B2
#define bUserInterrupt       cFlags.B3

// USB buffers must be in USB RAM, hence the "absolute" specifier...
uint8_t BANK4_RESERVED_FOR_USB[256] absolute 0x400; // Prevent compiler from allocating
                                                    // RAM variables in Bank 4 because
                                                    // the USB hardware uses that bank.
                                                    // Refer to the PIC18F25K50 datasheet
                                                    // section "6.4.1 USB RAM" for more
                                                    // information.
uint8_t usbFromHost[1+1] absolute 0x500;  // Buffer for PIC <-- Host (ReportId + 1 byte)
uint8_t usbToHost[1+3]   absolute 0x508;  // Buffer for PIC --> Host (ReportId + 3 bytes)


union
{
  uint16_t xxyy;            // xxyy (xyy is changed by UP/DOWN buttons)
  struct
  {                         // This overlays the Little-Endian xxyy above
    uint8_t yy;             //   yy
    t_keyModifiers xx;      // xx   (x is changed by CTL/ATL/SHIFT buttons)
  } s;
} usbData;

#define PAGE_KEYBOARD              0x0

#define PAGE_SYSTEM_CONTROL        0x1

#define PAGE_CONSUMER_DEVICE       0x2

#define PAGE_DO                    0xD
  #define DO_DELETE                  0x0
  #define DO_REDISPLAY               0x1
  //      DO_                        0x2
  //      DO_                        0x3
  //      DO_                        0x4
  //      DO_                        0x5
  //      DO_                        0x6
  //      DO_                        0x7
  //      DO_                        0x8
  //      DO_                        0x9
  //      DO_                        0xA
  //      DO_                        0xB
  //      DO_                        0xC
  //      DO_                        0xD
  #define DO_LOAD                    0xE
  #define DO_SAVE                    0xF

#define PAGE_EXECUTE               0xE
  #define EXECUTE_SET                0x0
  #define EXECUTE_GET                0x1
  #define EXECUTE_PUT                0x2
  #define EXECUTE_COMPARE_IMMEDIATE  0x3
  #define EXECUTE_COMPARE            0x4
  #define EXECUTE_SAY                0x5
  #define EXECUTE_FORMAT             0x6
  #define EXECUTE_ADD_IMMEDIATE      0x7
  #define EXECUTE_SUB_IMMEDIATE      0x8
  #define EXECUTE_CLEAR              0x9
  #define EXECUTE_ADD                0xA
  #define EXECUTE_SUB                0xB
  #define EXECUTE_MUL                0xC
  #define EXECUTE_DIV                0xD
  #define EXECUTE_WAIT_MS            0xE
  #define EXECUTE_WAIT_SEC           0xF

#define PAGE_JUMP                  0xF
  #define JUMP_RELATIVE              0x0
  #define JUMP_IF_CARRY              0x1
  #define JUMP_IF_HIGH               0x2
  #define JUMP_IF_HIGH_OR_CARRY      0x3
  #define JUMP_IF_LOW                0x4
  #define JUMP_IF_LOW_OR_CARRY       0x5
  #define JUMP_IF_NOT_ZERO_OR_CARRY  0x6
  #define JUMP_IF_NOT_ZERO           0x7
  #define JUMP_IF_ZERO               0x8
  #define JUMP_IF_ZERO_OR_CARRY      0x9
  #define JUMP_IF_NOT_LOW_OR_CARRY   0xA
  #define JUMP_IF_NOT_LOW            0xB
  #define JUMP_IF_ZERO_OR_LOW        0xC
  #define JUMP_IF_NOT_HIGH           0xD
  #define JUMP_IF_NOT_CARRY          0xE
  #define JUMP                       0xF


const char ASCII_to_USB[] =
{ // Top bit on means capitalise with Left Shift
  //         00    01    02    03    04    05    06    07    08    09    0A    0B    0C    0D    0E    0F
  //        NUL   SOH   STX   ETX   EOT   ENQ   ACK   BEL    BS    HT    LF    VT    FF    CR    SO    SI
  /* 00 */ 0x2C, 0x2C, 0x2C, 0x2C, 0x2C, 0x2C, 0x2C, 0x2C, 0x2A, 0x2B, 0x28, 0x2C, 0x2C, 0x4A, 0x2C, 0x2C,
  //        DLE   DC1   DC2   DC3   DC4   NAK    SYN  ETB   CAN    EM   SUB   ESC    FS    GS    RS    US
  /* 10 */ 0x2C, 0x2C, 0x2C, 0x2C, 0x2C, 0x2C, 0x2C, 0x2C, 0x2C, 0x2C, 0x2C, 0x2C, 0x2C, 0x2C, 0x2C, 0x2C,
  //                !     "     #     $     %     &     '     (     )     *     +     ,     -     .     /
  /* 20 */ 0x2C, 0x9E, 0x2C, 0xA0, 0xA1, 0xA2, 0xA4, 0x34, 0xA6, 0xA7, 0xA5, 0xAE, 0x36, 0x2D, 0x37, 0x38,
  //          0     1     2     3     4     5     6     7     8     9     :     ;     <     =     >     ?
  /* 30 */ 0x27, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0xB3, 0x33, 0xB6, 0x2E, 0xB7, 0xB8,
  //          @     A     B     C     D     E     F     G     H     I     J     K     L     M     N     O
  /* 40 */ 0x9F, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F, 0x90, 0x91, 0x92,
  //          P     Q     R     S     T     U     V     W     X     Y     Z     [     \     ]     ^     _
  /* 50 */ 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x2F, 0x31, 0x30, 0xA3, 0xAD,
  //          `     a     b     c     d     e     f     g     h     i     j     k     l     m     n     o
  /* 60 */ 0x35, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12,
  //          p     q     r     s     t     u     v     w     x     y     z     {     |     }     ~   DEL
  /* 70 */ 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0xAF, 0xB1, 0xB0, 0xB2, 0x2A,
};

const char * const UNSHIFTED_USB_DESC[] =
{
  /* 00 */ "No Op", "",          "",         "",        "a",          "b",    "c",      "d",       "e",        "f",        "g",         "h",           "i",       "j",     "k",        "l",
  /* 10 */ "m",     "n",         "o",        "p",       "q",          "r",    "s",      "t",       "u",        "v",        "w",         "x",           "y",       "z",     "1",        "2",
  /* 20 */ "3",     "4",         "5",        "6",       "7",          "8",    "9",      "0",       "Enter",    "Esc",      "Backspace", "Tab",         "Space",   "-",     "=",        "[",
  /* 30 */ "]",     "\\",        "#",        ";",       "'",          "`",    ",",      ".",       "/",        "CapsLock", "F1",        "F2",          "F3",      "F4",    "F5",       "F6",
  /* 40 */ "F7",    "F8",        "F9",       "F10",     "F11",        "F12",  "PrtScr", "ScrLock", "Pause",    "Ins",      "Home",      "PageUp",      "Delete",  "End",   "PageDown", "Right",
  /* 50 */ "Left",  "Down",      "Up",       "NumLock", "KP /",       "KP *", "KP -",   "KP +",    "KP Enter", "KP 1",     "KP 2",      "KP 3",        "KP 4",    "KP 5",  "KP 6",     "KP 7",
  /* 60 */ "KP 8",  "KP 9",      "KP 0",     "KP .",    "\\",         "Appl", "Power",  "KP =",    "F13",      "F14",      "F15",       "F16",         "F17",     "F18",   "F19",      "F20",
  /* 70 */ "F21",   "F22",       "F23",      "F24",     "Exec",       "Help", "Menu",   "Select",  "Stop",     "Again",    "Undo",      "Cut",         "Copy",    "Paste", "Find",     "Mute",
  /* 80 */ "Vol+",  "Vol-",      "LockCaps", "LockNum", "LockScroll", "KP ,", "KP =",
};

const char * const SHIFTED_USB_DESC[] =
{
  /* 00 */ "",      "",          "",         "",        "A",          "B",    "C",      "D",       "E",        "F",        "G",         "H",           "I",       "J",     "K",        "L",
  /* 10 */ "M",     "N",         "O",        "P",       "Q",          "R",    "S",      "T",       "U",        "V",        "W",         "X",           "Y",       "Z",     "!",        "@",
  /* 20 */ "#",     "$",         "%",        "^",       "&",          "*",    "(",      ")",       "",         "",         "",          "",            "",        "_",     "+",        "{",
  /* 30 */ "}",     "|",         "~",        ":",       "\"",         "~",    "<",      ">",       "?",        "",         "",          "",            "",        "",      "",         "",
  /* 40 */ "",      "",          "",         "",        "",           "",     "",       "",        "",         "",         "",          "",            "",        "",      "",         "",
  /* 50 */ "",      "",          "",         "Clear",   "",           "",     "",       "",        "",         "KP End",   "KP Down",   "KP PageDown", "KP Left", "",      "KP Right", "KP Home",
  /* 60 */ "KP Up", "KP PageUp", "KP Ins",   "KP Del",  "|",
};

const char * const SYSTEM_CONTROL_DESC[] =
{
// 81 System Power Down,OSC,
// 82 System Sleep,OSC,
// 83 System Wake Up,OSC,
// 84 System Context Menu,OSC,
// 85 System Main Menu,OSC,
// 86 System App Menu,OSC,
// 87 System Menu Help,OSC,
// 88 System Menu Exit,OSC,
// 89 System Menu Select,OSC,
// 8A System Menu Right,RTC,
// 8B System Menu Left,RTC,
// 8C System Menu Up,RTC,
// 8D System Menu Down,RTC,
// 8E System Cold Restart,OSC,
// 8F System Warm Restart,OSC,
   "", "Power Down", "Sleep", "Wake Up", "Context Menu", "App Menu", "Menu Help", "Menu Exit", "Menu Select", "Menu Right" , "Menu Left", "Menu Up", "Menu Down", "Cold Restart", "Warm Restart",
};

const char * const CONSUMER_DEVICE_DESC[] =
{
// 00 Unassigned
// 01 Consumer Control,CA,
// 02 Numeric Key Pad,NAry,
// 03 Programmable Buttons,NAry,
// 04 Microphone,CA,
// 05 Headphone,CA,
// 06 Graphic Equalizer,CA,
  "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
  "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
// 20 +10,OSC,Plus10
// 21 +100,OSC,Plus100
// 22 AM/PM,OSC,
  "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
// 30 Power,OOC,
// 31 Reset,OSC,
// 32 Sleep,OSC,
// 33 Sleep After,OSC,
// 34 Sleep Mode,RTC,
// 35 Illumination,OOC,
// 36 Function Buttons,NAry,
  "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
// 40 Menu,OOC,
// 41 Menu Pick,OSC,
// 42 Menu Up,OSC,
// 43 Menu Down,OSC,
// 44 Menu Left,OSC,
// 45 Menu Right,OSC,
// 46 Menu Escape,OSC,
// 47 Menu Value Increase,OSC,
// 48 Menu Value Decrease,OSC,
  "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
  "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
// 60 Data On Screen,OOC,
// 61 Closed Caption,OOC,
// 62 Closed Caption Select,OSC,
// 63 VCR/TV,OOC,
// 64 Broadcast Mode,OSC,
// 65 Snapshot,OSC,
// 66 Still,OSC,
// 67 Picture-in-Picture Toggle,OSC,
// 68 Picture-in-Picture Swap,OSC,
// 69 Red Menu Button,MC,
// 6A Green Menu Button,MC,
// 6B Blue Menu Button,MC,
// 6C Yellow Menu Button,MC,
// 6D Aspect,OSC,
// 6E 3D Mode Select,OSC,
// 6F Display Brightness Increment,RTC,
  "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
// 70 Display Brightness Decrement,RTC,
// 71 Display Brightness,LC,
// 72 Display Backlight Toggle,OOC,
// 73 Display Set Brightness to Minimum,OOC,
// 74 Display Set Brightness to Maximum,OOC,
// 75 Display Set Auto Brightness,OOC,
  "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
// 80 Selection,NAry,
// 81 Assign Selection,OSC,
// 82 Mode Step,OSC,
// 83 Recall Last,OSC,
// 84 Enter Channel,OSC,
// 85 Order Movie,OSC,
// 86 Channel,LC,
// 87 Media Selection,NAry,
// 88 Media Select Computer,Sel,
// 89 Media Select TV,Sel,
// 8A Media Select WWW,Sel,
// 8B Media Select DVD,Sel,
// 8C Media Select Telephone,Sel,
// 8D Media Select Program Guide,Sel,
// 8E Media Select Video Phone,Sel,
// 8F Media Select Games,Sel,
  "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
// 90 Media Select Messages,Sel,
// 91 Media Select CD,Sel,
// 92 Media Select VCR,Sel,
// 93 Media Select Tuner,Sel,
// 94 Quit,OSC,
// 95 Help,OOC,
// 96 Media Select Tape,Sel,
// 97 Media Select Cable,Sel,
// 98 Media Select Satellite,Sel,
// 99 Media Select Security,Sel,
// 9A Media Select Home,Sel,
// 9B Media Select Call,Sel,
// 9C Channel Increment,OSC,
// 9D Channel Decrement,OSC,
// 9E Media Select SAP,Sel,
  "", "", "", "", "", "", "", "", "", "", "", "", "", "Ch+", "Ch-", "",
// A0 VCR Plus,OSC,
// A1 Once,OSC,
// A2 Daily,OSC,
// A3 Weekly,OSC,
// A4 Monthly,OSC,
  "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
// B0 Play,OOC,
// B1 Pause,OOC,
// B2 Record,OOC,
// B3 Fast Forward,OOC,
// B4 Rewind,OOC,
// B5 Scan Next Track,OSC,
// B6 Scan Previous Track,OSC,
// B7 Stop,OSC,
// B8 Eject,OSC,
// B9 Random Play,OOC,
// BA Select Disc,NAry,
// BB Enter Disc,MC,
// BC Repeat,OSC,
// BD Tracking,LC,
// BE Track Normal,OSC,
// BF Slow Tracking,LC,
  "Play", "Pause", "Record", "FF", "Rew", "Next Track", "Prev Track", "Stop", "Eject", "Random", "", "", "Repeat", "", "", "",
// C0 Frame Forward,RTC,
// C1 Frame Back,RTC,
// C2 Mark,OSC,
// C3 Clear Mark,OSC,
// C4 Repeat From Mark,OOC,
// C5 Return To Mark,OSC,
// C6 Search Mark Forward,OSC,
// C7 Search Mark Backwards,OSC,
// C8 Counter Reset,OSC,
// C9 Show Counter,OSC,
// CA Tracking Increment,RTC,
// CB Tracking Decrement,RTC,
// CC Stop/Eject,OSC,
// CD Play/Pause,OSC,
// CE Play/Skip,OSC,
// CF Voice Command,OSC,
  "", "", "", "", "", "", "", "", "", "", "", "", "Stop/Eject", "Play/Pause", "", "",
  "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
// E0 Volume,LC,
// E1 Balance,LC,
// E2 Mute,OOC,
// E3 Bass,LC,
// E4 Treble,LC,
// E5 Bass Boost,OOC,
// E6 Surround Mode,OSC,
// E7 Loudness,OOC,
// E8 MPX,OOC,MPX
// E9 Volume Increment,RTC,
// EA Volume Decrement,RTC,
  "", "", "Mute", "", "", "Bass Boost", "Surround", "Loudness", "MPX", "Vol+", "Vol-", "", "", "", "", "",
// F0 Speed Select,OSC,
// F1 Playback Speed,NAry,
// F2 Standard Play,Sel,
// F3 Long Play,Sel,
// F4 Extended Play,Sel,
// F5 Slow,OSC,
  "", "", "", "", "", "Slow", "", "", "", "", "", "", "", "", "", "",
// 100 Fan Enable,OOC,
// 101 Fan Speed,LC,
// 102 Light Enable,OOC,
// 103 Light Illumination Level,LC,
// 104 Climate Control Enable,OOC,
// 105 Room Temperature,LC,
// 106 Security Enable,OOC,
// 107 Fire Alarm,OSC,
// 108 Police Alarm,OSC,
// 109 Proximity,LC,
// 10A Motion,OSC,
// 10B Duress Alarm,OSC,
// 10C Holdup Alarm,OSC,
// 10D Medical Alarm,OSC,
  "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
  "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
  "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
  "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
  "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
// 150 Balance Right,RTC,
// 151 Balance Left,RTC,
// 152 Bass Increment,RTC,
// 153 Bass Decrement,RTC,
// 154 Treble Increment,RTC,
// 155 Treble Decrement,RTC,
  "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
// 160 Speaker System,CL,
// 161 Channel Left,CL,
// 162 Channel Right,CL,
// 163 Channel Center,CL,
// 164 Channel Front,CL,
// 165 Channel Center Front,CL,
// 166 Channel Side,CL,
// 167 Channel Surround,CL,
// 168 Channel Low Frequency Enhancement,CL,
// 169 Channel Top,CL,
// 16A Channel Unknown,CL,
  "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
// 170 Sub-channel,LC,
// 171 Sub-channel Increment,OSC,
// 172 Sub-channel Decrement,OSC,
// 173 Alternate Audio Increment,OSC,
// 174 Alternate Audio Decrement,OSC,
  "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
// 180 Application Launch Buttons,NAry,
// 181 AL Launch Button Configuration Tool,Sel,
// 182 AL Programmable Button Configuration,Sel,
// 183 AL Consumer Control Configuration,Sel,
// 184 AL Word Processor,Sel,
// 185 AL Text Editor,Sel,
// 186 AL Spreadsheet,Sel,
// 187 AL Graphics Editor,Sel,
// 188 AL Presentation App,Sel,
// 189 AL Database App,Sel,
// 18A AL Email Reader,Sel,
// 18B AL Newsreader,Sel,
// 18C AL Voicemail,Sel,
// 18D AL Contacts/Address Book,Sel,
// 18E AL Calendar/Schedule,Sel,
// 18F AL Task/Project Manager,Sel,
  "", "", "", "", "Word Processor", "Text Editor", "Spreadsheet", "Graphics Editor", "Presentation", "Database", "Email", "News", "Voicemail", "Contacts", "Calendar", "Project Manager",
// 190 AL Log/Journal/Timecard,Sel,
// 191 AL Checkbook/Finance,Sel,
// 192 AL Calculator,Sel,
// 193 AL A/V Capture/Playback,Sel,
// 194 AL Local Machine Browser,Sel,
// 195 AL LAN/WAN Browser,Sel,
// 196 AL Internet Browser,Sel,
// 197 AL Remote Networking/ISP Connect,Sel,
// 198 AL Network Conference,Sel,
// 199 AL Network Chat,Sel,
// 19A AL Telephony/Dialer,Sel,
// 19B AL Logon,Sel,
// 19C AL Logoff,Sel,
// 19D AL Logon/Logoff,Sel,
// 19E AL Terminal Lock/Screensaver,Sel,
// 19F AL Control Panel,Sel,
  "", "", "Calculator", "", "", "", "Web Browser", "", "", "", "Telephony", "Logon", "Logoff", "", "Terminal Lock", "Control Panel",
// 1A0 AL Command Line Processor/Run,Sel,
// 1A1 AL Process/Task Manager,Sel,
// 1A2 AL Select Task/Application,Sel,
// 1A3 AL Next Task/Application,Sel,
// 1A4 AL Previous Task/Application,Sel,
// 1A5 AL Preemptive Halt Task/Application,Sel,
// 1A6 AL Integrated Help Center,Sel,
// 1A7 AL Documents,Sel,
// 1A8 AL Thesaurus,Sel,
// 1A9 AL Dictionary,Sel,
// 1AA AL Desktop,Sel,
// 1AB AL Spell Check,Sel,
// 1AC AL Grammar Check,Sel,
// 1AD AL Wireless Status,Sel,
// 1AE AL Keyboard Layout,Sel,
// 1AF AL Virus Protection,Sel,
  "Command Line", "Task Manager", "", "", "", "", "", "", "", "", "Desktop", "", "", "", "", "",
// 1B0 AL Encryption,Sel,
// 1B1 AL Screen Saver,Sel,
// 1B2 AL Alarms,Sel,
// 1B3 AL Clock,Sel,
// 1B4 AL File Browser,Sel,
// 1B5 AL Power Status,Sel,
// 1B6 AL Image Browser,Sel,
// 1B7 AL Audio Browser,Sel,
// 1B8 AL Movie Browser,Sel,
// 1B9 AL Digital Rights Manager,Sel,
// 1BA AL Digital Wallet,Sel,
// 1BC AL Instant Messaging,Sel,
// 1BD AL OEM Features/Tips/Tutorial Browser,Sel,
// 1BE AL OEM Help,Sel,
// 1BF AL Online Community,Sel,
  "", "", "", "Clock", "File Browser", "", "Image Browser", "Audio Browser", "Movie Browser", "", "", "Messaging", "", "", "", "",
// 1C0 AL Entertainment Content Browser,Sel,
// 1C1 AL Online Shopping Browser,Sel,
// 1C2 AL SmartCard Information/Help,Sel,
// 1C3 AL Market Monitor/Finance Browser,Sel,
// 1C4 AL Customized Corporate News Browser,Sel,
// 1C5 AL Online Activity Browser,Sel,
// 1C6 AL Research/Search Browser,Sel,
// 1C7 AL Audio Player,Sel,
  "", "", "", "", "", "", "Audio Player", "", "", "", "", "", "", "", "", "",
  "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
  "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
  "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
// 200 Generic GUI Application Controls,NAry,
// 201 AC New,Sel,
// 202 AC Open,Sel,
// 203 AC Close,Sel,
// 204 AC Exit,Sel,
// 205 AC Maximize,Sel,
// 206 AC Minimize,Sel,
// 207 AC Save,Sel,
// 208 AC Print,Sel,
// 209 AC Properties,Sel,
  "", "New", "Open", "Close", "Exit", "Maximise", "Minimise", "Save", "Print", "", "", "", "", "", "", "",
// 21A AC Undo,Sel,
// 21B AC Copy,Sel,
// 21C AC Cut,Sel,
// 21D AC Paste,Sel,
// 21E AC Select All,Sel,
// 21F AC Find,Sel,
  "", "", "", "", "", "", "", "", "", "", "Undo", "Copy", "Cut", "Paste", "Select All", "Find",
// 220 AC Find and Replace,Sel,
// 221 AC Search,Sel,
// 222 AC Go To,Sel,
// 223 AC Home,Sel,
// 224 AC Back,Sel,
// 225 AC Forward,Sel,
// 226 AC Stop,Sel,
// 227 AC Refresh,Sel,
// 228 AC Previous Link,Sel,
// 229 AC Next Link,Sel,
// 22A AC Bookmarks,Sel,
// 22B AC History,Sel,
// 22C AC Subscriptions,Sel,
// 22D AC Zoom In,Sel,
// 22E AC Zoom Out,Sel,
// 22F AC Zoom,LC,
  "Replace", "Search", "Go To", "Home", "Back", "Forward", "Stop", "Refresh", "Prev Link", "Next Link", "Bookmarks", "History", "", "Zoom In", "Zoom Out", "",
// 230 AC Full Screen View,Sel,
// 231 AC Normal View,Sel,
// 232 AC View Toggle,Sel,
// 233 AC Scroll Up,Sel,
// 234 AC Scroll Down,Sel,
// 235 AC Scroll,LC,
// 236 AC Pan Left,Sel,
// 237 AC Pan Right,Sel,
// 238 AC Pan,LC,
// 239 AC New Window,Sel,
// 23A AC Tile Horizontally,Sel,
// 23B AC Tile Vertically,Sel,
// 23C AC Format,Sel,
// 23D AC Edit,Sel,
// 23E AC Bold,Sel,
// 23F AC Italics,Sel,
  "Full Screen", "Normal View", "Toggle View", "Scroll Up", "Scroll Down", "", "", "", "", "New Window", "Tile Horz", "Tile Vert", "", "", "", "",
// 240 AC Underline,Sel,
// 241 AC Strikethrough,Sel,
// 242 AC Subscript,Sel,
// 243 AC Superscript,Sel,
// 244 AC All Caps,Sel,
// 245 AC Rotate,Sel,
// 246 AC Resize,Sel,
// 247 AC Flip horizontal,Sel,
// 248 AC Flip Vertical,Sel,
// 249 AC Mirror Horizontal,Sel,
// 24A AC Mirror Vertical,Sel,
// 24B AC Font Select,Sel,
// 24C AC Font Color,Sel,
// 24D AC Font Size,Sel,
// 24E AC Justify Left,Sel,
// 24F AC Justify Center H,Sel,
  "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
// 250 AC Justify Right,Sel,
// 251 AC Justify Block H,Sel,
// 252 AC Justify Top,Sel,
// 253 AC Justify Center V,Sel,
// 254 AC Justify Bottom,Sel,
// 255 AC Justify Block V,Sel,
// 256 AC Indent Decrease,Sel,
// 257 AC Indent Increase,Sel,
// 258 AC Numbered List,Sel,
// 259 AC Restart Numbering,Sel,
// 25A AC Bulleted List,Sel,
// 25B AC Promote,Sel,
// 25C AC Demote,Sel,
// 25D AC Yes,Sel,
// 25E AC No,Sel,
// 25F AC Cancel,Sel,
  "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
// 260 AC Catalog,Sel,
// 261 AC Buy/Checkout,Sel,
// 262 AC Add to Cart,Sel,
// 263 AC Expand,Sel,
// 264 AC Expand All,Sel,
// 265 AC Collapse,Sel,
// 266 AC Collapse All,Sel,
// 267 AC Print Preview,Sel,
// 268 AC Paste Special,Sel,
// 269 AC Insert Mode,Sel,
// 26A AC Delete,Sel,
// 26B AC Lock,Sel,
// 26C AC Unlock,Sel,
// 26D AC Protect,Sel,
// 26E AC Unprotect,Sel,
// 26F AC Attach Comment,Sel,
  "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
// 270 AC Delete Comment,Sel,
// 271 AC View Comment,Sel,
// 272 AC Select Word,Sel,
// 273 AC Select Sentence,Sel,
// 274 AC Select Paragraph,Sel,
// 275 AC Select Column,Sel,
// 276 AC Select Row,Sel,
// 277 AC Select Table,Sel,
// 278 AC Select Object,Sel,
// 279 AC Redo/Repeat,Sel,
// 27A AC Sort,Sel,
// 27B AC Sort Ascending,Sel,
// 27C AC Sort Descending,Sel,
// 27D AC Filter,Sel,
// 27E AC Set Clock,Sel,
// 27F AC View Clock,Sel,
  "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
// 280 AC Select Time Zone,Sel,
// 281 AC Edit Time Zones,Sel,
// 282 AC Set Alarm,Sel,
// 283 AC Clear Alarm,Sel,
// 284 AC Snooze Alarm,Sel,
// 285 AC Reset Alarm,Sel,
// 286 AC Synchronize,Sel,
// 287 AC Send/Receive,Sel,
// 288 AC Send To,Sel,
// 289 AC Reply,Sel,
// 28A AC Reply All,Sel,
// 28B AC Forward Msg,Sel,
// 28C AC Send,Sel,
// 28D AC Attach File,Sel,
// 28E AC Upload,Sel,
// 28F AC Download (Save Target As),Sel,
  "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
// 290 AC Set Borders,Sel,
// 291 AC Insert Row,Sel,
// 292 AC Insert Column,Sel,
// 293 AC Insert File,Sel,
// 294 AC Insert Picture,Sel,
// 295 AC Insert Object,Sel,
// 296 AC Insert Symbol,Sel,
// 297 AC Save and Close,Sel,
// 298 AC Rename,Sel,
// 299 AC Merge,Sel,
// 29A AC Split,Sel,
// 29B AC Distribute Horizontally,Sel,
// 29C AC Distribute Vertically,Sel,
  "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
};