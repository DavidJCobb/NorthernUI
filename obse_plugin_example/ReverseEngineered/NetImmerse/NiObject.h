#pragma once
#include "obse/NiObjects.h"

class NiProperty;
namespace RE {
   class NiAVObject : public NiObjectNET { // sizeof == 0xAC
      public:
         NiAVObject();
         ~NiAVObject();

         virtual void			UpdatePropertiesAndControllers(float arg);	// calls Update on properties and controllers
         virtual void			Unk_14(void);
         virtual void			Unk_15(void);
         virtual NiObjectNET*	GetObject(const char* name); // 16
         virtual void			Unk_17(void);
         virtual void			Unk_18(float arg, bool updateProperties);	// if(updateProperties) UpdatePropertiesAndControllers(arg); Unk_1D(); Unk_1E();
         virtual void			Unk_19(float arg);			// update controllers, if kFlag_SelUpdatePropControllers update properties, if(kFlag_SelUpdateTransforms) { Unk_1D(); Unk_1E(); }
         virtual void			Unk_1A(float arg);			// update controllers, if kFlag_SelUpdatePropControllers update properties, if(kFlag_SelUpdateTransforms) { Unk_1D(); Unk_1E(); }
         virtual void			Unk_1B(void* arg);			// empty
         virtual void			Unk_1C(void* arg);			// empty
         virtual void			UpdateTransform(void); // 1D		// update world transform based on local transform and parent (also update collision)
         virtual void			Unk_1E(void);				// empty
         virtual void			Cull(NiCullingProcess* tgt); // 1F	// accumulate for drawing? NiNode iterates children
         virtual void			Unk_20(void* arg);			// get NiMaterialProperty, pass to arg if found

         enum : UInt16 {
            kFlag_AppCulled = 1 << 0,
            kFlag_SelUpdate = 1 << 1,
            kFlag_SelUpdateTransforms = 1 << 2,
            kFlag_SelUpdatePropControllers = 1 << 3,
            kFlag_SelUpdateRigid = 1 << 4,
         };

         UInt16						m_flags;				// 018
         UInt8						pad01A[2];				// 01A
         NiObject					* m_parent;				// 01C
         NiSphere					m_kWorldBound;			// 020
         NiMatrix33					m_localRotate;			// 030
         NiVector3					m_localTranslate;		// 054
         float						m_fLocalScale;			// 060
         NiMatrix33					m_worldRotate;			// 064
         NiVector3					m_worldTranslate;		// 088
         float						m_worldScale;			// 094
         NiTListBase <NiProperty>	m_propertyList;			// 098 // technically it's a NiTPointerList<NiPointer<NiProperty>>
         NiObject					* m_spCollisionObject;	// 0A8

         MEMBER_FN_PREFIX(NiAVObject);
         DEFINE_MEMBER_FN(AddNiProperty,           void,        0x00405680, NiProperty*);
         DEFINE_MEMBER_FN(GetPropertyByType,       NiProperty*, 0x00707530, UInt32 propertyType);
         DEFINE_MEMBER_FN(RemoveAndReturnProperty, void,        0x00708560, NiProperty** out, UInt32 propertyType);
         DEFINE_MEMBER_FN(Subroutine00707370,      void,        0x00707370, float, UInt32);
   };
   static_assert(sizeof(NiAVObject) == 0xAC, "NiAVObject is the wrong size!");
};