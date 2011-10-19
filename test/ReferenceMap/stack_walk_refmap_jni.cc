// Copyright 2011 Google Inc. All Rights Reserved.

#include <stdio.h>

#include "UniquePtr.h"
#include "class_linker.h"
#include "dex_verifier.h"
#include "object.h"
#include "jni.h"

namespace art {

#define REG(method, reg_vector, reg) \
  ( ((reg) < (method)->NumRegisters()) &&                       \
    (( *((reg_vector) + (reg)/8) >> ((reg) % 8) ) & 0x01) )

#define CHECK_REGS(...) do {          \
    int t[] = {__VA_ARGS__};             \
    int t_size = sizeof(t) / sizeof(*t);      \
    for (int i = 0; i < t_size; ++i)          \
      EXPECT_TRUE(REG(m, reg_vector, t[i])) << "Error: Reg " << i << " is not in RegisterMap"; \
  } while(false)

// << "Error: Reg " << i << " is not in RegisterMap";

struct ReferenceMap2Visitor : public Thread::StackVisitor {
  ReferenceMap2Visitor() {
  }

  void VisitFrame(const Frame& frame, uintptr_t pc) {
    Method* m = frame.GetMethod();
    if (!m || m->IsNative()) {
      return;
    }
    LOG(INFO) << "At " << PrettyMethod(m, false);

    art::DexVerifier::RegisterMap* map = new art::DexVerifier::RegisterMap(
        m->GetRegisterMapHeader(),
        m->GetRegisterMapData());

    if (!pc) {
      // pc == NULL: m is either a native method or a phony method
      return;
    }
    if (m->IsCalleeSaveMethod()) {
      LOG(WARNING) << "no PC for " << PrettyMethod(m);
      return;
    }

    const uint8_t* reg_vector = NULL;
    std::string m_name = m->GetName()->ToModifiedUtf8();

    // Given the method name and the number of times the method has been called,
    // we know the Dex registers with live reference values. Assert that what we
    // find is what is expected.
    if (m_name.compare("f") == 0) {
      reg_vector = art::DexVerifier::RegisterMapGetLine(map, 0x01U);
      if (reg_vector) {
        LOG(WARNING) << "Reg1: " << *reg_vector << *(reg_vector+1);
        CHECK_REGS(7);  //v7: this
      }

      reg_vector = art::DexVerifier::RegisterMapGetLine(map, 0x02U);
      if (reg_vector) {
        LOG(WARNING) << "Reg2: " << *reg_vector << *(reg_vector+1);
        CHECK_REGS(7);  //v7: this
      }

      reg_vector = art::DexVerifier::RegisterMapGetLine(map, 0x03U);
      if (reg_vector) {
        LOG(WARNING) << "Reg3: " << std::hex << reg_vector[0] << reg_vector[1];
        CHECK_REGS(7);  //v7: this
      }

      reg_vector = art::DexVerifier::RegisterMapGetLine(map, 0x05U);
      if (reg_vector) {
        LOG(WARNING) << "Reg4: " << std::hex << *reg_vector << *(reg_vector+1);
        CHECK_REGS(0, 7);  //v0: x, v7: this
      }

      reg_vector = art::DexVerifier::RegisterMapGetLine(map, 0x06U);
      if (reg_vector) {
        LOG(WARNING) << "Reg5: " << std::hex << reg_vector[0] << reg_vector[1];
        CHECK_REGS(0, 7);  //v0: x, v7: this
      }

      reg_vector = art::DexVerifier::RegisterMapGetLine(map, 0x08U);
      if (reg_vector) {
        LOG(WARNING) << "Reg6: " << std::hex << *reg_vector << *(reg_vector+1);
        CHECK_REGS(0, 2, 7);  //v0: x, v2: y, v7: this
      }

      reg_vector = art::DexVerifier::RegisterMapGetLine(map, 0x0bU);
      if (reg_vector) {
        LOG(WARNING) << "Reg7: " << std::hex << *reg_vector << *(reg_vector+1);
        CHECK_REGS(0, 2, 7);  //v0: x, v2: y, v7: this
      }

      reg_vector = art::DexVerifier::RegisterMapGetLine(map, 0x0cU);
      if (reg_vector) {
        LOG(WARNING) << "Reg8: " << std::hex << *reg_vector << *(reg_vector+1);
        CHECK_REGS(0, 2, 7);  //v0: x, v2: y, v7: this
      }

      reg_vector = art::DexVerifier::RegisterMapGetLine(map, 0x38U);
      if (reg_vector) {
        LOG(WARNING) << "Reg9: " << std::hex << *reg_vector << *(reg_vector+1);
        CHECK_REGS(0, 2, 7);  //v0: x, v2: y, v7: this
      }

      reg_vector = art::DexVerifier::RegisterMapGetLine(map, 0x39U);
      if (reg_vector) {
        LOG(WARNING) << "Reg10: " << std::hex << *reg_vector << *(reg_vector+1);
        CHECK_REGS(0, 2, 7, 1);  //v0: x, v2: y, v7: this, v1: ex
      }

      reg_vector = art::DexVerifier::RegisterMapGetLine(map, 0x3aU);
      if (reg_vector) {
        LOG(WARNING) << "Reg11: " << std::hex << *reg_vector << *(reg_vector+1);
        CHECK_REGS(0, 2, 7, 1);  //v0: x, v2: y, v7: this, v1: y
      }

      reg_vector = art::DexVerifier::RegisterMapGetLine(map, 0x16U);
      if (reg_vector) {
        LOG(WARNING) << "Reg12: " << std::hex << *reg_vector << *(reg_vector+1);
        CHECK_REGS(0, 7, 1);  //v0: x, v7: this, v1: y
      }

      reg_vector = art::DexVerifier::RegisterMapGetLine(map, 0x20U);
      if (reg_vector) {
        LOG(WARNING) << "Reg13: " << std::hex << *reg_vector << *(reg_vector+1);
        CHECK_REGS(0, 7, 1);  //v0: x, v7: this, v1: y
      }

      reg_vector = art::DexVerifier::RegisterMapGetLine(map, 0x22U);
      if (reg_vector) {
        LOG(WARNING) << "Reg14: " << std::hex << *reg_vector << *(reg_vector+1);
        CHECK_REGS(0, 7, 1);  //v0: x, v7: this, v1: y
      }

      reg_vector = art::DexVerifier::RegisterMapGetLine(map, 0x25U);
      if (reg_vector) {
        LOG(WARNING) << "Reg15: " << std::hex << *reg_vector << *(reg_vector+1);
        CHECK_REGS(0);  //v0: x, v7: this, v1: y
      }

      reg_vector = art::DexVerifier::RegisterMapGetLine(map, 0x26U);
      if (reg_vector) {
        LOG(WARNING) << "Reg16: " << std::hex << *reg_vector << *(reg_vector+1);
        CHECK_REGS(0, 7, 1);  //v0: y, v7: this, v1: y
      }

      reg_vector = art::DexVerifier::RegisterMapGetLine(map, 0x14U);
      if (reg_vector) {
        LOG(WARNING) << "Reg17: " << std::hex << *reg_vector << *(reg_vector+1);
        CHECK_REGS(0, 7);  //v0: y, v7: this
      }
    }

  }
};

extern "C"
JNIEXPORT jint JNICALL Java_ReferenceMap_refmap(JNIEnv* env, jobject thisObj, jint count) {
  // Visitor
  ReferenceMap2Visitor mapper;
  Thread::Current()->WalkStack(&mapper);

  return count + 1;
}

}
