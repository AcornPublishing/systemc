
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
// This is a refined version of the source_sink_adapter
// example. Here, the FIFO_WRITE_HS adapter has been
// merged into the calling SOURCE module. As a result,
// adapter and source were replaced with the newly
// created HW_SOURCE. See section 9.3 for details.
//


#include "systemc.h"


// SOURCE module after merging in the FIFO_WRITE_HS
// adapter (cf. section 9.3).
SC_MODULE(HW_SOURCE) { // name changed

    // output port
    // sc_fifo_out<int> output // removed original port
    // new ports; imported from adapter
    sc_in_clk clock;
    sc_out<int> data; // replaced T with int
    sc_out<bool> valid;
    sc_in<bool> ready;

    // blocking write
    void write(const int& x) { // replaced T with int
        data = x; // drive data line
        valid = true; // signal that data is valid
        do { // wait until data was read
            wait(clock->posedge_event());
        } while (ready.read() != true);
        valid = false; // make sure data doesn't get read twice
    }

    // the one and only process
    void my_process() {
        int counter = 0;
        while(1) {
            counter++; // placeholder for decent functionality
            // output->write(counter);
            write(counter);
            // replaced output->write() with write()
        }
    }

    SC_CTOR(HW_SOURCE) { // name changed
        SC_THREAD(my_process); // declare process
    }
};


// A simple dataflow sink. It consumes input tokens
// and prints their value on stdout. After 100 tokens
// it stops consuming data.
SC_MODULE(SINK) {

    // input port
    sc_fifo_in<int> input;

    // the one and only process
    void process() {
        // consume 100 samples and return
        for (int i = 0; i < 100; i++) {
            cout << name() << ": read " << input.read()
                 << " [t=" << sc_time_stamp() << "]"
                 << endl;
        }
        return;
    }

    // constructor
    SC_CTOR(SINK) {
        SC_THREAD(process);
    }

};


// This adapter converts the FIFO read interface into a bi-
// directional handshake protocol. It is a hierarchical channel
// that implements the sc_fifo_in_if interface. FIFO_READ_HS
// has four ports; one for the data, one for the clock, and two
// for the control lines
template <class T> 
class FIFO_READ_HS 
  : public sc_module,
    public sc_fifo_in_if<T>
{
public:

    // ports
    sc_in_clk clock;
    sc_in<T> data;
    sc_in<bool> valid;
    sc_out<bool> ready;

    // blocking read
    virtual void read(T& x) {
        // signal that we are ready to consume a token
        ready.write(true);
        // wait until we've got valid data
        do
            wait(clock->posedge_event());
        while (valid.read() != true);
        // read data
        x = data.read();
        // no more consumption for the moment
        ready.write(false);
    }

    virtual T read() {
        T tmp;
        read(tmp);
        return tmp;
    }

    // provide dummy implementations for unneeded
    // sc_fifo_out_if<T> methods:
    bool nb_read(T&)
    { assert(0); return false; }

    int num_available() const
    { assert(0); return 0; }

    const sc_event& data_written_event() const
    { static sc_event dummy; assert(0); return dummy; }
    // NB: data_written_event() was introduced with SystemC 2.0.1

    // constructor
    SC_CTOR(FIFO_READ_HS) {
        ready.initialize(false);
    }

};


// A FIFO model with a pin-level interface. See
// section 7.5 for details.
template <class T> class hw_fifo : public sc_module
{
public:
    sc_in<bool> clk;
    sc_in<T> data_in;
    sc_in<bool> valid_in;
    sc_out<bool> ready_out;
    sc_out<T> data_out;
    sc_out<bool> valid_out;
    sc_in<bool> ready_in;

    SC_HAS_PROCESS(hw_fifo);

    hw_fifo(sc_module_name name, unsigned size)
        : sc_module(name), _size(size)
    {
        assert(size > 0);
        _first = _items = 0;
        _data = new T[_size];
        SC_METHOD(main);
        sensitive << clk. pos();
        ready_out. initialize(true);
        valid_out. initialize(false);
    }

    ~hw_fifo() { delete[] _data; }

protected:

    void main()
    {
        if (valid_in. read() && ready_out. read())
        {
            // store new data item into fifo
            _data[(_first + _items) % _size] = data_in;
            ++_items;
        }
        if (ready_in. read() && valid_out. read())
        {
            // discard data item that was just read from fifo
            -- _items;
            _first = (_first + 1) % _size;
        }
        // update all output signals
        ready_out = (_items < _size);
        valid_out = (_items > 0);
        data_out = _data[_first];
    }

    unsigned _size, _first, _items;
    T* _data;
};


int sc_main(int argc, char* argv[])
{
    // module instances
    HW_SOURCE hw_source("hw_source");
    SINK   sink("sink");
    hw_fifo<int> fifo("fifo", 10);

    // hierarchical channels
    FIFO_READ_HS<int> read_adapter("read_adapter");

    // primitive channels
    sc_clock clock("clock", 10, SC_NS);
    sc_signal<int> source_data("source_data");
    sc_signal<bool> source_ready("source_ready");
    sc_signal<bool> source_valid("source_valid");
    sc_signal<int> sink_data("sink_data");
    sc_signal<bool> sink_ready("sink_ready");
    sc_signal<bool> sink_valid("sink_valid");

    // interconnect
    sink.input(read_adapter);
    hw_source.clock(clock);
    hw_source.data(source_data);
    hw_source.ready(source_ready);
    hw_source.valid(source_valid);
    read_adapter.clock(clock);
    read_adapter.data(sink_data);
    read_adapter.ready(sink_ready);
    read_adapter.valid(sink_valid);
    fifo.clk(clock);
    fifo.data_in(source_data);
    fifo.ready_out(source_ready);
    fifo.valid_in(source_valid);
    fifo.data_out(sink_data);
    fifo.ready_in(sink_ready);
    fifo.valid_out(sink_valid);

    // start the simulation
    sc_start(200, SC_NS);

    return 0;
}
