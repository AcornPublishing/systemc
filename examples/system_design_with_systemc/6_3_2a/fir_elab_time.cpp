
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

// module "fir"
// Constructor parameters:
//   sc_module_name name - specifies instance name
//   const double* coeffs - pointer to coefficient array
//     coeffs array must contain "n" coefficients
//   unsigned w - total bit width of fixed pt type used
//   unsigned I - number of integer bits of fixed pt type
//   unsigned n - number of taps in FIR

class fir : public sc_module {
public:
    sc_in<bool> clock;

    // For the sake of simplicity, the "sc_fix" data-type 
    // is only used inside this module, and all external
    // interfaces rely on "double"

    sc_in<double> in;
    sc_out<double> out;

    SC_HAS_PROCESS(fir);

    fir(sc_module_name name, const double* coeffs, unsigned w,
        unsigned i, unsigned n) : 
        sc_module(name), _w(w), _i(i), _n(n) 
    {
        assert(n > 0); assert(w > 0);
        SC_METHOD(main);
        sensitive << clock.pos();

        // see discussion below for explanation of next line
        sc_fxtype_context c1(sc_fxtype_params(_w, _i));
        _delay_line = new sc_fix[_n];
        _coeffs = new sc_fix[_n];

        // copy input coeffs array and convert to sc_fix type
        for (unsigned j=0; j < _n; j++)
            _coeffs[j] = coeffs[j];
    }

    ~fir() { delete[] _delay_line; delete[] _coeffs; }

private:
    sc_fix *_delay_line;
    sc_fix *_coeffs;
    const unsigned _w, _i, _n;
  
    void main() {
        // shift samples in delay line
        for (int j=_n-1; j > 0; j--)
            _delay_line[j] = _delay_line[j-1];

        // read new data sample
        _delay_line[0] = in.read();

        // compute fir output
        sc_fix sum(_w, _i);
        sum = 0;
        for (unsigned i=0; i < _n; i++)
            sum += _delay_line[i] * _coeffs[i];

        out.write(sum);
    }
};


// simple stimulus generator

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

// simple response logger

template <class T> class response : public sc_module {
public:
    sc_in<T> in;

    SC_HAS_PROCESS(response);

    response(sc_module_name name) : sc_module(name) {
        SC_METHOD(main);
        sensitive << in;
    }

    void main() {
        cout << "at time: " << sc_time_stamp() << " output: " 
             << in.read() << endl;
    }
};

int sc_main (int argc , char *argv[])
{
    const double coeffs[] = {1.1111, 2.2222, 3.3333, 4.4444};
    const int taps = sizeof(coeffs) / sizeof(coeffs[0]);
    int w = 8; // total width of fixed pt numbers
    int i = 5; // integer bits of fixed pt numbers

    if (argc > 1) w = atoi(argv[1]);

    if (argc > 2) i = atoi(argv[2]);

    sc_clock clock("c1", 1, SC_NS);
    sc_signal<double> fir_in;
    sc_signal<double> fir_out;

    fir fir1("fir1", &coeffs[0], w, i, taps);
    fir1.clock(clock);
    fir1.in(fir_in);
    fir1.out(fir_out);

    stimulus<double> stim1("stim1");
    stim1.out(fir_in);

    response<double> resp1("resp1");
    resp1.in(fir_out);

    sc_start(100, SC_NS);
    return 0;
}

