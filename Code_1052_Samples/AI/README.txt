Script Descriptions

IPerson: Interfaces that all person AI implement

AbstractPerson: Abstract Class that implements the IPerson interface and is inherited by all person AI, used for civlian AI.

AbstractCombatPerson: Abstract Class that inherits from the AbstractPerson class and is used for all combat ready AI (Agents and Enemies).

Agent: Inherits from the AbstractCombatPerson class and is used for the player controlled AI.

Enemy: Inherits from the AbstractCombatPerson class and is used for the NPC enemy AI.

MissionInput: Handles the players input during missions and passes the information to the player controlled AI.