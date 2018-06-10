#pragma once

//
// Class to read x86 opcodes from a target location, and throw when 
// encountering one that doesn't match.
//

class x86Reader {
   protected:
      void* address;

   public:
      void _Byte(char);
      void _ByteAnyOf(unsigned int);
      void _Word(unsigned short);
      void _Dword(unsigned int);
   protected:
      void _Throw();

      template <typename... T>
      struct dependent_false { static constexpr bool value = false; };

   public:
      x86Reader(void*  offset) : address(offset) {};
      x86Reader(UInt32 offset) : address((void*)offset) {}; // UInt64 for x64, but why would you?

      enum Register : UInt8 {
         eax = 0,
         ecx = 1,
         edx = 2,
         ebx = 3,
         esp = 4,
         ebp = 5,
         esi = 6,
         edi = 7,
      };

      void ADD(Register, UInt8);
      void ADD(Register, UInt32);
      void CALL(Register);
      void CALL(UInt32 target); // near call
      void CMP(Register r, UInt32 value); // CMP r1, immediate
      void INC(Register);
      void JMP(UInt32 target); // near jump
      void LEA(Register to, Register base, const UInt8 offset); // LEA to, [base + offset]
      void LEA(Register to, Register base, Register index, UInt8 scale, const UInt8 offset); // LEA to, [base + index * scale + offset]
      void MOV(Register to, Register from);
      void MOV(Register, UInt8); // WARNING: Doesn't clear upper bytes! // TODO: convert to template specialization to prevent accidental use
      void MOV(Register, UInt32);
      void NOP();
      void POP(Register);
      void POPAD();
      void PUSH(Register); // push register onto the stack
      void PUSH(SInt8);    // push raw value onto the stack // note: the pushed value is sign-extended to a 32-bit argument by the CPU
      void PUSH(UInt32);   // push raw value onto the stack
      void PUSHAD();
      void RETN(UInt32 argCount = 0); // near return
      void SETNZ(Register);
      void SETZ(Register);
      void SUB(Register target, Register subject);
      void SUB(Register, UInt8);
      void SUB(Register, UInt32);
      void TEST(Register); // TEST r, r
      void XOR(Register, Register); // TODO: Template this; specialize to UInt32 (0x33) and UInt8 (0x32)

      UInt32 CallToAnyAddress(); // returns called address
      void   FloatPopDiscard(UInt8 index); // FSTP ST(x), discarding the value without saving it anywhere
      void   JumpThroughAnyStatic();        // JMP DWORD PTR [any const address]
      void   JumpThroughStatic(UInt32 ptr); // JMP DWORD PTR [ptr]

