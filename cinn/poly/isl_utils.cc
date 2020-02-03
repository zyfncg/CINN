#include "cinn/poly/isl_utils.h"

namespace cinn {
namespace poly {

std::vector<std::string> GetDimNames(const isl::set &x) {
  std::vector<std::string> res;
  for (int i = 0; i < isl_set_dim(x.get(), isl_dim_set); i++) {
    res.push_back(isl_set_get_dim_name(x.get(), isl_dim_set, i));
  }
  return res;
}

}  // namespace poly
}  // namespace cinn
