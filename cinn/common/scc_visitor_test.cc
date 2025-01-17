// Copyright (c) 2023 CINN Authors. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "cinn/common/scc_visitor.h"

#include <glog/logging.h>
#include <gtest/gtest.h>

namespace cinn {
namespace common {

TEST(SccVisitor, trivial) {
  std::list<std::pair<int, int>> edges{{0, 3}, {1, 2}, {1, 3}, {2, 4}, {3, 4}};

  SccVisitor<int> visitor(
      [&](int node, const std::function<void(int)>& NodeHandler) {
        for (const auto& pair : edges) {
          if (pair.second == node) {
            NodeHandler(pair.first);
          }
        }
      },
      [&](int node, const std::function<void(int)>& NodeHandler) {
        for (const auto& pair : edges) {
          if (pair.first == node) {
            NodeHandler(pair.second);
          }
        }
      });
  std::vector<int> sources{0, 1};
  std::vector<std::vector<int>> outputs;
  visitor(sources.begin(), sources.end(), [&](const auto& nodes) { outputs.push_back(nodes); });
  std::vector<std::vector<int>> expected{{1}, {2}, {0}, {3}, {4}};
  EXPECT_TRUE((outputs == expected));
}

TEST(SccVisitor, circle) {
  std::list<std::pair<int, int>> edges{
      {0, 1},
      {1, 2},
      {2, 3},
      {3, 4},
      {4, 0},
  };

  SccVisitor<int> visitor(
      [&](int node, const std::function<void(int)>& NodeHandler) {
        for (const auto& pair : edges) {
          if (pair.second == node) {
            NodeHandler(pair.first);
          }
        }
      },
      [&](int node, const std::function<void(int)>& NodeHandler) {
        for (const auto& pair : edges) {
          if (pair.first == node) {
            NodeHandler(pair.second);
          }
        }
      });
  std::vector<int> sources{0};
  std::vector<std::vector<int>> outputs;
  visitor(sources.begin(), sources.end(), [&](const auto& nodes) { outputs.push_back(nodes); });
  std::vector<std::vector<int>> expected{{0, 4, 3, 2, 1}};
  EXPECT_TRUE((outputs == expected));
}

TEST(SccVisitor, double_circle) {
  std::list<std::pair<int, int>> edges{
      {0, 1},
      {1, 0},
      {1, 2},
      {2, 3},
      {3, 2},
  };

  SccVisitor<int> visitor(
      [&](int node, const std::function<void(int)>& NodeHandler) {
        for (const auto& pair : edges) {
          if (pair.second == node) {
            NodeHandler(pair.first);
          }
        }
      },
      [&](int node, const std::function<void(int)>& NodeHandler) {
        for (const auto& pair : edges) {
          if (pair.first == node) {
            NodeHandler(pair.second);
          }
        }
      });
  std::vector<int> sources{0};
  std::vector<std::vector<int>> outputs;
  visitor(sources.begin(), sources.end(), [&](const auto& nodes) { outputs.push_back(nodes); });
  std::vector<std::vector<int>> expected{{0, 1}, {2, 3}};
  EXPECT_TRUE((outputs == expected));
}

}  // namespace common
}  // namespace cinn