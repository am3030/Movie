#ifndef DRIVER_FUNCTIONS_H
#define DRIVER_FUNCTIONS_H

#include <stdlib.h>
#include <iostream>
#include <set>
#include <map>
#include <algorithm>

#include "Movie.h"

#include "mysql_connection.h"

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>

/* set_match() matches the two sets by making sure all entries from first1 and first2 are in result
 * result may overlap with first1 */
template <class InputIterator, class OutputIterator, class Compare>
OutputIterator set_join(InputIterator first1, InputIterator last1, 
			InputIterator first2, InputIterator last2, 
			OutputIterator result, Compare comp)
{
  while (true) {
    if (first1 == last1) return copy(first2, last2, result);
    if (first2 == last2) return result;
    
    if (comp(*first1, *first2))
      *first1++;      
    /* result has to have a weight of 0; probably have to create a new Tag object
     * so we don't mess up the second set 
     *
     * have to remove InputIterator if we do this to say that this function ONLY accepts
     * objects of type set<Tag, greater<Tag> >::iterator
     */
    else if (comp(*first2 ,*first1))      
      *result++ = *first2++;
    else {
      *first1++; *first2++;
    }
  }  
}

/* retrieve_genres() takes a genre string delimited by "|" and fills the container passed
 * with all the genres. result needs to be an iterator for a templated container of type
 * string
 */
template <class OutputIterator>
OutputIterator retrieve_genres(std::string genres, OutputIterator result) {
  std::string genre;
  for (char c : genres) {
    if (c != '|') genre += c;
    else { *result++ = genre; genre=""; }  
  }
  *result++ = genre;
  return result;
}

/* retrieve_tags() returns a map that pairs all the tags for the movie with id movieId
 *   in the movie database with their respective counts. 
 *   doesn't account for stemming yet, only normalizes the case (upper and lower) */
std::set<Tag, std::greater<Tag> > retrieve_tags(sql::Connection *con, int movieId) {
  sql::PreparedStatement *p_stmt;
  sql::ResultSet *res;
  std::map<std::string, double> tag_counts;
  std::set<Tag, std::greater<Tag> > tags;  
  int total = 0;
  
  try {
    p_stmt = con->prepareStatement("SELECT * FROM tags WHERE movieID=?");
    p_stmt->setInt(1, movieId);
    res = p_stmt->executeQuery();
    while (res->next()) {
      std::string tag = res->getString("tag");
      total++;
      std::transform(tag.begin(), tag.end(), tag.begin(), ::tolower);
      if (!(tag_counts.insert(std::pair<std::string, int>(tag, 1))).second)
	tag_counts[tag]++;      
    }
    /* finds the weight of the tag. The weight needs to be used instead 
     * of the count, because otherwise there would be a bias towards movies
     * that were tagged more frequently i.e. more popular */
    for (auto &a : tag_counts) {
      a.second /= total;
      tags.insert(Tag(a.first, a.second));
    }
    
  }
  catch (sql::SQLException &e) {
    std::cout << e.what() << std::endl;
  }
  
  return tags;
}

#endif
