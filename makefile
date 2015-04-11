#C++11 is currently used so that std::vector.resize calls constructors (for Pool)
FLAGS= g++ -std=c++11 -c -Os -Wall
EXECUTABLE_NAME= LOCAL_horizon
OBJ_DIR= LOCAL_obj
LINK= g++ -std=c++11 -Wall -o "$(EXECUTABLE_NAME)" 
LINKLIBS= -lBox2D -lbase2.0 -lsfml-graphics -lsfml-window -lsfml-system
horizon: $(OBJ_DIR)/debugText.o $(OBJ_DIR)/pool.o $(OBJ_DIR)/quadTree.o $(OBJ_DIR)/simplexnoise.o $(OBJ_DIR)/micropather.o $(OBJ_DIR)/coord.o $(OBJ_DIR)/time.o $(OBJ_DIR)/item.o $(OBJ_DIR)/itemDatabase.o $(OBJ_DIR)/itemProcessor.o $(OBJ_DIR)/consumableItemProcessor.o $(OBJ_DIR)/itemManager.o $(OBJ_DIR)/object.o $(OBJ_DIR)/objectProcessor.o $(OBJ_DIR)/pickupObjectProcessor.o $(OBJ_DIR)/agentProcessor.o $(OBJ_DIR)/objectProcessorDir.o $(OBJ_DIR)/objectManager.o $(OBJ_DIR)/cell.o $(OBJ_DIR)/world.o $(OBJ_DIR)/path.o $(OBJ_DIR)/pathManager.o $(OBJ_DIR)/needMath.o $(OBJ_DIR)/need.o $(OBJ_DIR)/process.o $(OBJ_DIR)/useItemProcess.o $(OBJ_DIR)/processMap.o $(OBJ_DIR)/agent.o $(OBJ_DIR)/needProcessor.o $(OBJ_DIR)/hungerNeedProcessor.o $(OBJ_DIR)/processDirectory.o $(OBJ_DIR)/species.o $(OBJ_DIR)/main.o
	@echo -----------Linking horizon-------------------------
	mv *.o $(OBJ_DIR)
	(cd $(OBJ_DIR) && $(LINK) debugText.o pool.o quadTree.o simplexnoise.o micropather.o coord.o time.o item.o itemDatabase.o itemProcessor.o consumableItemProcessor.o itemManager.o object.o objectProcessor.o pickupObjectProcessor.o agentProcessor.o objectProcessorDir.o objectManager.o cell.o world.o path.o pathManager.o needMath.o need.o process.o useItemProcess.o processMap.o agent.o needProcessor.o hungerNeedProcessor.o processDirectory.o species.o main.o $(LINKLIBS))
	mv $(OBJ_DIR)/$(EXECUTABLE_NAME) .
	#cppcheck --enable=all --std=c++11 --template=gcc -q -v src

$(OBJ_DIR)/debugText.o: src/utilities/debugText.hpp src/utilities/debugText.cpp
	$(FLAGS) src/utilities/debugText.hpp
	$(FLAGS) src/utilities/debugText.cpp
$(OBJ_DIR)/pool.o: src/utilities/pool.hpp src/utilities/pool.cpp
	$(FLAGS) src/utilities/pool.hpp
	$(FLAGS) src/utilities/pool.cpp
$(OBJ_DIR)/quadTree.o: src/utilities/quadTree.hpp src/utilities/quadTree.cpp
	$(FLAGS) src/utilities/quadTree.hpp
	$(FLAGS) src/utilities/quadTree.cpp
$(OBJ_DIR)/simplexnoise.o: src/utilities/simplexnoise.h src/utilities/simplexnoise.cpp
	$(FLAGS) src/utilities/simplexnoise.h
	$(FLAGS) src/utilities/simplexnoise.cpp
$(OBJ_DIR)/micropather.o: src/utilities/micropather.h src/utilities/micropather.cpp
	$(FLAGS) src/utilities/micropather.h
	$(FLAGS) src/utilities/micropather.cpp
$(OBJ_DIR)/coord.o: src/world/coord.hpp src/world/coord.cpp
	$(FLAGS) src/world/coord.hpp
	$(FLAGS) src/world/coord.cpp
$(OBJ_DIR)/time.o: src/world/time.hpp src/world/time.cpp
	$(FLAGS) src/world/time.hpp
	$(FLAGS) src/world/time.cpp
$(OBJ_DIR)/item.o: src/item/item.hpp src/item/item.cpp
	$(FLAGS) src/item/item.hpp
	$(FLAGS) src/item/item.cpp
$(OBJ_DIR)/itemDatabase.o: src/item/itemDatabase.hpp src/item/itemDatabase.cpp
	$(FLAGS) src/item/itemDatabase.hpp
	$(FLAGS) src/item/itemDatabase.cpp
