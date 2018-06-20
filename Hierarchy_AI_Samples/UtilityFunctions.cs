using UnityEngine;
using System.Collections.Generic;

public static class UtilityFunctions
{
    public static bool Vector2Compare(Vector2 one, Vector2 two)
    {
        if ((int)one.x == (int)two.x && (int)one.y == (int)two.y)
            return true;
        else
            return false;
    }

    public static List<T> CopyList<T>(List<T> list)
    {
        List<T> copy = new List<T>();
        foreach (T element in list)
            copy.Add(element);

        return copy;
    }

    #region Direction Methods

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

    public static bool IsTileWalkable(Vector2 tileWorldPos)
    {
        return IsTileWalkable(MazeHandler.Instance.GetMazeTileTypes(), tileWorldPos);
    }

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

    private static bool IsTileValid(Vector2 tileWorldPos, int mazeWidth, int mazeHeight)
    {
        if (tileWorldPos.x >= 0.0f && tileWorldPos.y <= 0.0f && tileWorldPos.x < mazeWidth && -tileWorldPos.y < mazeHeight)
            return true;
        else
            return false;
    }

    #endregion

    #region Pathing Methods

    public static List<Vector2> FindPath(Vector2 startWorldPos, Vector2 destWorldPos, int mazeWidth, int mazeHeight)
    {
        return FindPath(MazeHandler.Instance.GetMazeTileTypes(), startWorldPos, destWorldPos, mazeWidth, mazeHeight);
    }

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

    public static List<Vector2> GetValidDirections(Vector2 tileWorldPos, Vector2 previousTileWorldPos, int mazeWidth, int mazeHeight)
    {
        return GetValidDirections(MazeHandler.Instance.GetMazeTileTypes(), tileWorldPos, previousTileWorldPos, mazeWidth, mazeHeight);
    }

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

    public static bool IsValidDirection(TileType[,] map, Vector2 previousTileWorldPos, Vector2 tileCheck, int mazeWidth, int mazeHeight)
    {
        if (IsTileValid(tileCheck, mazeWidth, mazeHeight) && IsTileWalkable(map, tileCheck) && !Vector2Compare(tileCheck, previousTileWorldPos))
            return true;
        else
            return false;
    }

    public static bool HasPathLooped(List<Vector2> path, Vector2 nextTile)
    {
        foreach (Vector2 tile in path)
            if (Vector2Compare(nextTile, tile))
                return true;
        return false;
    }

    #endregion
}