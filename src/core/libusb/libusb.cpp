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
#include<core/core.h>

// usb
#include<libusb.h>
#include<core/libusb/ezusb.h>

////////////////////////////////////////////////////////////////////////////////
//
// Globals
//
////////////////////////////////////////////////////////////////////////////////
static UsbGuid               gLibUsbGuid;
static ssize_t               gLibUsbDeviceListCnt = 0;
static libusb_device**       gLibUsbDeviceList    = 0;
static libusb_device_handle* gLibUsbDevice        = 0;
static uint                  gLibUsbVendorId      = 0;
static uint                  gLibUsbProductId     = 0;
static byte                  gLibUsbSerialId      = 0;
static usbCallback           gCallback            = 0;

String UsbGuid::asString()
{
   FORMAT_BUFFER();
   FORMAT("0x%08x, 0x%04x, 0x%04x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x", data1, data2, data3, data4[0], data4[1], data4[2], data4[3], data4[4], data4[5], data4[6], data4[7]);
   String temp = formatBuffer;
   return temp;
}

////////////////////////////////////////////////////////////////////////////////
//
// usbRegisterCallback
//
////////////////////////////////////////////////////////////////////////////////
int usbRegisterCallback(usbCallback callback)
{
   gCallback = callback;
   return 0;
}

////////////////////////////////////////////////////////////////////////////////
//
// UsbGuid
//
////////////////////////////////////////////////////////////////////////////////
UsbGuid::UsbGuid()
{
   memset(this,0,sizeof(UsbGuid));
}

////////////////////////////////////////////////////////////////////////////////
//
// usbTransferDataIn
//
////////////////////////////////////////////////////////////////////////////////
int usbTransferDataIn(int endPoint, unsigned char* dest, unsigned int size, int swapBytes, unsigned int timeout, int& transfered)
{
   if( usbIsConnected() )
   {
       transfered = 0;
       #if defined(MAC)
         int iterations = 1;
         int division = size/iterations;
         int modulo   = size%iterations;
         int t = 0;
         for(int i=0;i<iterations;i++)
         {
           libusb_bulk_transfer( gLibUsbDevice, endPoint|LIBUSB_ENDPOINT_IN, dest, division, &t, timeout );
           dest += t;
           transfered += t;
         }
         if( modulo > 0 )
         {
            libusb_bulk_transfer( gLibUsbDevice, endPoint|LIBUSB_ENDPOINT_IN, dest, modulo, &t, timeout );
            transfered += t;
         }
      #else
         libusb_bulk_transfer( gLibUsbDevice, endPoint|LIBUSB_ENDPOINT_IN, dest, size, &transfered, timeout );
      #endif
      //
      // size = transfered;
      if( swapBytes )
      {
         uint count = size/2;
         ushort *swapBuffer = (ushort*)dest;
         for(uint i=0;i<count;i++)
         {
            swapBuffer[i] = endianSwap16( swapBuffer[i] );
         }
      }
      return 1;
   }
   return 1;
}

////////////////////////////////////////////////////////////////////////////////
//
// usbTransferDataOut
//
////////////////////////////////////////////////////////////////////////////////
int usbTransferDataOut(int endPoint,byte *src,uint size,int swapBytes,uint timeOut)
{
   int transfered = 0;
   //
   if( usbIsConnected() )
   {
      if( swapBytes )
      {
         uint count = size/2;
         ushort *swapBuffer = (ushort*)src;
         for(uint i=0;i<count;i++)
         {
            swapBuffer[i] = endianSwap16( swapBuffer[i] );
         }
      }
      //
      libusb_bulk_transfer( gLibUsbDevice, endPoint|LIBUSB_ENDPOINT_OUT, src, size, &transfered, timeOut );
   }
   return 1;
}

////////////////////////////////////////////////////////////////////////////////
//
// usbUploadFirmwareToFx2
//
////////////////////////////////////////////////////////////////////////////////
int usbUploadFirmwareToFx2(byte *buffer,uint size,uint hex)
{
   if( usbIsConnected() )
   {
      // int res = libusb_set_auto_detach_kernel_driver(gLibUsbDevice, 1);
      int res = 0;
      FORMAT_BUFFER();
      FORMAT_PATH("data/firmware/fx2.hex");

      res = ezusb_load_ram(gLibUsbDevice, formatBuffer, FX_TYPE_FX2LP, IMG_TYPE_HEX, 0);
      // libusb_set_auto_detach_kernel_driver(gLibUsbDevice, 0);
      return res;
   }
   return 0;
}

