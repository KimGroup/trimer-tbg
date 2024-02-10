all: mc/pocket.cpp
	g++ mc/pocket.cpp --std=c++2a -lpthread -O3 -o pocket -Wall -Wextra -Wpedantic -Wconversion -g
test: mc/pocket.cpp
	g++ mc/pocket.cpp --std=c++2a -lpthread -o pocket_test -Wall -Wextra -Wpedantic -Wconversion -g -DTEST -D _GLIBCXX_DEBUG
	./pocket_test
