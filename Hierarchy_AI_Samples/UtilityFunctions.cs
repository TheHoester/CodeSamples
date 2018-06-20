using UnityEngine;
using System.Collections.Generic;

public static class UtilityFunctions
{
	
	/// <summary>
	/// Will compare two vector2s to check if they are the same vector or not.
	/// </summary>
	/// <param name="one"> Vector2 to be compared. </param>
	/// <param name="two"> Vector2 to be compared. </param>
	/// <returns> True if vectors are the same. </returns>
    public static bool Vector2Compare(Vector2 one, Vector2 two)
    {
        if ((int)one.x == (int)two.x && (int)one.y == (int)two.y)
            return true;
        else
            return false;
    }

	/// <summary>
	/// Will create a copy of a provided list due to lists being passed by reference rather than by value.
	/// </summary>
	/// <param name="list"> List to be copied. </param>
	/// <returns> Copy of the list provided. </returns>
    public static List<T> CopyList<T>(List<T> list)
    {
        List<T> copy = new List<T>();
        foreach (T element in list)
            copy.Add(element);

        return copy;
    }

    #region Direction Methods

	/// <summary>
	/// Will get the left direction from the direction given.
	/// </summary>
	/// <param name="currectDirection"> Current direction the AI is facing. </param>
	/// <returns> The left direction from direction given. </returns>
    public static Direction GetLeftDirection(Direction currentDirection)
    {
        switch (currentDirection)
        {
            case Direction.North:
                return Direction.West;
            case Direction.East:
                return Direction.North;
            case Direction.South:
                return Direction.East;
            case Direction.West:
                return Direction.South;
            default:
                return Direction.North;
        }
    }

	/// <summary>
	/// Will get the right direction from the direction given.
	/// </summary>
	/// <param name="currectDirection"> Current direction the AI is facing. </param>
	/// <returns> The right direction from direction given. </returns>
    public static Direction GetRightDirection(Direction currentDirection)
    {
        switch (currentDirection)
        {
            case Direction.North:
                return Direction.East;
            case Direction.East:
                return Direction.South;
            case Direction.South:
                return Direction.West;
            case Direction.West:
                return Direction.North;
            default:
                return Direction.North;
        }
    }

	/// <summary>
	/// Will get the back direction from the direction given.
	/// </summary>
	/// <param name="currectDirection"> Current direction the AI is facing. </param>
	/// <returns> The back direction from direction given. </returns>
    public static Direction GetBackDirection(Direction currentDirection)
    {
        switch (currentDirection)
        {
            case Direction.North:
                return Direction.South;
            case Direction.East:
                return Direction.West;
            case Direction.South:
                return Direction.North;
            case Direction.West:
                return Direction.East;
            default:
                return Direction.North;
        }
    }

    #endregion

    #region Check Methods

	/// <summary>
	/// Checks whether a tile is walkable.
	/// </summary>
	/// <param name="tileWorldPos"> World position of the tile to be checked. </param>
	/// <returns> True if the tile is walkable. </returns>
    public static bool IsTileWalkable(Vector2 tileWorldPos)
    {
        return IsTileWalkable(MazeHandler.Instance.GetMazeTileTypes(), tileWorldPos);
    }

	/// <summary>
	/// Checks whether a tile is walkable.
	/// </summary>
	/// <param name="map"> A map of the maze (Usually the memory of the AI). </param>
	/// <param name="tileWorldPos"> World position of the tile to be checked. </param>
	/// <returns> True if the tile is walkable. </returns>
    public static bool IsTileWalkable(TileType[,] map, Vector2 tileWorldPos)
    {
        if (tileWorldPos.x == 0.5f)
            return false;

        TileType tileType = map[(int)tileWorldPos.x, -(int)tileWorldPos.y];
        if (tileType == TileType.Door && !MazeHandler.Instance.IsDoorOpen(new Vector2(tileWorldPos.x, -tileWorldPos.y)))
            return false;
        else
            return IsTileWalkable(tileType);
    }

	/// <summary>
	/// Checks whether a tile is walkable.
	/// </summary>
	/// <param name="type"> The type of the tile to be checked. </param>
	/// <returns> True if the tile is walkable. </returns>
    public static bool IsTileWalkable(TileType type)
    {
        switch (type)
        {
            case TileType.Clear:
                return false;
            case TileType.Wall:
                return false;
            case TileType.Floor:
                return true;
            case TileType.Trap:
                return false;
            case TileType.Door:
                return true;
            case TileType.Lever:
                return true;
            case TileType.Start:
                return true;
            case TileType.End:
                return true;
            default:
                return false;
        }
    }

