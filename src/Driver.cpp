
/******************** THINGS TO ADD **************************
 * - Add tags to algorithm
 * - Sort the set of recommendations by number
 *   of users who liked it, not alphabetically
 *   - Change recs to type map<Movie, int> to show how many
 *     times a movie has been "recommended"
 *   - May have to overload the > operator for Movie to make
 *     it easier to get the top recomendations depending on how
 *     map is implemented in the standard library 
 * 
 *    * DONE, but it doesn't seem relevant to what movies 
 *      are related. It just shows more popular movies
 *    
 *  - May use cosine similarity to compare a set of tags
 *    - May be costly, but it's probably the best way to compare 
 *      how similar two sets of tags are
 *  - Consider using the Jaccard similarity instead of TF-IDF
 *    - Delete all tags that only one or two people gave
 *      - If one person just gives a bunch of movies a certain 
 *        tag, it's not really important because no one else 
 *        believes that the tag fits the movie either
 *    - Stem all of the tags to normalize them
 *      - Could be done in the database to save time, or
 *      - Could be done in the Driver so that new tags could 
 *        be added easily (but at the same time, stemming could
 *        be done when the tags are added
 *  - Can use Jaccard Index to find similar genres
 *************************************************************/

#include "../include/driver_functions.h"

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

    map<Movie, int> recs; 
    string genres = "%";
    string title = argv[1];
    vector<int> ids;    
    set<Tag, greater<Tag> > tags;
    int movieId = 0;
    
    title += "%";

    /* establish connection to movie data database */
    con = get_driver_instance()->connect("tcp://127.0.0.1:3306", "root", "");
    stmt = con->createStatement();
    stmt->execute("USE movie");
    
    /* get the data for the input movie */
    p_stmt = con->prepareStatement("SELECT * FROM movies WHERE title LIKE ? LIMIT 1");
    p_stmt->setString(1, title);
    res = p_stmt->executeQuery();
    if (res->next()) {
      title = res->getString("title");
      genres += res->getString("genres");    
      movieId = res->getInt("movieId");
    }

    /* if the movie title wasn't found */
    else {
      cout << "No results found for '" << argv[1] << "'" << endl;
      return EXIT_SUCCESS;
    }      

    /* get the tags and genres for the input movie and create a Movie object for it */
    tags = retrieve_tags(con, movieId);
    unique_ptr<Movie> input(new Movie(title, genres, tags));
    vector<string> genre_list;
    retrieve_genres(input->get_genres(), back_inserter(genre_list));
    
    /* find which users liked the input movie and store their userIds */
    genres += "%";
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

      for (auto &c : genres)
      	if (c == '|')
      	  c = '%';
      
      /* complete the prepared statement and adds it to the set of recommendations  */
      p_stmt->setString(2, genres);
      p_stmt->setString(3, title);
      res = p_stmt->executeQuery();      
      while (res->next()) {	
	unique_ptr<Movie> m(new Movie(res->getString("title"), res->getString("genres"), retrieve_tags(con, res->getInt("movieId"))));
	if (!(recs.insert(pair<Movie, int>(*m, 1)).second)) recs[*m]++;			
      }
    }    
      

    /* the final step is to sort all movies based on how close their tags are to the input movies using Cosine Similarity */  
    /* for each recommendation, compare the tags */
    for (auto s : recs) {
      set<Tag, greater<Tag> > test_tags = s.first.get_tags();    
      set<Tag, greater<Tag> > diffs;
      set_join(tags.begin(), tags.end(), test_tags.begin(), test_tags.end(), inserter(tags, tags.begin()), greater<Tag>());
      /* perform cosine similarity here */
    }      
          
     for (auto m : recs)
      cout << m.first << ": " << m.second << endl;
    
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
