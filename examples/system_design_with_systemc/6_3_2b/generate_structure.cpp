
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




#include <systemc.h>

// reg: a simple register that stores an "int"

class reg : public sc_module {
public:
    sc_in<bool> clock;
    sc_in<int> in;
    sc_out<int> out;

    SC_HAS_PROCESS(reg);

    reg(sc_module_name name) : sc_module(name) {
        SC_METHOD(main);
        sensitive << clock.pos();
    }

    void main() { out = in.read(); }
};

// shiftreg: a shift register that stores "ints".
//   Overall shift register length is set via
//   its constructor argument

class shiftreg : public sc_module {
public:
    sc_in<bool> clock;
    sc_in<int> in;
    sc_out<int> out;

    shiftreg(sc_module_name name, unsigned len)
      : sc_module(name) 
    {
        char buf[10];

        // "prev" points to the signal connected to the
        // output of the previous "reg" instance

        sc_signal<int>* prev = 0;

        // loop to create and connect all the "reg" instances:

        for (unsigned i=0; i < len; i++) 
        {
            sprintf(buf, "r%d", i);

            // instantiate the "reg" instance
            reg* regp = new reg(buf);

            // connect the "clock" port of every instance
            regp->clock(clock);

            // if this is the first instance, connect the "reg"
            // input port to the "shiftreg" input port, else
            // connect the "reg" input port to the output 
            // signal of the previous "reg" instance
            if (i == 0)
                regp->in(in);
            else
                regp->in(*prev);

            if (i < len - 1) {
                // for each "reg" instance except the last,
                // create a new signal and attach it to the 
                // output port on "reg"
                sc_signal<int>* sigp = new sc_signal<int>;
                regp->out(*sigp);
                prev = sigp;
            }
            else {
              // connect the output port of the last "reg" 
              // instance to the output port of "shiftreg"
               regp->out(out);
            }   
        }
    }

    ~shiftreg()
    {
        // a robust version would delete all the dynamically
        // allocated signals and modules
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
    int w = 10;

    if (argc > 1) w = atoi(argv[1]);
    if (w < 1) w = 1;
    if (w > 50) w = 50;

    sc_clock clock("c1", 1, SC_NS);
    sc_signal<int> shiftreg_in;
    sc_signal<int> shiftreg_out;

    shiftreg sr1("sr1", w);
    sr1.clock(clock);
    sr1.in(shiftreg_in);
    sr1.out(shiftreg_out);

    stimulus<int> stim1("stim1");
    stim1.out(shiftreg_in);

    response<int> resp1("resp1");
    resp1.in(shiftreg_out);

    sc_start(100, SC_NS);
    return 0;
} 

