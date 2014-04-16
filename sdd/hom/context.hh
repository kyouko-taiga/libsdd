#ifndef _SDD_HOM_CONTEXT_HH_
#define _SDD_HOM_CONTEXT_HH_

#include <memory> // make_shared, shared_ptr

#include "sdd/dd/context.hh"
#include "sdd/hom/context_fwd.hh"
#include "sdd/hom/definition_fwd.hh"
#include "sdd/hom/evaluation.hh"
#include "sdd/hom/evaluation_error.hh"
#include "sdd/hom/rewrite.hh"
#include "sdd/mem/cache.hh"

namespace sdd { namespace hom {

/*------------------------------------------------------------------------------------------------*/

/// @internal
/// @brief The evaluation context of homomorphisms.
///
/// Its purpose is to be able to create local caches at different points of the evaluation.
/// The cache is wrapped in a std::shared_ptr to enable cheap copy.
template <typename C>
class context
{
public:

  /// @brief Homomorphism evaluation cache type.
  using cache_type = mem::cache< context, cached_homomorphism<C>, evaluation_error<C>
                               , should_cache<C>>;

  /// @brief SDD operation context type.
  using sdd_context_type = sdd::dd::context<C>;

private:

  /// @brief The type of a unified SDD.
  using sdd_unique_type = typename SDD<C>::unique_type;

  /// @brief The type of a smart pointer to a unified SDD.
  using sdd_ptr_type = typename SDD<C>::ptr_type;

  /// @brief The set of a SDD.
  mem::unique_table<sdd_unique_type>& unique_table_;

  /// @brief Cache of union homomorphisms.
  std::shared_ptr<cache_type> cache_;

  /// @brief Context of SDD operations.
  ///
  /// It already implements cheap-copy, we don't need to use a shared_ptr.
  sdd_context_type sdd_context_;

public:

  /// @brief Construct a new context.
  context(mem::unique_table<sdd_unique_type>& ut, std::size_t size, sdd_context_type& sdd_cxt)
   	: unique_table_(ut)
    , cache_(std::make_shared<cache_type>(*this, "homomorphism_cache", size))
    , sdd_context_(sdd_cxt)
  {}

  /// @brief Copy constructor.
  context(const context&) = default;

  /// @brief Call the GC to remove unused SDD.
  void
  gc()
  noexcept
  {
    unique_table_.gc();
  }

  /// @brief Return the cache of homomorphism evaluation.
  cache_type&
  cache()
  noexcept
  {
    return *cache_;
  }

  /// @brief Return the context of SDD operations.
  sdd_context_type&
  sdd_context()
  noexcept
  {
    return sdd_context_;
  }

  /// @brief Remove all cache entries of this context.
  void
  clear()
  noexcept
  {
    cache_->clear();
  }
};

/*------------------------------------------------------------------------------------------------*/

}} // namespace sdd::hom

#endif // _SDD_HOM_CONTEXT_HH_
