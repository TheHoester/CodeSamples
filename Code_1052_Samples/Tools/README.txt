Script Description

EmemySpawnHandler: Top level handler that handles all the SpawnGroupHandlers, is used to get spawn information for the missions and creates new spawn points when in editor.

SpawnGroupHandler: Next level handler that handles all the SpawnGroups within a city block. Each Block has its own dedicated SpawnGroupHandler.

SpawnGroup: SpawnGroups have 6 spawns that can be moved around when in editor to set the spawn locations in game. Holds all the data for each spawn to be accessed in game.