#ifdef PLATFORM_MAC
    extern "C" void logerror(const char* format,...)
#else
    #ifdef CODE_BLOCKS
        extern "C" void logerror(const char* format,...)
    #else
        void logerror(const char* format,...)
    #endif // CODE_BLOCKS
#endif
{
   char str[2049] = {0};
   va_list argptr;
   va_start(argptr, format);
   vsprintf( str, format, argptr );
   va_end(argptr);
   printf(str);
}

////////////////////////////////////////////////////////////////////////////////
//
// usbUploadFirmwareToFpga
//
////////////////////////////////////////////////////////////////////////////////
int usbUploadFirmwareToFpga(byte *buffer,uint size,uint swapBits)
{
   if( usbIsConnected() )
   {
      byte data   = 0;
      uint length = 0;

      // go to reset mode
      libusb_control_transfer( gLibUsbDevice,
                                         LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_ENDPOINT_OUT | LIBUSB_RECIPIENT_DEVICE,
                                         0xB2,
                                         0,
                                         0,
                                         &data,
                                         length,
                                         1000 );

      // upload
      if(swapBits)
      {
         for(uint i=0;i<size;i++)
         {
            byte temp = buffer[i];
            byte bit0 =      temp&0x1;
            byte bit1 = (temp>>1)&0x1;
            byte bit2 = (temp>>2)&0x1;
            byte bit3 = (temp>>3)&0x1;
            byte bit4 = (temp>>4)&0x1;
            byte bit5 = (temp>>5)&0x1;
            byte bit6 = (temp>>6)&0x1;
            byte bit7 = (temp>>7)&0x1;
            buffer[i] = (bit0<<7)|(bit1<<6)|(bit2<<5)|(bit3<<4)|(bit4<<3)|(bit5<<2)|(bit6<<1)|(bit7<<0);
         }
      }
      int transfered = 0;
      libusb_bulk_transfer( gLibUsbDevice, 2|LIBUSB_ENDPOINT_OUT, buffer, size, &transfered, 1000 );

      // out of reset mode
      libusb_control_transfer( gLibUsbDevice,
                                LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_ENDPOINT_OUT | LIBUSB_RECIPIENT_DEVICE,
                                0xB3,
                                0,
                                0,
                                &data,
                                length,
                                10000 );
   }
   return 0;
}

////////////////////////////////////////////////////////////////////////////////
//
// usbReadEEPROM
//
////////////////////////////////////////////////////////////////////////////////
int usbReadEEPROM(byte *buffer, uint size)
{
   int transfered = 0;

   if (usbIsConnected())
   {
      // read eeprom
      return libusb_control_transfer(gLibUsbDevice,
         LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_ENDPOINT_IN | LIBUSB_RECIPIENT_DEVICE,
         0xB4,
         0,
         0,
         buffer,
         size,
         100000);
   }
   return 0;
}

////////////////////////////////////////////////////////////////////////////////
//
// usbWriteEEPROM
//
////////////////////////////////////////////////////////////////////////////////
int usbWriteEEPROM(byte* buffer, uint size,uint offset)
{
   if (usbIsConnected())
   {
      // write eeprom
      return libusb_control_transfer(gLibUsbDevice,
         LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_ENDPOINT_OUT | LIBUSB_RECIPIENT_DEVICE,
         0xB4,
         offset,
         0,
         buffer,
         size,
         10000);
   }
   return 0;
}

////////////////////////////////////////////////////////////////////////////////
//
// usbIsConnected
//
////////////////////////////////////////////////////////////////////////////////
int usbIsConnected()
{
   return gLibUsbDevice ? 1 : 0;
}

////////////////////////////////////////////////////////////////////////////////
//
//  / init / exit / open / close
//
////////////////////////////////////////////////////////////////////////////////
int LIBUSB_CALL hotplug_callback(libusb_context *ctx, libusb_device *device, libusb_hotplug_event event, void *user_data)
{
   //switch (event) {
   //case LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED:
   //   usbOpen();
    //  gCallback(USB_CALLBACK_DEVICE_ARIVED);
    //  break;
  // case LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT:
   //   usbClose();
   //   gCallback(USB_CALLBACK_DEVICE_LEFT);
  //    break;
  // }
   return LIBUSB_SUCCESS;
}

void usbInit()
{
   libusb_init(0);
   // libusb_set_debug(0, LIBUSB_LOG_LEVEL_ERROR);
   // int ret = libusb_hotplug_register_callback(0, (libusb_hotplug_event)(LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED | LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT), LIBUSB_HOTPLUG_ENUMERATE, gLibUsbVendorId, gLibUsbProductId, LIBUSB_HOTPLUG_MATCH_ANY, hotplug_callback, 0, 0);
}

