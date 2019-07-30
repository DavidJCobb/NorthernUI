#pragma once

namespace RE {
   enum TagIDs {
      kEntityID_false   =  1,
      kEntityID_true    =  2,
      kEntityID_generic = -1,
      //
      // Xbox stuff:
      //
      kEntityID_xbox      = kEntityID_false,
      kEntityID_xboxhint  = kEntityID_xbox,
      kEntityID_xbuttona  = kEntityID_false,
      kEntityID_xbuttonb  = kEntityID_false,
      kEntityID_xbuttonlb = kEntityID_false,
      kEntityID_xbuttonls = kEntityID_false,
      kEntityID_xbuttonlt = kEntityID_false,
      kEntityID_xbuttonrb = kEntityID_false,
      kEntityID_xbuttonrs = kEntityID_false,
      kEntityID_xbuttonrt = kEntityID_false,
      kEntityID_xbuttonx  = kEntityID_false,
      kEntityID_xbuttony  = kEntityID_false,
      kEntityID_xenon     = kEntityID_xbox,
      //
      kEntityID_left   = 1, // text alignment
      kEntityID_center = 2, // text alignment
      kEntityID_right  = 4, // text alignment
      //
      // Parser:
      //
      kTagID_Invalid       = -1,
      kTagID_StartTag      = 0x0A, // any start tag // converted to something else in parse Stage 1.5
      kTagID_MQTextContent = 0x0D, // seen in MenuQue; possibly tag text content
      kTagID_EndTag        = 0x0F, // any end   tag // converted to something else in parse Stage 1.5
      kTagID_ContainerOperatorStart = 0x14, // start of non-const, non-src operator
      kTagID_ContainerOperatorEnd   = 0x19, // end   of non-const, non-src operator
      kTagID_NonConstTraitStart = 0x1E, // start of non-const trait // during Stage 1.5 parsing, some of these are converted to ConstTraits.
      kTagID_NonConstTraitEnd   = 0x23, // end   of non-const trait
      kTagID_TileStart       = 0x28, // start of a tile: result and tagType are tag ID, string is name attr
      kTagID_TileMQAttribute = 0x2A, // MenuQue extension; most likely "any attribute"
      kTagID_TileEnd         = 0x2D, // end of a tile
      kTagID_ConstTrait      = 0x32, // trait with const value
      kTagID_ConstOperator   = 0x37, // operator with const value
      kTagID_SrcOperator     = 0x3C, // operator with src: result is operator ID; string is operator SRC; tagType is containing trait's ID
      kTagID_OperatorNoOp    = 0x65, // used for no-op Expressions (which can retain semantic data, i.e. operand.ref)
      //
      kEntityID_click_past    = 0x065, // menu stackingtype
      kEntityID_no_click_past = 0x066, // menu stackingtype
      kEntityID_mixed_Menu    = 0x067, // menu stackingtype
      kEntityID_prev  = 0x068, // listindex
      kEntityID_next  = 0x069, // listindex
      kEntityID_first = 0x06A, // listindex
      kEntityID_last  = 0x06B, // listindex
      kEntityID_xlist = 0x06C,
      kEntityID_xitem = 0x06D,
      kEntityID_MenuQue_Scroll         = 0x0F9,
      kEntityID_MenuQue_FocusBox       = 0x0FA,
      kEntityID_MenuQue_TextEdit       = 0x0FB,
      kEntityID_MenuQue_DragRect       = 0x0FC,
      kEntityID_MenuQue_DragHorizontal = 0x0FD,
      kEntityID_MenuQue_DragVertical   = 0x0FE,
      kEntityID_scale = 0x0FF,
      //
      // Tile types (decimal > 900):
      //
      kTagID_rect     = 0x385,
      kTagID_image    = 0x386,
      kTagID_text     = 0x387,
      kTagID_3d       = 0x388,   kTagID_nif = kTagID_3d,
      kTagID_menu     = 0x389,
      kTagID_MenuQue_link = 0x38A, // KYO::TileLink* // probably can't create these in XML, huh?
      kTagID_window   = 0x38B,
      //
      kTagID_template = 0x3E7, // decimal 999
      //
      // Menu IDs (decimal > 1000):
      //
      kMenuID_MessageMenu   = 0x3E9,
      kMenuID_InventoryMenu = 0x3EA,
      kMenuID_StatsMenu     = 0x3EB,
      kMenuID_HUDMainMenu   = 0x3EC,
      kMenuID_HUDInfoMenu   = 0x3ED,
      kEntityID_HUDReticle  = 0x3EE,
      kMenuID_LoadingMenu   = 0x3EF,
      kMenuID_ContainerMenu = 0x3F0,
      kMenuID_DialogMenu    = 0x3F1,
      kMenuID_HUDSubtitleMenu = 0x3F2,
      kMenuID_GenericMenu   = 0x3F3,
      kMenuID_SleepWaitMenu = 0x3F4,
      kMenuID_PauseMenu     = 0x3F5,
      kMenuID_LockPickMenu  = 0x3F6,
      kMenuID_OptionsMenu   = 0x3F7,
      kMenuID_QuantityMenu  = 0x3F8,
      kMenuID_AudioMenu     = 0x3F9,
      kMenuID_VideoMenu     = 0x3FA,
      kMenuID_VideoDisplayMenu = 0x3FB,
      kMenuID_GameplayMenu   = 0x3FC,
      kMenuID_ControlsMenu   = 0x3FD,
      kMenuID_MagicMenu      = 0x3FE,
      kMenuID_MapMenu        = 0x3FF,
      kMenuID_MagicPopupMenu = 0x400,
      kMenuID_NegotiateMenu  = 0x401,
      kMenuID_BookMenu       = 0x402,
      kMenuID_LevelUpMenu    = 0x403,
      kMenuID_TrainingMenu   = 0x404,
      kMenuID_BirthSignMenu  = 0x405,
      kMenuID_ClassMenu      = 0x406,
      kMenuID_SkillsMenu     = 0x408,
      kMenuID_PersuasionMenu = 0x40A,
      kMenuID_RepairMenu     = 0x40B,
      kMenuID_RaceSexMenu    = 0x40C,
      kMenuID_SpellPurchaseMenu = 0x40D,
      kMenuID_LoadMenu       = 0x40E,
      kMenuID_SaveMenu       = 0x40F,
      kMenuID_AlchemyMenu    = 0x410,
      kMenuID_SpellmakingMenu = 0x411,
      kMenuID_EnchantmentMenu = 0x412,
      kMenuID_EffectSettingMenu = 0x413,
      kMenuID_MainMenu       = 0x414,
      kMenuID_BreathMenu     = 0x415,
      kMenuID_QuickKeysMenu  = 0x416,
      kMenuID_CreditsMenu    = 0x417,
      kMenuID_SigilStoneMenu = 0x418,
      kMenuID_RechargeMenu   = 0x419,
      kMenuID_TextEditMenu   = 0x41B,
      //
      // Operators (decimal > 2000):
      //
      kTagID_copy = 0x7D1,
      kTagID_add  = 0x7D2,
      kTagID_sub  = 0x7D3,
      kTagID_mul  = 0x7D4,   kTagID_mult = kTagID_mul,
      kTagID_div  = 0x7D5,
      kTagID_rand = 0x7D6,
      kTagID_user = 0x7D7,
      kTagID_gt   = 0x7D8,
      kTagID_gte  = 0x7D9,
      kTagID_eq   = 0x7DA,
      kTagID_lte  = 0x7DB,
      kTagID_lt   = 0x7DC,
      kTagID_min  = 0x7DD,
      kTagID_max  = 0x7DE,
      kTagID_and  = 0x7DF,
      kTagID_or   = 0x7E0,
      kTagID_neq  = 0x7E1,
      kTagID_mod  = 0x7E2,
      kTagID_floor = 0x7E3,   kTagID_trunc = kTagID_floor,
      kTagID_abs   = 0x7E4,
      kTagID_onlyif    = 0x7E5,
      kTagID_onlyifnot = 0x7E6,
      kTagID_ln   = 0x7E7,
      kTagID_log  = 0x7E8,
      kTagID_ceil = 0x7E9,
      kTagID_not  = 0x7EA,
      kTagID_ref  = 0x7EB,
      kTagID_MenuQue_append   = 0x7EC, // MenuQue string operator
      kTagID_MenuQue_prepend  = 0x7ED, // MenuQue string operator
      kTagID_MenuQue_cmp      = 0x7EE, // MenuQue string operator
      kTagID_MenuQue_incl     = 0x7EF, // MenuQue string operator
      kTagID_MenuQue_tostring = 0x7F0, // MenuQue string operator
      kTagID_MenuQue_length   = 0x7F1, // MenuQue string operator
      //
      // Attributes (decimal > 3000):
      //
      kAttrID_value = 0xBB9,
      kAttrID_name  = 0xBBA,
      kAttrID_src   = 0xBBB,
      kAttrID_trait = 0xBBC,
      kAttrID_MenuQue_dest = 0xBBD,
      //
      // Traits (decimal > 4000):
      //
      kTagID_visible      = 0xFA1,
      kTagID_class        = 0xFA2,
      kTagID_listclip     = 0xFA3,
      kTagID_clipwindow   = 0xFA4,
      kTagID_stackingtype = 0xFA5,
      kTagID_locus        = 0xFA6,
      kTagID_alpha        = 0xFA7,
      kTagID_id           = 0xFA8,
      kTagID_disablefade  = 0xFA9,
      kTagID_listindex    = 0xFAA,
      kTagID_depth        = 0xFAB,
      kTagID_y            = 0xFAC,
      kTagID_x            = 0xFAD,
      kTagID_user0  = 0xFAE,
      kTagID_user1  = 0xFAF,
      kTagID_user2  = 0xFB0,
      kTagID_user3  = 0xFB1,
      kTagID_user4  = 0xFB2,
      kTagID_user5  = 0xFB3,
      kTagID_user6  = 0xFB4,
      kTagID_user7  = 0xFB5,
      kTagID_user8  = 0xFB6,
      kTagID_user9  = 0xFB7,
      kTagID_user10 = 0xFB8,
      kTagID_user11 = 0xFB9,
      kTagID_user12 = 0xFBA,
      kTagID_user13 = 0xFBB,
      kTagID_user14 = 0xFBC,
      kTagID_user15 = 0xFBD,
      kTagID_user16 = 0xFBE,
      kTagID_user17 = 0xFBF,
      kTagID_user18 = 0xFC0,
      kTagID_user19 = 0xFC1,
      kTagID_user20 = 0xFC2,
      kTagID_user21 = 0xFC3,
      kTagID_user22 = 0xFC4,
      kTagID_user23 = 0xFC5,
      kTagID_user24 = 0xFC6,
      kTagID_user25 = 0xFC7,
      kTagID_clips  = 0xFC8,
      kTagID_target = 0xFC9,
      kTagID_height = 0xFCA,
      kTagID_width  = 0xFCB,
      kTagID_red    = 0xFCC,
      kTagID_green  = 0xFCD,
      kTagID_blue   = 0xFCE,
      kTagID_tile   = 0xFCF,
      kTagID_child_count = 0xFD0,
      kTagID_childcount  = kTagID_child_count,
      kTagID_justify = 0xFD1,
      kTagID_zoom    = 0xFD2,
      kTagID_font    = 0xFD3,
      kTagID_wrapwidth = 0xFD4,
      kTagID_wraplimit = 0xFD5,
      kTagID_wraplines = 0xFD6,
      kTagID_pagenum   = 0xFD7,
      kTagID_ishtml    = 0xFD8,
      kTagID_cropoffsety = 0xFD9,   kTagID_cropy = kTagID_cropoffsety,
      kTagID_cropoffsetx = 0xFDA,   kTagID_cropx = kTagID_cropoffsetx,
      kTagID_menufade    = 0xFDB,
      kTagID_explorefade = 0xFDC,
      kTagID_mouseover   = 0xFDD,
      kTagID_string      = 0xFDE,
      kTagID_shiftclicked = 0xFDF,
      kTagID_focusinset   = 0xFE0,
      kTagID_clicked      = 0xFE1,
      kTagID_clickcountbefore = 0xFE2,
      kTagID_clickcountafter  = 0xFE3,
      kTagID_clickedfunc = 0xFE4,
      kTagID_clicksound  = 0xFE5,
      kTagID_filename   = 0xFE6,
      kTagID_filewidth  = 0xFE7,
      kTagID_fileheight = 0xFE8,
      kTagID_repeatvertical   = 0xFE9,
      kTagID_repeathorizontal = 0xFEA,
      kTagID_returnvalue = 0xFEB,
      kTagID_animation   = 0xFEC,
      kTagID_depth3d     = 0xFED,
      kTagID_linecount   = 0xFEE,
      kTagID_pagecount   = 0xFEF,
      kTagID_xdefault    = 0xFF0,
      kTagID_xup     = 0xFF1,
      kTagID_xdown   = 0xFF2,
      kTagID_xleft   = 0xFF3,
      kTagID_xright  = 0xFF4,
      kTagID_xscroll = 0xFF5,
      kTagID_xlist   = 0xFF6,
      kTagID_xbuttona  = 0xFF7,
      kTagID_xbuttonb  = 0xFF8,
      kTagID_xbuttonx  = 0xFF9,
      kTagID_xbuttony  = 0xFFA,
      kTagID_xbuttonlt = 0xFFB,
      kTagID_xbuttonrt = 0xFFC,
      kTagID_xbuttonlb = 0xFFD,
      kTagID_xbuttonrb = 0xFFE,
      kTagID_xbuttonstart = 0x1001,
      kTagID_MenuQue_focus     = 0x1002, // MenuQue trait (boolean)
      kTagID_MenuQue_focussize = 0x1003, // MenuQue trait (float)
      kTagID_MenuQue_rotation  = 0x1005, // MenuQue trait (float)
      kTagID_MenuQue_rotationx = 0x1006, kTagID_MenuQue_rotx = 0x1006, // MenuQue trait (float)
      kTagID_MenuQue_rotationy = 0x1007, kTagID_MenuQue_roty = 0x1007, // MenuQue trait (float)
      kTagID_MenuQue_rotationp = 0x1008, kTagID_MenuQue_rotp = 0x1008, // MenuQue trait (float)
      kTagID_MenuQue_locusx        = 0x1009, // MenuQue trait
      kTagID_MenuQue_locusy        = 0x100A, // MenuQue trait
      kTagID_MenuQue_locusdepth    = 0x100B, // MenuQue trait
      kTagID_MenuQue_locusrotation = 0x100C, // MenuQue trait
      kTagID_MenuQue_ratio    = 0x100D, // MenuQue trait
      kTagID_MenuQue_animtime = 0x100E, // MenuQue trait
      kTagID_parent  = 0x1389,
      kTagID_me      = 0x138A,
      kTagID_MenuQue_active = 0x138B, // MenuQue selector
      kTagID_sibling = 0x138C,
      kTagID_child   = 0x138D,
      kTagID_screen  = 0x138E,
      kTagID_strings = 0x138F,
      //
      // Menu stackingtypes:
      //
      kTagID_unknown_menu_trait = 0x1772, // not registered for XML use; ID is used directly by vanilla code
      kTagID_unknown_stack_type = 0x1776, // checked for by pretty much all menu-opening functions, but never registered
      kEntityID_does_not_stack  = 0x1778, // dec 6008
      //
      // Misc:
      //
      kTagID_tempTraitStart = 0x2710, // dec 10000 // IDs >= this number are underscore-prefixed traits
   };
};