/// \file label/string_label.h
///
/// \details
/// Contains the declaration of the StringLabel class (represents string data
/// which is associated with a Node). For example, if one wants a tree with
/// strings as labels, one could use this class as a template argument for the
/// Node class to accomplish this.

#pragma once

#include <string>

namespace label {

class StringLabel {
  
public:
    StringLabel(const std::string& label);

    /// Operator overloadings.
    /// @{
    bool operator==(const StringLabel& other) const;
    /// @}

    /// Generates a string representation of the label.
    ///
    /// \return String representation of the label.
    const std::string& to_string() const;

private:
    /// The label to be associated with a node.
    std::string label_{};
};

// Implementation details
#include "string_label_impl.h"

} // namespace label

// Custom specialization of std::hash - injected in namespace std.
// This is needed for LabelDictionary.
namespace std
{
  template<> struct hash<label::StringLabel> {
    typedef label::StringLabel argument_type;
    typedef std::size_t result_type;
    result_type operator()(argument_type const& s) const noexcept {
        result_type const h ( std::hash<std::string>{}(s.to_string()) );
        return h;
    }
  };
}