$(OBJ_DIR)/itemProcessor.o: src/item/itemProcessor.hpp src/item/itemProcessor.cpp
	$(FLAGS) src/item/itemProcessor.hpp
	$(FLAGS) src/item/itemProcessor.cpp
$(OBJ_DIR)/consumableItemProcessor.o: src/item/processors/consumableItemProcessor.hpp src/item/processors/consumableItemProcessor.cpp
	$(FLAGS) src/item/processors/consumableItemProcessor.hpp
	$(FLAGS) src/item/processors/consumableItemProcessor.cpp
$(OBJ_DIR)/itemManager.o: src/item/itemManager.hpp src/item/itemManager.cpp
	$(FLAGS) src/item/itemManager.hpp
	$(FLAGS) src/item/itemManager.cpp
$(OBJ_DIR)/object.o: src/object/object.hpp src/object/object.cpp
	$(FLAGS) src/object/object.hpp
	$(FLAGS) src/object/object.cpp
$(OBJ_DIR)/objectProcessor.o: src/object/objectProcessor.hpp src/object/objectProcessor.cpp
	$(FLAGS) src/object/objectProcessor.hpp
	$(FLAGS) src/object/objectProcessor.cpp
$(OBJ_DIR)/pickupObjectProcessor.o: src/object/processors/pickupObjectProcessor.hpp src/object/processors/pickupObjectProcessor.cpp
	$(FLAGS) src/object/processors/pickupObjectProcessor.hpp
	$(FLAGS) src/object/processors/pickupObjectProcessor.cpp
$(OBJ_DIR)/agentProcessor.o: src/object/processors/agentProcessor.hpp src/object/processors/agentProcessor.cpp
	$(FLAGS) src/object/processors/agentProcessor.hpp
	$(FLAGS) src/object/processors/agentProcessor.cpp
$(OBJ_DIR)/objectProcessorDir.o: src/object/objectProcessorDir.hpp src/object/objectProcessorDir.cpp
	$(FLAGS) src/object/objectProcessorDir.hpp
	$(FLAGS) src/object/objectProcessorDir.cpp
$(OBJ_DIR)/objectManager.o: src/object/objectManager.hpp src/object/objectManager.cpp
	$(FLAGS) src/object/objectManager.hpp
	$(FLAGS) src/object/objectManager.cpp
$(OBJ_DIR)/cell.o: src/world/cell.hpp src/world/cell.cpp
	$(FLAGS) src/world/cell.hpp
	$(FLAGS) src/world/cell.cpp
$(OBJ_DIR)/world.o: src/world/world.hpp src/world/world.cpp
	$(FLAGS) src/world/world.hpp
	$(FLAGS) src/world/world.cpp
$(OBJ_DIR)/path.o:src/world/path.hpp src/world/path.cpp
	$(FLAGS) src/world/path.hpp
	$(FLAGS) src/world/path.cpp
$(OBJ_DIR)/pathManager.o:src/world/pathManager.hpp src/world/pathManager.cpp
	$(FLAGS) src/world/pathManager.hpp
	$(FLAGS) src/world/pathManager.cpp
$(OBJ_DIR)/needMath.o: src/agent/needMath.hpp src/agent/needMath.cpp
	$(FLAGS) src/agent/needMath.hpp
	$(FLAGS) src/agent/needMath.cpp	
$(OBJ_DIR)/need.o: src/agent/need.hpp src/agent/need.cpp
	$(FLAGS) src/agent/need.hpp
	$(FLAGS) src/agent/need.cpp
$(OBJ_DIR)/process.o: src/agent/process.hpp src/agent/process.cpp
	$(FLAGS) src/agent/process.hpp
	$(FLAGS) src/agent/process.cpp
$(OBJ_DIR)/useItemProcess.o: src/agent/processes/useItemProcess.hpp src/agent/processes/useItemProcess.cpp
	$(FLAGS) src/agent/processes/useItemProcess.hpp
	$(FLAGS) src/agent/processes/useItemProcess.cpp
$(OBJ_DIR)/processMap.o: src/agent/processMap.hpp src/agent/processMap.cpp
	$(FLAGS) src/agent/processMap.hpp
	$(FLAGS) src/agent/processMap.cpp
$(OBJ_DIR)/agent.o: src/agent/agent.hpp src/agent/agent.cpp
	$(FLAGS) src/agent/agent.hpp
	$(FLAGS) src/agent/agent.cpp
$(OBJ_DIR)/needProcessor.o: src/agent/needProcessor.hpp src/agent/needProcessor.cpp
	$(FLAGS) src/agent/needProcessor.hpp
	$(FLAGS) src/agent/needProcessor.cpp
$(OBJ_DIR)/hungerNeedProcessor.o: src/agent/needProcessors/hungerNeedProcessor.hpp src/agent/needProcessors/hungerNeedProcessor.cpp
	$(FLAGS) src/agent/needProcessors/hungerNeedProcessor.hpp
	$(FLAGS) src/agent/needProcessors/hungerNeedProcessor.cpp
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
