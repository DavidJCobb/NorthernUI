#pragma once
#include "obse/Utilities.h"

namespace RE {
   class NiTListFloat {
      public:
         enum {
            kVtbl = 0x00A6A1CC,
         };
         NiTListFloat();
         ~NiTListFloat();
         //
         struct Node {
            Node* next;
            Node* prev;
            float data;
         };
         virtual void  Dispose(bool);
         virtual Node* AllocateNode();
         virtual void  FreeNode(Node* node);
         //
         Node*  start;    // 04
         Node*  end;      // 08
         UInt32 numItems; // 0C
         //
         static NiTListFloat* Create();
         //
         MEMBER_FN_PREFIX(NiTListFloat);
         DEFINE_MEMBER_FN(Prepend,     void,  0x005896F0, float*);
         DEFINE_MEMBER_FN(RemoveFirst, float, 0x005889F0);
   };

   template <typename T>
   class NiTListBase { // sizeof == 0x10
      public:
         NiTListBase();
         ~NiTListBase();

         struct Node {
            Node* next;
            Node* prev;
            T*    data;
         };

         virtual void  Destructor();
         virtual Node* AllocateNode();
         virtual void  FreeNode(Node* node);

         //	void	** _vtbl;	// 00
         Node*  start;    // 04
         Node*  end;      // 08
         UInt32 numItems; // 0C

         __declspec(noinline) void Append(T* subject) {
            this->numItems++;
            auto added = this->AllocateNode();
            added->data = subject;
            added->next = nullptr;
            auto prior = this->end;
            added->prev = prior;
            this->end = added;
            if (!prior) {
               this->start = added;
            } else
               prior->next = added;
         };
         __declspec(noinline) void Prepend(T* subject) {
            this->numItems++;
            auto added = this->AllocateNode();
            added->data = subject;
            added->prev = nullptr;
            auto first = this->start;
            added->next = first;
            this->start = added;
            if (first)
               first->prev = added;
            else if (!this->end)
               this->end = added;
         };
         __declspec(noinline) T* RemoveLast() {
            auto last = this->end;
            if (!last)
               return nullptr;
            auto data = last->data;
            auto prev = last->prev;
            this->end = prev;
            if (prev) {
               prev->next = nullptr;
            } else {
               this->start = nullptr;
            }
            this->numItems--;
            this->FreeNode(last);
            return data;
         };
   };
};