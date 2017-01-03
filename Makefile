MovieRec: src/Tag.cpp src/Movie.cpp src/Driver.cpp 
	g++ -Wall -O3 -I/usr/include/cppconn -o bin/MovieRec src/Driver.cpp src/Tag.cpp src/Movie.cpp -L/usr/lib -lmysqlcppconn

clean: 
	rm bin/MovieRec
