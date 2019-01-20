////////////////////////////////////////////////////////////////////////////////
//    ScopeFun Oscilloscope ( http://www.scopefun.com )
//    Copyright (C) 2016 David Košenina
//
//    This file is part of ScopeFun Oscilloscope.
//
//    ScopeFun Oscilloscope is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    ScopeFun Oscilloscope is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this ScopeFun Oscilloscope.  If not, see <http://www.gnu.org/licenses/>.
//
////////////////////////////////////////////////////////////////////////////////
#ifndef __CORE__LIBUSB__
#define __CORE__LIBUSB__

#define EEPROM_BYTE_COUNT 16

struct usbDevice;
struct usbDeviceHandle;

typedef struct usbDevice       usbDevice;
typedef struct usbDeviceHandle usbDeviceHandle;

////////////////////////////////////////////////////////////////////////////////
//
// UsbEEPROM
//
////////////////////////////////////////////////////////////////////////////////
class UsbEEPROM {
public:
   byte  bytes[EEPROM_BYTE_COUNT];
public:
   UsbEEPROM()
   {
      memset(this, 0, sizeof(UsbEEPROM));
   }
};

////////////////////////////////////////////////////////////////////////////////
// UsbGuid
////////////////////////////////////////////////////////////////////////////////
class UsbGuid {
public:
   uint   data1;
   ushort data2;
   ushort data3;
   byte   data4[8];
public:
   UsbGuid();
public:
   String asString();
};

#define USB_CALLBACK_DEVICE_ARIVED 0x1
#define USB_CALLBACK_DEVICE_LEFT   0x2

typedef int (*usbCallback)(int flag);

////////////////////////////////////////////////////////////////////////////////
//
// functions
//
////////////////////////////////////////////////////////////////////////////////
int  usbRegisterCallback(usbCallback callback);
int  usbTransferDataIn(int endPoint,byte *dest,uint size,int swapBytes,uint timeOut,int& transfered);
int  usbTransferDataOut(int endPoint,byte *src,uint size,int swapBytes,uint timeOut);
int  usbUploadFirmwareToFx2(byte *buffer,uint size,uint hex);
int  usbUploadFirmwareToFpga(byte *buffer,uint size,uint swapBits);
int  usbReadEEPROM(byte* buffer,uint size);
int  usbWriteEEPROM(byte* buffer, uint size,uint offset);
int  usbIsConnected();
void usbInit();
void usbGuidVidPid(UsbGuid guid, uint vendorId, uint productId,byte serialId );
void usbExit();
int  usbFindList(usbDevice** foundList, uint maxCount);
int  usbFreeList();
int  usbOpenSerial(byte checkSerial, usbDevice** foundList, uint maxCount);
int  usbOpenNormal(usbDevice** foundList,uint maxCount);
void usbOpen(usbDevice* device);
void usbClose();
void usbReset();
void usbClaimInterface(int i=0);
void usbReleaseInterface(int i=0);
void usbUpdate();

#endif
////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
