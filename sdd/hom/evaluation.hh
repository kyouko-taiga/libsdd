#ifndef _SDD_HOM_EVALUATION_HH_
#define _SDD_HOM_EVALUATION_HH_

#include <cassert>

#include "sdd/hom/context_fwd.hh"

namespace sdd { namespace hom {

/// @cond INTERNAL_DOC

/*-------------------------------------------------------------------------------------------*/

/// @brief Evaluate an homomorphism.
template <typename C>
struct evaluation
{
  /// @brief Used by internal::util::variant.
  typedef SDD<C> result_type;

  template <typename H>
  bool
  operator()(const H& h, const zero_terminal<C>&, const SDD<C>&, context<C>&)
  const noexcept
  {
    assert(false);
    __builtin_unreachable();
  }

  template <typename H>
  SDD<C>
  operator()(const H& h, const one_terminal<C>&, const SDD<C>& x, context<C>& cxt)
  const noexcept
  {
    return h(cxt, x);
  }

  /// @brief Dispatch evaluation to the concrete homomorphism.
  ///
  /// Implement a part of the automatic saturation: evaluation is propagated on successors
  /// whenever possible.
  template <typename H, typename Node>
  SDD<C>
  operator()(const H& h, const Node& node, const SDD<C>& x, context<C>& cxt)
  const
  {
    if (h.skip(node.variable()))
    {
      square_union<C, typename Node::valuation_type> su;
      su.reserve(node.size());
      for (const auto& arc : node)
      {
        SDD<C> new_successor = h(cxt, arc.successor());
        if (not new_successor.empty())
        {
          su.add(new_successor, arc.valuation());
        }
      }
      return SDD<C>(node.variable(), su(cxt.sdd_context()));
    }
    else
    {
      return h(cxt, x);
    }
  }
};

/*-------------------------------------------------------------------------------------------*/

/// @brief Default traits for homomorphisms.
template <typename T>
struct homomorphism_traits
{
  static constexpr bool should_cache = true;
};

/*-------------------------------------------------------------------------------------------*/

/// @brief The evaluation of an homomorphism in the cache.
template <typename C>
struct cached_homomorphism
{
  /// @brief Needed by the cache.
  typedef SDD<C> result_type;

  /// @brief The evaluation context.
  context<C>& cxt_;

  /// @brief The homomorphism to evaluate.
  const homomorphism<C> h_;

  /// @brief The homomorphism's operand.
  const SDD<C> sdd_;

  /// @brief Constructor.
  cached_homomorphism(context<C>& cxt, const homomorphism<C>& h, const SDD<C>& s)
    : cxt_(cxt)
    , h_(h)
    , sdd_(s)
  {
  }

  /// @brief Launch the evaluation.
  SDD<C>
  operator()()
  const
  {
    return apply_binary_visitor( evaluation<C>(), h_->data(), sdd_->data(), sdd_, cxt_);
  }
};

/*-------------------------------------------------------------------------------------------*/

/// @related cached_homomorphism
template <typename C>
inline
bool
operator==(const cached_homomorphism<C>& lhs, const cached_homomorphism<C>& rhs)
noexcept
{
  return lhs.h_ == rhs.h_ and lhs.sdd_ == rhs.sdd_;
}

/*-------------------------------------------------------------------------------------------*/

/// @brief Used by the cache as a filter to know if an homomorphism should be cached.
template <typename C>
struct should_cache
{
  /// @brief Needed by variant.
  typedef bool result_type;

  /// @brief Dispatch to each homomorphism's trait.
  template <typename T>
  constexpr bool
  operator()(const T&)
  const noexcept
  {
    return homomorphism_traits<T>::should_cache;
  }

  /// @brief Application.
  bool
  operator()(const cached_homomorphism<C>& ch)
  const noexcept
  {
    return apply_visitor(*this, ch.h_->data());
  }
};

/*-------------------------------------------------------------------------------------------*/

}} // namespace sdd::hom

namespace std {

/*-------------------------------------------------------------------------------------------*/

/// @brief Hash specialization for sdd::hom::cached_homomorphism
template <typename C>
struct hash<sdd::hom::cached_homomorphism<C>>
{
  std::size_t
  operator()(const sdd::hom::cached_homomorphism<C>& op)
  const noexcept
  {
    std::size_t seed = 0;
    sdd::internal::util::hash_combine(seed, op.h_);
    sdd::internal::util::hash_combine(seed, op.sdd_);
    return seed;
  }
};

/// @endcond

/*-------------------------------------------------------------------------------------------*/

} // namespace std

#endif // _SDD_HOM_EVALUATION_HH_