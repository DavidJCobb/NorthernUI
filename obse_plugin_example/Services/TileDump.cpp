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

void TileCyclicalReferenceChecker::Check(RE::Tile::Value* base, bool isRecursing) {
   if (!isRecursing) {
      _MESSAGE("=========================================================");
      _MESSAGE("Running cyclical reference check for %s::%s...", base->owner->name.m_data, RE::TagIDToName(base->id));
   } else {
      _MESSAGE("Inbound reference from %s::%s...", base->owner->name.m_data, RE::TagIDToName(base->id));
   }
   for (auto it = this->found.begin(); it != this->found.end(); ++it) {
      if (*it == base) {
         _MESSAGE("!!! !!! !!! CYCLICAL REFERENCE DETECTED !!! !!! !!!");
         _MESSAGE("%s::%s has been seen before!", base->owner->name.m_data, RE::TagIDToName(base->id));
         _MESSAGE("");
         return;
      }
   }
   //
   // Mimic Tile::Value::DoActionEnumeration(bool):
   //
   this->found.push_back(base);
   auto current = base->operators->next;
   _MESSAGE("Operators:");
   if (!base->operators->next)
      _MESSAGE(" - <None>");
   for (auto current = base->operators->next; current; current = current->next) {
      auto name = RE::TagIDToName(current->opcode);
      switch (current->opcode) {
         case 0xA:
            _MESSAGE(" - %08X <%s>...", current, name);
            break;
         case 0xF:
            _MESSAGE(" - %08X ...</%s>", current, name);
            break;
         case 0x65:
            _MESSAGE(" - %08X List Head", current);
            break;
         default:
            if (current->opcode == 0)
               name = "[NEUTRALIZED]";
            if (name) {
               if (current->refPrev) {
                  auto p = current->refPrev;
                  while (p->prev)
                     p = p->prev;
                  if (p->opcode == 0x65) {
                     auto source = p->operand.ref;
                     if (source) {
                        auto tile  = source->owner->name.m_data;
                        auto trait = RE::TagIDToName(source->id);
                        if (trait)
                           _MESSAGE(" - %08X <%s src=\"%s\" trait=\"%s\" /> == %f", current, name, tile, trait, current->operand.immediate);
                        else
                           _MESSAGE(" - %08X <%s src=\"%s\" trait=\"0x%08X\" /> == %f", current, name, tile, source->id, current->operand.immediate);
                     } else
                        _MESSAGE(" - %08X <%s src=\"?\" trait=\"?\" /> == %f", current, name, current->operand.immediate);
                  } else {
                     _MESSAGE(" - %08X <%s src=\"?\" trait=\"?\" /> == %f", current, name, current->operand.immediate);
                  }
               } else {
                  _MESSAGE(" - %08X <%s>%f</%s>", current, name, current->operand.immediate, name);
               }
            } else {
               _MESSAGE(" - %08X with opcode %03X", current, current->opcode);
            }
      }
   }
   _MESSAGE("");
   //
   // Mimic Tile::Value::UpdateInboundReferences():
   //
   RE::Tile::Value* lastIncomingValue = nullptr;
   for (auto incoming = base->incomingRefs->refNext; incoming; incoming = incoming->refNext) {
      if (incoming->opcode == 0) {
         _MESSAGE("    - Found dead opcode %08X with operand %08X.", incoming, incoming->operand.opcode);
         continue;
      }
      RE::Tile::Value::Expression* rel = incoming;
      while (rel->prev)
         rel = rel->prev;
      auto relValue = rel->operand.ref;
      if (!relValue || relValue == lastIncomingValue)
         continue;
      this->Check(relValue, true);
      _MESSAGE(" = Returning to %s::%s inbound references...\n", base->owner->name.m_data, RE::TagIDToName(base->id));
      lastIncomingValue = relValue;
   }
   //
   // Done.
   //
   this->found.pop_back();
   if (!isRecursing) {
      _MESSAGE("Check complete. If no circular reference was listed, then none were found.");
      _MESSAGE("=========================================================");
      this->found.clear();
   }
}