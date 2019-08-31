#pragma once
#include "ReverseEngineered/_BASE.h"
#include "ReverseEngineered/UI/TagIDs.h"
#include "ReverseEngineered/UI/Menu.h"
#include "obse/GameAPI.h"
#include "obse/GameTiles.h"
#include "obse/NiTypes.h"

//
// The XML parse process works as follows:
//
// Stage 1:
// 
//    Convert raw XML code to a series of very basic tokens. Each token has a 
//    parse-code, a float value, a number value, a result value, and a line 
//    number. The result value is generally unfilled.
//
//    Parse-codes at this stage are very simple -- typically 0xA for the start 
//    of a start-tag, and 0xF for the start of an end-tag; additionally, each 
//    supported attribute has its own ID. The tokens are in some ways implicit; 
//    for example, there is no token for the ">" in any tag.
//
//    The struct used for tokens is Tile::TileTemplateItem.
//
// Stage 1.5:
//
//    TileTemplate::AddTemplateItem converts the simple codes above to more 
//    meaningful representations. The starting tag of a tile might have two 
//    tokens, one for the tag start and name and another for the attribte type 
//    and value-as-string; this stage of the parse process would crunch both 
//    of those tokens into a single token representing the start of a tile, 
//    with the tile ID and name stored as the float and string, respectively, 
//    on that token.
//
//    The exception is this: operators that serve as grouping elements are 
//    still saved with generic 0xA and 0xF (open-tag and close-tag) codes. 
//    This is because traits don't store a hierarchy of operators; they store 
//    a flat list, and the 0xA and 0xF entries denote nesting and de-nesting 
//    in that list.
//
// Stage 2:
//
//    A Tile hierarchy is generated for tokens refined in Stage 1.5, and then 
//    appended to a suitable containing Tile. For tiles outside of a template, 
//    this happens immediately; templated content is kept at Stage 1.5, and is 
//    only put through Stage 2 when instances of the template are appended to 
//    the displayed content.
//
//     - The hierarchy is created using Tile::CreateTemplatedChildren, which 
//       iterates through TileTemplate's items paying attention only to tile 
//       starts and ends.
//
//     - Once the hierarchy is created, it becomes possible to properly 
//       resolve src attributes on traits; Tile::UpdateTemplatedChildren is 
//       used to walk TileTemplate's items and add all traits to all tiles as 
//       necessary.
//
// Additional notes for understanding disassembly of the parser:
//
//  - Internally, operators are called "actions."
//
//  - All IDs -- parse codes, operator IDs, trait IDs, tile IDs, and attribute 
//    IDs -- exist in the same space. OBSE labels trait IDs under the kTileValue 
//    enum, but "tag ID" is a more appropriate term.
//
//  - Operators are stored in flat lists, not hierarchies. Within these flat 
//    lists, the 0xA and 0xF codes are used to denote nesting and de-nesting of 
//    operators and values.
//

class NiControllerManager;
namespace RE {
   class  Menu;
   struct TileParseState;
   struct TileTemplate;
   class  Tile;

   extern TileParseState* const g_tileParseState;

   DEFINE_SUBROUTINE_EXTERN(Tile*,       FindSrcFromTile,         0x0058B800, Tile* basis, const char* src); // parses the src with ProcessSrcAtributeVal
   DEFINE_SUBROUTINE_EXTERN(Tile*,       GetDescendantTileByName, 0x00589B10, Tile* parent, const char* name); // searches recursively
   DEFINE_SUBROUTINE_EXTERN(UInt32,      GetOrCreateTempTagID,    0x0058B040, const char* traitName, SInt32 alwaysNegativeOne); // used for traits beginning with an underscore or ampersand
   DEFINE_SUBROUTINE_EXTERN(UInt32,      ProcessSrcAttributeVal,  0x0058B800, const char* src, char** outSrcArg); // returns trait ID; outSrcArg is set to the argument, e.g. "sibling(IT_IS_SET_TO_THIS)"
   DEFINE_SUBROUTINE_EXTERN(const char*, TagIDToName,             0x00589080, SInt32 tagID);        // OBSE: TileStrIDToStr
   DEFINE_SUBROUTINE_EXTERN(SInt32,      TagNameToID,             0x00588EF0, const char* tagName); // OBSE: TileStrToStrID // MUST return SInt32, because it's used for XML entities as well!

   class Tile {
      public:
         static Tile* Decorate(::Tile* obj) {
            return (Tile*) obj;
         }

         struct StringListElement { // sizeof == 0x10
            //
            // This struct represents a temporary trait (i.e. a trait with an underscore-prefixed name) 
            // and is stored in global arrays and lists. Underscore-prefixed traits have IDs created 
            // for them at run-time in the order they're encountered; these IDs start at 0x2710.
            //
            UInt32    traitID;  // 00
            UInt32    refCount; // 04 // actually, it's the ref count minus one
            BSStringT name;     // 08

