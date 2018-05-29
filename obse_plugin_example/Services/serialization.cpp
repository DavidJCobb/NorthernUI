#include "serialization.h"
#include "shared.h"

// ==================================================================================================
//    SerializationHelper
// ==================================================================================================

template<typename T>
bool SerializationHelper::Read(T* out) {
   if (this->isWriting)
      return false;
   return this->serializationInterface->ReadRecordData(out, sizeof(T)) == sizeof(T);
};
bool SerializationHelper::Read(std::string& value) {
   if (this->isWriting)
      return false;
   UInt32 length = 0;
   if (!this->Read(&length))
      return false;
   value.resize(length);
   return this->serializationInterface->WriteRecordData(value.data(), length);
};
bool SerializationHelper::ReadBuffer(char* value, const UInt32 length) {
   if (this->isWriting)
      return false;
   return this->serializationInterface->WriteRecordData(value, length);
};

template<typename T>
bool SerializationHelper::Write(const T* value) {
   if (!this->isWriting)
      return false;
   return this->serializationInterface->WriteRecordData(value, sizeof(T));
};
bool SerializationHelper::Write(const std::string& value) {
   if (!this->isWriting)
      return false;
   UInt32 length = value.length();
   if (!this->Write(&length))
      return false;
   return this->serializationInterface->WriteRecordData(value.c_str(), value.length());
};
bool SerializationHelper::Write(const char* value, const UInt32 length) {
   if (!this->isWriting)
      return false;
   if (!this->Write(&length))
      return false;
   return this->serializationInterface->WriteRecordData(value, length);
};

// ==================================================================================================
//    SerializationManager
// ==================================================================================================

SerializationManager& SerializationManager::GetInstance() {
   static SerializationManager manager;
   return manager;
};

void SerializationManager::Init(PluginHandle& pluginHandle, OBSESerializationInterface* serializationInterface) {
   if (this->initialized)
      return;
   if (SerializationManager::bDebugLogging)
      _MESSAGE("SerializationManager is initializing...");

   g_serialization->SetSaveCallback   (pluginHandle, SerializationManager::OnSave);
   g_serialization->SetLoadCallback   (pluginHandle, SerializationManager::OnLoad);
   g_serialization->SetNewGameCallback(pluginHandle, SerializationManager::OnNewGame);
   //
   if (SerializationManager::bUsesPreload)
      g_serialization->SetPreloadCallback(pluginHandle, SerializationManager::OnPreload);

   this->initialized = true;
   //
   if (SerializationManager::bDebugLogging)
      _MESSAGE("SerializationManager initialized..");
};

void SerializationManager::OnSave(void* reserved) {
   if (SerializationManager::bDebugLogging)
      _MESSAGE("SerializationManager caught OnSave.");
   //
   auto manager = SerializationManager::GetInstance();
   SerializationHelper helper(g_serialization, false);

   for (auto it = manager.records.begin(); *it; it++) {
      auto record = *it;
      //
      g_serialization->OpenRecord(record->GetSignature(), record->GetVersion());
      record->Save(&helper);
   }
};
void SerializationManager::OnLoad(void* reserved) {
   if (SerializationManager::bDebugLogging)
      _MESSAGE("SerializationManager caught OnLoad.");
   //
   auto manager = SerializationManager::GetInstance();
   SerializationHelper helper(g_serialization, true);

   UInt32 type, version, length;
   while (g_serialization->GetNextRecordInfo(&type, &version, &length)) {
      if (SerializationManager::bDebugLogging)
         _MESSAGE("SerializationManager loaded record %08X (%.4s) %08X %08X", type, &type, version, length);
      //
      bool found = false;
      for (auto it = manager.records.begin(); *it; it++) {
         UInt32 signature = (*it)->GetSignature();
         if (signature == type) {
            found = true;
            (*it)->Load(&helper, version, length);
            break;
         }
      }
      if (!found)
         _MESSAGE("SerializationManager found an unknown record signature: $08X.", type);
   }
};
void SerializationManager::OnPreload(void* reserved) {
   if (SerializationManager::bDebugLogging)
      _MESSAGE("SerializationManager caught OnPreload.");
   //
   auto manager = SerializationManager::GetInstance();
   for (auto it = manager.records.begin(); *it; it++)
      (*it)->OnPreload(reserved);
};
void SerializationManager::OnNewGame(void* reserved) {
   if (SerializationManager::bDebugLogging)
      _MESSAGE("SerializationManager caught OnNewGame.");
   //
   auto manager = SerializationManager::GetInstance();
   for (auto it = manager.records.begin(); *it; it++)
      (*it)->OnNewGame(reserved);
};

void SerializationManager::RegisterRecord(std::shared_ptr<SerializableTopLevelRecord> record) {
   if (!this->initialized) {
      _MESSAGE("SerializationManager was asked to register a record with signature %08X before being initialized.", record->GetSignature());
      return;
   }
   //
   // Add the record to the list only if it's not already in the list.
   //
   auto it = std::find(this->records.begin(), this->records.end(), record);
   if (it == this->records.end())
      this->records.push_back(record);
};
void SerializationManager::UnregisterRecord(std::shared_ptr<SerializableTopLevelRecord> record) {
   if (!this->initialized) {
      _MESSAGE("SerializationManager was asked to unregister a record with signature %08X before being initialized.", record->GetSignature());
      return;
   }
   #if PRESERVE_ORDER_WHEN_REMOVING == 0
      //
      // Swap a to-be-removed element with the last element in the list, 
      // to prevent its removal from shifting everything after it in the 
      // list.
      //
      auto it = std::find(this->records.begin(), this->records.end(), record);
      if (it != this->records.end()) {
         std::swap(*it, this->records.back());
         this->records.pop_back();
      }
   #else
      auto it = std::find(this->records.begin(), this->records.end(), record);
      if (it != this->records.end())
         this->records.erase(it);
   #endif
};