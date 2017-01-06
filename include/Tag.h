
/* 
 * Header file for Tag class
 * 
 * Each tag contains a string that represents the actual tag, and a double that represents 
 * the frequency of the tag in it's movie's tag set
 *
 */

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

  const Tag& operator=(const Tag &rhs);
  bool operator>(const Tag &t) const;
  bool operator==(const Tag &t) const;
  friend std::ostream &operator<<(std::ostream &sout, const Tag &t);
    
};

#endif
