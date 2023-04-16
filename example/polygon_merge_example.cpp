
#include <PolygonMerge.h>

#include <algorithm>
#include <glm/gtx/io.hpp>
#include <iostream>


int main(int argc, const char** argv) {
  std::vector<glm::vec2> edge_list{
      {10, 10},
      {100, 10},
      {10, 100},
      {10, 10},
  };

  std::vector<glm::vec2> edge_list2{
      {100, 10},
      {10, 100},
      {100, 100},
      {100, 10},
  };


  TRM::PolygonMerge merge{};

  merge.do_merge(edge_list, edge_list2);


  return 0;
}