            MEMBER_FN_PREFIX(StringListElement);
            DEFINE_MEMBER_FN(Constructor, StringListElement&, 0x00589F20, SInt32, BSStringT);
         };
         struct TileTemplateItem { // these are better thought of as "tokens" created from the XML parser
            // constructor: 00589FA0
            enum Code : SInt32 { // values for unk00
               kCode_Invalid               =   -1,
               kCode_StartTag              = 0x0A, // any start tag // converted to something else in parse Stage 1.5
               kCode_MQTextContent         = 0x0D, // seen in MenuQue; possibly tag text content
               kCode_EndTag                = 0x0F, // any end   tag // converted to something else in parse Stage 1.5
               kCode_NonConstNonSrcOperatorStart = 0x14, // start of non-const, non-src operator
               kCode_NonConstNonSrcOperatorEnd   = 0x19, // end   of non-const, non-src operator
               kCode_ContainerOperatorStart = kCode_NonConstNonSrcOperatorStart,
               kCode_ContainerOperatorEnd   = kCode_NonConstNonSrcOperatorEnd,
               kCode_NonConstTraitStart    = 0x1E, // start of non-const trait // during Stage 1.5 parsing, some of these are converted to ConstTraits.
               kCode_NonConstTraitEnd      = 0x23, // end   of non-const trait
               kCode_TileStart             = 0x28, // start of a tile: result and tagType are tag ID, string is name attr; CreateTemplatedChildren builds it while UpdateTemplatedChildren uses it to determine what to apply traits, etc., to
               kCode_TileMQAttribute       = 0x2A, // MenuQue extension; most likely "any attribute"
               kCode_TileEnd               = 0x2D, // end   of a tile (or trait??)
               kCode_ConstTrait            = 0x32, // trait with const value
               kCode_ConstOperator         = 0x37, // operator with const value
               kCode_XXNConstStringOperator = 0x38, // operator with const string value
               kCode_SrcOperator           = 0x3C, // operator with src: result is operator ID; string is operator SRC; tagType is containing trait's ID
               kCode_TextContent           = 0xBB9, // can this also be a 'value=""' attribute? // converted to something else in parse Stage 1.5
               kCode_AttributeName         = 0xBBA, // converted to something else in parse Stage 1.5
               kCode_AttributeSrc          = 0xBBB, // converted to something else in parse Stage 1.5
               kCode_AttributeTrait        = 0xBBC, // converted to something else in parse Stage 1.5
               kCode_AttributeMQDest       = 0xBBD, // MenuQue extension; further information not yet known
            };

            static TileTemplateItem* Create(UInt32 code, float tagIdOrValue, const char* string, SInt32 result, UInt32 lineNumber);
            TileTemplateItem(UInt32 code, float tagIdOrValue, const char* string, SInt32 result, UInt32 lineNumber);
            ~TileTemplateItem();

            UInt32    unk00;      // 00 // one of the codes above OR a tag ID
            float     tagType;    // 04 // tile/trait type ID or float trait value
            BSStringT string;     // 08 // tag name, attr name, or attr value
            union {
               SInt32 result;     // 10 // SInt32 trait ID
               Tile*  resultTile; // 10 // generated tile, during second parser pass
            };
            UInt32    lineNumber; // 14

            bool ExtendedStrOrNumParse(); // returns true if it's a number
         };
         struct Value {
            union Operand {
               Value*      ref;
               float       immediate;
               UInt32      opcode; // used if the outer opcode is 0xA or 0xF, denoting nesting; this is then the "true" opcode (e.g. <copy> nested stuff </copy> this would be Copy)
               const char* string; // MenuQue and NorthernUI; we both allocate on the game's heap
            };
            struct Expression { // sizeof == 0x18
               static Expression* CreateOnGameHeap();
               //
               Expression* prev;    // 00
               Expression* next;    // 04
               Operand     operand; // 08 // first entry in any linked list is always ref; others are immediate
               UInt16      opcode;  // 0C // i.e. 7D1 "copy", 7D2 "add", etc // 0x65 == unknown
               bool        isString; // 0E // MenuQue extension; in the vanilla game, (opcode) is a dword; among other things, that means circular reference errors force this to false (0)
               UInt8       pad0F;
               Expression* refPrev = nullptr; // 10
               Expression* refNext = nullptr; // 14
               //
               ~Expression();
               //
               Expression() {};
               Expression(Value* v) {
                  CALL_MEMBER_FN(this, ConstructorA)(v);
               };
               //
               MEMBER_FN_PREFIX(Expression);
               DEFINE_MEMBER_FN(ConstructorA, Expression&, 0x005888C0, Value*);
               DEFINE_MEMBER_FN(Destructor,   void,        0x005888E0);
               //
               enum Type { // unofficial
                  kType_Immediate,
                  kType_Ref,
                  kType_Opcode,
                  kType_String, // MenuQue and NorthernUI
               };
               //
               void DebugLog(std::string* indent = nullptr) const;
               Expression* GetLastRef() {
                  auto n = this;
                  while (n->refNext)
                     n = n->refNext;
                  return n;
               };
               Expression* GetRootExpression() {
                  auto e = this;
                  while (e->refPrev)
                     e = e->refPrev;
                  while (e->prev)
                     e = e->prev;
                  return e;
               };
               Value* GetRootValue() {
                  return this->GetRootExpression()->operand.ref;
               };
               void RemoveFromRefs() {
                  auto p = this->refPrev;
                  auto n = this->refNext;
                  if (p) {
                     if (n) {
                        n->refPrev = p;
                        p->refNext = n;
                        this->refPrev = this->refNext = nullptr;
                     } else {
                        p->refNext = nullptr;
                        this->refPrev = nullptr;
                     }
                  } else {
                     if (n)
                        n->refPrev = nullptr;
                     this->refNext = nullptr;
                  }
               };

