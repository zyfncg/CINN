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

#pragma once

#include "cinn/hlir/framework/graph.h"
#include "cinn/utils/type_defs.h"
#include "cinn/hlir/pass/fusion_helper_base.h"

namespace cinn {
namespace api {

class OpNode;

using shape_t = utils::ShapeType;

class TensorNode {
 public:
  TensorNode(const hlir::framework::NodeData* node_data, const hlir::framework::Graph* graph) : node_data_(node_data), graph_(graph) {}

  // Get the shape of tensor.
  const shape_t& Shape() const {
    const auto& shape_dict = graph_->GetAttrs<absl::flat_hash_map<std::string, shape_t>>("infershape");
    CHECK(shape_dict.count(node_data_->id())) << "Can't find " << node_data_->id() << " 's shape!";
    return shape_dict.at(node_data_->id());
  }

  OpNode Producer() const;

  class ConsumerOpListView {
   public:
    ConsumerOpListView(const std::set<common::Shared<common::GraphEdge>, common::GraphEdgeCompare>& edges, const hlir::framework::Graph* graph) : edges_(edges), graph_(graph) {}

    ConsumerOpListView(const ConsumerOpListView& other) = delete;
    ConsumerOpListView(ConsumerOpListView&& other) = delete;

    class Iterator {
     public:
      Iterator(std::set<common::Shared<common::GraphEdge>, common::GraphEdgeCompare>::const_iterator it, const hlir::framework::Graph* graph) : iter_(it), graph_(graph) {}

      Iterator& operator++() {
        ++iter_;
        return *this;
      }

      Iterator operator++(int) {
        Iterator tmp = *this;
        ++iter_;
        return tmp;
      }

      bool operator==(const Iterator& other) const {
        return iter_ == other.iter_;
      }

      bool operator!=(const Iterator& other) const {
          return !(*this == other);
      }

      OpNode operator*() const;

     private:
      std::set<common::Shared<common::GraphEdge>, common::GraphEdgeCompare>::const_iterator iter_;
      const hlir::framework::Graph* graph_;
    };

    size_t size() const { return edges_.size(); }

    Iterator begin() const {
      return  Iterator(this->edges_.begin(), graph_);
    }

    Iterator end() const {
      return  Iterator(this->edges_.end(), graph_);
    }

   private:
    const std::set<Shared<common::GraphEdge>, common::GraphEdgeCompare>& edges_;
    const hlir::framework::Graph* graph_;
  };

  size_t ConsumerSize() const {
    return node_data_->outlinks().size();
  }

  ConsumerOpListView Consumers() const {
    return ConsumerOpListView(node_data_->outlinks(), graph_);
  }

 private:
  const hlir::framework::NodeData* node_data_;
  const hlir::framework::Graph* graph_;
};

}  // namespace api
}  // namespace cinn