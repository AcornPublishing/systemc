
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

// hw_fifo: clocked RTL Hardware FIFO with
// ready/valid handshake signals for FIFO input and output

template <class T> class hw_fifo : public sc_module
{
public:
  sc_in<bool>  clk;

  sc_in<T>     data_in;
  sc_in<bool>  valid_in;
  sc_out<bool> ready_out;

  sc_out<T>    data_out;
  sc_out<bool> valid_out;
  sc_in<bool>  ready_in;

  SC_HAS_PROCESS(hw_fifo);

  hw_fifo(sc_module_name name, unsigned size)
    : sc_module(name), _size(size)
  {
    assert(size > 0);
    _first = _items = 0;
    _data = new T[_size];

    SC_METHOD(main);
    sensitive << clk.pos();

    ready_out.initialize(true);
    valid_out.initialize(false);
  }

  ~hw_fifo() { delete[] _data; }

protected:

  void main()
  {
    if (valid_in.read() && ready_out.read())
    {
      // store new data item into fifo
      _data[(_first + _items) % _size] = data_in;
      ++_items;
    }

    if (ready_in.read() && valid_out.read())
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

  unsigned _size;
  unsigned _first;
  unsigned _items;
  T* _data;
};


// hw_fifo_wrapper<T>: hierarchical channel that implements the
// sc_fifo<T> interfaces. Uses embedded hw_fifo to implement
// FIFO, and has a clock input port to drive the hw_fifo.

template <class T>
class hw_fifo_wrapper
: public sc_module,
  public sc_fifo_in_if<T>,
  public sc_fifo_out_if<T>
{
public:
  sc_in<bool> clk;

protected:
  // embedded channels

  sc_signal<T>    write_data;
  sc_signal<bool> write_valid;
  sc_signal<bool> write_ready;

  sc_signal<T>    read_data;
  sc_signal<bool> read_valid;
  sc_signal<bool> read_ready;

  // embedded module

  hw_fifo<T> hw_fifo_;

public:

  hw_fifo_wrapper(sc_module_name name, unsigned size)
   : sc_module(name), hw_fifo_("hw_fifo1", size)
  {
    hw_fifo_.clk(clk);

    hw_fifo_.data_in  (write_data);
    hw_fifo_.valid_in (write_valid);
    hw_fifo_.ready_out(write_ready);

    hw_fifo_.data_out (read_data);
    hw_fifo_.valid_out(read_valid);
    hw_fifo_.ready_in (read_ready);
  }

  virtual void write(const T& data)
  {
    write_data = data;
    write_valid = true;

    do {
      wait(clk->posedge_event());
    } while (write_ready != true);

    write_valid = false;
  }

  virtual T read()
  {
    read_ready = true;

    do {
      wait(clk->posedge_event());
    } while (read_valid != true);

    read_ready = false;
    return read_data.read();
  }

  virtual void read(T& d) { d = read(); }

  // Provide dummy implementations for unneeded sc_fifo<T> interface methods

  virtual bool nb_read(T&) { assert(0); return false; }

  virtual bool nb_write(const T&) { assert(0); return false; }

  virtual int num_available() const { assert(0); return 0; }

  virtual int num_free() const { assert(0); return 0; }

  // following two functions needed in SystemC 2.0.1 and later 

  virtual const sc_event& data_read_event() const
	{ assert(0); return this->default_event(); }

  virtual const sc_event& data_written_event() const
	{ assert(0); return this->default_event(); }
};

class producer : public sc_module
{
  public:
    sc_port<sc_fifo_out_if<char> > out;

    SC_HAS_PROCESS(producer);

    producer(sc_module_name name) : sc_module(name)
    {
      SC_THREAD(main);
    }

    void main()
    {
      const char *str =
        "Visit www.systemc.org and see what SystemC can do for you today!\n";
      const char *p = str;

      while (true)
      {
        if (rand() & 1)
        {
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

    consumer(sc_module_name name) : sc_module(name)
    {
      SC_THREAD(main);
    }

    void main()
    {
      char c;

      while (true) {
        if (rand() & 1)
        {
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
    hw_fifo_wrapper<char> fifo_inst;
    producer prod_inst;
    consumer cons_inst;
    sc_clock clk;

    top(sc_module_name name, int size) :
        sc_module(name) ,
        fifo_inst("Fifo1", size) , 
        prod_inst("Producer1") , 
        cons_inst("Consumer1"),
        clk("c1", 1, SC_NS)
    {
      prod_inst.out(fifo_inst);
      cons_inst.in(fifo_inst);
      fifo_inst.clk(clk);
    }
};

int sc_main (int argc , char *argv[]) 
{
  int size = 10;

  if (argc > 1)
    size = atoi(argv[1]);

  if (size < 1)
    size = 1;

  if (size > 10000)
    size = 10000;

  top top1("Top1", size);
  sc_start(1000, SC_NS);
  cout << endl << endl;
  return 0;
}

