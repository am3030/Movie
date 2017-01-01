
/******************** THINGS TO ADD **************************
 * - Add tags to algorithm
 * - Make database bigger
 * - Sort the set of recommendations by number 
 *   of users who liked it, not alphabetically
 *   - Change recs to type map<Movie, int> to show how many
 *     times a movie has been "recommended"
 *   - May have to overload the > operator for Movie to make
 *     it easier to get the top recomendations depending on how
 *     map is implemented in the standard library 
 *  - Use TF-IDF to weight the tags
 *  - Clean up the tag table by removing all unique entries
 *    - If only one person ever gave a movie a tag, it isn't 
 *      important 
 *************************************************************/

#include <stdlib.h>
#include <iostream>
#include <set>
#include <map>

#include "Movie.h"
#include "mysql_connection.h"

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>

using namespace std;

int main(int argc, char *argv[]) {
  
  /* validate the number of command line arguments */
  if (argc != 2) {
    cout << "Usage: " << argv[0] << " <movie-title>" << endl;
    return EXIT_FAILURE;
  }

  try {

    sql::Connection *con;
    sql::Statement *stmt;
    sql::PreparedStatement *p_stmt;
    sql::ResultSet *res;    

    set<Movie> recs; 
    string genres = "%";
    string title = argv[1];
    vector<int> ids;    
    map<std::string, int> tag_counts;
    int movieId = 0;
    
    title += "%";

    /* establish connection to movie data database */
    con = get_driver_instance()->connect("tcp://127.0.0.1:3306", "root", "");
    stmt = con->createStatement();
    stmt->execute("USE movie");
    

    /* get the genres for the input movie */
    p_stmt = con->prepareStatement("SELECT * FROM movies WHERE title LIKE ? LIMIT 1");
    p_stmt->setString(1, title);
    res = p_stmt->executeQuery();
    if (res->next()) {
      genres += res->getString("genres");    
      movieId = res->getInt("movieId");
      genres += "%";
    }

    /* if the movie title wasn't found */
    else {
      cout << "No results found for '" << argv[1] << "'" << endl;
      return EXIT_SUCCESS;
    }      


    p_stmt = con->prepareStatement("SELECT * FROM tags WHERE movieID=?");
    p_stmt->setInt(1, movieId);
    res = p_stmt->executeQuery();
    while (res->next()) {
      /* insert the tag if its unique, otherwise increment a count for that tag
       * may need to use a custom data structure for this although I'm sure that the set can be used */
      
    }
    
    /* find which users liked the input movie and store their userIds */
    p_stmt = con->prepareStatement("SELECT * FROM ratings JOIN movies ON ratings.movieId=movies.movieId WHERE title LIKE ? AND rating >= 4.0");
    p_stmt->setString(1, title);    
    res = p_stmt->executeQuery();
    
    while (res->next()) 
      ids.push_back(res->getInt("userId"));

    /* for each user who also liked the input movie, see what other movies they liked that are related to the input movie
     *   - currently just looks at genres, need to add a way that compares genres AND tags */
    for (auto a : ids) {
      p_stmt = con->prepareStatement("SELECT * FROM ratings JOIN movies ON ratings.movieId=movies.movieId WHERE rating >= 4.0 AND userId=? AND genres LIKE ? AND title NOT LIKE ?");
      p_stmt->setInt(1, a);
      
      /* replace "|" with a "%" in genres */
      for (auto &c : genres)
      	if (c == '|')
      	  c = '%';
      
      /* complete the prepared statement and adds it to the set of recommendations  */
      p_stmt->setString(2, genres);
      p_stmt->setString(3, title);
      res = p_stmt->executeQuery();      
      while (res->next()) {
	unique_ptr<Movie> m(new Movie(res->getString("title"), res->getString("genres")));
	recs.insert(*m);	
      }
    }    
    
    /* prints out 10 recommendations at most - use when algorithm is complete
     * 
     * set<Movie>::iterator it2 = recs.begin();
     * for (int i = 0; it2 != recs.end() && i < 10 ; i++, it2++)
     *   cout << *it2 << endl;
     */

    for (auto m : recs)
      cout << m << endl;
    
    delete p_stmt;
    delete stmt;
    delete con;

  } 

  /* catches any SQL exceptions */
  catch (sql::SQLException &e) {
    cout << "# ERR: SQLException in " << __FILE__;
    cout << "(" << __FUNCTION__ << ") on line " 
	 << __LINE__ << endl;
    cout << "# ERR: " << e.what();
    cout << " (MySQL error code: " << e.getErrorCode();
    cout << ", SQLState: " << e.getSQLState() << 
      " )" << endl;
  }

  return EXIT_SUCCESS;
}
