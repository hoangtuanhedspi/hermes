// Copyright (c) Facebook, Inc. and its affiliates.
//
// This source code is licensed under the MIT license found in the LICENSE
// file in the root directory of this source tree.
//
// RUN: (! %hermes %s 2>&1 ) | %FileCheck --match-full-lines %s

"use strict";

"\001";  // this should be ok
x = 010;
//CHECK: {{.*}}octal.js:11:5: error: Octal literals are not allowed in strict mode
//CHECK-NEXT: x = 010;
//CHECK-NEXT:     ^~~
