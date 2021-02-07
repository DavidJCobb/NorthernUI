#pragma once

#define DEFINE_SUBROUTINE_EXTERN(retType, name, address, ...) extern retType(*name##)(__VA_ARGS__);
#define DEFINE_SUBROUTINE(retType, name, address, ...) retType(*name##)(__VA_ARGS__) = (retType(*)(__VA_ARGS__))address;

#include "obse/GameTypes.h"
namespace RE {
   /*class BSStringT : public ::BSStringT { // can't be constructed as a local var, since OBSE didn't define a constructor
      public:
         BSStringT() { ; };
         //
         MEMBER_FN_PREFIX(BSStringT);
         DEFINE_MEMBER_FN(Replace_MinBufLen, void, 0x004028D0, const char*, UInt32 minBufferLength);

         UInt16 GetLength() { // inlined
            UInt16 out = this->m_dataLen;
            if (out == 0xFFFF) {
               if (!this->m_data)
                  return 0;
               out = strlen(this->m_data) + 1;
            }
            return out;
         };
   };*/
   class BSStringT { // won't support OBSE methods unless we add them; can't be passed to OBSE definitions without a cast
      public:
         BSStringT() {};
         ~BSStringT();
         //
         char*  m_data    = nullptr;
         UInt16 m_dataLen = 0;
         UInt16 m_bufLen  = 0;
         //
         MEMBER_FN_PREFIX(BSStringT);
         DEFINE_MEMBER_FN(Append,             void,       0x00412FA0, const char*);
         DEFINE_MEMBER_FN(CopyValueFromOther, BSStringT*, 0x004FB4C0, BSStringT*); // returns self
         DEFINE_MEMBER_FN(IsNotEqualTo,       bool,       0x00517B20, BSStringT*);
         DEFINE_MEMBER_FN(Replace_MinBufLen,  void,       0x004028D0, const char*, UInt32 minBufferLength);
         DEFINE_MEMBER_FN(Replace,            void,       0x00488810, const char*);
         DEFINE_MEMBER_FN(Sprintf,            void,       0x00402D00, const char* format, UInt32* varArgs);

         __declspec(noinline) UInt16 GetLength() { // inlined
            UInt16 out = this->m_dataLen;
            if (out == 0xFFFF) {
               if (!this->m_data)
                  return 0;
               out = strlen(this->m_data) + 1;
            }
            return out;
         };
         bool IsEqual(BSStringT* other, bool caseSensitive = true) const;
         void ToStdString(std::string* out) const;
   };
   DEFINE_SUBROUTINE_EXTERN(void, SprintfBSStringT, 0x00402E30, BSStringT* out, const char* format, ...);

   template<typename T>
   struct tList {
      T* data;
      tList<T>* next;

      MEMBER_FN_PREFIX(tList);
      DEFINE_MEMBER_FN(Append,  void, 0x0067B1E0, T* item);
      DEFINE_MEMBER_FN(Prepend, void, 0x00446CB0, T* item);

      void ClearSelfAndAfter() {
         auto node = this->next;
         do {
            auto next = node->next;
            node->data = nullptr;
            FormHeap_Free(node);
            node = next;
         } while (node);
      }

      struct iterator {
         friend struct tList<T>; // for constructor access
         protected:
            tList<T>* node;
            //
            iterator(tList<T>* t) : node(t) {}
            //
         public:
            iterator& operator++() { // prefix
               if (!this->node)
                  return *this;
               this->node = this->node->next;
               return *this;
            }
            iterator operator++(int) { // postfix
               iterator temp(this->node);
               ++temp;
               return temp;
            }
            T* operator*() const {
               return this->node->data;
            }
            //
            inline bool operator==(const iterator& o) const noexcept { return this->node == o.node; }
            inline bool operator!=(const iterator& o) const noexcept { return this->node != o.node; }
      };
      iterator begin() { return iterator(this); }
      iterator end() { return iterator(nullptr); }
      iterator find(T* value) {
         auto it = this->begin();
         for (; it != this->end(); ++it)
            if (*it == value)
               return it;
         return it;
      }
   };
   template<typename T> using LinkedPointerList = tList<T>;
};