#pragma once
using namespace label;


StringLabel::StringLabel(const std::string& label) : label_(label) {}

// const std::string& StringLabel::label() const {
//   return label_;
// }

bool StringLabel::operator==(const StringLabel& other) const {
  return (label_.compare(other.to_string()) == 0);
}

const std::string& StringLabel::to_string() const {
  return label_;
}