	/// <summary>
	/// Checks whether a tile position exists within the current maze.
	/// </summary>
	/// <param name="tileWorldPos"> World position of the tile to be checked. </param>
	/// <param name="mazeWidth"> Tile width of the maze. </param>
	/// <param name="mazeHeight"> Tile height of the maze. </param>
	/// <returns> True if the tile is walkable. </returns>
    private static bool IsTileValid(Vector2 tileWorldPos, int mazeWidth, int mazeHeight)
    {
        if (tileWorldPos.x >= 0.0f && tileWorldPos.y <= 0.0f && tileWorldPos.x < mazeWidth && -tileWorldPos.y < mazeHeight)
            return true;
        else
            return false;
    }

    #endregion

    #region Pathing Methods

	/// <summary>
	/// Will try to find a path from the start position to the end position.
	/// </summary>
	/// <param name="startWorldPos"> Starting World position of the path. </param>
	/// <param name="destWorldPos"> End World position of the path. </param>
	/// <param name="mazeWidth"> Tile width of the maze. </param>
	/// <param name="mazeHeight"> Tile height of the maze. </param>
	/// <returns> List of tiles in the path. Empty list if no path found. </returns>
    public static List<Vector2> FindPath(Vector2 startWorldPos, Vector2 destWorldPos, int mazeWidth, int mazeHeight)
    {
        return FindPath(MazeHandler.Instance.GetMazeTileTypes(), startWorldPos, destWorldPos, mazeWidth, mazeHeight);
    }

	/// <summary>
	/// Will try to find a path from the start position to the end position.
	/// </summary>
	/// <param name="map"> The map to check against. </param>
	/// <param name="startWorldPos"> Starting World position of the path. </param>
	/// <param name="destWorldPos"> End World position of the path. </param>
	/// <param name="mazeWidth"> Tile width of the maze. </param>
	/// <param name="mazeHeight"> Tile height of the maze. </param>
	/// <returns> List of tiles in the path. Empty list if no path found. </returns>
    public static List<Vector2> FindPath(TileType[,] map, Vector2 startWorldPos, Vector2 destWorldPos, int mazeWidth, int mazeHeight)
    {
        List<List<Vector2>> listOfPaths = new List<List<Vector2>>();
        listOfPaths.Add(new List<Vector2> { new Vector2(startWorldPos.x, startWorldPos.y) });

        bool found = false;

        while (!found)
        {
            if (listOfPaths.Count == 0)
            {
                Debug.Log("All paths fails");
                return null;
            }

            for (int i = 0; i < listOfPaths.Count; i++)
            {
                if (i == 0 || listOfPaths[i].Count < listOfPaths[0].Count)
                {
                    List<Vector2> diffDir = new List<Vector2>();

                    if (listOfPaths[i].Count > 1)
                        diffDir = GetValidDirections(map, listOfPaths[i][listOfPaths[i].Count - 1], listOfPaths[i][listOfPaths[i].Count - 2], mazeWidth, mazeHeight);
                    else if (listOfPaths[i].Count == 1)
                        diffDir = GetValidDirections(map, listOfPaths[i][listOfPaths[i].Count - 1], listOfPaths[i][listOfPaths[i].Count - 1], mazeWidth, mazeHeight);
                    else if (listOfPaths[i].Count == 0)
                    {
                        listOfPaths.RemoveAt(i);
                        i--;
                        continue;
                    }

                    if (diffDir.Count == 0)
                    {
                        listOfPaths.RemoveAt(i);
                        i--;
                        continue;
                    }
                    else if (diffDir.Count == 1)
                    {
                        if (HasPathLooped(listOfPaths[i], diffDir[0]))
                        {
                            listOfPaths.RemoveAt(i);
                            i--;
                            continue;
                        }

                        listOfPaths[i].Add(diffDir[0]);
                        if (Vector2Compare(diffDir[0], destWorldPos))
                        {
                            found = true;
                            return listOfPaths[i];
                        }
                    }
                    else if (diffDir.Count > 1)
                    {
                        List<Vector2> originalPath = CopyList(listOfPaths[i]);
                        for (int j = 0; j < diffDir.Count; j++)
                        {
                            if (j == 0)
                            {
                                if (HasPathLooped(listOfPaths[i], diffDir[j]))
                                {
                                    listOfPaths.RemoveAt(i);
                                    i--;
                                    continue;
                                }

                                listOfPaths[i].Add(diffDir[j]);
                                if (Vector2Compare(diffDir[j], destWorldPos))
                                {
                                    found = true;
                                    return listOfPaths[i];
                                }
                            }
                            else
                            {
                                if (HasPathLooped(originalPath, diffDir[j]))
                                    continue;

                                List<Vector2> newPath = CopyList(originalPath);
                                newPath.Add(diffDir[j]);
                                if (Vector2Compare(diffDir[j], destWorldPos))
                                {
                                    found = true;
                                    return newPath;
                                }
                                else
                                    listOfPaths.Add(newPath);
                            }
                        }
                    }
                }
            }
        }

        return null;
    }