               inline Type GetType() const {
                  switch (this->opcode) {
                     case 0x65:
                        return kType_Ref;
                     case 0xA:
                     case 0xF:
                        return kType_Opcode;
                  }
                  if (this->isString)
                     return kType_String;
                  return kType_Immediate;
               }
               inline bool IsListHead() const {
                  return this->opcode == kTagID_OperatorNoOp;
               }
               inline bool WasNukedDueToCircularReference() const { // see TileValueFormsCircularReference at 0x0058BAD0
                  return this->opcode == 0;
               }

               // delete all contents of a container operator, including the container-start
               // only does anything if (this) is an 0xF container-end; does not delete (this) but does set it to a no-op
               void NukeContainerOperator();

               const char* GetStringValue() const;
            };

            bool IsNum()    { return bIsNum == 1; }
            bool IsString() { return bIsNum == 0; }

            Tile*     owner;  // 00
            float     num;    // 04
            BSStringT str;    // 08
            Expression* operators;    // 10 // operators for this value; first list item is always opcode 0x65, whose operand.ref == this value; if an operator in another trait wants to copy
            Expression* incomingRefs; // 14 // linked list refPrev/refNext; first list item belongs to this value, and is referenced by operators in other traits using refPrev
            UInt16    id;     // 18
            UInt8     bIsNum; // 1A 0 = string, 1 = number
            UInt8     pad1B;  // 1B // gets used by MenuQue; see MQ+1DA87 in v16b

            //
            // Re: operators:
            // 
            // A linked list of operators for this trait's value, using Expression::prev and 
            // Expression::next. The first list item is always opcode 0x65, and its operand 
            // is a ref pointer linking back to the Value that contains it. If an operator 
            // Expression in trait Y wants to copy the value of trait X, that Expression will 
            // have a refPrev set to the 0x65-opcode list head in X. The dummy list head 
            // exists because Expression operators cannot hold pointers to Values; there must 
            // be an Expression they can point to.
            //
            // Re: incomingRefs:
            // 
            // A linked list of operators pulling this trait's value, using Expression::refPrev 
            // and Expression::refNext. The first list item is always a dummy pointing back to 
            // the Value that owns it; these have identical data to the dummy list head in the 
            // "operators" list, but each list has a separate Expression as its dummy list head. 
            // Subsequent list items are Expressions from other traits' "operators" linked lists.
            // 
            // If several tiles pull from one trait's value, the relationship between those 
            // tiles looks like this:
            // 
            // A --- B --- D
            //   \
            //    \
            //     \ C --- E         / H
            //         \            /
            //          \          /
            //           \ F --- G --- I
            //                     \
            //                      \ 
            //                       \ J
            // 
            // Because each Expression represents a single operator, Expressions that pull from 
            // another tile contain the pulled value, unaltered. This is how refNext works: it's 
            // a linked list. There's no difference between the operator in J pulling from the 
            // trait in G, versus the operator in J somehow pulling from the operator in I that 
            // pulls from the trait in G; it's the same value being pulled; therefore, the link-
            // ed list contains:
            //
            // A - B - C - D - E - F - G - H - I - J
            // 
            // Another demonstration, using an example confirmed through memory inspection:
            // 
            //    <tile name="A">
            //       <user1>8.0</user1>
            //    </tile>
            //    <tile name="B">
            //       <user1><copy src="A" trait="user1" /></user1>
            //    </tile>
            //    <tile name="C">
            //       <user1><copy src="B" trait="user1" /></user1>
            //    </tile>
            // 
            // A'S USER1 TRAIT stores its const float value in its (num) field. However, it 
            // also has a single operator with opcode 0x65, whose operand is a ref-pointer that 
            // just points back to the user1 trait. Accordingly, that Expression is also the 
            // trait's sole incomingRefs pointer; for some reason, it doesn't have a refNext 
            // pointer.
            // 
            // B'S USER1 TRAIT stores its computed float value in its (num) field.
            // 
            // B's user1 trait has two operators. The first has opcode 0x65, and its operand is 
            // a ref-pointer that points back to its container (B's user1 trait). The second 
            // operator is a copy operator with no value, whose refPrev pointer points to the 
            // 0x65-operator on A's user1 trait.
            // 
            // B's user1 trait has two incomingRefs. The first is a second dummy list head, with 
            // opcode 0x65; it has identical data to the 0x65-operator but is a separate object. 
            // Its refNext pointer is the copy operator in C's user1 trait.
            // 
            // C'S USER1 TRAIT stores its computed float value in its (num) field.
            // 
            // C's user1 trait has two operators -- 0x65 and COPY, as with B's user1 trait. The 
            // copy operator's refPrev pointer points to the dummy list head in B!user1's 
            // incomingRefs, not the dummy list head in B!user1's operators.
            // 
            // C's user1 trait has one incomingRefs pointer: a dummy list head with identical 
            // data to the operators dummy list head (opcode 0x65; ref points back to C's user1 
            // trait).
            //

