#include "x86Reader.h"

void x86Reader::_Byte(unsigned char v) {
   auto c = *(char*)(this->address);
   if (c != v)
      this->_Throw();
   this->address = (decltype(this->address)) ((UInt32)this->address + 1);
}
void x86Reader::_ByteAnyOf(unsigned int v) {
   auto c = *(char*)(this->address);
   if (
      c != (v & 0xFF)
   && c != ((v >> 0x08) & 0xFF)
   && c != ((v >> 0x10) & 0xFF)
   && c != ((v >> 0x18))
   ) {
      this->_Throw();
   }
   this->address = (decltype(this->address))((UInt32)this->address + 1);
};
void x86Reader::_Word(unsigned short v) {
   auto c = *(unsigned short*)(this->address);
   if (c != v)
      this->_Throw();
   this->address = (decltype(this->address))((UInt32)this->address + 2);
}
void x86Reader::_Dword(unsigned int v) {
   auto c = *(unsigned int*)(this->address);
   if (c != v)
      this->_Throw();
   this->address = (decltype(this->address))((UInt32)this->address + 4);
}
void x86Reader::_Throw() {
   _MESSAGE("x86Reader instance encountered a mismatch at %08X", this->address);
   throw std::runtime_error("Failed to find the expected x86 opcode.");
};

// -------------------------------------------------

void x86Reader::ADD(Register r, UInt8 v) {
   this->_Byte(0x83);
   this->_Byte(0xC0 | r);
   this->_Byte(v);
};
void x86Reader::ADD(Register r, UInt32 v) {
   this->_Byte(0x81);
   this->_Byte(0xC0 | r);
   this->_Dword(v);
};
void x86Reader::CALL(Register which) {
   this->_Byte(0xFF);
   this->_Byte(0xC0 | (which << 3) | which); // should be right; seems weird, though
};
void x86Reader::CALL(UInt32 target) {
   UInt32 current = (UInt32)this->address;
   this->_Byte (0xE8);
   this->_Dword(target - current - 1 - 4);
}
void x86Reader::CMP(Register r, UInt32 value) {
   this->_Byte(0x81); // shared by several opcodes; parts of next byte distinguish
   this->_Byte(0xF8 | r); // MOD RM byte -- bits x111yyyy is used to specify which opcode we're using, and topmost bit (x) is always 1
   this->_Dword(value);
};
void x86Reader::INC(Register which) {
   this->_Byte(0x40 + which);
};
void x86Reader::JMP(UInt32 target) {
   UInt32 current = (UInt32)this->address;
   this->_Byte (0xE9);
   this->_Dword(target - current - 1 - 4);
}
void x86Reader::MOV(Register to, Register from) {
   this->_Byte(0x8B);
   this->_Byte(0xC0 | (to << 3) | from); // Second opcode argument type, and arguments (32-bit register; register, register)
}
void x86Reader::LEA(Register to, Register base, const UInt8 offset) {
   this->LEA(to, base, x86Reader::esp, 1, offset);
};
void x86Reader::LEA(Register to, Register base, Register index, UInt8 scale, const UInt8 offset) {
   this->_Byte(0x8D);
   this->_Byte(0x40 | (to << 3) | esp); // MOD RM // 0x40 = use UInt8 offset; x86Reader::esp as low-bits in MOD RM signals that the next byte is SIB
   UInt8 scaleBits = 0;
   {
      switch (scale) {
         case 1:
            break;
         case 2:
            scaleBits = 1;
            break;
         case 4:
            scaleBits = 2;
            break;
         case 8:
            scaleBits = 3;
            break;
         default:
            _MESSAGE("WARNING: Invalid OFFSET specified in an x86 check!");
      }
   }
   this->_Byte((scaleBits << 6) | (index << 3) | base); // SIB // x86Reader::esp as index signals no scaled index in use
   this->_Byte(offset);
};
void x86Reader::MOV(Register r, UInt8 v) {
   this->_Byte(0xB0 + r);
   this->_Byte(v);
};
void x86Reader::MOV(Register r, UInt32 v) {
   this->_Byte(0xB8 + r);
   this->_Dword(v);
};
void x86Reader::NOP() {
   this->_Byte(0x90);
};
void x86Reader::POP(Register which) {
   this->_Byte(0x58 + which);
};
void x86Reader::POPAD() {
   this->_Byte(0x61);
};
void x86Reader::PUSH(Register which) {
   this->_Byte(0x50 + which);
}
void x86Reader::PUSH(SInt8 value) {
   this->_Byte(0x6A);
   this->_Byte(value);
}
void x86Reader::PUSH(UInt32 value) {
   this->_Byte(0x68);
   this->_Dword(value);
}
void x86Reader::PUSHAD() {
   this->_Byte(0x60);
};
void x86Reader::RETN(UInt32 argCount) {
   if (!argCount) {
      this->_Byte(0xC3);
      return;
   }
   this->_Byte(0xC2);
   this->_Word(argCount * 4);
}
void x86Reader::SETNZ(Register r) {
   this->_Word(0x950F); // 0F 95
   this->_Byte(0xC0 | r);
};
void x86Reader::SETZ(Register r) {
   this->_Word(0x940F); // 0F 94
   this->_Byte(0xC0 | r);
};
void x86Reader::SUB(Register to, Register from) {
   this->_Byte(0x2B);
   this->_Byte(0xC0 | (to << 3) | from);
};
void x86Reader::SUB(Register r, UInt8 v) {
   this->_Byte(0x83);
   this->_Byte(0xC0 | (5 << 3) | r);
   this->_Byte(v);
};
void x86Reader::SUB(Register r, UInt32 v) {
   this->_Byte(0x81);
   this->_Byte(0xC0 | (5 << 3) | r);
   this->_Dword(v);
};
void x86Reader::TEST(Register which) {
   this->_Byte(0x85);
   this->_Byte(0xC0 | (which << 3) | which);
};
void x86Reader::XOR(Register target, Register other) {
   this->_Byte(0x33);
   this->_Byte(0xC0 | (target << 3) | other);
};

// -------------------------------------------------

UInt32 x86Reader::CallToAnyAddress() { // returns called address
   this->_Byte(0xE8);
   UInt32 a = *(UInt32*)this->address;
   a += (UInt32)this->address + 4;
   this->address = (decltype(this->address))((UInt32)this->address + 4);
   return a;
};
void x86Reader::FloatPopDiscard(UInt8 index) {
   if (index > 7) {
      throw std::invalid_argument("x86Reader::FloatPop(UInt8 index) only accepts indices below 8 (i.e. ST0 - ST7).");
   }
   this->_Byte(0xDD);
   this->_Byte(0xD8 | index);
};
void x86Reader::JumpThroughAnyStatic() {
   this->_Byte(0xFF);
   this->_Byte(0x25); // 0x20 indicates opcode extension; 0x5 indicates the use of a 32-bit immediate
   this->address = (decltype(this->address))((UInt32)this->address + 4);
};
void x86Reader::JumpThroughStatic(UInt32 ptr) {
   this->_Byte(0xFF);
   this->_Byte(0x25); // 0x20 indicates opcode extension; 0x5 indicates the use of a 32-bit immediate
   this->_Dword(ptr);
};