#include "gtest/gtest.h"

#include "tests/hom/common.hh"
#include "tests/hom/common_inductives.hh"

/*-------------------------------------------------------------------------------------------*/

const SDD one = sdd::one<conf>();
const hom id = sdd::hom::Id<conf>();

struct hom_local_test
  : public testing::Test
{
};

/*-------------------------------------------------------------------------------------------*/

TEST_F(hom_local_test, construction)
{
  order<conf> o;
  {
    ASSERT_EQ(id, Local("0", o, id));
  }
  {
    const hom h1 = Local("0", o, Inductive<conf>(targeted_incr("0",1)));
    const hom h2 = Local("0", o, Inductive<conf>(targeted_incr("0",1)));
    ASSERT_EQ(h1, h2);
  }
  {
    const hom h1 = Local("0", o, Inductive<conf>(targeted_incr("0",1)));
    const hom h2 = Local("0", o, Inductive<conf>(targeted_incr("0",2)));
    ASSERT_NE(h1, h2);
  }
}

/*-------------------------------------------------------------------------------------------*/

TEST_F(hom_local_test, evaluation)
{
  order<conf> p {"a"};
  order<conf> q {"b"};
  order<conf> o;
  o.add("y", q);
  o.add("x", p);
  const SDD s0 = SDD(0, SDD('a', {0}, one), SDD(1, SDD('b', {1}, one), one));
  const hom h1 = Local("x", o, Inductive<conf>(targeted_incr("a",1)));
  ASSERT_EQ( SDD(1, SDD(0, {1}, one), SDD(0, SDD(0, {1}, one), one))
           , h1(o, s0));
  const hom h2 = Local("y", o, Inductive<conf>(targeted_incr("b",1)));
  ASSERT_EQ( SDD(0, SDD('a', {0}, one), SDD(1, SDD('b', {2}, one), one))
           , h2(o, s0));
}

/*-------------------------------------------------------------------------------------------*/

//TEST_F(hom_local_test, error)
//{
//  const SDD s0 = SDD('a', {0}, SDD('b', {1}, one));
//  const hom h1 = Local('c', Inductive<conf>(targeted_incr('x',1)));
//  ASSERT_THROW(h1(s0), sdd::hom::evaluation_error<conf>);
//  try
//  {
//    h1(s0);
//  }
//  catch (sdd::hom::evaluation_error<conf>& e)
//  {
//    ASSERT_EQ(one, e.operand());
//    ASSERT_NE(nullptr, e.what());
//  }
//}


/*-------------------------------------------------------------------------------------------*/