            MEMBER_FN_PREFIX(Value);
            DEFINE_MEMBER_FN(AppendConstOperator,            void,        0x0058CBE0, float value, UInt32 tagID);
            DEFINE_MEMBER_FN(AppendContainerOperator,        void,        0x0058CB70, UInt32 operatorTagID, UInt32 groupTagID); // groupTagID should be 0xA or 0xF for the start or end of the container operator
            DEFINE_MEMBER_FN(AppendSrcOperator,              void,        0x0058CC60, Tile* srcTile, UInt32 srcTrait, UInt32 tagID);
            DEFINE_MEMBER_FN(Destructor,                     void,        0x00589BB0);
            DEFINE_MEMBER_FN(DoActionEnumeration,            void,        0x0058BEE0, bool forceUpdateInboundTraits); // reprocesses this trait's operators and computes a new value
            DEFINE_MEMBER_FN(HasStringValue,                 bool,        0x00589770);
            DEFINE_MEMBER_FN(RemoveAllExpressionsAfterFirst, Expression*, 0x00588930); // SetFloatValue and SetStringValue use this to wipe any operator data
            DEFINE_MEMBER_FN(SetFloatValue,                  void,        0x0058CA00, float value);
            DEFINE_MEMBER_FN(SetStringValue,                 void,        0x0058CA50, const char* value);
            DEFINE_MEMBER_FN(UpdateInboundReferences,        void,        0x0058BDD0); // called at the end of DoActionEnumeration, to ensure that other traits pulling from this trait are updated too

            void AppendStringOperator(UInt32 operatorID, const char* str);
         };

         virtual void        Dispose(bool);
         virtual void        Unk_01(Tile* parent, const char* name, Tile* unkMaybeSibling); // initialize
         virtual NiNode*     CreateRenderedNode(); // 02 // sole caller is Tile::HandleChangeFlags. Tile::Unk_01 sets change flags to ensure this gets called ASAP.
         virtual UInt32      GetTypeID(); // 03
         virtual const char* GetType(); // 04
         virtual UInt32      UpdateField(UInt32 traitID, float floatValue, const char* strValue); // Allows tiles to respond to trait changes with special behavior, e.g. an image setting the size changeflag when width or height change. If it returns truthy, then HandleTraitChanged will not be called after the trait is altered.
         virtual void        Unk_06(void); // 06 // does something with tile's NiNode

