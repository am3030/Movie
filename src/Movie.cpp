
#include "../include/Movie.h"

Movie::Movie() : m_title(""), m_genres("") {/**/}

Movie::Movie(std::string title, std::string genres) : 
  m_title(title),
  m_genres(genres) {/**/}

Movie::Movie(std::string title, std::string genres, std::set<Tag, std::greater<Tag> > tags) : 
  m_title(title),
  m_genres(genres),
  m_tags(tags) {/**/}

std::string Movie::get_title(void) const {
  return m_title;
}

std::string Movie::get_genres(void) const {
  return m_genres;
}

std::set<Tag, std::greater<Tag> > Movie::get_tags(void) const {
  return m_tags;
}

bool Movie::operator<(const Movie &m) const {
  return m_title < m.m_title;
}

std::ostream& operator<<(std::ostream &sout, const Movie &movie) {
  sout << movie.m_title;
  return sout;
}
