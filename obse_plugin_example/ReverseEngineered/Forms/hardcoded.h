#pragma once

class TESEyes;
class TESFaction;
class TESObjectMISC;
class TESObjectSTAT;
class TESRace;
class TESSoulGem;
class TESWaterForm;
namespace RE {
   static const auto ptrDoorMarkerForm      = (TESObjectSTAT**) 0x00B35EA4;
   static const auto ptrMapMarkerForm       = (TESObjectSTAT**) 0x00B35EA8;
   static const auto ptrXMarkerForm         = (TESObjectSTAT**) 0x00B35EAC;
   static const auto ptrXMarkerHeadingForm  = (TESObjectSTAT**) 0x00B35EB0;
   static const auto ptrTravelMarkerForm    = (TESObjectSTAT**) 0x00B35EB4;
   static const auto ptrNorthMarkerForm     = (TESObjectSTAT**) 0x00B35EB8;
   static const auto ptrPrisonMarkerForm    = (TESObjectSTAT**) 0x00B35EBC;
   static const auto ptrTempleMarkerForm    = (TESObjectSTAT**) 0x00B35EC0;
   static const auto ptrDivineMarkerForm    = (TESObjectSTAT**) 0x00B35EC4;
   static const auto ptrLockpickForm        = (TESObjectMISC**) 0x00B35EC8;
   static const auto ptrSkeletonKeyForm     = (TESObjectMISC**) 0x00B35ECC;
   static const auto ptrRepairHammerForm    = (TESObjectMISC**) 0x00B35ED0;
   static const auto ptrHorseMarkerForm     = (TESObjectSTAT**) 0x00B35ED4;
   static const auto ptrWelkyndStoneForm    = (TESObjectMISC**) 0x00B35ED8;
   static const auto ptrVarlaStoneForm      = (TESObjectMISC**) 0x00B35EDC;
   static const auto ptrBlackSoulGemForm    = (TESSoulGem**)    0x00B35EE0;
   static const auto ptrAzuraStoneForm      = (TESSoulGem**)    0x00B35EE4; // yes, the executable calls it the "Azura Stone"
   static const auto ptrDefaultWaterForm    = (TESWaterForm**)  0x00B360AC;
   static const auto ptrCreatureFactionForm = (TESFaction**)    0x00B36298;
   static const auto ptrEyeReanimateForm    = (TESEyes**)       0x00B362BC;
   static const auto ptrVampireRaceForm     = (TESRace**)       0x00B36308;
};