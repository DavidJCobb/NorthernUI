#pragma once
#include "obse/NiNodes.h"
#include "obse/NiObjects.h"

struct DDS_PIXELFORMAT;
class NiDX9Renderer;
namespace RE {
   class NiTexture : public NiObjectNET { // sizeof == 0x30
      public:
         NiTexture();
         ~NiTexture();

         virtual UInt32	GetWidth(void) = 0;
         virtual UInt32	GetHeight(void) = 0;

         // 8
         struct Str028 {
            UInt32	unk0;
            UInt32	unk4;
         };

         class RendererData {
            public:
               virtual void	Dispose(bool) = 0;
               virtual UInt32	GetWidth(void) = 0;
               virtual UInt32	GetHeight(void) = 0;
               virtual TextureFormat* GetTextureFormat(void) = 0; // returns &this->unk0C;
			   virtual void*  Unk_04() = 0; // NiDX9TextureData: returns &this->unk5C;
			   virtual void*  Unk_05() = 0; // NiDX9TextureData: returns &this->unk50;
			   virtual UInt32 Unk_06();
			   virtual void*  Unk_07(); // NiDX9SourceTextureData: returns this; NiDX9TextureData: returns nullptr
			   virtual UInt32 Unk_08();
			   virtual bool   Unk_09(UInt32);
         };

         enum {
            kPixelLayout_Palette8BPP = 0,
            kPixelLayout_Raw16BPP,
            kPixelLayout_Raw32BPP,
            kPixelLayout_Compressed,
            kPixelLayout_Bumpmap,
            kPixelLayout_Palette4BPP,
            kPixelLayout_Default,
         };
         enum {
            kMipMap_Disabled = 0,
            kMipMap_Enabled,
            kMipMap_Default,
         };
         enum {
            kAlpha_None = 0,
            kAlpha_Binary,	// 1bpp
            kAlpha_Smooth,	// 8bpp
            kAlpha_Default,
         };

         UInt32        pixelLayout;  // 018
         UInt32        alphaFormat;  // 01C
         UInt32        mipmapFormat; // 020
         RendererData* rendererData; // 024
         NiTexture*    nextTex; // 028 // linked list of all loaded textures
         NiTexture*    prevTex; // 02C
   };
   class NiDX9TextureData : public NiTexture::RendererData {
      public:
         NiTexture*     owner; // 04
         NiDX9Renderer* unk08; // 08
         TextureFormat  unk0C; // 0C
		 void*          unk50  = nullptr; // 50 // pointer to a class defined in D3D.DLL; is NOT a D3D_PIXELFORMAT; is NOT an interface
         UInt32         width  = 0; // 54
         UInt32         height = 0; // 58
         UInt32         unk5C  = 0; // 5C // actually an int

         MEMBER_FN_PREFIX(NiDX9TextureData);
         DEFINE_MEMBER_FN(Constructor, NiDX9TextureData*, 0x007738E0, UInt32 unk04, UInt32 unk08);
   };
   class NiDX9DynamicTextureData : public NiDX9TextureData { // sizeof == 0x68
      public:
         UInt32 unk60 = 0;
         UInt8  unk64 = 0;
         UInt8  pad65[3];

         MEMBER_FN_PREFIX(NiDX9RenderedTextureData);
         DEFINE_MEMBER_FN(Destructor, void, 0x007796B0);
   };
   class NiDX9RenderedTextureData : public NiDX9TextureData { // sizeof == 0x64
      public:
         UInt32 unk60 = 0;

         MEMBER_FN_PREFIX(NiDX9RenderedTextureData);
         DEFINE_MEMBER_FN(Constructor, NiDX9RenderedTextureData*, 0x007616A0, UInt32 unk04, UInt32 unk08);
         DEFINE_MEMBER_FN(Destructor, void, 0x00761A30);
   };
   class NiDX9SourceTextureData : public NiDX9TextureData {
      public:
		 virtual void  Unk_0A();
		 virtual void* Unk_0B(); // NiDX9SourceTextureData: returns &this->unk6C;

         UInt32 unk60 = 0;
         UInt8  unk64 = 0;
         UInt8  unk65 = 0;
         UInt16 pad66;
         UInt32 unk68 = 0;
         UInt32 unk6C = 0;
         UInt32 unk70 = 0;
         UInt32 unk74 = 0;
         UInt32 unk78 = 0;

         MEMBER_FN_PREFIX(NiDX9SourceTextureData);
         DEFINE_MEMBER_FN(Constructor, NiDX9SourceTextureData*, 0x00760BF0, UInt32 unk04, UInt32 unk08);
   };

   // NiDX9Direct3DTexture - not referenced

   // 048
   class NiSourceTexture : public NiTexture {
      public:
		  NiSourceTexture();
		  ~NiSourceTexture();

		  virtual void	Unk_15(void);
		  virtual void	FreePixelData(void);
		  virtual bool	Unk_17(void);

		  UInt8 unk030;    // 030 - is static?
		  UInt8 unk031[3]; // 031
		  const char* unk034;       // 034 // redundant copy of fileName?
		  const char* fileName;     // 038
		  NiObject*   pixelData;    // 03C // NiPixelData
		  UInt8 loadDirectToRender; // 040
		  UInt8 persistRenderData;  // 041
		  UInt8 pad042[2]; // 042
		  void* unk044;    // 044
   };
};