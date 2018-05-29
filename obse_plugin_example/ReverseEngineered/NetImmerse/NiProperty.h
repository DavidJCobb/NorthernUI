#pragma once
#include "obse/NiProperties.h"

class NiAlphaProperty;
class NiRendererSpecificProperty;
class NiShadeProperty;
class NiStencilProperty;
class NiVertexColorProperty;
class NiWireframeProperty;
class NiZBufferProperty;
namespace RE {
   class NiMaterialProperty;
   class NiTexturingProperty;

   class NiPropertyState : public NiRefObject { // sizeof == 0x30
      public:
         NiPropertyState();
         ~NiPropertyState();

         //
         // These are actually frequently iterated over as an array.
         //
         NiAlphaProperty*       alpha;       // 08
         void*                  unk0C;       // 0C // given that this class is just an alphabetized list of properties, it's likely Dither or Fog
         NiMaterialProperty*    material;    // 10
         NiRendererSpecificProperty* rendererSpecific; // 14
         NiShadeProperty*       shade;       // 18
         NiStencilProperty*     stencil;     // 1C
         NiTexturingProperty*   texturing;   // 20
         NiVertexColorProperty* vertexColor; // 24
         NiWireframeProperty*   wireframe;   // 28
         NiZBufferProperty*     zBuffer;     // 2C
   };

   class NiMaterialProperty : public NiProperty { // sizeof == 0x5C
      public:
         NiMaterialProperty();
         ~NiMaterialProperty();

         enum { kType = 0x02 };

         UInt32  unk18;    // 18 // values observed include: 0x0FB, 0x136, 0x243, 0x1783
         NiColor ambient;  // 1C // default to (0.5, 0.5, 0.5)?
         NiColor diffuse;  // 28 // default to (0.5, 0.5, 0.5)?
         NiColor specular; // 34 // default to (0.0, 0.0, 0.0)?
         NiColor emit;  // 40 // default to (1.0, 1.0, 1.0)? // when you set RGB traits on a TileImage, it appears that this gets modified to match
         float	  shine;	// 4C // default to 4.0?
         float   alpha;	// 50 // default to 1.0?
         UInt32  count; // 54
         void*   list;  // 58 // count is the number of items in this list. what is an item? dunno. MAY be four floats per. // pointer can be null even if count is non-zero
   };
   class NiTexturingProperty : public NiProperty {
      public:
         NiTexturingProperty();
         ~NiTexturingProperty();

         enum { kType = 0x06 };

         enum  {
            // ------------xxx-	apply mode
            kTexFlags_ApplyMode_Shift = 1,
            kTexFlags_ApplyMode_Mask = 0x0007,

            kTexFlags_ApplyMode_Replace = 0 << kTexFlags_ApplyMode_Shift, // very likely wrong; Replace should be 1, not 0, I think
            kTexFlags_ApplyMode_Decal = 1 << kTexFlags_ApplyMode_Shift,
            kTexFlags_ApplyMode_Modulate = 2 << kTexFlags_ApplyMode_Shift,
            kTexFlags_ApplyMode_Hilight = 3 << kTexFlags_ApplyMode_Shift,
            kTexFlags_ApplyMode_Hilight2 = 4 << kTexFlags_ApplyMode_Shift,
         };

         class Map { // sizeof == 0x10
            public:
               virtual void	Destructor(bool arg);
               virtual void	Load(NiStream * stream);
               virtual void	Save(NiStream * stream);

               enum {
                  // xxxx------------	clamp mode
                  // ----xxxx--------	filter mode
                  // --------xxxxxxxx	texture coord idx

                  kClampMode_WrapT = 1 << 12,
                  kClampMode_WrapS = 2 << 12,

                  kFilterMode_Nearest = 0x00 << 8,
                  kFilterMode_Bilerp = 0x01 << 8,
                  kFilterMode_Trilerp = 0x02 << 8,
                  kFilterMode_NearestMipNearest = 0x03 << 8,
                  kFilterMode_NearestMipLerp = 0x04 << 8,
                  kFilterMode_BilerpMipNearest = 0x05 << 8,
                  kFilterMode_Mask = 0x0F << 8,

                  kTexcoord_Mask = 0xFF << 0
               };

               UInt16	  mode = kClampMode_WrapT | kClampMode_WrapS | kFilterMode_Bilerp; // 04
               UInt8	     pad06[2]; // 06
               NiTexture* texture = nullptr; // 08
               UInt32	  unk0C = 0; // 0C
         };
         class BumpMap : public Map { // sizeof == 0x28
            float	m_fLumaScale;	// 10
            float	m_fLumaOffset;	// 14
            float	m_fBumpMat00;	// 18
            float	m_fBumpMat01;	// 1C
            float	m_fBumpMat10;	// 20
            float	m_fBumpMat11;	// 24
         };
         class ShaderMap : public Map { // sizeof == 0x14
            UInt32 m_uiID;	// 10
         };

         enum {
            kMap_Base = 0,
            kMap_Dark,
            kMap_Detail,
            kMap_Gloss,
            kMap_Glow,
            kMap_Bump,
         };

         UInt16 m_texFlags; // 018 // TileImage uses kTexFlags_ApplyMode_Modulate
         UInt8  pad01A[2];  // 01A
         NiTArray<Map*> m_maps; // 01C
         NiTArray<ShaderMap*>* m_shaderMaps; // 02C

         MEMBER_FN_PREFIX(NiTexturingProperty);
         DEFINE_MEMBER_FN(SetFirstMapClampMode,  void, 0x00405870, UInt32 mode); // creates the map if it does not exist
         DEFINE_MEMBER_FN(SetFirstMapFilterMode, void, 0x00405900, UInt32 mode); // creates the map if it does not exist // takes enum i.e. 0, 1, 2, 3, 4, 5
         DEFINE_MEMBER_FN(SetFirstMapTexture,    void, 0x004057B0, NiTexture*);  // creates the map if it does not exist
   };
};