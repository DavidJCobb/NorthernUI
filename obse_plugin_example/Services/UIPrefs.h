#pragma once

class UIPrefManager {
   public:
      static UIPrefManager& GetInstance() {
         static UIPrefManager instance;
         return instance;
      };
};

void _RunPrefXMLParseTest();