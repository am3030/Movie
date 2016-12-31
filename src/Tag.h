
#ifndef TAG_H
#define TAG_H

#include <string>

class Tag {
 private:
  std::string m_tag;
  double m_weight;
  
 public:
  Tag();
  Tag(std::string tag, double weight);

  bool operator<(const Tag &t) const;
  bool operator==(const Tag &t) const;
  friend std::ostream &operator<<(std::ostream &sout, const Tag &t);
    
};

#endif
