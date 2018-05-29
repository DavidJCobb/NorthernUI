#pragma once
#include "obse/NiTypes.h"

namespace RE {
   class BSFile { // sizeof == 0x154
      public:
         BSFile();
         ~BSFile();

         virtual void	Dispose(bool); // 00
         virtual void	Unk_01(void);
         virtual void	Unk_02(void);
         virtual void	Unk_03(void);
         virtual void	Unk_04(void);
         virtual void	DumpAttributes(NiTArray<char*>* dst); // 05
         virtual void   Unk_06(void); // assumed insertion
         virtual UInt32	GetSize(void); // 07
         virtual void	Unk_08(void);
         virtual void	Unk_09(void);
         virtual void	Unk_0A(void);
         virtual void	Unk_0B(void);
         virtual void	Unk_0C(void);
         virtual void	Unk_0D(void);
         virtual void	Unk_Read(void);
         virtual void	Unk_Write(void);

         //	void** m_vtbl;		// 000
         void*  m_readProc;	// 004 - function pointer, six args
         void*  m_writeProc;	// 008 - function pointer
         UInt32 m_bufSize;		// 00C
         UInt32 m_unk010;		// 010 - init'd to m_bufSize
         UInt32 m_unk014;		// 014
         void*  m_buf;		// 018
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
         UInt32 m_pos2;			// 148 - used if m_pos is 0xFFFFFFFF
         UInt32 m_unk14C;		// 14C
         UInt32 m_fileSize;		// 150
   };

   class SaveGameFile : public BSFile {}; // This class definition is a stub. You can help by expanding it.
}