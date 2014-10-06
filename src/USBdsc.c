/*
  Passifier! One Button Keyboard $Rev:  $
  Copyright (C) 2010-2014 Andrew J. Armstrong

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

#include "USBdsc.h"

#define STRING_INDEX_LANGUAGE      0
#define STRING_INDEX_MANUFACTURER  1
#define STRING_INDEX_PRODUCT       2

const unsigned int USB_VENDOR_ID = 0x4150;   // 'PA'
const unsigned int USB_PRODUCT_ID = 0x5353;  // 'SS'
const char USB_SELF_POWER = 0x80;            // 0x80 = Bus powered, 0xC0 = Self powered
const char USB_MAX_POWER = 50;               // Bus power required in units of 2 mA
const char USB_TRANSFER_TYPE = 0x03;         // 0x03 = Interrupt transfers
const char EP_IN_INTERVAL = 5;               // Measured in frame counts i.e. 1 ms units for Low Speed (1.5 Mbps) or Full Speed (12 Mbps), and 125 us units for High Speed (480 Mbps)
                                             // This device supportseither Low Speed (FSEN=0, 6 MHz USB clock) or Full Speed (FSEN=1, 48 MHz USB clock) mode.
                                             // PIC 18F25K50 does not support USB High Speed mode
                                             // n x 1 millisecond units (for USB Low/Full Speed devices)
                                             // 2**(n-1) x 125 microsecond units (for USB2 High Speed devices)
                                             // The Host interrupts PIC for keyboard input this often.

const char EP_OUT_INTERVAL = 5;              // n x 1 millisecond units (for USB Low/Full Speed devices)
                                             // 2**(n-1) x 125 microsecond units (for USB2 High Speed devices)
                                             // The Host interrupts PIC for LED status output at most this often (if LED status change is pending).

const char USB_INTERRUPT = 1;
const char USB_HID_EP = 1;
const char USB_HID_RPT_SIZE = 50   // Keyboard       --> host
                            + 21   // Keyboard       <-- host
                            + 25   // SystemControl  --> host
                            + 25;  // ConsumerDevice --> host
/* Device Descriptor */
const struct
{
    char bLength;               // bLength         - Descriptor size in bytes (12h)
    char bDescriptorType;       // bDescriptorType - The constant DEVICE (01h)
    unsigned int bcdUSB;        // bcdUSB          - USB specification release number (BCD)
    char bDeviceClass;          // bDeviceClass    - Class Code
    char bDeviceSubClass;       // bDeviceSubClass - Subclass code
    char bDeviceProtocol;       // bDeviceProtocol - Protocol code
    char bMaxPacketSize0;       // bMaxPacketSize0 - Maximum packet size for endpoint 0
    unsigned int idVendor;      // idVendor        - Vendor ID
    unsigned int idProduct;     // idProduct       - Product ID
    unsigned int bcdDevice;     // bcdDevice       - Device release number (BCD)
    char iManufacturer;         // iManufacturer   - Index of string descriptor for the manufacturer
    char iProduct;              // iProduct        - Index of string descriptor for the product.
    char iSerialNumber;         // iSerialNumber   - Index of string descriptor for the serial number.
    char bNumConfigurations;    // bNumConfigurations - Number of possible configurations
} device_dsc = 
  {
      0x12,                   // bLength
      0x01,                   // bDescriptorType
      0x0110,                 // bcdUSB   (USB 1.1)
      0x00,                   // bDeviceClass
      0x00,                   // bDeviceSubClass
      0x00,                   // bDeviceProtocol
      8,                      // bMaxPacketSize0
      USB_VENDOR_ID,          // idVendor
      USB_PRODUCT_ID,         // idProduct
      0x0003,                 // bcdDevice
      STRING_INDEX_MANUFACTURER, // iManufacturer
      STRING_INDEX_PRODUCT,  // iProduct
      0x00,                   // iSerialNumber
      0x01                    // bNumConfigurations
  };

