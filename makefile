
.PHONY:all
all:parser debug  httpserver

parser:parser.cpp
	g++ parser.cpp -o parser -lboost_system -lboost_filesystem -ljsoncpp

debug:debug.cpp
	g++ debug.cpp -o debug -ljsoncpp

httpserver:server.cpp
	g++ server.cpp -o httpserver -ljsoncpp

.PHONY:clean
clean:
	rm -rf parser debug httpserver ./data/raw_html/raw.txt
	touch ./data/raw_html/raw.txt
	