         MEMBER_FN_PREFIX(Tile);
         DEFINE_MEMBER_FN(AnimateTrait,                   void,   0x00589980, UInt16 traitID, float currentValue, float stopValue, float stepValue);
         DEFINE_MEMBER_FN(AppendContainerOperatorToTrait, void,   0x0058CEF0, UInt32 myTraitID, UInt32 operatorTagID, UInt32 groupTagID); // calls Tile::Value::AppendContainerOperator
         DEFINE_MEMBER_FN(AppendSrcOperatorToTrait,       void,   0x0058CF10, UInt32 myTraitID, Tile* srcTile, UInt32 srcTrait, UInt32 tagID); // calls Tile::Value::AppendSrcOperator
         DEFINE_MEMBER_FN(AppendToTile,                void,   0x0058D1C0, Tile* parent, Tile* unkMaybeInsertAfter);
         DEFINE_MEMBER_FN(DiscardRenderedNode,         void,   0x00589AA0);
         DEFINE_MEMBER_FN(GetAbsoluteDepth,            float,  0x00588D90);
         DEFINE_MEMBER_FN(GetAbsoluteXCoordinate,      float,  0x00588C50);
         DEFINE_MEMBER_FN(GetAbsoluteYCoordinate,      float,  0x00588CF0);
         DEFINE_MEMBER_FN(GetAnimatingTraitEndValue,   float,  0x005894A0, UInt16 traitID); // If the trait is not animating, its current value is returned
         DEFINE_MEMBER_FN(GetContainingMenu,           Menu*,  0x005898F0);
         DEFINE_MEMBER_FN(GetFloatTraitValue,          float,  0x00588BD0, UInt16 traitID);
         DEFINE_MEMBER_FN(GetNearestNiNode,            NiNode*,     0x005894D0);
         DEFINE_MEMBER_FN(GetStringTraitValue,         const char*, 0x00588C10, UInt16 traitID);
         DEFINE_MEMBER_FN(GetTrait,                    Value*, 0x00588B50, UInt16 traitID);
         DEFINE_MEMBER_FN(GetOrCreateTrait,            Value*, 0x0058B220, UInt16 traitID);
         DEFINE_MEMBER_FN(HandleChangeFlags,           void,   0x0058E870);
         DEFINE_MEMBER_FN(HandleTraitChanged,          Tile*,  0x0058B2F0, UInt32 traitID, float fValue, const char* sValue); // updates change flags; returns self if the change is valid
         DEFINE_MEMBER_FN(NiNodeIsNotCulled,           bool,   0x005893B0); // also checks ancestor tiles' NiNodes
         DEFINE_MEMBER_FN(ReadXML,                     Tile*,  0x00590420, const char* xmlPath);
         DEFINE_MEMBER_FN(ResolveTraitReference,       Tile*,  0x0058E3B0, UInt32 traitID, UInt32* traitIDOut); // checks what tile-and-trait the (traitID) trait is referring to (via REF tags or entities); returns the target tile; writes the target ID to the second arg. Typically used to handle keyboard navigation.
         DEFINE_MEMBER_FN(Subroutine0058FBA0,          void,   0x0058FBA0, UInt32);
         DEFINE_MEMBER_FN(UpdateFloat,                 void,   0x0058CEB0, UInt32 traitID, float value);
         DEFINE_MEMBER_FN(UpdateFloatFromSInt32,       void,   0x00578ED0, UInt32 traitID, SInt32 value);
         DEFINE_MEMBER_FN(UpdateString,                void,   0x0058CED0, UInt32 traitID, const char* value); // If you pass in "", the value that gets applied is " ".
         DEFINE_MEMBER_FN(XScrollIntoView,             void,   0x0058E7D0); // Grabs the scrollbar specified by the parent tile's <xscroll><ref/></xscroll>; scrolls that scrollbar to the index specified by this tile's XSCROLL, by modifying the scrollbar's user5 (the trait specified by the REF operator is ignored).

         // These methods create child and descendant Tiles for this Tile, using parsed data. 
         // Specifically, these subroutines carry out Stage 2 of the parse process, first by 
         // creating the tiles (paying attention only to "tile start" and "tile end" tokens; 
         // and then by updating those created tiles with all trait information.
         //
         // This is used both for parsing menus from XML, and for instantiating templated 
         // Tiles within a Menu that has templated content.
         //
         // Call CreateTemplatedChildren on this tile, and then call UpdateTemplatedChildren 
         // on CTC's return value.
         //
         DEFINE_MEMBER_FN(CreateTemplatedChildren, Tile*, 0x00590330, TileTemplate*); // returns last created tile; writes each to their template items
         DEFINE_MEMBER_FN(UpdateTemplatedChildren, void,  0x0058CF40, TileTemplate*); // call after CreateTemplatedChildren; call it on the created child

         void AppendToTile(::Tile* target); // 0058D1C0
         void GetAbsoluteCoordinates(float& outX, float& outY, float& outDepth);

         typedef NiTListBase<Tile>  RefList;
         typedef NiTListBase<Value> ValueList;

         Tile* GetRoot(void);

         bool GetFloatValue (UInt32 valueType, float* out);
         bool SetFloatValue (UInt32 valueType, float newValue);
         bool GetStringValue(UInt32 valueType, const char** out);
         bool SetStringValue(UInt32 valueType, const char* newValue);
         bool DeleteValue   (UInt32 valueType);
         void DeleteChildren();
         SInt32 GetFontIndex(); // zero-indexed, not one-indexed as in the trait; accounts for xxnFontPath

         void AppendStringOperatorToTrait(UInt32 traitID, UInt32 operatorID, const char* str);

         //	void* vtbl;	 // 00
         UInt8     unk04;     // 04 - 0 = base tile
         UInt8     unk05;     // 05 - looks like bool, possibly bModified? Seems only used for x, y, width, height changed
         UInt8     unk06;     // 06
         UInt8     pad07;     // 07
         BSStringT name;      // 08
         Tile*     parent;    // 10
         ValueList valueList; // 14
         NiNode*   renderedNode; // 24
         void*     unk28;     // 28
         UInt32    flags;     // 2C
         RefList   childList; // 30 // stored in REVERSE ORDER!!

         // Tile::unk28 is a linked list of these
         struct Struct00588A70 { // traits that are animating
            //
            // Constructor sets changeflag 0x80 on the tile it's created with.
            //
            Tile*  owner; // 00 // tile that owns me (and the list I am a part of)
            UInt32 trait; // 04 // trait ID
            float  value; // 08 // trait current value
            float  endValue; // 0C // animation ends when the trait has this value?
            float  stepValue; // 10 // animation changes the trait value by this much per frame?
            Struct00588A70* next;  // 14
            //
            // ...?