/* Configuration 1 Descriptor */
const char configDescriptor1[] = 
{
    // Configuration Descriptor
    0x09,                   // bLength             - Descriptor size in bytes
    0x02,                   // bDescriptorType     - The constant CONFIGURATION (02h)
    41,0x00,                // wTotalLength        - The number of bytes in the configuration descriptor and all of its subordinate descriptors
    1,                      // bNumInterfaces      - Number of interfaces in the configuration
    1,                      // bConfigurationValue - Identifier for Set Configuration and Get Configuration requests
    STRING_INDEX_MANUFACTURER,   // iConfiguration      - Index of string descriptor for the configuration
    USB_SELF_POWER,         // bmAttributes        - Self/bus power and remote wakeup settings
    USB_MAX_POWER,          // bMaxPower           - Bus power required in units of 2 mA

    // Interface Descriptor
    0x09,                   // bLength - Descriptor size in bytes (09h)
    0x04,                   // bDescriptorType - The constant Interface (04h)
    0,                      // bInterfaceNumber - Number identifying this interface
    0,                      // bAlternateSetting - A number that identifies a descriptor with alternate settings for this bInterfaceNumber.
    2,                      // bNumEndpoint - Number of endpoints supported not counting endpoint zero
    0x03,                   // bInterfaceClass - Class code  (0x03 = HID)
    0,                      // bInterfaceSubclass - Subclass code (0x00 = No Subclass)
    0,                      // bInterfaceProtocol - Protocol code (0x00 = No protocol)
                            // Valid combinations of Class, Subclass, Protocol are as follows:
                            // Class Subclass Protocol Meaning
                            //   3       0       0     Class=HID with no specific Subclass or Protocol: 
                            //                         Can have ANY size reports (not just 8-byte reports)
                            //                         IRK! uses 3-byte reports because it only ever sends one key press at a time
                            //   3       1       1     Class=HID, Subclass=BOOT device, Protocol=keyboard: 
                            //                         REQUIRES 8-byte reports in order for it to be recognised by BIOS when booting.
                            //                         That is because the entire USB protocol cannot be implemented in BIOS, so
                            //                         motherboard manufacturers have agreed to use a fixed 8-byte report during booting.
                            //   3       1       2     Class=HID, Subclass=BOOT device, Protocol=mouse
                            // The above information is documented in Appendix E.3 "Interface Descriptor (Keyboard)"
                            // of the "Device Class Definition for Human Interface Devices (HID) v1.11" document (HID1_11.pdf) from www.usb.org
    STRING_INDEX_PRODUCT,  // iInterface - Interface string index

    // HID Class-Specific Descriptor
    0x09,                   // bLength - Descriptor size in bytes.
    0x21,                   // bDescriptorType - This descriptor's type: 21h to indicate the HID class.
    0x01,0x01,              // bcdHID - HID specification release number (BCD).
    0x00,                   // bCountryCode - Numeric expression identifying the country for localized hardware (BCD) or 00h.
    1,                      // bNumDescriptors - Number of subordinate report and physical descriptors.
    0x22,                   // bDescriptorType - The type of a class-specific descriptor that follows
    USB_HID_RPT_SIZE,0x00,  // wDescriptorLength - Total length of the descriptor identified above.

    // Endpoint Descriptor - Inbound to host (i.e. key press codes)
    0x07,                   // bLength - Descriptor size in bytes (07h)
    0x05,                   // bDescriptorType - The constant Endpoint (05h)
    USB_HID_EP | 0x80,      // bEndpointAddress - Endpoint number (0x01) and direction (0x80 = IN to host)
    USB_TRANSFER_TYPE,      // bmAttributes - Transfer type and supplementary information
    0x08,0x00,              // wMaxPacketSize - Maximum packet size supported
                            // This determines the size of the transmission time slot allocated to this device
    EP_IN_INTERVAL,         // bInterval - Service interval or NAK rate

    // Endpoint Descriptor - Outbound from host (i.e. LED indicator status bits)
    0x07,                   // bLength - Descriptor size in bytes (07h)
    0x05,                   // bDescriptorType - The constant Endpoint (05h)
    USB_HID_EP,             // bEndpointAddress - Endpoint number (0x01) and direction (0x00 = OUT from host)
    USB_TRANSFER_TYPE,      // bmAttributes - Transfer type and supplementary information
    0x08,0x00,              // wMaxPacketSize - Maximum packet size supported
                            // This determines the size of the transmission time slot allocated to this device
    EP_OUT_INTERVAL         // bInterval - Service interval or NAK rate
};

