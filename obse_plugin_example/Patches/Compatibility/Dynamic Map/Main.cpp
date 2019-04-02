#include "Main.h"
#include "Types.h"

#include "obse_common/SafeWrite.h"

#include "ReverseEngineered/UI/InterfaceManager.h"
#include "ReverseEngineered/UI/Tile.h"
#include "ReverseEngineered/UI/TagIDs.h"
#include "ReverseEngineered/UI/Menus/MapMenu.h"
#include "Miscellaneous/strings.h"

#include "obse/GameData.h" // FileFinder
#include "obse/ModTable.h" // for Dynamic Map emulation


namespace CobbPatches {
   namespace DynamicMapEmulation {
      //
      // Emulate the functions of Dynamic Map, for compatibility. This only 
      // works if the mod is installed (we're just doing what its scripts do 
      // but in C++; we're still using its definitions and assets).
      //
      constexpr const char* fileDefinitions = "Data\\Ini\\Dynamic Map.ini";
      enum {
         kDynamicMapStage_DefineWorldspace = 10,
         kDynamicMapStage_DefineOverlay    = 20,
      };
      //
      void _trim_and_explode(std::string& line, std::vector<std::string>& out) {
         out.clear();
         std::stringstream ss(line);
         std::string current;
         while (std::getline(ss, current, ' '))
            if (!current.empty())
               out.push_back(current);
      };
      void _find_quoted(std::string& line, std::string& out) {
         out.clear();
         size_t i = line.find_first_of('"');
         if (i == std::string::npos)
            return;
         size_t j = line.find_first_of('"', i + 1);
         if (j == std::string::npos)
            return;
         out = line.substr(i, j - i + 1);
      };
      //
      struct LoadedData {
         static LoadedData& GetInstance() {
            static LoadedData instance;
            return instance;
         };
         //
         std::vector<_dme::Worldspace> worldspaces;
         //
         inline const _dme::Worldspace* GetWorldspace(UInt32 formID) const {
            auto& list = this->worldspaces;
            for(auto it = list.begin(); it != list.end(); ++it)
               if (it->formID == formID)
                  return &(*it);
            return nullptr;
         };
      };
      void Load() {
         _MESSAGE("Loading Dynamic Map's definitions file, if it exists...");
         /*// Dynamic Map uses an OBSE function to load the script file; this function doesn't use FileFinder, so we don't need to either (you need FileFinder for BSAs)
         if ((*g_FileFinder)->FindFile(fileDefinitions, 0, 0, -1) == FileFinder::kFileStatus_NotFound) {
            _MESSAGE("Datastore file is missing. The datastore will be nullptr.");
            return;
         }*/
         FILE* file = fopen(fileDefinitions, "r");
         if (file == nullptr)
            return;
         _MESSAGE("Parsing Dynamic Map's definitions file...");
         try {
            SInt32 lineNumber = -1;
            SInt32 currentWorldspaceIndex = -1;
            _dme::Undifferentiated current;
            char rawLine[4096];
            while (fgets(rawLine, sizeof(rawLine), file)) {
               lineNumber++;
               //
               // Dynamic Map defines its overlays in a data file, except... it's not 
               // actually a data file. It's a script -- a set of console commands to 
               // be executed in-game. The commands write certain values to the script 
               // object attached to a quest, and then set a stage on that quest; the 
               // quest reacts to this by acting on those objects. This is akin to 
               // creating a data structure, using it, and then discarding it.
               //
               // The parsing here is designed to be fast to implement, not accurate. 
               // We look for any substring in double-quotes and save whatever we find; 
               // we then split the whole line up by spaces. From there, we make certain 
               // assumptions about which space-separated tokens are "keys" or "values" 
               // based on the command (Set or SetStage) we're decoding. This does mean 
               // that a malformed file (e.g. quoted substring in the wrong spot) can be 
               // read incorrectly, but... tough.
               //
               std::string line = rawLine;
               cobb::trim(line);
               if (line[0] == ';') // skip comments
                  continue;
               size_t i = 0;
               size_t size = line.size();
               if (!size)
                  continue;
               std::vector<std::string> chunks;
               std::string scriptedString; // needed for string values, since the "explode" function will wreck those
               _trim_and_explode(line, chunks);
               _find_quoted(line, scriptedString);
               if (!chunks.size())
                  continue;
               if (scriptedString.size())
                  scriptedString = scriptedString.substr(1, scriptedString.size() - 2); // strip quotes
               //
               if (cobb::strieq(chunks[0], "set")) {
                  std::string key = chunks[1];
                  {
                     size_t delim = key.find_first_of('.');
                     if (delim == std::string::npos)
                        continue;
                     key = key.substr(delim + 1);
                  }
                  if (cobb::strieq(key, "mod")) { // set tnoDM.mod to sv_Construct "Oblivion.esm"
                     current.name = scriptedString;
                  } else if (cobb::strieq(key, "or_mod")) {
                     current.name_alternate = scriptedString;
                  } else if (cobb::strieq(key, "img")) {
                     current.image = scriptedString;
                  } else if (cobb::strieq(key, "fid")) { // set tnoDM.fid to sv_Construct "00003C"
                     current.formID = stol(scriptedString, nullptr, 16);
                  } else if (cobb::strieq(key, "x")) { // set tnoDM.nw_x to -59
                     current.x = stof(chunks[3]);
                  } else if (cobb::strieq(key, "y")) {
                     current.y = stof(chunks[3]);
                  } else if (cobb::strieq(key, "w")) {
                     current.width = stof(chunks[3]);
                  } else if (cobb::strieq(key, "h")) {
                     current.height = stof(chunks[3]);
                  } else if (cobb::strieq(key, "nw_x")) {
                     current.xMin = stof(chunks[3]);
                  } else if (cobb::strieq(key, "nw_y")) {
                     current.yMin = stof(chunks[3]);
                  } else if (cobb::strieq(key, "se_x")) {
                     current.xMax = stof(chunks[3]);
                  } else if (cobb::strieq(key, "se_y")) {
                     current.yMax = stof(chunks[3]);
                  } else if (cobb::strieq(key, "zoom")) {
                     current.zoom = stof(chunks[3]);
                  }
               } else if (cobb::strieq(chunks[0], "setstage")) { // SetStage tnoDM 10
                  int stage = std::stol(chunks[2]); // this is so garbage under the hood tho
                  switch (stage) {
                     case kDynamicMapStage_DefineWorldspace:
                        {
//_MESSAGE("Committed worldspace.");
                           auto& list = LoadedData::GetInstance().worldspaces;
                           list.emplace_back(current);
                           currentWorldspaceIndex = list.size() - 1;
                        }
                        break;
                     case kDynamicMapStage_DefineOverlay:
                        {
//_MESSAGE("Committed overlay.");
                           if (current.image.empty()) {
                              current.image = current.name;
                              size_t i = current.image.size() - 3;
                              current.image[i] = 'd';
                              current.image[i + 1] = 'd';
                              current.image[i + 2] = 's';
                           }
                           current.image.insert(0, "Data\\Textures\\Menus\\DynamicMap\\");
                           auto& worlds = LoadedData::GetInstance().worldspaces;
                           if (currentWorldspaceIndex >= worlds.size())
                              throw std::runtime_error("INI file: Overlay defined before any worldspace!");
                           worlds[currentWorldspaceIndex].overlays.emplace_back(current);
                        }
                        break;
                     default:
                        throw std::runtime_error("INI file: SetStage command with unrecognized stage.");
                  }
                  current = _dme::Undifferentiated();
               } else
                  continue;
            }
         } catch (std::runtime_error& e) {
            _MESSAGE("An irrecoverable parsing error occurred when trying to read Dynamic Map's data definitions.");
            if (e.what())
               _MESSAGE(" - Error details: %s", e.what());
         }
         fclose(file);
      };
      namespace Patch {
         void Inner(RE::MapMenu* menu) {
            constexpr float ce_depthIncrement = 0.01F;
            //
            auto& data = LoadedData::GetInstance();
            auto worldData = data.GetWorldspace(menu->worldSpace->refID);
            if (!worldData)
               return;
            auto& list  = worldData->overlays;
            float depth = 0.01F;
            for (auto it = list.begin(); it != list.end(); ++it) {
               auto& entry = *it;
               {  // Is the entry's mod active?
                  auto& table = ModTable::Get();
                  if (entry.name.empty() || !table.IsModLoaded(entry.name.c_str()))
                     if (entry.name_alternate.empty() || !table.IsModLoaded(entry.name_alternate.c_str()))
                        continue;
               }
               auto tile = CALL_MEMBER_FN(menu, CreateTemplatedTile)(menu->worldMap, "dynamic_map_overlay", nullptr);
               if (!tile)
                  continue;
               CALL_MEMBER_FN(tile, UpdateString)(RE::kTagID_user0, entry.image.c_str());
               CALL_MEMBER_FN(tile, UpdateFloat)(RE::kTagID_user1, entry.x);
               CALL_MEMBER_FN(tile, UpdateFloat)(RE::kTagID_user2, entry.y);
               CALL_MEMBER_FN(tile, UpdateFloat)(RE::kTagID_user3, entry.width);
               CALL_MEMBER_FN(tile, UpdateFloat)(RE::kTagID_user4, entry.height);
               CALL_MEMBER_FN(tile, UpdateFloat)(RE::kTagID_user5, entry.zoom);
               CALL_MEMBER_FN(tile, UpdateFloat)(RE::kTagID_user6, depth);
               //
               depth += ce_depthIncrement;
            }
         };
         __declspec(naked) void Outer() {
            _asm {
               mov  eax, 0x0065D830; // reproduce patched-over call // PlayerCharacter::Subroutine0065D830
               call eax;             //
               push eax; // protect
               push esi;
               call Inner;
               add  esp, 4;
               pop  eax; // restore
               mov  ecx, 0x005B991B;
               jmp  ecx;
            };
         };
         void Apply() {
            WriteRelJump(0x005B9916, (UInt32)&Outer); // UpdateMapMenuWorldMap+0x736
         };
      };
      //
      void Apply() {
         if (!ModTable::Get().IsModLoaded("Dynamic Map.esp")) // NOTE: crashes if run too early
            return;
         Patch::Apply();
         Load();
      };
   };
};