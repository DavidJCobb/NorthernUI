#include "NiTypes.h"
#include "obse/GameAPI.h"
#include "obse/Utilities.h"

namespace RE {
   NiTListFloat::~NiTListFloat() {
      this->Dispose(false);
   };
   void NiTListFloat::Dispose(bool freeMemory) {
      ThisStdCall(0x00589D40, this, freeMemory);
   };
   NiTListFloat::Node* NiTListFloat::AllocateNode() {
      return (NiTListFloat::Node*) ThisStdCall(0x00589740, this);
   };
   void NiTListFloat::FreeNode(Node* node) {
      ThisStdCall(0x00589750, this, node);
   };
   NiTListFloat* NiTListFloat::Create() {
      auto memory = FormHeap_Allocate(sizeof(NiTListFloat));
      if (memory) {
         *(UInt32*)(memory)       = kVtbl;
         *((UInt32*)memory + 0x4) = 0;
         *((UInt32*)memory + 0x8) = 0;
         *((UInt32*)memory + 0xC) = 0;
         return (NiTListFloat*) memory;
      }
      return nullptr;
   };
   /*//
   void NiTListFloat::Append(float* subject) {
      this->numItems++;
      auto added = this->AllocateNode();
      added->data = *subject;
      added->next = nullptr;
      auto prior = this->end;
      added->prev = prior;
      this->end = added;
      if (!prior) {
         this->start = added;
      } else
         prior->next = added;
   };
   void NiTListFloat::Prepend(float* subject) {
      this->numItems++;
      auto added = this->AllocateNode();
      added->data = *subject;
      added->prev = nullptr;
      auto first = this->start;
      added->next = first;
      this->start = added;
      if (first)
         first->prev = added;
      else if (!this->end)
         this->end = added;
   };
   void NiTListFloat::RemoveFirst() {
      auto first = this->end;
      if (!first)
         return;
      auto data = first->data;
      auto next = first->next;
      this->start = next;
      if (next)
         next->prev = nullptr;
      else
         this->start = nullptr;
      this->numItems--;
      this->FreeNode(first);
   };
   void NiTListFloat::RemoveLast() {
      auto last = this->end;
      if (!last)
         return;
      auto data = last->data;
      auto prev = last->prev;
      this->end = prev;
      if (prev)
         prev->next = nullptr;
      else
         this->start = nullptr;
      this->numItems--;
      this->FreeNode(last);
   };
   //*/
};