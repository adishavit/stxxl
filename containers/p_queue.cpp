 /***************************************************************************
 *            p_queue.cpp
 *
 *  Fri Jul  4 11:31:34 2003
 *  Copyright  2003  Roman Dementiev
 *  dementiev@mpi-sb.mpg.de
 ****************************************************************************/
#include "priority_queue.h"

using namespace stxxl;


struct my_type
{
    typedef long long int key_type;
	key_type key;
	char data[128 - sizeof(key_type)];
	my_type(){}
	explicit my_type(key_type k):key(k) {}
};

std::ostream & operator << (std::ostream & o,const my_type & obj)
{
	o << obj.key;
	return o;
}

struct dummy_merger
{
  int & cnt;
  dummy_merger(int & c):cnt(c) {}
  template <class OutputIterator>
  void multi_merge(OutputIterator b,OutputIterator e)
  {
    while(b!=e)
    {
      *b = cnt;
      ++b;
      ++cnt;
    }
  }
};

struct my_cmp // greater
{
  bool operator () (const my_type & a, const my_type & b) const { return a.key > b.key; }
  my_type min_value() const { return my_type(std::numeric_limits<my_type::key_type>::max()); }
  my_type max_value() const { return my_type(std::numeric_limits<my_type::key_type>::min()); }
};

int main()
{/*
      unsigned BufferSize1_ = 32, // equalize procedure call overheads etc. 
      unsigned N_ = 512, // bandwidth
      unsigned IntKMAX_ = 64, // maximal arity for internal mergers
      unsigned IntLevels_ = 4, 
      unsigned BlockSize_ = (2*1024*1024),
      unsigned ExtKMAX_ = 64, // maximal arity for external mergers
      unsigned ExtLevels_ = 2,
  */
  //typedef priority_queue<priority_queue_config<my_type,my_cmp,
  //  32,512,64,3,(4*1024),0x7fffffff,1> > pq_type;
  const unsigned volume = 128*1024; // GB
  typedef PRIORITY_QUEUE_GENERATOR<my_type,my_cmp,20*1024*1024,volume/sizeof(my_type)>::result pq_type;
  typedef pq_type::block_type block_type;
 
  STXXL_MSG("Block size: "<<block_type::raw_size)
  //STXXL_MSG(settings::EConsumption);
  /*
  STXXL_MSG(settings::AE);
  STXXL_MSG(settings::settings::B);
  STXXL_MSG(settings::N); */
  
  prefetch_pool<block_type> p_pool(10);
  write_pool<block_type>    w_pool(10);
  pq_type p(p_pool,w_pool);
  off_t nelements = off_t(volume/sizeof(my_type))*1024,i;
  STXXL_MSG("Internal memory consumption of the priority queue: "<<p.mem_cons()<<" bytes")
  STXXL_MSG("Max elements: "<<nelements)
  for(i = 0;i<nelements ;i++ )
  {
    if((i%(1024*1024)) == 0)
		STXXL_MSG("Inserting element "<<i)
    p.push(my_type(nelements - i));
  }
  
  STXXL_MSG("Internal memory consumption of the priority queue: "<<p.mem_cons()<<" bytes")
  for(i = 0; i<(nelements) ;i++ )
  {
    assert( !p.empty() );
    STXXL_MSG( p.top() )
    assert(p.top().key == i+1);
    p.pop();
	if((i%(1024*1024)) == 0)
		STXXL_MSG("Element "<<i<<" poped")
  }
  STXXL_MSG("Internal memory consumption of the priority queue: "<<p.mem_cons()<<" bytes")
  
}
