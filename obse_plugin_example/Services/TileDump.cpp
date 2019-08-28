#include "TileDump.h"

void _traceOperator(RE::Tile::Value::Expression* op, const char* indent) {
   auto name = RE::TagIDToName(op->opcode);
   if (!name)
      name = "[UNKNOWN]";
   switch (op->GetType()) {
      case RE::Tile::Value::Expression::kType_Immediate:
      case RE::Tile::Value::Expression::kType_Ref:
         if (op->refPrev) {
            auto p = op->refPrev;
            while (p->prev)
               p = p->prev;
            if (p->opcode == 0x65) {
               auto source = p->operand.ref;
               if (source) {
                  auto tile = source->owner->name.m_data;
                  auto trait = RE::TagIDToName(source->id);
                  if (trait)
                     _MESSAGE("%s - %08X <%s src=\"%s\" trait=\"%s\" /> == %f", indent, op, name, tile, trait, op->operand.immediate);
                  else
                     _MESSAGE("%s - %08X <%s src=\"%s\" trait=\"0x%08X\" /> == %f", indent, op, name, tile, source->id, op->operand.immediate);
               } else
                  _MESSAGE("%s - %08X <%s src=\"?\" trait=\"?\" /> == %f", indent, op, name, op->operand.immediate);
            } else {
               _MESSAGE("%s - %08X <%s src=\"?\" trait=\"?\" /> == %f", indent, op, name, op->operand.immediate);
            }
         } else {
            _MESSAGE("%s - %08X <%s>%f</%s>", indent, op, name, op->operand.immediate, name);
         }
         break;
   }
   return;
   //
   // old code:
   //
   _MESSAGE("%s - (Expression*)%08X: opcode %03X (%s)", indent, op, op->opcode, RE::TagIDToName(op->opcode));
   if (op->GetType() == op->kType_Immediate) {
      _MESSAGE("%s    - const %f", indent, op->operand.immediate);
      return;
   }
   auto root = op->GetRootExpression();
   if (root && root->IsListHead()) {
      auto value = root->operand.ref;
      if (value) {
         auto owner = value->owner;
         if (owner) {
            _MESSAGE("%s    - Root: Tile \"%s\" trait %s", indent, owner->name.m_data, RE::TagIDToName(value->id));
            return;
         }
      }
   }
   _MESSAGE("%s    - Unable to identify source.");
}
void _tryPrintSources(RE::Tile::Value* source) {
   auto e = source->operators;
   if (!e) {
      return;
   }
   _MESSAGE("       - Operators:");
   UInt32 depth = 0;
   for (; e; e = e->next) {
      auto op = e->opcode;
      switch (op) {
         case 0xA:
            depth++;
            _MESSAGE("          - %08X <%s>...", e, RE::TagIDToName(e->operand.opcode));
            break;
         case 0xF:
            depth--;
            _MESSAGE("          - %08X ...</%s>", e, RE::TagIDToName(e->operand.opcode));
            break;
         case 0x65: // pointer
            _MESSAGE("          - List head at %08X.", e);
            break;
         default:
            _traceOperator(e, "         ");
            break;
      }
   }
   _MESSAGE("       - End of operator list.");
};
void TileDump(RE::Tile* tile) {
   _MESSAGE(" - Tile address is 0x%08X.", tile);
   {  // Log traits.
      _MESSAGE(" - Traits:");
      auto node = tile->valueList.start;
      if (node) {
         do {
            auto value = node->data;
            auto name = RE::TagIDToName(value->id);
            if (!name)
               name = "?????";
            if (value->bIsNum)
               _MESSAGE("    - NUMBER %s: %f", name, value->num);
            else
               _MESSAGE("    - STRING %s: %s", name, value->str.m_data);
            _tryPrintSources(value);

         } while (node = node->next);
      } else
         _MESSAGE("    - <None>");
   }
   {  // Log children.
      _MESSAGE(" - Children:");
      auto node = tile->childList.end;
      if (node) {
         do {
            auto child = node->data;
            if (!child)
               continue;
            _MESSAGE("    - %s", child->name.m_data);
         } while (node = node->prev);
      } else
         _MESSAGE("    - <None>");
   }
   _MESSAGE(" - Tile logged.");
}

