
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





#define SC_INCLUDE_FX
#include <systemc.h>

template <class T, int N> class fir : public sc_module {
public:
  sc_in<bool> clock;
  sc_in<T> in;
  sc_out<T> out;

  SC_HAS_PROCESS(fir);

  fir(sc_module_name name) : sc_module(name) {
    SC_METHOD(main);
    sensitive << clock.pos();

    for (int i=0; i < N; i++)
      _delay_line[i] = _coefs[i] = 0;
  }

  void set_coef(unsigned i, T val) {
    if (i < N) 
      _coefs[i] = val;
  }

private:
  T _delay_line[N];
  T _coefs[N];

  void main() {
    // shift samples in delay line
    for (int j=N-1; j > 0; j--)
      _delay_line[j] = _delay_line[j-1];

    // read new data sample
    _delay_line[0] = in.read();

    // compute fir output
    T sum = 0;
    for (int i=0; i < N; i++)
      sum += _delay_line[i] * _coefs[i];

    out.write(sum);
  }
};

template <class T> class stimulus : public sc_module {
public:
  sc_out<T> out;

  SC_HAS_PROCESS(stimulus);

  stimulus(sc_module_name name) : sc_module(name) {
    SC_THREAD(main);
  }

  void main() {
    out.write(0);
    wait(10, SC_NS);
    out.write(2);
    wait(1, SC_NS);
    out.write(0);
  }
};

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


int sc_main (int argc , char *argv[]) {
  typedef sc_fixed<8,5> fir_T;
//  typedef double fir_T;
  const fir_T coefs[] = {1.1111, 2.2222, 3.3333, 4.4444};
  const int taps = sizeof(coefs) / sizeof(coefs[0]);

  sc_clock clock("c1", 1, SC_NS);
  sc_signal<fir_T> fir_in;
  sc_signal<fir_T> fir_out;

  fir<fir_T, taps> fir1("fir1");
  fir1.clock(clock);
  fir1.in(fir_in);
  fir1.out(fir_out);

  for (int j=0; j < taps; j++)
    fir1.set_coef(j, coefs[j]);

  stimulus<fir_T> stim1("stim1");
  stim1.out(fir_in);

  response<fir_T> resp1("resp1");
  resp1.in(fir_out);

  sc_start(100, SC_NS);
  return 0;
}
