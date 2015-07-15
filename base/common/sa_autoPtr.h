#ifndef _SA_AUTO_PTR_H
#define _SA_AUTO_PTR_H

#if defined (_MSC_VER)
#  pragma warning(push)
#  pragma warning(disable: 4284)
#endif /* _MSC_VER */

enum _eTagFreeType
{
  ec_cpp_delete = 0x01,
  ec_c_free = 0x02,
  ec_no_free = 0x04
};

/**
 * @class
 *
 * @brief Implements the draft C++ standard auto_ptr abstraction.
 * This class allows one to work on non-object (basic) types
 */
template <typename X>
class t_basicAutoPtr
{
public:
  typedef X element_type;

  // = Initialization and termination methods
  explicit t_basicAutoPtr (X * p = 0, uint free_type = ec_cpp_delete) 
  : free_type_(free_type), p_ (p) {}

  t_basicAutoPtr (t_basicAutoPtr<X> & ap);
  t_basicAutoPtr<X> &operator= (t_basicAutoPtr<X> & rhs);
  ~t_basicAutoPtr(void);

  // = Accessor methods.
  X &operator *() const;
  X *get (void) const;
  X *release (void);
  void reset (X * p = 0, uint free_type = ec_cpp_delete);
  void setFreeType(uint free_type) { free_type_ = free_type; }
  uint free_type_;
protected:
  X *p_;
};

/**
 * @class auto_ptr
 *
 * @brief Implements the draft C++ standard auto_ptr abstraction.
 */
template <typename X>
class t_autoPtr : public t_basicAutoPtr<X>
{
public:
  typedef X element_type;

  // = Initialization and termination methods
  explicit t_autoPtr (X * p = 0, uint free_type = ec_cpp_delete) 
  : t_basicAutoPtr<X> (p,free_type) {}
  t_autoPtr(t_autoPtr<X> & ap) : t_basicAutoPtr<X> (ap.release ()) {}

  X *operator-> () const;
};

#include "sa_autoPtr.inl"

#if defined (_MSC_VER)
#  pragma warning(pop)
#endif /* _MSC_VER */


#endif 
