#include "x86Writer.h"

void x86Writer::_Byte(char v) {
   *(char*)(this->baseAddress + this->offset) = v;
   this->offset++;
}
void x86Writer::_Word(unsigned short v) {
   *(unsigned short*)(this->baseAddress + this->offset) = v;
   this->offset += 2;
}
void x86Writer::_Dword(unsigned int v) {
   *(unsigned int*)(this->baseAddress + this->offset) = v;
   this->offset += 4;
}

// -------------------------------------------------

x86Writer::x86Writer(UInt32 size) {
   this->baseAddress = (UInt32) malloc(size);
   this->offset = 0;
};

void x86Writer::LogDebug() const {
   UInt32 i = 0;
   for (; i + 4 <= this->offset; i += 4) {
      _MESSAGE("%02X %02X %02X %02X", *(char*)(this->baseAddress + i) & 0xFF, *(char*)(this->baseAddress + i + 1) & 0xFF, *(char*)(this->baseAddress + i + 2) & 0xFF, *(char*)(this->baseAddress + i + 3) & 0xFF);
   }
   for (; i + 2 <= this->offset; i += 2) {
      _MESSAGE("%02X %02X", (char) *(char*)(this->baseAddress + i) & 0xFF, *(char*)(this->baseAddress + i + 1) & 0xFF);
   }
   if (i < this->offset)
      _MESSAGE("%02X", *(char*)(this->baseAddress + i) & 0xFF);
};
UInt32 x86Writer::GetAddress() const {
   return this->baseAddress;
};

void x86Writer::ADD(Register r, UInt8 v) {
   this->_Byte(0x83);
   this->_Byte(0xC0 | r);
   this->_Byte(v);
};
void x86Writer::ADD(Register r, UInt32 v) {
   this->_Byte(0x81);
   this->_Byte(0xC0 | r);
   this->_Dword(v);
};
void x86Writer::CALL(Register which) {
   this->_Byte(0xFF);
   this->_Byte(0xC0 | (which << 3) | which); // should be right; seems weird, though
};
void x86Writer::CALL(UInt32 target) {
   UInt32 current = this->baseAddress + this->offset;
   *(char*)(current) = 0xE8;
   *(UInt32*)(current + 1) = target - current - 1 - 4;
   this->offset += 5;
}
void x86Writer::INC(Register which) {
   this->_Byte(0x40 + which);
};
void x86Writer::JMP(UInt32 target) {
   UInt32 current = this->baseAddress + this->offset;
   *(char*) (current) = 0xE9;
   *(UInt32*) (current + 1) = target - current - 1 - 4;
   this->offset += 5;
}
void x86Writer::MOV(Register to, Register from) {
   this->_Byte(0x8B);
   this->_Byte(0xC0 | (to << 3) | from); // Second opcode argument type, and arguments (32-bit register; register, register)
}
void x86Writer::MOV(Register r, UInt8 v) {
   this->_Byte(0xB0 + r);
   this->_Byte(v);
};
void x86Writer::MOV(Register r, UInt32 v) {
   this->_Byte(0xB8 + r);
   this->_Dword(v);
};
void x86Writer::NOP() {
   this->_Byte(0x90);
};
void x86Writer::POP(Register which) {
   this->_Byte(0x58 + which);
};
void x86Writer::PUSH(Register which) {
   this->_Byte(0x50 + which);
}
void x86Writer::PUSH(SInt8 value) {
   this->_Byte(0x6A);
   this->_Byte(value);
}
void x86Writer::PUSH(UInt32 value) {
   this->_Byte (0x68);
   this->_Dword(value);
}
void x86Writer::RETN(UInt32 argCount) {
   if (!argCount) {
      this->_Byte(0xC3);
      return;
   }
   this->_Byte(0xC2);
   this->_Word(argCount * 4);
}
void x86Writer::SETNZ(Register r) {
   this->_Word(0x950F); // 0F 95
   this->_Byte(0xC0 | r);
};
void x86Writer::SETZ(Register r) {
   this->_Word(0x940F); // 0F 94
   this->_Byte(0xC0 | r);
};
void x86Writer::SUB(Register to, Register from) {
   this->_Byte(0x2B);
   this->_Byte(0xC0 | (to << 3) | from);
};
void x86Writer::SUB(Register r, UInt8 v) {
   this->_Byte(0x83);
   this->_Byte(0xC0 | (5 << 3) | r);
   this->_Byte(v);
};
void x86Writer::SUB(Register r, UInt32 v) {
   this->_Byte(0x81);
   this->_Byte(0xC0 | (5 << 3) | r);
   this->_Dword(v);
};
void x86Writer::TEST(Register which) {
   this->_Byte(0x85);
   this->_Byte(0xC0 | (which << 3) | which);
};
void x86Writer::XOR(Register target, Register other) {
   this->_Byte(0x33);
   this->_Byte(0xC0 | (target << 3) | other);
};

void x86Writer::Abs(Register which) {
   if (which != eax)
      this->MOV(eax, which);
   this->_Byte(0x99); // CDQ
   this->XOR(eax, edx);
   this->SUB(eax, edx);
};
void x86Writer::StackFrame() {
   this->PUSH(ebp);
   this->MOV (ebp, esp);
};
void x86Writer::StackFrameExit() {
   this->MOV(esp, ebp);
   this->POP(ebp);
};

template<>
void x86Writer::REP_MOVS<UInt8>() {
   this->_Word(0xA4F3); // F3 A4
};
template<>
void x86Writer::REP_MOVS<UInt32>() {
   this->_Word(0xA5F3); // F3 A5
};
template<>
void x86Writer::REP_STOS<UInt8>() {
   this->_Word(0xAAF3); // F3 AA
};
template<>
void x86Writer::REP_STOS<UInt32>() {
   this->_Word(0xABF3); // F3 AB
};

// -------------------------------------------------
// -------------------------------------------------

void x86WriterAuto::_Guarantee(UInt32 add) {
   UInt32 target = this->offset + add;
   if (target > this->size)
      this->_Realloc(target + 10);
};
void x86WriterAuto::_Realloc(UInt32 size) {
   auto buf = malloc(size);
   memcpy(buf, (void*) this->baseAddress, this->offset);
   free((void*) this->baseAddress);
   this->baseAddress = (UInt32) buf;
};
void x86WriterAuto::_Shrink() {
   auto buf = malloc(this->offset);
   memcpy(buf, (void*) this->baseAddress, this->offset);
   free((void*) this->baseAddress);
   this->baseAddress = (UInt32) buf;
};

void x86WriterAuto::_Byte(char v) {
   this->_Guarantee(1);
   *(char*) (this->baseAddress + this->offset) = v;
   this->offset++;
}
void x86WriterAuto::_Word(unsigned short v) {
   this->_Guarantee(2);
   *(unsigned short*) (this->baseAddress + this->offset) = v;
   this->offset += 2;
}
void x86WriterAuto::_Dword(unsigned int v) {
   this->_Guarantee(4);
   *(unsigned int*) (this->baseAddress + this->offset) = v;
   this->offset += 4;
}