void usbGuidVidPid(UsbGuid guid, uint vendorId, uint productId,byte serialId)
{
   memcpy(&gLibUsbGuid, &guid, sizeof(UsbGuid));
   gLibUsbVendorId  = vendorId;
   gLibUsbProductId = productId;
   gLibUsbSerialId = serialId;
}

void usbExit()
{
   libusb_exit(0);
}

int usbFindList(usbDevice** foundList,uint maxCount)
{
   if (gLibUsbDevice) return false;
   if (gLibUsbDeviceList) return false;

   gLibUsbDeviceListCnt = libusb_get_device_list(NULL, &gLibUsbDeviceList);
   if (gLibUsbDeviceListCnt < 0)
      return false;

   uint index = 0;
   for (ssize_t i = 0; i < gLibUsbDeviceListCnt; i++)
   {
      libusb_device *device = gLibUsbDeviceList[i];
      if (device)
      {
         struct libusb_device_descriptor desc;
         libusb_get_device_descriptor(device, &desc);

         if (desc.idVendor == gLibUsbVendorId && desc.idProduct == gLibUsbProductId)
         {
            foundList[index] = (usbDevice*)device;
            index++;
            index = clamp(index, 0U, maxCount - 1);
         }
      }
   }

   if (index == 0) return false;
   return true;
}

int usbFreeList()
{
   libusb_free_device_list(gLibUsbDeviceList, gLibUsbDeviceListCnt);
   gLibUsbDeviceList    = 0;
   gLibUsbDeviceListCnt = 0;
   return true;
}

int usbOpenSerial(byte checkSerial, usbDevice** foundList,uint maxCount)
{
   for (uint i = 0; i < maxCount; i++)
   {
      libusb_device *device = (libusb_device*)foundList[i];
      if (device)
      {
         if (checkSerial > 0)
         {
            usbOpen((usbDevice*)device);
            usbUploadFirmwareToFx2(0, 0, 0);
            usbClose();

            // 3s, delay
            SDL_Delay(3000);

            usbOpen( (usbDevice*)device );
            UsbEEPROM data;
            usbReadEEPROM(data.bytes, EEPROM_BYTE_COUNT);
            if (data.bytes[5] == gLibUsbSerialId)
            {
               return true;
            }
            else
            {
               usbClose();
            }
         }
         else
         {
            usbOpen((usbDevice*)device);
            usbUploadFirmwareToFx2(0, 0, 0);
            usbClose();

            // 3s, delay
            SDL_Delay(3000);

            usbOpen((usbDevice*)device);
            return true;
         }
      }
   }
   return false;
}


int usbOpenNormal(usbDevice** foundList,uint maxCount)
{
   for (uint i = 0; i < maxCount; i++)
   {
      libusb_device *device = (libusb_device*)foundList[i];
      if (device)
      {
         usbOpen((usbDevice*)device);
         return true;
      }
   }
   return false;
}


void usbOpen(usbDevice* device)
{
   if( gLibUsbDevice ) return;

   int err = libusb_open( (libusb_device*)device, &gLibUsbDevice);
   if (err) return;

   // claim
   usbClaimInterface(0);
   usbClaimInterface(1);
   usbClaimInterface(2);
   usbClaimInterface(4);
   usbClaimInterface(6);

}

void usbClose()
{
   if( !gLibUsbDevice ) return;

   // release
   usbReleaseInterface(0);
   usbReleaseInterface(1);
   usbReleaseInterface(2);
   usbReleaseInterface(4);
   usbReleaseInterface(6);

   // close
   libusb_close( gLibUsbDevice );

   // null
   gLibUsbDevice = 0;
}

void usbClaimInterface(int interface)
{
   if( !gLibUsbDevice ) return;
   libusb_claim_interface(gLibUsbDevice, interface);
   libusb_clear_halt(gLibUsbDevice, interface);
}

void usbReleaseInterface(int interface)
{
   if( !gLibUsbDevice ) return;
   libusb_release_interface(gLibUsbDevice, interface);
}

void usbReset()
{
   if( !gLibUsbDevice ) return;

   libusb_reset_device(gLibUsbDevice);
}

void usbUpdate()
{
   //if (libusb_has_capability(LIBUSB_CAP_HAS_HOTPLUG))
   //{
      // int rc = libusb_handle_events(0);
   //}
}

////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