	/// <summary>
	/// Gets a list of the valid directions the AI can travel.
	/// </summary>
	/// <param name="tileWorldPos"> Current world position to check from. </param>
	/// <param name="previoudWorldPos"> Previous world position that it came from. </param>
	/// <param name="mazeWidth"> Tile width of the maze. </param>
	/// <param name="mazeHeight"> Tile height of the maze. </param>
	/// <returns> List of tiles that are valid. </returns>
    public static List<Vector2> GetValidDirections(Vector2 tileWorldPos, Vector2 previousTileWorldPos, int mazeWidth, int mazeHeight)
    {
        return GetValidDirections(MazeHandler.Instance.GetMazeTileTypes(), tileWorldPos, previousTileWorldPos, mazeWidth, mazeHeight);
    }

	/// <summary>
	/// Gets a list of the valid directions the AI can travel.
	/// </summary>
	/// <param name="map"> The map to check against. </param>
	/// <param name="tileWorldPos"> Current world position to check from. </param>
	/// <param name="previoudWorldPos"> Previous world position that it came from. </param>
	/// <param name="mazeWidth"> Tile width of the maze. </param>
	/// <param name="mazeHeight"> Tile height of the maze. </param>
	/// <returns> List of tiles that are valid. </returns>
    public static List<Vector2> GetValidDirections(TileType[,] map, Vector2 tileWorldPos, Vector2 previousTileWorldPos, int mazeWidth, int mazeHeight)
    {
        List<Vector2> validDirections = new List<Vector2>();
        Vector2 tileCheck = new Vector2(tileWorldPos.x, tileWorldPos.y + 1);
        if (IsValidDirection(map, previousTileWorldPos, tileCheck, mazeWidth, mazeHeight))
            validDirections.Add(tileCheck);
        tileCheck = new Vector2(tileWorldPos.x + 1, tileWorldPos.y);
        if (IsValidDirection(map, previousTileWorldPos, tileCheck, mazeWidth, mazeHeight))
            validDirections.Add(tileCheck);
        tileCheck = new Vector2(tileWorldPos.x, tileWorldPos.y - 1);
        if (IsValidDirection(map, previousTileWorldPos, tileCheck, mazeWidth, mazeHeight))
            validDirections.Add(tileCheck);
        tileCheck = new Vector2(tileWorldPos.x - 1, tileWorldPos.y);
        if (IsValidDirection(map, previousTileWorldPos, tileCheck, mazeWidth, mazeHeight))
            validDirections.Add(tileCheck);

        return validDirections;
    }

	/// <summary>
	/// Checks if a direction is valid.
	/// </summary>
	/// <param name="map"> The map to check against. </param>
	/// <param name="previousTileWorldPos"> Previous world position it came from. </param>
	/// <param name="tileCheck"> Tile world position to be checked. </param>
	/// <param name="mazeWidth"> Tile width of the maze. </param>
	/// <param name="mazeHeight"> Tile height of the maze. </param>
	/// <returns> List of tiles that are valid. </returns>
    public static bool IsValidDirection(TileType[,] map, Vector2 previousTileWorldPos, Vector2 tileCheck, int mazeWidth, int mazeHeight)
    {
        if (IsTileValid(tileCheck, mazeWidth, mazeHeight) && IsTileWalkable(map, tileCheck) && !Vector2Compare(tileCheck, previousTileWorldPos))
            return true;
        else
            return false;
    }

	/// <summary>
	/// Compares a new tile with all the tiles currently in the path to see if the path has looped.
	/// </summary>
	/// <param name="path"> The path that will be checked. </param>
	/// <param name="nextTile"> The new tile position. </param>
	/// <returns> True if there will be a loop. </returns>
    public static bool HasPathLooped(List<Vector2> path, Vector2 nextTile)
    {
        foreach (Vector2 tile in path)
            if (Vector2Compare(nextTile, tile))
                return true;
        return false;
    }

    #endregion
}