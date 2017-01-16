

#include "../include/Tag.h"
#include <iostream>

Tag::Tag() {
  m_tag = "";
}

Tag::Tag(std::string tag, double weight = 0.0) : 
  m_tag(tag),
  m_weight(weight) {/**/}

std::string Tag::get_tag() const {
  return m_tag;
}

double Tag::get_weight() const {
  return m_weight;
}
const Tag& Tag::operator=(const Tag &rhs) {
  if (this != &rhs) {
    m_tag = rhs.m_tag;
    m_weight = 0;
  }
  return *this;
}

bool Tag::operator>(const Tag &t) const {
  return m_tag > t.m_tag;
}

bool Tag::operator<(const Tag &t) const {
  return m_tag < t.m_tag;
}

bool Tag::operator==(const Tag &t) const {
  return m_tag == t.m_tag;
}

bool Tag::operator!=(const Tag &t) const {
  return m_tag != t.m_tag;
}

std::ostream &operator<<(std::ostream &sout, const Tag &t) {
  sout << t.m_tag;
  sout << ": ";
  sout << t.m_weight;
  return sout;
}

