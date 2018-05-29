#pragma once
#include "obse/PluginAPI.h"

#include <memory>
#include <vector>

// NOTE: SERIALIZATION DOES NOT WORK USING THE DEFAULT OPCODE BASE IN RELEASE BUILDS OF OBSE.
//       It does work in debug OBSE builds.
//
// As of this writing, this means that none of these systems have actually been tested.

// Usage instructions:
//
//  - Subclass SerializableTopLevelRecord and create a single instance of your subclass.
//
//     = Make sure none of your subclasses use the same signature, or they will conflict.
//
//  - Pass a shared_ptr to the instance to the SerializationManager, e.g.
//
//       std::shared_ptr<MySubclass> data01;
//       SerializationManager::GetInstance().RegisterRecord(std::static_pointer_cast<SerializableTopLevelRecord>(data01));
//
//  - Your subclass should define Save and Load and use methods on SerializationHelper to 
//    write/read values, similarly to how it's done in SKSE. E.g.
//
//       UInt32 someValue = 0;
//       if (!reader->Read(&someValue)) _MESSAGE("Failed!");
//
//  - Currently, this isn't all done; for example, I should really rewrite Preload to work 
//    the same way as Load.

class SerializationHelper;

class SerializableTopLevelRecord {
   public:
      virtual UInt32 GetSignature() = 0; // should return a four-character signature as a UInt32, e.g. 'DATA', 'STUF'
      virtual UInt32 GetVersion()   = 0; // read by the serialization manager when saving; should be a constant
      //
      virtual bool Save(SerializationHelper* writer) = 0;
      virtual bool Load(SerializationHelper* reader, UInt32 version, UInt32 length) = 0;
      //
      virtual void OnNewGame(void* reserved) = 0;
      virtual void OnPreload(void* reserved) = 0;
      //
      virtual bool NeedsPreload() = 0;
};

class SerializationHelper {
   private:
      bool isWriting;
      OBSESerializationInterface* serializationInterface;
   public:
      SerializationHelper(OBSESerializationInterface* sI, bool writeMode) : serializationInterface(sI), isWriting(writeMode) {
         if (sI == nullptr) {
            _MESSAGE("ERROR: SerializationHelper created with null OBSESerializationInterface!");
            // TODO: raise exception
         }
      };
      
      template<typename T>
      bool Read(T* out);
      bool Read(std::string& out);
      bool ReadBuffer(char* out, const UInt32 length);
      //
      template<typename T>
      bool Write(const T* value);
      bool Write(const std::string& value);
      bool Write(const char* value, const UInt32 length);
};

#define PRESERVE_ORDER_WHEN_REMOVING 0
class SerializationManager {
   private:
      bool initialized = false;
      std::vector<std::shared_ptr<SerializableTopLevelRecord>> records;
      //
      static void OnSave(void* reserved);
      static void OnLoad(void* reserved);
      static void OnPreload(void* reserved);
      static void OnNewGame(void* reserved);
      //
   public:
      static SerializationManager& GetInstance();

      static const bool bUsesPreload  = false;
      static const bool bDebugLogging = false;

      void Init(PluginHandle& pluginHandle, OBSESerializationInterface* serializationInterface);

      void RegisterRecord  (std::shared_ptr<SerializableTopLevelRecord> record);
      void UnregisterRecord(std::shared_ptr<SerializableTopLevelRecord> record);
};