void TileCyclicalReferenceChecker::Log(const char* format, ...) {
   if (this->lineCount >= ce_safeMaxLines)
      return;
   va_list args;
   va_start(args, format);
   std::string formatPrefix;
   formatPrefix += format;
   {  // Some of Bethesda's format messages are wrong.
      std::size_t found = formatPrefix.find("%S", 20);
      while (found != std::string::npos) {
         formatPrefix[found + 1] = 's';
         found = formatPrefix.find("%S", found);
      }
   }
   gLog.Log(IDebugLog::kLevel_Message, formatPrefix.c_str(), args);
   va_end(args);
   this->lineCount++;
   if (this->lineCount >= ce_safeMaxLines) {
      _MESSAGE("-- HIT MAX LOG LINE THRESHOLD FOR THIS CHECK; WILL ABORT SOON --");
   }
}
bool TileCyclicalReferenceChecker::IsLoggingSafe() const {
   return this->lineCount < ce_safeMaxLines;
}
RE::Tile::Value* TileCyclicalReferenceChecker::FindCyclical(RE::Tile::Value* base, bool isRecursing) {
   for (auto it = this->found.begin(); it != this->found.end(); ++it) {
      if (*it == base)
         return base;
   }
   this->found.push_back(base);
   //
   // Mimic Tile::Value::UpdateInboundReferences():
   //
   RE::Tile::Value* lastIncomingValue = nullptr;
   for (auto incoming = base->incomingRefs->refNext; incoming; incoming = incoming->refNext) {
      if (!this->IsLoggingSafe())
         return nullptr;
      if (incoming->opcode == 0)
         continue;
      RE::Tile::Value::Expression* rel = incoming;
      while (rel->prev)
         rel = rel->prev;
      auto relValue = rel->operand.ref;
      if (!relValue || relValue == lastIncomingValue)
         continue;
      auto cyclical = this->FindCyclical(relValue, true);
      if (cyclical)
         return cyclical;
      lastIncomingValue = relValue;
   }
   //
   // Done.
   //
   this->found.pop_back();
   return nullptr;
}
void TileCyclicalReferenceChecker::Check(RE::Tile::Value* base, bool isRecursing) {
   if (!isRecursing) {
      this->indent = "";
      _MESSAGE("=========================================================");
      _MESSAGE("Running cyclical reference check for %s::%s...", base->owner->name.m_data, RE::TagIDToName(base->id));
      auto cyclical = this->FindCyclical(base);
      if (!cyclical) {
         _MESSAGE("No cyclical reference found.");
         _MESSAGE("=========================================================");
         return;
      }
      _MESSAGE("Found cyclical reference! Stack:");
      for (auto it = this->found.begin(); it != this->found.end(); ++it) {
         auto value = *it;
         if (value == cyclical)
            _MESSAGE(" > %s::%s", value->owner->name.m_data, RE::TagIDToName(value->id));
         else
            _MESSAGE(" - %s::%s", value->owner->name.m_data, RE::TagIDToName(value->id));
      }
      _MESSAGE("");
      _MESSAGE("Tracing reference chain from end to start...");
      _MESSAGE("NOTE: Srcs shown on operators represent the final resolved value: selectors \n"
               "like me() and sibling() will be replaced by the final resolved tile's name.");
      _MESSAGE("");
   }
   auto count = this->found.size();
   if (count) {
      RE::Tile::Value* value    = *this->found.rbegin();
      RE::Tile::Value* previous = nullptr;
      if (count > 1)
         previous = *(this->found.rbegin() + 1);
      this->found.pop_back();
      auto prefix = this->indent.c_str();
      if (isRecursing)
         _MESSAGE("%s - Outbound reference to value %s::%s, whose operators are:", prefix, value->owner->name.m_data, RE::TagIDToName(value->id));
      else
         _MESSAGE("%s > Circular reference to value %s::%s, whose operators are:", prefix, value->owner->name.m_data, RE::TagIDToName(value->id));
      auto op = value->operators->next;
      if (op) {
         bool loggedRelevant = false;
         do {
            if (loggedRelevant && isRecursing) {
               _MESSAGE("%s    + Remaining operators for this tile are irrelevant and will not be logged.", prefix);
               break;
            }
            bool isPotentiallyRelevant = false;
            //
            RE::Tile::Value* sourceVal = nullptr;
            RE::Tile::Value::Expression* sourceHead;
            for (sourceHead = op->refPrev; sourceHead; sourceHead = sourceHead->refPrev) {
               if (sourceHead->opcode == 0x65)
                  break;
            }
            if (sourceHead) {
               sourceVal = sourceHead->operand.ref;
               if (sourceVal)
                  isPotentiallyRelevant = sourceVal == previous || !previous;
            }
            char bullet = isPotentiallyRelevant ? '-' : '+';
            auto name = RE::TagIDToName(op->opcode);
            switch (op->opcode) {
               case 0xA:
                  _MESSAGE("%s    %c %08X <%s>...", prefix, bullet, op, name);
                  break;
               case 0xF:
                  _MESSAGE("%s    %c %08X ...</%s>", prefix, bullet, op, name);
                  break;
               case 0x65:
                  _MESSAGE("%s    %c %08X List Head", prefix, bullet, op);
                  break;
               default:
                  if (op->opcode == 0)
                     name = "[NEUTRALIZED]";
                  if (name) {
                     if (op->refPrev) {
                        auto p = op->refPrev;
                        while (p->prev)
                           p = p->prev;
                        if (p->opcode == 0x65) {
                           auto source = p->operand.ref;
                           if (source) {
                              auto tile  = source->owner->name.m_data;
                              auto trait = RE::TagIDToName(source->id);
                              if (trait)
                                 _MESSAGE("%s    %c %08X <%s src=\"%s\" trait=\"%s\" /> == %f", prefix, bullet, op, name, tile, trait, op->operand.immediate);
                              else
                                 _MESSAGE("%s    %c %08X <%s src=\"%s\" trait=\"0x%08X\" /> == %f", prefix, bullet, op, name, tile, source->id, op->operand.immediate);
                           } else
                              _MESSAGE("%s    %c %08X <%s src=\"?\" trait=\"?\" /> == %f", prefix, bullet, op, name, op->operand.immediate);
                        } else {
                           _MESSAGE("%s    %c %08X <%s src=\"?\" trait=\"?\" /> == %f", prefix, bullet, op, name, op->operand.immediate);
                        }
                     } else {
                        _MESSAGE("%s    %c %08X <%s>%f</%s>", prefix, bullet, op, name, op->operand.immediate, name);
                     }
                  } else {
                     _MESSAGE("%s    %c %08X with opcode %03X", prefix, bullet, op, op->opcode);
                  }
            }
            if (isPotentiallyRelevant) {
               if (previous) {
                  auto size = this->indent.size();
                  this->indent.resize(size + 6, ' ');
                  this->Check(previous, true);
                  this->indent.resize(size);
                  prefix = this->indent.c_str();
               } else {
                  _MESSAGE("%s       > Circular reference to the outermost-logged value.", prefix);
               }
               loggedRelevant = true;
            }
         } while (op = op->next);
      }
   }
   if (!isRecursing) {
      this->found.clear();
      _MESSAGE("\nTrace complete.");
      _MESSAGE("=========================================================");
   }
}