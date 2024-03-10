all: mc/cpp/pocket.cpp
	g++ mc/cpp/pocket.cpp --std=c++2a -lpthread -o pocket -Wall -Wextra -Wpedantic -Wconversion -g -O3
test: mc/cpp/pocket.cpp
	g++ mc/cpp/pocket.cpp --std=c++2a -lpthread -o pocket_test -Wall -Wextra -Wpedantic -Wconversion -g -DTEST -D _GLIBCXX_DEBUG
	./pocket_test
