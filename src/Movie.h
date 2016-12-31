
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
  
  /* tags need to be implemented */
  std::set<Tag, std::greater<Tag> > m_tags;

 public:
  Movie();
  Movie(std::string title, std::string genres);

  /* this constructor isn't being used yet; need to implement tags */
  Movie(std::string title, std::string genres, std::set<Tag, std::greater<Tag> > tags);
  
  std::string get_title() const; 
  std::string get_genres() const;

  bool operator<(const Movie &m) const;   
  friend std::ostream& operator<<(std::ostream &sout, const Movie &movie);
};


#endif
