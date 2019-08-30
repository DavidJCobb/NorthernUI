#pragma once

namespace CobbPatches {
   namespace TagIDs {
      void Apply();
      //
      bool IsOperator(UInt32 id);
      bool IsCustomOperator(UInt32 id);
      bool IsOperatorWithStringOperand(UInt32 id);
      bool IsTile(UInt32 id);
      bool IsTrait(UInt32 id);
      bool IsCustomStringTrait(UInt32 id);
      bool IsStringTraitAffectingTextRender(UInt32 id); // must be maintained manually
      //
      enum TagIDType {
         kType_Trait     = 1,
         kType_Operator  = 2,
         kType_Selector  = 3,
         kType_TileClass = 4,
      };
      struct TagID {
         TagID(UInt32 a, const char* b, TagIDType c, bool d, bool e) : id(a), name(b), type(c), isMenuQue(d), isString(e) {};
         //
         const UInt32    id;
         const char*     name;
         const TagIDType type;
         const bool      isMenuQue = false;
         const bool      isString  = false; // is a string operator or a string trait
      };
      #define XXN_NEW_TAG_ID(id, name, type, menuque, isString) id, name, type, menuque, isString
      ;

      // Constexprs provided for use in inline assembly if needed.
      //
      constexpr UInt32 _traitFontPath   = 0x1600; // trait is handled in patch Fonts.cpp
      constexpr UInt32 _traitNorthernUI = 0x1601; // selector
      constexpr UInt32 _traitDescendant = 0x1602; // selector
      constexpr UInt32 _traitKeepNiProperties = 0x1603; // trait is handled in patch Miscellaneous.cpp: CobbPatches::Miscellaneous::Tile3DRenderChanges::KeepNiPropertiesInTile3D
      constexpr UInt32 _traitNiScale    = 0x1604; // trait is handled in patch Miscellaneous.cpp: CobbPatches::Miscellaneous::Tile3DRenderChanges::Scale
      constexpr UInt32 _traitOpSine     = 0x1605; // operator (float)
      constexpr UInt32 _traitOpCosine   = 0x1606; // operator (float)
      constexpr UInt32 _traitOpTangent  = 0x1607; // operator (float)
      constexpr UInt32 _traitOpAtan2    = 0x1608; // operator (float)
      constexpr UInt32 _traitOpStringEq = 0x1609; // operator (string)
      constexpr UInt32 _traitOpStringAppend = 0x160A; // operator (string)
      constexpr UInt32 _traitOpBinaryAnd = 0x160B; // operator (float)
      constexpr UInt32 _traitOpBinaryOr  = 0x160C; // operator (float)
      constexpr UInt32 _traitStrings     = 0x160D; // selector
      constexpr UInt32 _traitOpSetIfZero = 0x160E; // operator (float)
      constexpr UInt32 _traitLocConfig   = 0x160F; // selector
      constexpr UInt32 _traitOpPrefModify     = 0x1610; // operator (string operand)
      constexpr UInt32 _traitOpPrefClampToMin = 0x1611; // operator (string operand)
      constexpr UInt32 _traitOpPrefClampToMax = 0x1612; // operator (string operand)
      constexpr UInt32 _traitOpPrefReset      = 0x1613; // operator (string operand)
      constexpr UInt32 _traitPrefs       = 0x1614; // selector
      constexpr UInt32 _traitOpPrefModulo     = 0x1615; // operator (string operand)
      constexpr UInt32 _traitOpPrefCarousel   = 0x1616; // operator (string operand)
      //
      // NOTE: ID 0x1771 is not registered, but the vanilla code sets it on the menu root.
      //
      // NOTE: ID 0x1772 is not registered, but the vanilla code uses it to indicate some form of 
      //       menu state.
      //
      // NOTE: Tile::Value::AppendSrcOperator doesn't allow traits with IDs below 0xFA1 to be 
      //       used as the target of a src="" attribute.
      //
      // NOTE: IDs 0x2710 and above are used for underscore-prefixed traits.
      //
      const TagID tagIDs[] = {
         { XXN_NEW_TAG_ID(_traitFontPath,   "xxnfontpath",   kType_Trait,    0, 1) },
         { XXN_NEW_TAG_ID(_traitNorthernUI, "northernui",    kType_Selector, 0, 0) },
         { XXN_NEW_TAG_ID(_traitDescendant, "descendant",    kType_Selector, 0, 0) },
         { XXN_NEW_TAG_ID(_traitKeepNiProperties, "xxnkeepniproperties", kType_Trait, 0, 0) },
         { XXN_NEW_TAG_ID(_traitNiScale,    "xxnniscale",    kType_Trait,    0, 0) },
         { XXN_NEW_TAG_ID(_traitOpSine,     "xxnopsine",     kType_Operator, 0, 0) },
         { XXN_NEW_TAG_ID(_traitOpCosine,   "xxnopcosine",   kType_Operator, 0, 0) },
         { XXN_NEW_TAG_ID(_traitOpSine,     "xxnopsine",     kType_Operator, 0, 0) },
         { XXN_NEW_TAG_ID(_traitOpTangent,  "xxnoptangent",  kType_Operator, 0, 0) },
         { XXN_NEW_TAG_ID(_traitOpAtan2,    "xxnopatan2",    kType_Operator, 0, 0) },
         { XXN_NEW_TAG_ID(_traitOpStringEq, "xxnopstringeq", kType_Operator, 0, 1) },
         { XXN_NEW_TAG_ID(_traitOpStringAppend, "xxnopstringappend", kType_Operator, 0, 1) },
         { XXN_NEW_TAG_ID(_traitOpBinaryAnd, "xxnopbinaryand", kType_Operator, 0, 0) },
         { XXN_NEW_TAG_ID(_traitOpBinaryOr,  "xxnopbinaryor",  kType_Operator, 0, 0) },
         { XXN_NEW_TAG_ID(_traitStrings,     "xxnstrings",     kType_Selector, 0, 0) },
         { XXN_NEW_TAG_ID(_traitOpSetIfZero, "xxnopsetifzero", kType_Operator, 0, 0) },
         { XXN_NEW_TAG_ID(_traitLocConfig,   "xxnlocalization", kType_Selector, 0, 0) },
         { XXN_NEW_TAG_ID(_traitOpPrefModify,     "xxnopprefmodifyvalue", kType_Operator, 0, 1) },
         { XXN_NEW_TAG_ID(_traitOpPrefClampToMin, "xxnopprefclamptomin", kType_Operator, 0, 1) },
         { XXN_NEW_TAG_ID(_traitOpPrefClampToMax, "xxnopprefclamptomax", kType_Operator, 0, 1) },
         { XXN_NEW_TAG_ID(_traitOpPrefReset,      "xxnopprefresetvalue", kType_Operator, 0, 1) },
         { XXN_NEW_TAG_ID(_traitPrefs,       "xxnprefs",     kType_Selector, 0, 0) },
         { XXN_NEW_TAG_ID(_traitOpPrefModulo,     "xxnopprefmodulo", kType_Operator, 0, 1) },
         { XXN_NEW_TAG_ID(_traitOpPrefCarousel,   "xxnopprefcarousel", kType_Operator, 0, 1) },
      };
      constexpr UInt32 menuQueOperatorMin = 0x7EC;
      constexpr UInt32 menuQueOperatorMax = 0x7F0;
      constexpr UInt32 menuQueTraitMin = 0x1002;
      constexpr UInt32 menuQueTraitMax = 0x100E;
      constexpr UInt32 menuQueSelectorMin = 0x138B;
      constexpr UInt32 menuQueSelectorMax = 0x138B;
   };
};