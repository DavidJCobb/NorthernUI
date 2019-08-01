#include "shared.h"

const UInt32 g_pluginVersion = 0x01030100; // major, minor, patch, build

extern void SafeMemcpy(UInt32 destination, UInt32 source, UInt32 size) { // based on OBSE SafeWriteXX // TODO: some patch files still have their own copy; have them share this one instead
   UInt32 oldProtect;
   VirtualProtect((void*)destination, size, PAGE_EXECUTE_READWRITE, &oldProtect);
   memcpy((void*)destination, (void*)source, size);
   VirtualProtect((void*)destination, size, oldProtect, &oldProtect);
};
extern void SafeMemset(UInt32 destination, UInt8 byte, UInt32 size) { // based on OBSE SafeWriteXX // TODO: some patch files still have their own copy; have them share this one instead
   UInt32 oldProtect;
   VirtualProtect((void*)destination, size, PAGE_EXECUTE_READWRITE, &oldProtect);
   memset((void*)destination, byte, size);
   VirtualProtect((void*)destination, size, oldProtect, &oldProtect);
};