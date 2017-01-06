
/* 
 * Header file for movie class
 * 
 * Each Movie contains a string that represents a title, a string that represents its genres 
 * (seperated by pipes "|"), and a set of tags sorted by decreasing weight 
 *
 */

#ifndef MOVIE_H
#define MOVIE_H

#include <string>
#include <vector> 
#include <set>
#include "Tag.h"

class Movie {
 private:
  std::string m_title;
  std::string m_genres; 
  std::set<Tag, std::greater<Tag> > m_tags;

 public:
  Movie();
  Movie(std::string title, std::string genres);
  Movie(std::string title, std::string genres, std::set<Tag, std::greater<Tag> > tags);
  
  std::string get_title(void) const; 
  std::string get_genres(void) const;
  std::set<Tag, std::greater<Tag> > get_tags(void) const;

  bool operator<(const Movie &m) const;   
  friend std::ostream& operator<<(std::ostream &sout, const Movie &movie);
};


#endif
