/*
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the LICENSE
 * file in the root directory of this source tree.
 */
#ifndef HERMES_VM_SLOTACCEPTOR_H
#define HERMES_VM_SLOTACCEPTOR_H

#include "hermes/VM/HermesValue.h"
#include "hermes/VM/SymbolID.h"

namespace hermes {
namespace vm {

class WeakRefBase;

/// SlotAcceptor is an interface to be implemented by acceptors of objects in
/// the heap.
/// An acceptor should accept all of the pointers and other markable fields in
/// an object, and tell the GC that they exist, updating if necessary.
/// This is used by a visitor, see \c SlotVisitor.
struct SlotAcceptor {
  static constexpr bool shouldMarkWeak = true;

  virtual ~SlotAcceptor() {}
  virtual void accept(void *&ptr) = 0;
  virtual void accept(BasedPointer &ptr) = 0;
  virtual void accept(GCPointerBase &ptr) = 0;
  virtual void accept(HermesValue &hv) = 0;
  virtual void accept(SymbolID sym) {
    // By default, symbol processing is a noop.  Only a handful of acceptors
    // need to override it.
  }
  virtual void accept(WeakRefBase &wr) {
    // By default, weak reference processing is a noop.  Only a handful of
    // acceptors need to override it.
  }

  /// When we want to call an acceptor on "raw" root pointers of
  /// some JSObject subtype T, this method does the necessary
  /// reinterpret_cast to allow us to call the "void *&" accept
  /// method above.
  template <typename T>
  void acceptPtr(T *&ptr) {
    accept(reinterpret_cast<void *&>(ptr));
  }
};

struct SlotAcceptorWithNames : public SlotAcceptor {
  virtual ~SlotAcceptorWithNames() {}

  using SlotAcceptor::acceptPtr;

  void accept(void *&ptr) override final {
    accept(ptr, nullptr);
  }
  virtual void accept(void *&ptr, const char *name) = 0;

  void accept(BasedPointer &ptr) override final {
    accept(ptr, nullptr);
  }
  virtual void accept(BasedPointer &ptr, const char *name) = 0;

  void accept(GCPointerBase &ptr) override final {
    accept(ptr, nullptr);
  }
  virtual void accept(GCPointerBase &ptr, const char *name) = 0;

  void accept(HermesValue &hv) override final {
    accept(hv, nullptr);
  }
  virtual void accept(HermesValue &hv, const char *name) = 0;

  void accept(SymbolID sym) override final {
    accept(sym, nullptr);
  }
  virtual void accept(SymbolID sym, const char *name) {
    // By default, symbol processing is a noop.  Only a handful of acceptors
    // need to override it.
  }

  void accept(WeakRefBase &wr) override final {
    accept(wr, nullptr);
  }
  virtual void accept(WeakRefBase &wr, const char *name) {
    // By default, weak reference processing is a noop.  Only a handful of
    // acceptors need to override it.
  }

  template <typename T>
  void acceptPtr(T *&ptr, const char *name) {
    accept(reinterpret_cast<void *&>(ptr), name);
  }
};

struct RootAcceptor : public SlotAcceptorWithNames {
  enum class Section {
#define ROOT_SECTION(name) name,
#include "hermes/VM/RootSections.def"
    NumSections,
    // Sentinel value to be used to represent an invalid section.
    InvalidSection,
  };

  virtual void beginRootSection(Section section) {}
  virtual void endRootSection() {}
};

template <typename Acceptor>
struct DroppingAcceptor final : public RootAcceptor {
  static_assert(
      std::is_base_of<SlotAcceptor, Acceptor>::value,
      "Can only use this with a subclass of SlotAcceptor");
  Acceptor &acceptor;

  DroppingAcceptor(Acceptor &acceptor) : acceptor(acceptor) {}

  using SlotAcceptorWithNames::accept;

  void accept(void *&ptr, const char *) override {
    acceptor.accept(ptr);
  }

  void accept(BasedPointer &ptr, const char *) override {
    acceptor.accept(ptr);
  }

  void accept(GCPointerBase &ptr, const char *) override {
    acceptor.accept(ptr);
  }

  void accept(HermesValue &hv, const char *) override {
    acceptor.accept(hv);
  }

  void accept(SymbolID sym, const char *) override {
    acceptor.accept(sym);
  }

  void accept(WeakRefBase &wr, const char *) override {
    acceptor.accept(wr);
  }
};

} // namespace vm
} // namespace hermes

#endif
