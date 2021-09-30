
#pragma once

namespace candidate_index {

/// This is an element holding data of a set.
struct SetData {
  /// Overlap so far for a specific set.
  int overlap = 0;
  /// Index prefix for this set to avoid recomputations.
  int prefix = 0;
  /// Constructor.
  SetData();
  /// Constructor.
  SetData(int overlap, int prefix);
};

candidate_index::SetData::SetData() {}

candidate_index::SetData::SetData(int overlap, int prefix)
    : overlap(overlap), prefix(prefix) {}

}
