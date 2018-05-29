#pragma once
#include "shared.h"
#include "Services/serialization.h"

namespace ScriptCommands {
   namespace OBSESamples {
      class SerializedData01 {
         public:
            virtual UInt32 GetSignature() { return 'STR '; };
            virtual UInt32 GetVersion()   { return 0; };
            //
            virtual bool Save(SerializationHelper* writer);
            virtual bool Load(SerializationHelper* reader, UInt32 version, UInt32 length);
            //
            virtual void OnNewGame(void* reserved);
            virtual void OnPreload(void* reserved);
            //
            virtual bool NeedsPreload() { return false; };
      };
      class SerializedData02 {
         public:
            virtual UInt32 GetSignature() { return 'ASDF'; };
            virtual UInt32 GetVersion()   { return 1234; };
            //
            virtual bool Save(SerializationHelper* writer);
            virtual bool Load(SerializationHelper* reader, UInt32 version, UInt32 length);
            //
            virtual void OnNewGame(void* reserved) {};
            virtual void OnPreload(void* reserved) {};
            //
            virtual bool NeedsPreload() { return false; };
      };

      bool RegisterCommands(const OBSEInterface* obse);
   };
}