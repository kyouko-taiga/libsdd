#ifndef _SDD_DD_NODE_HH_
#define _SDD_DD_NODE_HH_

#include <algorithm>  // equal, for_each
#include <functional> // hash
#include <iosfwd>

#include "sdd/dd/alpha.hh"
#include "sdd/dd/definition.hh"
#include "sdd/internal/util/hash.hh"
#include "sdd/internal/util/packed.hh"

namespace sdd {

/*-------------------------------------------------------------------------------------------*/

/// @brief  A non-terminal node in an SDD.
/// \tparam Valuation If a set of values, define a flat node; if an SDD, define a hierarchical
/// node.
///
/// For the sake of canonicity, a node shall not exist in several locations, thus we prevent
/// its copy. Also, to enforce this canonicity, we need that nodes have always the same
/// address, thus they can't be moved to an other memory location once created.
template <typename C, typename Valuation>
class _LIBSDD_ATTRIBUTE_PACKED node
{
  // Can't copy a node.
  node& operator=(const node&) = delete;
  node(const node&) = delete;  

  // Can't move a node.
  node& operator=(node&&) = delete;
  node(node&&) = delete;

public:

  /// @brief The type of the variable of this node.
  typedef typename C::Variable          variable_type;

  /// @brief The type of the valuation of this node.
  typedef Valuation                     valuation_type;

  /// @brief The type used to store the number of arcs of this node.
  typedef typename C::alpha_size_type   alpha_size_type;

  /// @brief A (const) iterator on the arcs of this node.
  typedef const arc<C, Valuation>*      const_iterator;

private:

  /// @brief The variable of this node.
  const variable_type variable_;

  /// @brief The number of arcs of this node.
  const alpha_size_type size_;

public:

  /// @brief Constructor.
  ///
  /// O(n) where n is the number of arcs in the builder.
  /// It can't throw as the memory for the alpha has already been allocated.
  node(const variable_type& var, alpha_builder<C, Valuation>& builder)
  noexcept
    : variable_(var)
    , size_(static_cast<alpha_size_type>(builder.size()))
  {
    // Instruct the alpha builder to place it right after the node.
    builder.consolidate(alpha_addr());
  }

  /// @brief Destructor.
  ///
  /// O(n) where n is the number of arcs in the builder.  
  ~node()
  {
    for (auto& arc : *this)
    {
      arc.~arc<C, Valuation>();
    }
  }

  /// @brief Get the variable of this node.
  ///
  /// O(1).
  const variable_type&
  variable()
  const noexcept
  {
    return variable_;
  }

  /// @brief Get the beginning of arcs.
  ///
  /// O(1).
  const_iterator
  begin()
  const noexcept
  {
    return reinterpret_cast<const arc<C, Valuation>*>(alpha_addr());
  }

  /// @brief Get the end of arcs.
  ///
  /// O(1).
  const_iterator
  end()
  const noexcept
  {
    return reinterpret_cast<const arc<C, Valuation>*>(alpha_addr()) + size_;
  }

  /// @brief Get the number of arcs.
  ///
  /// O(1).
  alpha_size_type
  size()
  const noexcept
  {
    return size_;
  }

private:

  /// @brief Return the address of the alpha function.
  ///
  /// O(1).
  /// The alpha function is located right after the current node, so to compute its address,
  /// we just have to add the size of a node to the address of the current node.
  char*
  alpha_addr()
  const noexcept
  {
    return reinterpret_cast<char*>(const_cast<node*>(this)) + sizeof(node);
  }
};

/*-------------------------------------------------------------------------------------------*/

/// @brief   Equality of two nodes.
/// @related node
///
/// O(1) if nodes don't have the same number of arcs; otherwise O(n) where n is the number of
/// arcs.
template <typename C, typename Valuation>
inline
bool
operator==(const node<C, Valuation>& lhs, const node<C, Valuation>& rhs)
noexcept
{
  return lhs.size() == rhs.size() and lhs.variable() == rhs.variable()
     and std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

/// @brief   Export a node to a stream.
/// @related node
template <typename C, typename Valuation>
std::ostream&
operator<<(std::ostream& os, const node<C, Valuation>& n)
{
  os << n.variable() << "[";
  std::for_each( n.begin(), n.end() - 1
               , [&](const arc<C, Valuation>& a)
                    {os << a.valuation() << " --> " << a.successor() << " || ";});
  return os << (n.end() - 1)->valuation() << " --> " << (n.end() - 1)->successor() << "]";
}

/*-------------------------------------------------------------------------------------------*/

} // namespace sdd

/// @cond INTERNAL_DOC

namespace std {

/*-------------------------------------------------------------------------------------------*/

/// @brief Hash specialization for sdd::node
template <typename C, typename Valuation>
struct hash<sdd::node<C, Valuation>>
{
  std::size_t
  operator()(const sdd::node<C, Valuation>& n)
  const noexcept
  {
    std::size_t seed = 0;
    sdd::internal::util::hash_combine(seed, n.variable());
    for (auto& arc : n)
    {
      sdd::internal::util::hash_combine(seed, arc.valuation());
      sdd::internal::util::hash_combine(seed, arc.successor());
    }
    return seed;
  }
};

/*-------------------------------------------------------------------------------------------*/

} // namespace std

/// @endcond

#endif // _SDD_DD_NODE_HH_
