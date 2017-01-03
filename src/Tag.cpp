

#include "../include/Tag.h"

Tag::Tag() {
  m_tag = "";
}

Tag::Tag(std::string tag, double weight = 0.0) : 
  m_tag(tag),
  m_weight(weight) {/**/}

bool Tag::operator>(const Tag &t) const {
  return m_weight > t.m_weight;
}

bool Tag::operator==(const Tag &t) const {
  return m_weight == t.m_weight;
}

std::ostream &operator<<(std::ostream &sout, const Tag &t) {
  sout << t.m_tag;
  return sout;
}