const struct
{
  char report[USB_HID_RPT_SIZE];
} hid_rpt_desc =
  {
    {
// Some rules:
// 1. If you have multiple reports to send on a single interface then ALL reports
//    on that interface MUST have a unique report id.
// 2. If you want to send one report with NO report id and another report WITH a
//    report id, then you must have separate interfaces. For example:
//    Interface 1: report with NO report id
//    Interface 2: report(s) WITH report id
//    ...otherwise, the host driver will not know whether to expect a report id
//    or not.

/*
Keyboard Input Report (PIC --> Host) 4 bytes as follows:
    .---------------------------------------.
    |          REPORT_ID_KEYBOARD           | IN: Report Id
    |---------------------------------------|
    |RGUI|RALT|RSHF|RCTL|LGUI|LALT|LSHF|LCTL| IN: Ctrl/Shift/Alt/GUI keys on left and right hand side of keyboard
    |---------------------------------------|
    |                (pad)                  | IN: pad (strangely, this pad byte is necessary)
    |---------------------------------------|
    |                 Key                   | IN: Key that is currently pressed
    '---------------------------------------'
*/
  0x05, 0x01,                  // (GLOBAL) USAGE_PAGE         0x0001 Generic Desktop Page
  0x09, 0x06,                  // (LOCAL)  USAGE              0x00010006 Keyboard (CA=Application Collection)
  0xA1, 0x01,                  // (MAIN)   COLLECTION         0x01 Application (Usage=0x00010006: Page=Generic Desktop Page, Usage=Keyboard, Type=CA)
  0x85, REPORT_ID_KEYBOARD,    //   (GLOBAL) REPORT_ID          0x4B (75) 'K'
  0x05, 0x07,                  //   (GLOBAL) USAGE_PAGE         0x0007 Keyboard/Keypad Page
  0x09, 0xE1,                  //   (LOCAL) USAGE               0x000700E1 Keyboard Left Shift
  0x09, 0xE0,                  //   (LOCAL) USAGE               0x000700E0 Keyboard Left Control
  0x09, 0xE2,                  //   (LOCAL) USAGE               0x000700E0 Keyboard Left Alt
  0x09, 0xE3,                  //   (LOCAL) USAGE               0x000700E0 Keyboard Left GUI
  0x09, 0xE5,                  //   (LOCAL) USAGE               0x000700E1 Keyboard Right Shift
  0x09, 0xE4,                  //   (LOCAL) USAGE               0x000700E0 Keyboard Right Control
  0x09, 0xE6,                  //   (LOCAL) USAGE               0x000700E0 Keyboard Right Alt
  0x09, 0xE7,                  //   (LOCAL) USAGE               0x000700E0 Keyboard Right GUI
  0x25, 0x01,                  //   (GLOBAL) LOGICAL_MAXIMUM    0x01 (1)
  0x75, 0x01,                  //   (GLOBAL) REPORT_SIZE        0x01 (1) Number of bits per field
  0x95, 0x08,                  //   (GLOBAL) REPORT_COUNT       0x08 (8) Number of fields
  0x81, 0x02,                  //   (MAIN)   INPUT              0x00000002 (8 fields x 1 bit) 0=Data 1=Variable 0=Absolute 0=NoWrap 0=Linear 0=PrefState 0=NoNull 0=NonVolatile 0=Bitmap
  
  0x75, 0x08,                  //   (GLOBAL) REPORT_SIZE        0x08 (8) Number of bits per field
  0x95, 0x01,                  //   (GLOBAL) REPORT_COUNT       0x01 (1) Number of fields
  0x81, 0x03,                  //   (MAIN)   INPUT              0x00000003 (1 field x 8 bits) 1=Constant 1=Variable 0=Absolute 0=NoWrap 0=Linear 0=PrefState 0=NoNull 0=NonVolatile 0=Bitmap
  
  0x26, 0xFF, 0x00,            //   (GLOBAL) LOGICAL_MAXIMUM    0x00FF (255)
  0x19, 0x00,                  //   (LOCAL)  USAGE_MINIMUM      0x00070000 Keyboard No event indicated (Sel=Selector) <-- Redundant: USAGE_MINIMUM is already 0x0000
  0x2A, 0xFF, 0x00,            //   (LOCAL)  USAGE_MAXIMUM      0x000700FF
  0x81, 0x00,                  //   (MAIN)   INPUT              0x00000000 (1 field x 8 bits) 0=Data 0=Array 0=Absolute 0=Ignored 0=Ignored 0=PrefState 0=NoNull
/*
Output Report (PIC <-- Host) 2 bytes as follows:

    .---------------------------------------.
    |          REPORT_ID_KEYBOARD           | OUT: Report Id
    |---------------------------------------|
    |    |    |    |    |    |SCRL|CAPL|NUML| OUT: NumLock,CapsLock,ScrollLock - and 5 unused pad bits
    '---------------------------------------'
*/
  0x75, 0x01,                  //   (GLOBAL) REPORT_SIZE        0x01 (1) Number of bits per field
  0x95, 0x03,                  //   (GLOBAL) REPORT_COUNT       0x03 (3) Number of fields
  0x05, 0x08,                  //   (GLOBAL) USAGE_PAGE         0x0008 LED Indicator Page
  0x19, 0x01,                  //   (LOCAL)  USAGE_MINIMUM      0x00080001 Num Lock (OOC=On/Off Control)
  0x29, 0x03,                  //   (LOCAL)  USAGE_MAXIMUM      0x00080003 Scroll Lock (OOC=On/Off Control)
  0x25, 0x01,                  //   (GLOBAL) LOGICAL_MAXIMUM    0x01 (1)
  0x91, 0x02,                  //   (MAIN)   OUTPUT             0x00000002 (3 fields x 1 bit) 0=Data 1=Variable 0=Absolute 0=NoWrap 0=Linear 0=PrefState 0=NoNull 0=NonVolatile 0=Bitmap
  0x75, 0x05,                  //   (GLOBAL) REPORT_SIZE        0x05 (5) Number of bits per field
  0x95, 0x01,                  //   (GLOBAL) REPORT_COUNT       0x01 (1) Number of fields
  0x91, 0x03,                  //   (MAIN)   OUTPUT             0x00000003 (1 field x 5 bits) 1=Constant 1=Variable 0=Absolute 0=NoWrap 0=Linear 0=PrefState 0=NoNull 0=NonVolatile 0=Bitmap
  0xC0,                        // (MAIN)   END_COLLECTION     Application

/*
System Control Input Report (PIC --> Host) 2 bytes as follows:
    .---------------------------------------.
    |        REPORT_ID_SYSTEM_CONTROL       | IN: Report Id
    |---------------------------------------|
    |           Power Control Code          | IN: Power Off, Sleep, Power On
    '---------------------------------------'
*/
  0x05, 0x01,                  // (GLOBAL) USAGE_PAGE         0x0001 Generic Desktop Page
  0x09, 0x80,                  // (LOCAL)  USAGE              0x00010080 System Control (CA=Application Collection)
  0xA1, 0x01,                  // (MAIN)   COLLECTION         0x01 Application (Usage=0x00010080: Page=Generic Desktop Page, Usage=System Control, Type=CA)
  0x85, REPORT_ID_SYSTEM_CONTROL, //   (GLOBAL) REPORT_ID       0x53 (83) 'S'
  0x1A, 0x81, 0x00,            //   (LOCAL)  USAGE_MINIMUM      0x00010081
  0x2A, 0x8F, 0x00,            //   (LOCAL)  USAGE_MAXIMUM      0x0001008F
  0x15, 0x01,                  //   (GLOBAL) LOGICAL_MINIMUM    0x00 (1)
  0x25, 0x0F,                  //   (GLOBAL) LOGICAL_MAXIMUM    0x0F (15)
  0x75, 0x08,                  //   (GLOBAL) REPORT_SIZE        0x08 (8) Number of bits per field
  0x95, 0x01,                  //   (GLOBAL) REPORT_COUNT       0x01 (1) Number of fields <-- Redundant: REPORT_COUNT is already 1
  0x81, 0x00,                  //   (MAIN)   INPUT              0x00000000 (1 field x 8 bits) 0=Data 0=Array 0=Absolute 0=Ignored 0=Ignored 0=PrefState 0=NoNull
  0xC0,                        // (MAIN)   END_COLLECTION     Application

/*
Consumer Device Input Report (PIC --> Host) 3 bytes as follows:
    .---------------------------------------.
    |       REPORT_ID_CONSUMER_DEVICE       | IN: Report Id
    |---------------------------------------|
    |    Consumer Device Code (Low byte)    | IN: Mute, Vol+, Vol- etc
    |---------------------------------------|
    |    Consumer Device Code (High byte)   | IN:
    '---------------------------------------'
*/
  0x05, 0x0C,                  // (GLOBAL) USAGE_PAGE         0x000C Consumer Device Page
  0x09, 0x01,                  // (LOCAL)  USAGE              0x000C0001 Consumer Control (CA=Application Collection)
  0xA1, 0x01,                  // (MAIN)   COLLECTION         0x01 Application (Usage=0x000C0001: Page=Consumer Device Page, Usage=Consumer Control, Type=CA)
  0x85, REPORT_ID_CONSUMER_DEVICE, // (GLOBAL) REPORT_ID          0x43 (67) 'C'
  0x19, 0x00,                  //   (LOCAL)  USAGE_MINIMUM      0x000C0000 Unassigned <-- Redundant: USAGE_MINIMUM is already 0x0000
  0x2A, 0x3C, 0x02,            //   (LOCAL)  USAGE_MAXIMUM      0x000C023C AC Format (Sel=Selector)
  0x15, 0x00,                  //   (GLOBAL) LOGICAL_MINIMUM    0x00 (0) <-- Redundant: LOGICAL_MINIMUM is already 0
  0x26, 0x3C, 0x02,            //   (GLOBAL) LOGICAL_MAXIMUM    0x023C (572)
  0x75, 0x10,                  //   (GLOBAL) REPORT_SIZE        0x10 (16) Number of bits per field
  0x95, 0x01,                  //   (GLOBAL) REPORT_COUNT       0x01 (1) Number of fields <-- Redundant: REPORT_COUNT is already 1
  0x81, 0x00,                  //   (MAIN)   INPUT              0x00000000 (1 field x 16 bits) 0=Data 0=Array 0=Absolute 0=Ignored 0=Ignored 0=PrefState 0=NoNull
  0xC0,                        // (MAIN)   END_COLLECTION     Application
    }
  };