      // -----------------------------------------------------------------------------------
      // CMP DWORD PTR [r1 + offset], r2
      template<typename T> void CMP(Register r1, Register r2, T offset) {
         static_assert(dependent_false<T>::value, "You must use a predefined template specialization when calling (CMP DWORD PTR [Reg + Offset], Reg)!");
      };
      template<> void CMP(Register r1, Register r2, UInt8 offset) {
         this->_Byte(0x39);
         this->_Byte(0x40 | (r2 << 3) | r1); // modRM
         {  // SIB allows four different ways of specifying each register when not using a "scaled index" (think LEA)
            UInt32 possibilities = 0;
            possibilities |= r1 | 0x20;
            possibilities |= (r1 | 0x60) << 0x08;
            possibilities |= (r1 | 0xA0) << 0x10;
            possibilities |= (r1 | 0xE0) << 0x18;
            this->_ByteAnyOf(possibilities);
         }
         this->_Byte(offset);
      };
      template<> void CMP(Register r1, Register r2, UInt32 offset) {
         this->_Byte(0x39);
         this->_Byte(0x80 | (r2 << 3) | r1); // modRM
         {  // SIB allows four different ways of specifying each register when not using a "scaled index" (think LEA)
            UInt32 possibilities = 0;
            possibilities |= r1 | 0x20;
            possibilities |= (r1 | 0x60) << 0x08;
            possibilities |= (r1 | 0xA0) << 0x10;
            possibilities |= (r1 | 0xE0) << 0x18;
            this->_ByteAnyOf(possibilities);
         }
         this->_Byte(offset);
      };
      // -----------------------------------------------------------------------------------
      // MOV to, DWORD PTR [from + offset]
      template<typename T>
      void MOV(Register to, Register from, T offset) {
         static_assert(dependent_false<T>::value, "You must use a predefined template specialization when calling (MOV Reg, DWORD PTR [Reg + Offset])!");
      };
      template<> void MOV(Register to, Register from, UInt8 offset) {
         this->_Byte(0x8B);
         this->_Byte(0x40 | (to << 3) | from); // modRM
         if (from == esp) { // must use an additional SIB byte to specify ESP
            //
            // SIB allows four different ways of specifying each register when not using a "scaled index" (think LEA)
            //
            UInt32 possibilities = 0;
            possibilities |= from | 0x20;
            possibilities |= (from | 0x60) << 0x08;
            possibilities |= (from | 0xA0) << 0x10;
            possibilities |= (from | 0xE0) << 0x18;
            this->_ByteAnyOf(possibilities);
         }
         this->_Byte(offset);
      };
      template<> void MOV(Register to, Register from, UInt32 offset) {
         this->_Byte(0x8B);
         this->_Byte(0x80 | (to << 3) | from); // modRM
         if (from == esp) { // must use an additional SIB byte to specify ESP
            //
            // SIB allows four different ways of specifying each register when not using a "scaled index" (think LEA)
            //
            UInt32 possibilities = 0;
            possibilities |= from | 0x20;
            possibilities |= (from | 0x60) << 0x08;
            possibilities |= (from | 0xA0) << 0x10;
            possibilities |= (from | 0xE0) << 0x18;
            this->_ByteAnyOf(possibilities);
         }
         this->_Byte(offset);
      };
      // -----------------------------------------------------------------------------------
      template<typename T> void JE(T offset) {
         static_assert(dependent_false<T>::value, "You must use a predefined template specialization when calling JE!");
      };
      template<> void JE(SInt8 offset) {
         this->_Byte(0x74);
         this->_Byte(offset);
      };
      template<> void JE(SInt32 offset) {
         this->_Byte(0x84);
         this->_Byte(offset);
      };
      // -----------------------------------------------------------------------------------
      template<typename T> void JG(T offset) {
         static_assert(dependent_false<T>::value, "You must use a predefined template specialization when calling JG!");
      };
      template<> void JG(SInt8 offset) {
         this->_Byte(0x7F);
         this->_Byte(offset);
      };
      template<> void JG(SInt32 offset) {
         this->_Byte(0x8F);
         this->_Byte(offset);
      };
      // -----------------------------------------------------------------------------------
      template<typename T> void JNE(T offset) {
         static_assert(dependent_false<T>::value, "You must use a predefined template specialization when calling JNE!");
      };
      template<> void JNE(SInt8 offset) {
         this->_Byte(0x75);
         this->_Byte(offset);
      };
      template<> void JNE(SInt32 offset) {
         this->_Byte(0x85);
         this->_Byte(offset);
      };
      // -----------------------------------------------------------------------------------
      // REP MOVS: memcpy(UInt8* edi, UInt8* esi, ecx * sizeof(T))
      template<typename T> void REP_MOVS() {
         static_assert(dependent_false<T>::value, "You must use a predefined template specialization when calling REP MOVS!");
      };
      template<> void REP_MOVS<UInt8>() {
         this->_Word(0xA4F3); // F3 A4
      };
      template<> void REP_MOVS<UInt32>() {
         this->_Word(0xA5F3); // F3 A5
      };
      // -----------------------------------------------------------------------------------
      // REP STOS: memset(UInt8* edi, (T)eax, ecx * sizeof(T))
      template<typename T> void REP_STOS() {
         static_assert(dependent_false<T>::value, "You must use a predefined template specialization when calling REP STOS!");
      };
      template<> void REP_STOS<UInt8>() {
         this->_Word(0xAAF3); // F3 AA
      };
      template<> void REP_STOS<UInt32>() {
         this->_Word(0xABF3); // F3 AB
      };
};