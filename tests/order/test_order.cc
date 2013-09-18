#include <algorithm> // shuffle, sort
#include <random>

#include "gtest/gtest.h"

#include "sdd/order/order.hh"

#include "tests/configuration.hh"

/*-------------------------------------------------------------------------------------------*/

template <typename C>
struct order_test
  : public testing::Test
{
  typedef C configuration_type;
};

/*------------------------------------------------------------------------------------------------*/

TYPED_TEST_CASE(order_test, configurations);
#include "tests/macros.hh"

/*-------------------------------------------------------------------------------------------*/

TYPED_TEST(order_test, builder)
{
  {
    order_builder ob;
    ASSERT_TRUE(ob.empty());
  }
  {
    order_builder ob {"a"};
    ASSERT_EQ("a", ob.identifier());
    ASSERT_TRUE(ob.nested().empty());
    ASSERT_TRUE(ob.next().empty());
  }
  {
    order_builder ob {"a", "b"};
    ASSERT_EQ("a", ob.identifier());
    ASSERT_TRUE(ob.nested().empty());
    ASSERT_FALSE(ob.next().empty());
    ASSERT_EQ("b", ob.next().identifier());
    ASSERT_TRUE(ob.next().next().empty());
  }
  {
    order_builder ob;
    ob.push("b").push("a");
    ASSERT_EQ("a", ob.identifier());
    ASSERT_TRUE(ob.nested().empty());
    ASSERT_FALSE(ob.next().empty());
    ASSERT_EQ("b", ob.next().identifier());
    ASSERT_TRUE(ob.next().next().empty());
  }
  {
    order_builder ob;
    ob.push("y", order_builder {"b"}).push("x", order_builder {"a"});

    ASSERT_EQ("x", ob.identifier());
    ASSERT_FALSE(ob.nested().empty());
    ASSERT_FALSE(ob.next().empty());
    ASSERT_EQ("a", ob.nested().identifier());
    ASSERT_TRUE(ob.nested().nested().empty());
    ASSERT_TRUE(ob.nested().next().empty());

    ASSERT_EQ("y", ob.next().identifier());
    ASSERT_FALSE(ob.next().nested().empty());
    ASSERT_TRUE(ob.next().next().empty());
    ASSERT_EQ("b", ob.next().nested().identifier());
    ASSERT_TRUE(ob.next().nested().nested().empty());
    ASSERT_TRUE(ob.next().nested().next().empty());
  }
}

/*-------------------------------------------------------------------------------------------*/

TYPED_TEST(order_test, constructed_order)
{
  {
    order o(order_builder {});
    ASSERT_TRUE(o.empty());
  }
  {
    order o(order_builder {"0","1","2"});
    ASSERT_FALSE(o.empty());
    ASSERT_FALSE(o.next().empty());
    ASSERT_FALSE(o.next().next().empty());
    ASSERT_TRUE(o.next().next().next().empty());
    ASSERT_TRUE(o.nested().empty());
    ASSERT_TRUE(o.next().nested().empty());
    ASSERT_TRUE(o.next().next().nested().empty());
    ASSERT_EQ("0", o.identifier());
    ASSERT_EQ("1", o.next().identifier());
    ASSERT_EQ("2", o.next().next().identifier());
 }
  {
    order o(order_builder().push("y", order_builder {"c"})
                           .push("x", order_builder().push("z", order_builder {"b"}))
                           .push("a"));

    const std::vector<std::string> ordered_identifiers {"a", "x", "z", "b", "y", "c"};

    std::vector<std::string> identifiers {"a", "b", "c", "x", "y", "z"};
    std::shuffle(identifiers.begin(), identifiers.end(), std::mt19937(std::random_device()()));
    std::sort( identifiers.begin(), identifiers.end()
             , [&o](const std::string& lhs, const std::string& rhs)
                   {
                     return o.compare(lhs, rhs);
                   }
             );

    ASSERT_EQ(ordered_identifiers, identifiers);

    ASSERT_TRUE(o.contains("y", "c"));
    ASSERT_TRUE(o.contains("x", "z"));
    ASSERT_TRUE(o.contains("x", "b"));
    ASSERT_TRUE(o.contains("z", "b"));

    ASSERT_FALSE(o.contains("y", "b"));
    ASSERT_FALSE(o.contains("x", "c"));
    ASSERT_FALSE(o.contains("x", "y"));
    ASSERT_FALSE(o.contains("x", "x"));
    ASSERT_FALSE(o.contains("a", "y"));
    ASSERT_FALSE(o.contains("a", "a"));

    ASSERT_FALSE(o.empty());
    ASSERT_EQ("a", o.identifier());
    ASSERT_TRUE(o.nested().empty());

    ASSERT_FALSE(o.next().empty());
    ASSERT_EQ("x", o.next().identifier());
    ASSERT_FALSE(o.next().nested().empty());
    ASSERT_EQ("z", o.next().nested().identifier());
    ASSERT_TRUE(o.next().nested().next().empty());
    ASSERT_FALSE(o.next().nested().nested().empty());
    ASSERT_EQ("b", o.next().nested().nested().identifier());
    ASSERT_TRUE(o.next().nested().nested().next().empty());
    ASSERT_TRUE(o.next().nested().nested().nested().empty());
 }
}

/*-------------------------------------------------------------------------------------------*/