//Language code string descriptor
const struct 
{
  char bLength;
  char bDscType;
  unsigned int string[1];
} sLanguage =
  {
      4,
      0x03,
      {0x0409}
  };


//Manufacturer string descriptor
const struct
{
  char bLength;
  char bDscType;
  unsigned int string[9]; // Max number of (2-byte) "characters"
} sManufacturer =
  {
    20,           //sizeof this descriptor string
    0x03,
    {'P','a','s', 's', 'i', 'f', 'i', 'e', 'r'}
  };

//Product string descriptor
const struct tagProduct
{
  char bLength;
  char bDscType;
  unsigned int string[19]; // Max number of (2-byte) "characters"
} sProduct =
  {
    40,           //sizeof this descriptor string (including length and type fields)
    0x03,
    {'O','n','e',' ','B','u','t','t','o','n',' ','K','e','y','b','o','a','r','d'}
  };
  

//Array of configuration descriptors
const char* USB_config_dsc_ptr[1];

//Array of string descriptors
const char* USB_string_dsc_ptr[3];

void USB_Init_Desc()
{
  USB_config_dsc_ptr[0] = &configDescriptor1;
  USB_string_dsc_ptr[STRING_INDEX_LANGUAGE]     = (const char*)&sLanguage;
  USB_string_dsc_ptr[STRING_INDEX_MANUFACTURER] = (const char*)&sManufacturer;
  USB_string_dsc_ptr[STRING_INDEX_PRODUCT]      = (const char*)&sProduct;
}