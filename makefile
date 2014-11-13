FLAGS= g++ -c -Os -Wall
LINK= g++ -Wall -o "LOCAL_horizon" 
LINKLIBS= -lbase2.0 -lsfml-graphics -lsfml-window -lsfml-system
horizon: need.o agent.o species.o main.o
	@echo -----------Linking horizon-------------------------
	$(LINK) need.o agent.o species.o main.o $(LINKLIBS)
	mv *.o LOCAL_obj
need.o: src/agent/need.hpp src/agent/need.cpp
	$(FLAGS) src/agent/need.hpp
	$(FLAGS) src/agent/need.cpp
agent.o: src/agent/agent.hpp src/agent/agent.cpp
	$(FLAGS) src/agent/agent.hpp
	$(FLAGS) src/agent/agent.cpp
species.o: src/agent/species.hpp src/agent/species.cpp
	$(FLAGS) src/agent/species.hpp
	$(FLAGS) src/agent/species.cpp
main.o: src/main.cpp
	$(FLAGS) src/main.cpp
clean:
	rm *.o
	rm *.gch
