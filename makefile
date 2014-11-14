FLAGS= g++ -c -Os -Wall
EXECUTABLE_NAME= LOCAL_horizon
OBJ_DIR= LOCAL_obj
LINK= g++ -Wall -o "$(EXECUTABLE_NAME)" 
LINKLIBS= -lbase2.0 -lsfml-graphics -lsfml-window -lsfml-system
horizon: $(OBJ_DIR)/coord.o $(OBJ_DIR)/time.o $(OBJ_DIR)/needMath.o $(OBJ_DIR)/need.o $(OBJ_DIR)/process.o $(OBJ_DIR)/processMap.o $(OBJ_DIR)/agent.o $(OBJ_DIR)/needProcessor.o $(OBJ_DIR)/processDirectory.o $(OBJ_DIR)/species.o $(OBJ_DIR)/main.o
	@echo -----------Linking horizon-------------------------
	mv *.o $(OBJ_DIR)
	(cd $(OBJ_DIR) && $(LINK) coord.o time.o needMath.o need.o process.o processMap.o agent.o needProcessor.o processDirectory.o species.o main.o $(LINKLIBS))
	mv $(OBJ_DIR)/$(EXECUTABLE_NAME) .

$(OBJ_DIR)/coord.o: src/world/coord.hpp src/world/coord.cpp
	$(FLAGS) src/world/coord.hpp
	$(FLAGS) src/world/coord.cpp
$(OBJ_DIR)/time.o: src/world/time.hpp src/world/time.cpp
	$(FLAGS) src/world/time.hpp
	$(FLAGS) src/world/time.cpp
$(OBJ_DIR)/needMath.o: src/agent/needMath.hpp src/agent/needMath.cpp
	$(FLAGS) src/agent/needMath.hpp
	$(FLAGS) src/agent/needMath.cpp	
$(OBJ_DIR)/need.o: src/agent/need.hpp src/agent/need.cpp
	$(FLAGS) src/agent/need.hpp
	$(FLAGS) src/agent/need.cpp
$(OBJ_DIR)/process.o: src/agent/process.hpp src/agent/process.cpp
	$(FLAGS) src/agent/process.hpp
	$(FLAGS) src/agent/process.cpp
$(OBJ_DIR)/processMap.o: src/agent/processMap.hpp src/agent/processMap.cpp
	$(FLAGS) src/agent/processMap.hpp
	$(FLAGS) src/agent/processMap.cpp
$(OBJ_DIR)/agent.o: src/agent/agent.hpp src/agent/agent.cpp
	$(FLAGS) src/agent/agent.hpp
	$(FLAGS) src/agent/agent.cpp
$(OBJ_DIR)/needProcessor.o: src/agent/needProcessor.hpp src/agent/needProcessor.cpp
	$(FLAGS) src/agent/needProcessor.hpp
	$(FLAGS) src/agent/needProcessor.cpp
$(OBJ_DIR)/processDirectory.o: src/agent/processDirectory.hpp src/agent/processDirectory.cpp
	$(FLAGS) src/agent/processDirectory.hpp
	$(FLAGS) src/agent/processDirectory.cpp
$(OBJ_DIR)/species.o: src/agent/species.hpp src/agent/species.cpp
	$(FLAGS) src/agent/species.hpp
	$(FLAGS) src/agent/species.cpp
$(OBJ_DIR)/main.o: src/main.cpp
	$(FLAGS) src/main.cpp
clean:
	rm -f $(OBJ_DIR)/*.o
	rm -f -r *.gch
	rm -f LOCAL_horizon
