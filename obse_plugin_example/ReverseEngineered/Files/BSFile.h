#pragma once
#include "obse/NiTypes.h"
#include "shared.h"

namespace RE {
   class BSFile;

   DEFINE_SUBROUTINE_EXTERN(void, OpenBSFile, 0x00431130, const char* filePath, UInt32, UInt32, UInt32);

   class BSFile { // sizeof == 0x154 // subclass of NiBinaryStream, sizeof == 0xC
      public:
         BSFile();
         ~BSFile();

         virtual void	Dispose(bool); // 00
         virtual void	Unk_01(void);
         virtual void	Unk_02(void);
         virtual void	Seek(SInt32 offset, UInt32 origin); // 03
         virtual void	Unk_04(void);
         virtual void	DumpAttributes(NiTArray<char*>* dst); // 05
         virtual bool   OpenFile(UInt32, UInt32); // 06 // called if m_file is null during virtual read
         virtual UInt32	GetSize(void); // 07
         virtual void	Unk_08(void);
         virtual void	Unk_09(void);
         virtual void	Unk_0A(char* out, UInt32, UInt16);
         virtual void	Unk_0B(void);
         virtual void	Unk_0C(void);
         virtual void	Unk_0D(void);
         virtual size_t Read(void* destination, UInt32 sizeToRead); // 0E // If the desired size exceeds the BSFile's buffer size, we just read to (destination); otherwise, we read to our own buffer and then memcpy to (destination).
         virtual void	Unk_Write(void);

         typedef size_t (*readProc)(BSFile*, void* destination, UInt32 sizeToRead, UInt32* ptrToOne, UInt32 one); // same return type as virtual 0x0E

         //	void** m_vtbl;		// 000
         readProc m_readProc;	// 004 - function pointer, five args
         void*  m_writeProc;	// 008 - function pointer
         UInt32 m_bufSize;		// 00C
         UInt32 lastReadSize; // 010 // return value of fread, fread_s, and friends, when we're reading into our own buffer
         UInt32 m_unk014; // 014 // number of bytes to read?
         void*  m_buf;		// 018 // buffer. BSFile doesn't load the full file contents at once; it grabs chunks on demand.
         FILE*  m_file;		// 01C
         UInt32 m_writeAccess;	// 020
         UInt8  m_good;			// 024
         UInt8  m_pad025[3];	// 025
         UInt8  m_unk028;		// 028
         UInt8  m_pad029[3];	// 029
         UInt32 m_unk02C;		// 02C
         UInt32 m_pos;			// 030
         UInt32 m_unk034;		// 034
         UInt32 m_unk038;		// 038
         char   m_path[0x104];	// 03C
         UInt32 m_unk140;		// 140
         UInt32 m_unk144;		// 144
         UInt32 m_pos2;			// 148 // used if m_pos is 0xFFFFFFFF // used in some places only if there is no write access // read-specific position?
         UInt32 m_unk14C;		// 14C
         UInt32 m_fileSize;		// 150

         MEMBER_FN_PREFIX(BSFile);
         DEFINE_MEMBER_FN(Constructor, BSFile&, 0x00430970, const char* filePath, UInt32, UInt32, UInt32);
         //
         // Internal:
         //
         DEFINE_MEMBER_FN(Read, size_t, 0x0042C8E0, void* destination, UInt32 sizeToRead);
         DEFINE_MEMBER_FN(Seek, UInt32, 0x00747E20, SInt32 offset, UInt32 origin); // same int args as fseek
   };

   class SaveGameFile : public BSFile {}; // This class definition is a stub. You can help by expanding it.
}