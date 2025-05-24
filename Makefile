CXX = g++
CXXFLAGS = -std=c++11 -Wall -g

SRCS = Card.cpp Deck.cpp Hands.cpp ScoreCalculator.cpp Player.cpp Game.cpp main.cpp
OBJS = $(SRCS:.cpp=.o)
TARGET = OOPPokerGame

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -w -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean