
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


//
// This simple example shows a derived FIFO class
// (my_fifo_t, cf section 10.3) that has built-in
// support for logging data into a file.
//


#include "systemc.h"

#ifndef WIN32
#include "fstream.h"
#endif

// Derived FIFO class with additional tracing support.
// Writes new data into logfile (separated by newlines) if
// the constructor is called with a non-NULL filename.

template <class T> class my_fifo_t : public sc_fifo<T>
{
public:

    // constructor w/ name, size, and filename
    my_fifo_t(const char* nm, int size, const char* filename = NULL)
        : sc_fifo<T>(nm, size), log(NULL)
    {
        if (filename != NULL) {
            log = new ofstream(filename);
            if (log->bad())
                cerr << this->name()
                     << ": (ERROR) could not open file "
                     << filename << endl;
        }
    }

    // blocking write
    virtual void write(const T& val) {
        sc_fifo<T>::write(val); // the actual write operation
        if (log != NULL)
            *log << val << endl; // log data
    }

    // non-blocking write
    virtual bool nb_write(const T& val) {
        // the actual write operation
        bool r = sc_fifo<T>::nb_write(val);
        // log data if writing was successful
        if (r == true && log != NULL)
            *log << val << endl;
        return r;
    }

    // destructor
    ~my_fifo_t() { if (log) log->close(); }

protected:
    ofstream* log; // file stream to log data

private:
    // Avoid unintentional use of default constructor
    // and copy constructor by making them private and
    // not implementing them.
    my_fifo_t(); 
    my_fifo_t(const my_fifo_t&);
};


// Source module
SC_MODULE(source) {
    sc_fifo_out<int> output;
    int counter_;
    SC_CTOR(source) : counter_(0) { SC_THREAD(pump); }
    void pump() { for(;;) output.write(counter_++); }
};


// Sink module
SC_MODULE(sink) {
    sc_fifo_in<int> input;
    SC_CTOR(sink) { SC_THREAD(proc); }
    void proc() { for (;;) { input.read(); wait(sc_time(1, SC_NS)); } }
};


int sc_main(int argc, char** argv)
{
    // channels
    my_fifo_t<int> my_fifo("my_fifo", 7, "log.dat");

    // modules
    source my_source("my_source");
    sink   my_sink("my_sink");

    // interconnect
    my_source.output(my_fifo);
    my_sink.input(my_fifo);

    // run the simulation
    sc_start(sc_time(200, SC_NS));

    return 0;
}
