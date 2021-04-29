
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

template <class T> class response : public sc_module {
public:
  sc_in<T> in;

  SC_HAS_PROCESS(response);

  response(sc_module_name name) : sc_module(name) {
    SC_METHOD(main);
    sensitive << in;
  }

  void main() {
    cout << "at time: " << sc_time_stamp() << " output: " << in.read() << endl;
  }
};

int sc_main(int argc, char *argv[])
{
  const int W = 32;
  sc_signal<sc_uint<W> > address;
  sc_signal<sc_uint<W> > data;
  sc_clock clk("clk", 1, SC_NS);
  
  cpu<W> cpu1("cpu1", 256);
  cpu1.clock(clk);
  cpu1.addr(address);
  cpu1.data(data);

  response<sc_uint<W> > resp("resp");
  resp.in(address);

  sc_start(10, SC_NS); 
  cout << "ending" << endl << endl;
  cout << endl;
  return 0;
}
