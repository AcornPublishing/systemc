
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






// This is the header file provided to customer

#include <systemc.h>

template <unsigned W> class cpu : public sc_module
{
public:
  sc_in<bool> clock;
  sc_out<sc_uint<W> > addr;
  sc_inout<sc_uint<W> > data;

  SC_HAS_PROCESS(cpu);
  cpu(sc_module_name nm, unsigned cache_size);

  void main();

private:
  unsigned _cache_size;
};