            MEMBER_FN_PREFIX(Struct00588A70);
            DEFINE_MEMBER_FN(DestroySelf, void, 0x005895E0); // deletes self
         };

         enum {
            //
            // Change flags are updated by Tile::HandleTraitChanged. Each flag corresponds to one or more 
            // traits, and some flags or traits are only valid for specific tile types.
            //
            // The "id" trait does not have a changeflag. Instead, the tile's containing menu is notified 
            // directly.
            //
            // The "depth3d" trait has no changeflag, but is handled within HandleTraitChanged if other 
            // positioning traits (e.g. x, y) have changed.
            //
            // A number of traits that I'd expect to use changeflags are yet to be determined:
            //    animation, justify, locus, repeathorizontal, repeatvertical, target, tile, x[button], 
            //    xdefault, xlist, xscroll, zoom
            //
            // Flag 002 seems to also be used when we need to trash a tile's node in response to clips 
            // having changed?
            //
            kTileFlag_ChangedCoords     = 0x001, // x, y, depth
            kTileFlag_ChangedText       = 0x002, // font, isHTML, pagenum, wraplimit, wraplines, wrapwidth; string, but only if set via the copy operator
            kTileFlag_ChangedVisible    = 0x004, // visible, listclip
            kTileFlag_ChangedColor      = 0x008, // alpha, red, green, blue
            kTileFlag_ChangedSize       = 0x010, // cropX, cropY, width, height
            kTileFlag_ChangedImage      = 0x020, // filename (image only); also set within HandleChangeFlags if Oblivion determines that an image's 3D should be recreated from scratch
            kTileFlag_Changed3DModel    = 0x040, // filename (3d only)
            kTileFlag_Changed_Unk080    = 0x080, // Tile::unk28 state?
            kTileFlag_ChangedClipwindow = 0x100, // clipwindow; also set if the size or position change while clipwindow is true
            kTileFlag_ChangedClips      = 0x200, // clips
            //
            // Old names:
            //
            kTileFlag_ChangedXY       = kTileFlag_ChangedCoords,
            kTileFlag_ChangedString   = kTileFlag_ChangedText,
            kTileFlag_ChangedFilename = kTileFlag_ChangedImage,
         };
   };
   static_assert(sizeof(Tile) >= 0x40, "RE::Tile is too small!");
   static_assert(sizeof(Tile) <= 0x40, "RE::Tile is too large!");

   DEFINE_SUBROUTINE_EXTERN(bool, TileValueFormsCircularReference, 0x0058BAD0, Tile::Value*);
   
   struct Struct0058CD30;
   struct TileTemplate { // sizeof == 0x1C
      //
      // This gets appended to a linked list using the function that we have labeled 
      // as Struct00575610::Unk20::Append. It is likely that that Unk20 struct is a 
      // simple "linked list entry" struct used in more places than just there; given 
      // that Struct00575610 is only used for font rendering and *this* struct is 
      // used for tiles.
      //
      BSStringT       name; // 00 // template name
      Struct0058CD30* owner = nullptr; // 08
      NiTListBase<Tile::TileTemplateItem> items; // 0C // NiTList<Tile::TileTemplateItem> // parsed data
      //
      TileTemplate(const char* name, Struct0058CD30* owner) { // constructor at 0x0058BCD0
         ThisStdCall(0x0058BCD0, this, name, owner);
         /*// Vanilla code:
         this->owner = owner;
         CALL_MEMBER_FN(&this->name, Replace_MinBufLen)(name, 0);
         //*/
      };
      ~TileTemplate() {
         ThisStdCall(0x005852C0, this);
      };
      //
      MEMBER_FN_PREFIX(TileTemplate);
      DEFINE_MEMBER_FN(Destructor,        void, 0x005852C0); // doesn't free memory; use FormHeap_Free after calling
      DEFINE_MEMBER_FN(AddTemplateItem,   void, 0x0058D2F0, UInt32 tagTypeID, const char* value, UInt32 lineNumber, Tile* typicallyNull);
      DEFINE_MEMBER_FN(CopyNameFromOther, void, 0x004FB4C0, TileTemplate* other);
      DEFINE_MEMBER_FN(DeleteAllItems ,   void, 0x0058BC20);
   };
   struct Struct0058CD30 { // sizeof == 0x14 // represents parsed data? or a template?
      TileTemplate* unk00 = new TileTemplate("main", this); // 00
      LinkedPointerList<TileTemplate*> unk04; // 04 // see RepairMenu code near 00585410, among other places
      TileTemplate* unk0C = nullptr; // 0C // template currently being parsed?
      UInt8  unk10 = 1;
      UInt8  unk11;
      UInt8  unk12;
      UInt8  unk13;
      //
      MEMBER_FN_PREFIX(Struct0058CD30);
      DEFINE_MEMBER_FN(AppendTileTemplate, TileTemplate*, 0x0058CE30, const char* name); // create and append template by name
      DEFINE_MEMBER_FN(GetTileTemplate,    TileTemplate*, 0x0058BC80, const char* name); // get template by name
   };
   struct TileParseState { // at 00B3AF10
      Tile::Value*    values[0x64]; // 0000 // array used to check for circular references when operators are executed; memory address is 0x00B3AF10
      bool            lastParseProcessFailed; // 0190 // whether we've logged an error in the current parse process; memory address is 00B3B0A0
      bool            force3DForImages; // 0191 // forces HandleChangeFlags to generate rendered 3D for image tiles, even if they or their ancestor(s) are hidden. uses include forcing the HUD reticle to generate 3D even though it's loaded before any UI is displayed
      bool            tileStillOpen;   // 0192 // managed by UpdateTemplatedChildren; if this is true after that runs, then one or more tiles were left unterminated
      UInt8           pad0193;
      UInt32          unk0194;
      Struct0058CD30* unk0198; // 0198 // memory address is 00B3B0A8
      UInt32          valuesCount; // 019C // counter; memory address is 00B3B0AC

