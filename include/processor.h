#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  float Utilization();  // TODO: See src/processor.cpp
  long CurrentTotal() const;
  long CurrentActive() const;
  long CurrentIdle() const;
  long PrevTotal() const;
  long PrevIdle() const;
  long PrevActive() const;
  void Update(long idle, long active, long total);

  // TODO: Declare any necessary private members
 private:
  long idle_;
  long active_;
  long total_;
};

#endif