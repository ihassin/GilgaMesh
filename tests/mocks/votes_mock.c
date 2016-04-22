#include "cmocka_includes.h"

bool save_vote(uint16_t voterId)
{
  check_expected(voterId);
  return mock_type(bool);
}

uint16_t get_vote_count()
{
  return mock_type(uint16_t);
}