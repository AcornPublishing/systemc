
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





#include "cpu.h"

// This ".cpp" file is compiled, and only the ".o"
// is provided to the customer

template <unsigned W>
cpu<W>::cpu(sc_module_name nm, unsigned cache_size) :
  sc_module(nm), _cache_size(cache_size)
{
   SC_METHOD(main);
   sensitive << clock.pos();
}

template <unsigned W>
void cpu<W>::main()
{
   static int i;
   addr = i++;
   data = 10 + i;
}

// explicitly instantiate cpu for 16, 32, 64 bit bus sizes:

template class cpu<16>;
template class cpu<32>;
template class cpu<64>;


