MovieRec: src/Tag.cpp src/Movie.cpp src/driver_functions.cpp src/Driver.cpp 
	g++ -O3 -Wall -I/usr/include/cppconn -o bin/MovieRec src/Driver.cpp src/Tag.cpp src/Movie.cpp -L/usr/lib -lmysqlcppconn

clean: 
	rm bin/MovieRec
