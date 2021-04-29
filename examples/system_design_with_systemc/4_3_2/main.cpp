
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


#include "systemc.h"
#include "euclid.h"


class top : public sc_module
{
public:
  SC_HAS_PROCESS(top);

  top(sc_module_name name) : div("d1"), clk("c1", 1, SC_NS)
  {
    SC_THREAD(t1);
    SC_THREAD(t2);
    SC_METHOD(t3);
    sensitive << c;

    div.A(a);
    div.B(b);
    div.C(c);
    div.CLOCK(clk);
    div.RESET(reset);
    div.READY(ready);
  }

  void t1() {
    reset.write(false);

    wait(10, SC_NS);
    a = 40;

    wait(10, SC_NS);
    a = 64;
  }

  void t2() {
    wait(10, SC_NS);
    b = 10;

    wait(10, SC_NS);
    b = 8;
  }

  void t3() {
    cout << "at time: " << sc_time_stamp() << " output: " << c.read() << endl;
  }

  euclid_gcd div;
  sc_signal<unsigned> a, b, c;
  sc_signal<bool> reset, ready;
  sc_clock clk;
};

int sc_main (int argc , char *argv[]) 
{
  top top1("Top1");
  sc_start(100, SC_NS);
  cout << endl << endl;
  return 0;
}

