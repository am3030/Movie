
/******************** THINGS TO ADD **************************
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
 *************************************************************/

#include "../include/driver_functions.h"

using namespace std;


bool comp(pair<Movie, double> &a, pair<Movie, double>& b) {
  return a.second < b.second;
}

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

    map<Movie, double> recs; 
    string title = argv[1];
    string genres;
    vector<int> ids;    
    set<Tag, greater<Tag> > tags;
    int movieId = 0;
    
    /* this is so that when the title is looked up in the database, it'll return a result even if 
     * the year of the movie isn't included (which normal people wouldn't type in) */
    title += "%";

    /* establish connection to movie data database */
    con = get_driver_instance()->connect("tcp://127.0.0.1:3306", "root", "");
    stmt = con->createStatement();
    stmt->execute("USE movie");
    
    /* get the data for the input movie */
    p_stmt = con->prepareStatement("SELECT * FROM movies WHERE title LIKE ? LIMIT 1;");
    p_stmt->setString(1, title);
    res = p_stmt->executeQuery();
    if (res->next()) {
      title = res->getString("title");
      genres = res->getString("genres");    
      movieId = res->getInt("movieId");
    }

    /* if the movie title wasn't found */
    else {
      cout << "No results found for '" << argv[1] << "'" << endl;
      return EXIT_SUCCESS;
    }      

    /* get the tags and genres for the input movie and create a Movie object for it */
    tags = retrieve_tags(con, movieId);
    unique_ptr<Movie> input(new Movie(title, genres, retrieve_tags(con, movieId)));
    vector<string> genre_list;    
    retrieve_genres(input->get_genres(), back_inserter(genre_list));
    sort(genre_list.begin(), genre_list.end());

    /* find which users liked the input movie and store their userIds */
    p_stmt = con->prepareStatement("SELECT * FROM ratings JOIN movies ON ratings.movieId=movies.movieId WHERE title LIKE ? AND rating >= 4.0 LIMIT 50;");
    p_stmt->setString(1, title);    
    res = p_stmt->executeQuery();    
    while (res->next()) 
      ids.push_back(res->getInt("userId"));
    
    /* for each user who also liked the input movie, see what other movies they liked that are related to the input movie
     *   - currently just looks at genres, need to add a way that compares genres AND tags */
    for (auto a : ids) {
      p_stmt = con->prepareStatement("SELECT * FROM ratings JOIN movies ON ratings.movieId=movies.movieId WHERE rating >= 4.0 AND userId=? AND title NOT LIKE ?;");
      p_stmt->setInt(1, a);    
      p_stmt->setString(2, title);
      res = p_stmt->executeQuery();      

      /* for each movie, make a movie object, and base the recommendation based off how similar the genres are using the Jaccard Similarity Index */
      while (res->next()) {		
	vector<string> genre_list_loop, genre_intersection, genre_union;
	unique_ptr<Movie> m(new Movie(res->getString("title"), res->getString("genres"), retrieve_tags(con, res->getInt("movieId"))));	
	retrieve_genres(m->get_genres(), back_inserter(genre_list_loop));
	sort(genre_list_loop.begin(), genre_list_loop.end());
	set_intersection(genre_list.begin(), genre_list.end(), genre_list_loop.begin(), genre_list_loop.end(), back_inserter(genre_intersection));
	set_union(genre_list.begin(), genre_list.end(), genre_list_loop.begin(), genre_list_loop.end(), back_inserter(genre_union));
	if (genre_union.size())
	  recs.insert(pair<Movie, double>(*m, (double) genre_intersection.size() / genre_union.size())); 
      }
    }    

    /* for each recommendation, compare the tags */
    for (auto &s : recs) {
      // set<Tag, greater<Tag> > test_tags = s.first.get_tags();    
      // set<Tag, greater<Tag> > diffs;
      // set_join(tags.begin(), tags.end(), test_tags.begin(), test_tags.end(), inserter(tags, tags.begin()), greater<Tag>());
      
      /* for now, just does a quick Jaccard Similarity Index; this isn't ideal because it doesn't take Tag weights into consideration
       * I imagine it'll still give decent results, but the cosine similarity will perform better 
       */
      set<Tag, greater<Tag> > test_tags = s.first.get_tags();     
      vector<Tag> tag_intersection, tag_union; 
      set_intersection(test_tags.begin(), test_tags.end(), tags.begin(), tags.end(), back_inserter(tag_intersection), greater<Tag>());
      set_union(test_tags.begin(), test_tags.end(), tags.begin(), tags.end(), back_inserter(tag_union), greater<Tag>());
      if (tag_union.size())
	s.second += (double) tag_intersection.size() / (double) tag_union.size();      

      /* PERFORM COSINE SIMILARITY HERE */
    }      
    
    /* display results in ascending order */
    vector<pair<Movie, double> > movies;
    for (auto m : recs)
      movies.push_back(m);
    sort(movies.begin(), movies.end(), comp);
    for (auto m : movies)
      if (m.second > 0.5)
	cout << m.first << ": " << m.second << endl;
    
    if (!movies.size())
      cout << "Sorry, not enough data has been collected for this movie to accurately give recommendations" << endl;

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
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
