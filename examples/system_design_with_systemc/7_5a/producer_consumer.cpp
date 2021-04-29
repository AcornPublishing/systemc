
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

class producer : public sc_module
{
public:
    sc_port<sc_fifo_out_if<char> > out;

    SC_HAS_PROCESS(producer);

    producer(sc_module_name name) : sc_module(name) {
        SC_THREAD(main);
    }

    void main() {
        const char *str = "Visit www.systemc.org!\n";
        const char *p = str;

        while (true) {
            if (rand() & 1) {
                out->write(*p++);
                if (!*p) p = str;
            }

            wait(1, SC_NS);
        }
    }
};

class consumer : public sc_module
{
public:
    sc_port<sc_fifo_in_if<char> > in;

    SC_HAS_PROCESS(consumer);

    consumer(sc_module_name name) : sc_module(name) {
        SC_THREAD(main);
    }

    void main() {
        char c;

        while (true) {
            if (rand() & 1) {
                in->read(c);
                cout << c;
            }

            wait(1, SC_NS);
        }
    }
};

class top : public sc_module
{
public:
    sc_fifo<char> fifo_inst;
    producer prod_inst;
    consumer cons_inst;

    top(sc_module_name name, int size) :
        sc_module(name),
        fifo_inst("Fifo1", size),
        prod_inst("Producer1"),
        cons_inst("Consumer1")
    {
        prod_inst.out(fifo_inst);
        cons_inst.in(fifo_inst);
    }
};

int sc_main (int argc, char *argv[])
{
    int size = 10;

    top top1("Top1", size);
    sc_start(1000, SC_NS);
    cout << endl << endl;
    return 0;
}


