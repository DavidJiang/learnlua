// -*- C++ -*-
#include <stdlib.h>

template<class X> inline
t_basicAutoPtr<X>::t_basicAutoPtr (t_basicAutoPtr<X> &rhs)
  : p_ (rhs.release ()), free_type_(rhs.free_type_)
{
}

template<class X> inline X *
t_basicAutoPtr<X>::get (void) const
{
  return this->p_;
}

template<class X> inline X *
t_basicAutoPtr<X>::release (void)
{
  X *old = this->p_;
  this->p_ = 0;
  return old;
}

template<class X> inline void
t_basicAutoPtr<X>::reset (X *p, uint free_type)
{
  if (this->get () != p)
  {
    if ( free_type_ == ec_cpp_delete )
    {
      delete this->get ();
    }
    else if ( free_type_ == ec_c_free )
    {
      ::free( this->get() );
    }
    else
    {
      //todo: do nothing.
    }
  }
  this->p_ = p;
  this->free_type_ = free_type;
}

template<class X> inline t_basicAutoPtr<X> &
t_basicAutoPtr<X>::operator= (t_basicAutoPtr<X> &rhs)
{
  if (this != &rhs)
    {
      this->reset (rhs.release (), rhs.free_type_);
    }
  return *this;
}

template<class X> inline
t_basicAutoPtr<X>::~t_basicAutoPtr (void)
{
  if ( free_type_ == ec_cpp_delete )
  {
    delete this->get ();
  }
  else if ( free_type_ == ec_c_free )
  {
    ::free(this->get());
  }
  else
  {
    //todo: do nothing.
  }
}

template<class X> inline X &
t_basicAutoPtr<X>::operator *() const
{
  return *this->get ();
}

template<class X> inline X *
t_autoPtr<X>::operator-> () const
{
  return this->get ();
}
