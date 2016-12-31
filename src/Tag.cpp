

#include "Tag.h"

Tag::Tag() {
  m_tag = "";
}

Tag::Tag(std::string tag, double weight = 0.0) : 
  m_tag(tag),
  m_weight(weight) {/**/}

bool Tag::operator<(const Tag &t) const {
  return m_tag < t.m_tag;
}

bool Tag::operator==(const Tag &t) const {
  return m_tag == t.m_tag;
}

std::ostream &operator<<(std::ostream &sout, const Tag &t) {
  sout << t.m_tag;
  return sout;
}
