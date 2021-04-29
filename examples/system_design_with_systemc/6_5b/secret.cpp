
//****************************************************************************
//****************************************************************************
//
// Copyright (c) 2002 Thorsten Groetker, Stan Liao, Grant Martin, Stuart Swan
//
// Permission is hereby granted to use, modify, and distribute this source
// code in any way as long as this entire copyright notice is retained
// in unmodified form within the source code.
//
// This software is distributed on an "AS IS" basis, without warranty
// of any kind, either express or implied.
//
// This source code is from the book "System Design with SystemC".
// For detailed discussion on this example, see the relevant section
// within the "System Design with SystemC" book.
//
// To obtain the book and find additional source code downloads, etc., visit
//     www.systemc.org 
// Look in the "Products & Solutions" -> "SystemC Books". Then look at the
// entry for "System Design with SystemC".
//
//****************************************************************************
//****************************************************************************






#include "wrapper.h"


// This is the "secret" functionality within a ".cpp" file
// that is compiled and delivered to the customer as a ".o"

secret::secret(sc_module_name nm, unsigned w)
    : sc_module(nm), _context(sc_fxtype_params(w, w)), _w(w)
{
    SC_METHOD(main);
    sensitive << in1 << in2;
    _context.end();
}

void secret::main() { out1 = in1.read() * in2.read() + _w; }

