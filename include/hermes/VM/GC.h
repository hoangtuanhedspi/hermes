/*
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the LICENSE
 * file in the root directory of this source tree.
 */
#ifndef HERMES_VM_GC_H
#define HERMES_VM_GC_H

#if defined(HERMESVM_GC_MALLOC)
#include "hermes/VM/MallocGC.h"
#elif defined(HERMESVM_GC_NONCONTIG_GENERATIONAL)
#include "hermes/VM/GenGCNC.h"
#else
#error "Unsupported HermesVM GCKIND" #HERMESVM_GCKIND
#endif

#endif // HERMES_VM_GC_H
