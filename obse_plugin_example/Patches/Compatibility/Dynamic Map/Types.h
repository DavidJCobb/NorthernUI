#pragma once

namespace _dme {
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
   struct Undifferentiated {
      std::string name; // key: "mod"
      std::string name_alternate; // key: "or_mod"
      std::string image; // includes the DDS extension; can be specified as "img"; otherwise, it's the name, minus the ESP/ESM file extension
      UInt32 formID = 0; // key: "fid"; defined as a quoted string
      float x;
      float y;
      float width;
      float height;
      float zoom = 1.0F;
      float xMin; // northwest X
      float yMin; // northwest Y
      float xMax; // southeast X
      float yMax; // southeast Y
      //
      void SetField(std::string key, std::string lineRemainder);
      void PrepField(std::string& key, float*& outFloat, UInt32*& outFormID, std::string*& outStr);
   };
   struct Overlay {
      std::string name;
      std::string name_alternate; // key: "or_mod"
      std::string image; // includes the DDS extension; can be specified as "img"; otherwise, it's the name, minus the ESP/ESM file extension
      float x;
      float y;
      float width;
      float height;
      float zoom = 1.0F;
      //
      Overlay() {};
      Overlay(Undifferentiated u) {
         std::swap(this->name, u.name);
         std::swap(this->name_alternate, u.name_alternate);
         std::swap(this->image, u.image);
         this->x = u.x;
         this->y = u.y;
         this->width = u.width;
         this->height = u.height;
         this->zoom = u.zoom;
      };
   };
   struct Worldspace {
      std::string name;
      UInt32 formID = 0;
      float width;
      float height;
      float xMin; // northwest X
      float yMin; // northwest Y
      float xMax; // southeast X
      float yMax; // southeast Y
      //
      std::vector<Overlay> overlays;
      //
      // Note: The tnoDM quest also has a "ws_additions" member, which is an array of 
      // maps (key: string; value: float). Each map corresponds to a worldspace, and has 
      // the keys "North", "East", "South", and "West", indicating adjustments for the 
      // overlay-container's dimensions.
      //
      // This functionality was never used in Dynamic Map's overlays, which is a double-
      // edged sword. We can't see how these "wordspace addition" dimensions are defined, 
      // so we can't write code to parse them; however, since they're not used, we 
      // shouldn't need to. Someone else would have to reverse-engineer the functionality 
      // AND find a use for it AND get maintenance permissions from Dynamic Map's author 
      // in order for this to become a problem for us... and I just don't see that 
      // happening -- especially since the feature shouldn't even be necessary anyway. 
      // This feature lets you resize and reposition the canvas that overlays are render-
      // ed to, but there's no need; you can just give an overlay out-of-bounds coordinates.
      //
      // Regardless, I'm describing the feature here for documentation's sake. The values 
      // are defined in the OBSE script dmMain and used in dmAdjustDimensions.
      //
      Worldspace() {};
      Worldspace(Undifferentiated u) {
         std::swap(this->name, u.name);
         this->formID = u.formID;
         this->width = u.width;
         this->height = u.height;
         this->xMin = u.xMin;
         this->yMin = u.yMin;
         this->xMax = u.xMax;
         this->yMax = u.yMax;
      };
   };
}