#pragma once

// Class for building callable x86 functions at run-time: a stupidly excessive 
// solution to the inability to create templated naked functions and the random 
// bizarre bugs that occur when #define-ing whole functions with ASM blocks.
//
// An x86Writer instance allocates memory to write its subroutine. You need to 
// know how large your subroutine will be in advance (unless I write code to 
// reallocate), but you don't need to free it. I mean, you'd be patching a call 
// to the memory right into the executable. You want the memory to exist for 
// the rest of the program session, no?
//
class x86Writer {
   protected:
      UInt32 baseAddress;
      UInt32 offset = 0;

      void _Byte (char);
      void _Word (unsigned short);
      void _Dword(unsigned int);

   public:
      x86Writer(UInt32 size);

      UInt32 GetAddress() const;
      void LogDebug() const;

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

      void ADD (Register, UInt8);
      void ADD (Register, UInt32);
      void CALL(Register);
      void CALL(UInt32 target); // near call
      void INC (Register);
      void JMP (UInt32 target); // near jump
      void MOV (Register to, Register from);
      void MOV (Register, UInt8); // WARNING: Doesn't clear upper bytes! // TODO: convert to template specialization to prevent accidental use
      void MOV (Register, UInt32);
      void NOP ();
      void POP (Register);
      void PUSH(Register); // push register onto the stack
      void PUSH(SInt8);    // push raw value onto the stack // note: the pushed value is sign-extended to a 32-bit argument by the CPU
      void PUSH(UInt32);   // push raw value onto the stack
      void RETN(UInt32 argCount = 0); // near return
      void SETNZ(Register);
      void SETZ(Register);
      void SUB (Register target, Register subject);
      void SUB (Register, UInt8);
      void SUB (Register, UInt32);
      void TEST(Register); // TEST r, r
      void XOR (Register, Register); // TODO: Template this; specialize to UInt32 (0x33) and UInt8 (0x32)

      void Abs(Register); // sets eax to the absolute value of the input SInt32 register; edx will also be modified in the process
      void StackFrame();
      void StackFrameExit();

      // REP MOVS: memcpy(UInt8* edi, UInt8* esi, ecx * sizeof(T))
      template<typename T>
      void REP_MOVS() {};
      template<>
      void REP_MOVS<UInt8>();
      template<>
      void REP_MOVS<UInt32>();

      // REP STOS: memset(UInt8* edi, (T)eax, ecx * sizeof(T))
      template<typename T>
      void REP_STOS() {};
      template<>
      void REP_STOS<UInt8>();
      template<>
      void REP_STOS<UInt32>();
};

// Automatically handles memory allocation, and reallocates if your subroutine 
// exceeds the current buffer.
//
class x86WriterAuto : public x86Writer {
   protected:
      UInt32 size;

      void _Guarantee(UInt32 add);
      void _Realloc(UInt32 size);
      void _Shrink();

      void _Byte(char);
      void _Word(unsigned short);
      void _Dword(unsigned int);

   public:
      x86WriterAuto() : x86Writer(10) {};
};