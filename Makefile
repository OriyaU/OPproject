# Compiler settings
CXX = g++ -g -std=c++17 -Wall -Wextra -pthread -fprofile-arcs -ftest-coverage
CXXFLAGS = -std=c++17 -Wall -Wextra -g -gdwarf-4 -pthread -fprofile-arcs -ftest-coverage

# Target executable
TARGET = server

# Source files
SRCS = server.cpp edge.cpp graph.cpp LeaderFollowerPool.cpp MST_Factory.cpp MST.cpp MSTTree.cpp MST_strategy.cpp Pipeline.cpp unionfind.cpp

# Object files
OBJS = $(SRCS:.cpp=.o)

# Default target
all: $(TARGET)

# Link target executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

# Compile individual object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean generated files
clean:
	rm -f $(OBJS) $(TARGET) *.gcno *.gcda





# CXX = g++ -g -std=c++17 -Wall -Wextra -pthread
# CXXFLAGS = -std=c++17 -Wall -Wextra -g -pthread --coverage
# LDFLAGS = --coverage

# TARGET = server

# SRCS = server.cpp edge.cpp graph.cpp LeaderFollowerPool.cpp MST_Factory.cpp MST.cpp MSTTree.cpp MST_strategy.cpp Pipeline.cpp unionfind.cpp
# OBJS = $(SRCS:.cpp=.o)

# all: $(TARGET)

# $(TARGET): $(OBJS)
# 	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $(TARGET) $(OBJS)

# %.o: %.cpp
# 	$(CXX) $(CXXFLAGS) -c $< -o $@

# clean:
# 	rm -f $(OBJS) $(TARGET) *.gcno *.gcda *.html *.css

