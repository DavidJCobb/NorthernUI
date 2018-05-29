#pragma once
#include "ReverseEngineered/_BASE.h"
#include "obse/NiGeometry.h"
#include "obse/NiTypes.h"

typedef NiVector3 NiPoint3;

namespace RE {
   class NiGeometryData : public NiObject {
      public:
         NiGeometryData();
         ~NiGeometryData();

         virtual void	SetNumVertices(UInt32 arg);
         virtual UInt16	GetNumVertices(void);
         virtual void	UpdateNormals(void) = 0;

         enum {
            kFormat_TextureSetCountShift = 0,
            kFormat_TextureSetCountMask = 0x3F << kFormat_TextureSetCountShift,
            //
            kFormat_NBTShift = 12,
            kFormat_NBTMask = 0x0F << kFormat_NBTShift,
         };

         struct UV {
            float u; // X-coordinate within texture; percentage of overall width
            float v; // Y-coordinate within texture; percentage of overall height
         };

         UInt16        vertexCount;  // 08
         UInt16        uniqueID;     // 0A // serial number incremented in constructor
         NiSphere      bound;        // 0C // center and radius
         NiPoint3*     vertices;     // 1C
         NiPoint3*     normals;      // 20
         NiColorAlpha* vertexColors; // 24
         UV*           uvs;          // 28 // one per vertex; indices in synch
         UInt16        vectorFlags;  // 2C
         UInt16        dirtyFlags;    // 2E
         UInt8         keepFlags;     // 30
         UInt8         compressFlags; // 31
         UInt8         pad32[2]; // 32
         NiAdditionalGeometryData* m_spAdditionalGeomData; // 34
         UInt32 unk38;    // 38
         UInt8  unk3C;    // 3C
         UInt8  unk3D;    // 3D
         UInt8  pad3E[2]; // 3E
   };

   class NiTriBasedGeomData : public NiGeometryData {
      public:
         NiTriBasedGeomData();
         ~NiTriBasedGeomData();

         virtual void	SetNumTris(UInt32 arg);
         virtual UInt16	GetNumTris(void);
         virtual void	GetTriIndices(UInt32 idx, UInt16 * a, UInt16 * b, UInt16 * c);
         virtual void	GetStripData(UInt16 * numStrips, UInt16 ** stripLengths, UInt16 ** stripLists, UInt32 * numStripsAndTris);

         UInt16 m_usTriangles; // 40
         UInt8  pad42[2];      // 42
   };

   class NiTriShapeData : public NiTriBasedGeomData {
      public:
         NiTriShapeData();
         ~NiTriShapeData();

         struct Tri {
            UInt16 indices[3]; // indices of vertices
         };

         // tri list

         UInt32  triangleCount;	// 44
         Tri*    triangles;		// 48
         UInt32  unk4C;				// 4C
         UInt16  unk50;				// 50
         UInt8   pad52[2];			// 52
         UInt32  unk54;				// 54
   };
};