      static TileParseState* GetInstance() {
         return g_tileParseState;
      };
      static void Initialize() {
         auto i = GetInstance();
         memset(i->values, 0, sizeof(i->values));
         i->valuesCount = 0;
      };
   };

   class Tile3D : public Tile {
      public:
         Tile3D();
         ~Tile3D();

         enum {
            kID = 0x0388
         };

         NiControllerManager*  unk40; // 40
         NiControllerSequence* unk44; // 44 // I think this gets reset to nullptr when the animation finishes? Some menus act when it's nullptr.
         BSStringT animationName; // 48
         BSStringT filename; // 50
         float     unk58; // 58 - initialized to -1 // gets passed to RefreshAnimation // LockPickMenu sets this, on its tumblers, to their heightOffsets; does that influence anim state? can't view the NIF yet

         MEMBER_FN_PREFIX(Tile3D);
         DEFINE_MEMBER_FN(RefreshAnimation,     void, 0x00590D20, float arg1); // updates local animation state based on the "animation" trait
         DEFINE_MEMBER_FN(UpdateAnimationState, void, 0x00590DD9, const char* animationName); // updates unk40 and unk44 based on the argument

         inline bool IsAnimating() {
            return !!this->unk44;
         }
         void SetAnimationTimePercentage(float);
   };
   class TileRect : public Tile { // sizeof == 0x44
      public:
         TileRect();
         ~TileRect();

         enum { kID = 0x0385 };

         UInt32 unk40; // 40

         static TileRect* CreateOnGameHeap();
   };
   class TileImage : public Tile {
      public:
         TileImage();
         ~TileImage();

         enum { kID = 0x386 };

         float   unk40 = 1.0F;    // 40 // related to zoom somehow; gets set to 1.0 in many cases; in some cases, gets set to (*SInt32ScreenHeight / GetNormalizedScreenHeight()) with a min of 0.5 if no zoom or 0.8 if zoom
         NiNode* unk44 = nullptr; // 44 // can be nullptr
         bool    clipStateApplied = false; // 48 // updated by CreateRenderedNode; used by HandleChangeFlags to know whether to trash the node when the clips trait changes
         UInt8   pad49[3]; // 49

         // unk40 ==
         //  - 1.0F by default
         //  - 0.5F if the normalized screen height is at least twice the height of the real screen height
         //  - 0.8F if the normalized screen height is at least 1.25x the height of the real screen height AND the tile has a positive non-zero zoom trait
         //
         // filewidth == texture->GetWidth() * zoom / this->unk40; fileheight is similar
         //
         // Interesting note: if the tile clips, it generates a BSScissorTriShape for its node; otherwise 
         // it uses a NiTriShape.
   };
   class TileText : public Tile { // sizeof == 0x54
      public:
         TileText();
         ~TileText();

         enum { kID = 0x0387 };

         UInt32 unk40;
         float  unk44; // related to 3D x
         float  unk48; // related to 3D y
         float  unk4C; // related to 3D z
         UInt8  unk50;
         UInt8  pad51;
         UInt8  pad52;
         UInt8  pad53;
   };
   class TileMenu : public TileRect {
      public:
         TileMenu();
         ~TileMenu();

         enum { kID = 0x0389 };

         Menu* menu;	// 44
   };

   namespace KYO { // MenuQue
      class TileLink : public Tile { // sizeof == 0x44
         public:
            TileLink();
            ~TileLink();

            enum { kID = 0x038A };

            // virtual 0x01 just calls vanilla Unk_01
            // virtual 0x02 is a no-op returning nullptr
            // virtual 0x03 returns 0x38A
            // virtual 0x04 returns "LINK"
            // virtual 0x05 is a no-op returning 0
            // virtual 0x06 is a no-op

            Tile* source; // 40

            //
            // Method MQ_14860(Tile* Arg1) from v16b does:
            //
            //  - Abort if this->source == Arg1.
            //
            //  - Set this->source = Arg1.
            //
            //  - Replace every trait in this tile with a COPY operator pointed at the 
            //    same trait in (source). Reprocess trait values (DoActionEnumeration).
            //
            //  - Loop over all of this tile's children:
            //
            //     - Let ebx = the nearest targetable ancestor of Arg1 that has the same 
            //       name as the current child.
            //
            //     - Replace every trait in the child with a COPY operator pointed at the 
            //       same trait in ebx, if and only if ebx has that trait. Traits that do 
            //       not exist in ebx will be reset to the string " ". Reprocess trait 
            //       values (DoActionEnumeration).
            //
            // The method is called twice from MQ_14A90, which is called via the hook at 
            // InterfaceManager::Update+0x32B. MQ_14A90 is passed the (mouseTarget) -- 
            // the currently-hovered tile -- as its argument.
            //
            //  - Call one: if mouseTarget is not yet the active tile (i.e. we've just 
            //    started hovering over it), and if there is an activeMenu, then get the 
            //    KYO::TileLink* for the activeMenu and call its MQ_14860 method.
            //
            //     - If the activeMenu is the mouseTarget's containing menu, then pass 
            //       the mouseTarget as the argument; otherwise, pass nullptr.
            //
            //  - Call two: if mouseTarget has a containing menu, and if that containing 
            //    menu is not the activeMenu, then get the KYO::TileLink* for mouseTarget's 
            //    containing menu, and call its MQ_14860 method with mouseTarget as the 
            //    argument.
            //

            //
            // MenuQue's active(arg) selector does this:
            //
            //  - Get the ID of the menu that contains the tile using the selector.
            //
            //  - Get or create a KYO::TileLink* for the containing menu ID. These are 
            //    stored in a flat array; there is no support for non-vanilla menu IDs.
            //
            //  - Find a descendant of that tile whose name matches the selector argument.
            //
            //     - If no such descendant exists, create a KYO::TileLink* child and set 
            //       its name to the selector argument.
            //
            //  - Return the found descendant (or nullptr if operating on a non-vanilla 
            //    menu ID).
            //
            // As such, active(...) returns a KYO::TileLink* that copies from some other 
            // tile.
            //

            //
            // In light of the above information, we can draw a few conclusions:
            //
            //  - All of a KYO::TileLink's traits will be updated when mouseover state 
            //    changes in the menu to which the KYO::TileLink is associated.
            //
            //  - KYO::TileLinks are used to allow an operator to refer to different 
            //    tiles at run-time. The operator always refers to the same tile, but 
            //    hooks can live-update that tile to copy its traits from elsewhere.
            //
            // It also looks like MenuQue *does* clean up KYO::TileLinks when their 
            // associated menus are deleted, but I can't make heads or tails of the 
            // relevant code.
            //
      };
   };

   /*
   auto g_tempTraitList = (NiTArray<Tile::StringListElement>*) 0x00B13BC4; // used for underscore-prefixed trait definitions (why?)
   auto g_tagIDLists = (NiTListBase<Tile::StringListElement>*) 0x00B3B0B0; // used for XML entities, traits, etc. // actually NiTList<Tile::StringListElement>*

   NiTListBase<Tile::StringListElement>* getTraitList(char firstCharOfTrait) {
      firstCharOfTrait -= '@';
      if (firstCharOfTrait > 0x20)
         firstCharOfTrait -= 0x20; // normalize case
      if (firstCharOfTrait <= 0x1A) {
         //
         // ???
         //
      }
      return (&traitLists)[firstCharOfTrait]; // ??? i think ???
   }
   */

   extern NiTArray<Tile::StringListElement>* const g_tempTraitList;
   extern NiTListBase<Tile::StringListElement>* const g_tagIDLists; // array; if a tag ID's first char is alphabetical, then it's in g_tagIDLists[toupper(firstChar) - ('A' - 1)]; entities are in index 27; all others are in index 0

   void RegisterTrait(const char* trait, SInt32 code); // Registers the trait as a Tile::StringListElement
   //
   // Note that trait IDs are UInt16s. Trait names may also be capped to 0xFF characters; 
   // there is no hard limit in the registration function, but the string appears to be 
   // copied onto the stack in Tile::Value::DoActionEnumeration with that size limit.
   //
   // At least one subroutine checks a trait's code: if the code is lower than 0x0FA1 
   // (visible), then it is not treated as a valid trait. Subroutine in question is 
   // Tile::Value::Subroutine0058CC60(Tile*, SInt32 code, UInt32).
   //
   // The XML parser uses codes 0xA and 0xF as sentinel values when handling values that 
   // can be a trait. Decimal 907 and 
};