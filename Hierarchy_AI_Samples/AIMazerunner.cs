﻿using System.Collections.Generic;
using UnityEngine;

public class AIMazerunner : MonoBehaviour
{
    #region Movement Variables

    [SerializeField]
    protected float speed = 1.0f;
    
    protected bool isMoving;
    protected Direction currentDirection;
    protected Direction nextDirection;
    protected Vector2 destination;

    #endregion

    #region Memory Variables

    protected bool[,] travelledMemory;

    #endregion

    #region Pathing Variables

    protected List<Vector2> currPath;
    protected bool isPathingToLever;

    #endregion

	#region Util Properties
	
    public Transform Trans { get; protected set; }
    public bool FoundGoal { get; protected set; }
    public float Speed { get { return speed; } set { speed = value; } }

	#endregion
	
    #region  Memory Properties

    public TileType[,] MazeMemory { get; protected set; }
    public int MazeWidth { get; protected set; }
    public int MazeHeight { get; protected set; }
    public int Actions { get; protected set; }
    public bool Paused { get; set; }

    #endregion

	/// <summary>
	/// Resets the AIs values to its starting state.
	/// </summary>
	/// <param name="position"> Tile position to use as start position. </param>
	/// <param name="width"> Maze tile width. </param>
	/// <param name="height"> Maze tile height. </param>
    public virtual void Reset(Vector2 position, int width, int height)
    {
        // Standard
        Trans = GetComponent<Transform>();
        Trans.position = new Vector3(position.x, position.y, Trans.position.z);
        Paused = true;
        // Movement
        currentDirection = Direction.North;
        destination = new Vector2(Trans.position.x, Trans.position.y);
        isMoving = false;
        // Memory
        FoundGoal = false;
        Actions = 0;
        MazeMemory = new TileType[width, height];
        travelledMemory = new bool[width, height];
        MazeWidth = width;
        MazeHeight = height;
        for (int x = 0; x < width; x++)
        {
            for (int y = 0; y < height; y++)
            {
                MazeMemory[x, y] = TileType.Clear;
                travelledMemory[x, y] = false;
            }
        }

        if (width != 0 && height != 0)
        {
            MazeMemory[(int)position.x, -(int)position.y] = TileType.Start;
            travelledMemory[(int)position.x, -(int)position.y] = true;
        }
        // Pathing
        currPath = new List<Vector2>();
        isPathingToLever = false;
        // Initial Setup
        CheckAdjacentTiles();
        nextDirection = currentDirection;
    }

    public virtual void Update()
    {
        if (Paused)
            return;

        if (FoundGoal)
            return;

        if (isMoving)
            Move();
        else
        {
            CheckAdjacentTiles();

            if (currPath == null || currPath.Count == 0)
                currentDirection = nextDirection;

            if (MazeMemory[(int)Trans.position.x, -(int)Trans.position.y] == TileType.End)
            {
                FoundGoal = true;
                return;
            }
            else if (HasFoundGoal() && (currPath == null || currPath.Count == 0 || !UtilityFunctions.Vector2Compare(currPath[currPath.Count - 1], GetGoalWorldPos())))
            {
                List<Vector2> path = UtilityFunctions.FindPath(destination, GetGoalWorldPos(), MazeWidth, MazeHeight);
                if (path != null)
                    currPath = path;
            }

            if (currPath != null && currPath.Count > 0)
            {
                FollowPath();
                return;
            }
            else if (isPathingToLever && (currPath == null || currPath.Count == 0))
            {
                isPathingToLever = false;
                PullLever();
            }
            else
                currPath = FindNextLeftTurn(currentDirection, out nextDirection, false);
            
            if (!FoundGoal && (currPath == null || currPath.Count == 0))
                FindNextDestination();
        }
    }

	/// <summary>
	/// Will move the AI to the next tile at a predetermined speed.
	/// </summary>
    protected virtual void Move()
    {
        switch (currentDirection)
        {
            case Direction.North:
                Trans.Translate(new Vector3(0.0f, speed * Time.deltaTime, 0.0f));
                if (Trans.position.y > destination.y)
                {
                    Trans.position = new Vector3(destination.x, destination.y, Trans.position.z);
                    isMoving = false;
                }
                break;
            case Direction.East:
                Trans.Translate(new Vector3(speed * Time.deltaTime, 0.0f, 0.0f));
                if (Trans.position.x > destination.x)
                {
                    Trans.position = new Vector3(destination.x, destination.y, Trans.position.z);
                    isMoving = false;
                }
                break;
            case Direction.South:
                Trans.Translate(new Vector3(0.0f, -speed * Time.deltaTime, 0.0f));
                if (Trans.position.y < destination.y)
                {
                    Trans.position = new Vector3(destination.x, destination.y, Trans.position.z);
                    isMoving = false;
                }
                break;
            case Direction.West:
                Trans.Translate(new Vector3(-speed * Time.deltaTime, 0.0f, 0.0f));
                if (Trans.position.x < destination.x)
                {
                    Trans.position = new Vector3(destination.x, destination.y, Trans.position.z);
                    isMoving = false;
                }
                break;
        }

        if (!isMoving)
        {
            travelledMemory[(int)destination.x, -(int)destination.y] = true;
            Actions++;
        }
    }

    #region Sight Methods

	/// <summary>
	/// Sight method to check what the AI sees in each direction.
	/// </summary>
    protected void CheckAdjacentTiles()
    {
        CheckAdjacentTile(UtilityFunctions.GetLeftDirection(currentDirection));
        CheckAdjacentTile(currentDirection);
        CheckAdjacentTile(UtilityFunctions.GetRightDirection(currentDirection));
        CheckAdjacentTile(UtilityFunctions.GetBackDirection(currentDirection));
    }

	/// <summary>
	/// Sight method to check what the AI sees in a specified direction.
	/// </summary>
	/// <param name="dir"> The direction to check. </param>
    protected void CheckAdjacentTile(Direction dir)
    {
        Vector2 tileWorldPos = GetNextDestination(dir);
        TileType type = MazeHandler.Instance.GetNextTile(destination, dir);
        if (type != TileType.Clear)
        {
            if (MazeMemory[(int)tileWorldPos.x, -(int)tileWorldPos.y] == TileType.Clear)
                MazeMemory[(int)tileWorldPos.x, -(int)tileWorldPos.y] = type;

            if (!isPathingToLever && type == TileType.Lever)
                isPathingToLever = SeenLever(tileWorldPos);
            else if (!isPathingToLever && type == TileType.Door)
                isPathingToLever = SeenDoor(tileWorldPos);

            if (type != TileType.Wall)
            {
                if (type != TileType.Door || (type == TileType.Door && MazeHandler.Instance.IsDoorOpen(new Vector2(tileWorldPos.x, -tileWorldPos.y))))
                {
                    Vector2 currTileCheck = GetNextDestination(tileWorldPos, UtilityFunctions.GetLeftDirection(dir));
                    TileType currTypeCheck = MazeHandler.Instance.GetNextTile(tileWorldPos, UtilityFunctions.GetLeftDirection(dir));
                    if (currTypeCheck != TileType.Clear && MazeMemory[(int)currTileCheck.x, -(int)currTileCheck.y] == TileType.Clear)
                        MazeMemory[(int)currTileCheck.x, -(int)currTileCheck.y] = currTypeCheck;

                    currTileCheck = GetNextDestination(tileWorldPos, UtilityFunctions.GetRightDirection(dir));
                    currTypeCheck = MazeHandler.Instance.GetNextTile(tileWorldPos, UtilityFunctions.GetRightDirection(dir));
                    if (currTypeCheck != TileType.Clear && MazeMemory[(int)currTileCheck.x, -(int)currTileCheck.y] == TileType.Clear)
                        MazeMemory[(int)currTileCheck.x, -(int)currTileCheck.y] = currTypeCheck;

                    CheckCorridor(tileWorldPos, dir);
                }
            }
        }
    }

	/// <summary>
	/// Sight method to check what the AI can see down a corridor in a specified direction.
	/// </summary>
	/// <param name="tileWorldPos"> Start position of the corridor. </param>
	/// <param name="dir"> The direction to check. </param>
    protected void CheckCorridor(Vector2 tileWorldPos, Direction dir)
    {
        TileType type = MazeHandler.Instance.GetNextTile(tileWorldPos, dir);
        tileWorldPos = GetNextDestination(tileWorldPos, dir);
        if (type != TileType.Clear && MazeMemory[(int)tileWorldPos.x, -(int)tileWorldPos.y] == TileType.Clear)
            MazeMemory[(int)tileWorldPos.x, -(int)tileWorldPos.y] = type;

        if (!isPathingToLever && type == TileType.Lever)
            isPathingToLever = SeenLever(tileWorldPos);
        else if (!isPathingToLever && type == TileType.Door)
            isPathingToLever = SeenDoor(tileWorldPos);

        while (type != TileType.Clear && type != TileType.Wall && (type != TileType.Door || (type == TileType.Door && MazeHandler.Instance.IsDoorOpen(new Vector2(tileWorldPos.x, -tileWorldPos.y)))))
        {
            Vector2 currTileCheck = GetNextDestination(tileWorldPos, UtilityFunctions.GetLeftDirection(dir));
            TileType currTypeCheck = MazeHandler.Instance.GetNextTile(tileWorldPos, UtilityFunctions.GetLeftDirection(dir));
            if (currTypeCheck != TileType.Clear && MazeMemory[(int)currTileCheck.x, -(int)currTileCheck.y] == TileType.Clear)
                MazeMemory[(int)currTileCheck.x, -(int)currTileCheck.y] = currTypeCheck;

            currTileCheck = GetNextDestination(tileWorldPos, UtilityFunctions.GetRightDirection(dir));
            currTypeCheck = MazeHandler.Instance.GetNextTile(tileWorldPos, UtilityFunctions.GetRightDirection(dir));
            if (currTypeCheck != TileType.Clear && MazeMemory[(int)currTileCheck.x, -(int)currTileCheck.y] == TileType.Clear)
                MazeMemory[(int)currTileCheck.x, -(int)currTileCheck.y] = currTypeCheck;

            type = MazeHandler.Instance.GetNextTile(tileWorldPos, dir);
            tileWorldPos = GetNextDestination(tileWorldPos, dir);
            if (MazeMemory[(int)tileWorldPos.x, -(int)tileWorldPos.y] != TileType.Clear)
                continue;

            if (type != TileType.Clear)
                MazeMemory[(int)tileWorldPos.x, -(int)tileWorldPos.y] = type;

            if (!isPathingToLever && type == TileType.Lever)
                isPathingToLever = SeenLever(tileWorldPos);
            else if (!isPathingToLever && type == TileType.Door)
                isPathingToLever = SeenDoor(tileWorldPos);
        }
    }

	/// <summary>
	/// Checks whether the AI can see a specific tile from its current position.
	/// </summary>
	/// <param name="tileWorldPos"> World position of the tile to be checked. </param>
	/// <returns> True if the AI can see the tile. </returns>
    protected bool CanSeeTile(Vector2 tileWorldPos)
    {
        if (tileWorldPos.x == Trans.position.x)
        {
            if (tileWorldPos.y < Trans.position.y)
            {
                while (tileWorldPos.y < Trans.position.y)
                {
                    if (UtilityFunctions.IsTileWalkable(MazeMemory, tileWorldPos))
                        tileWorldPos.y++;
                    else
                        return false;
                }
                return true;
            }
            else if (tileWorldPos.y > Trans.position.y)
            {
                while (tileWorldPos.y > Trans.position.y)
                {
                    if (UtilityFunctions.IsTileWalkable(MazeMemory, tileWorldPos))
                        tileWorldPos.y--;
                    else
                        return false;
                }
                return true;
            }
            else
                return true;
        }
        else if (tileWorldPos.y == Trans.position.y)
        {
            if (tileWorldPos.x < Trans.position.x)
            {
                while (tileWorldPos.x < Trans.position.x)
                {
                    if (UtilityFunctions.IsTileWalkable(MazeMemory, tileWorldPos))
                        tileWorldPos.x++;
                    else
                        return false;
                }
                return true;
            }
            else if (tileWorldPos.x > Trans.position.x)
            {
                while (tileWorldPos.x > Trans.position.x)
                {
                    if (UtilityFunctions.IsTileWalkable(MazeMemory, tileWorldPos))
                        tileWorldPos.x--;
                    else
                        return false;
                }
                return true;
            }
            else
                return true;
        }
        else
            return false;
    }

    #endregion

    #region Destination Methods

	/// <summary>
	/// Find a path to the closest untraveled junction.
	/// </summary>
	/// <param name="startWorldPos"> The starting position of the check. Usually AIs currect position. </param>
	/// <param name="currDir"> Current heading direction. </param>
	/// <param name="nextDir"> Current next direction. </param>
	/// <returns> A path to the closest untraveled junction. </returns>
    protected List<Vector2> FindPathToClosestUntravelled(Vector2 startWorldPos, Direction currDir, out Direction nextDir)
    {
        List<List<Vector2>> listOfPaths = new List<List<Vector2>>();
        listOfPaths.Add(new List<Vector2> { new Vector2(startWorldPos.x, startWorldPos.y) });

        bool found = false;

        while (!found)
        {
            if (listOfPaths.Count == 0)
            {
                Debug.Log("All paths fails");
                nextDir = currDir;
                return null;
            }

            for (int i = 0; i < listOfPaths.Count; i++)
            {
                if (i == 0 || listOfPaths[i].Count < listOfPaths[0].Count)
                {
                    List<Vector2> diffDir = new List<Vector2>();

                    if (listOfPaths[i].Count > 1)
                        diffDir = UtilityFunctions.GetValidDirections(listOfPaths[i][listOfPaths[i].Count - 1], listOfPaths[i][listOfPaths[i].Count - 2], MazeWidth, MazeHeight);
                    else if (listOfPaths[i].Count == 1)
                        diffDir = UtilityFunctions.GetValidDirections(listOfPaths[i][listOfPaths[i].Count - 1], listOfPaths[i][listOfPaths[i].Count - 1], MazeWidth, MazeHeight);
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
                        if (UtilityFunctions.HasPathLooped(listOfPaths[i], diffDir[0]))
                        {
                            listOfPaths.RemoveAt(i);
                            i--;
                            continue;
                        }

                        listOfPaths[i].Add(diffDir[0]);
                        if (MazeHandler.Instance.IsTileJunction(diffDir[0]))
                        {
                            List<Vector2> validDir = new List<Vector2>();
                            Vector2 current = diffDir[0];
                            Vector2 previous = diffDir[0];
                            if (listOfPaths[i].Count > 1)
                                previous = listOfPaths[i][listOfPaths[i].Count - 2];

                            if (!GetValidDirectionsFromMemory(current, previous, out validDir))
                            {
                                Direction dirFrom = GetDirectionToTile(previous, current);
                                nextDir = UtilityFunctions.GetBackDirection(dirFrom);

                                foreach (Vector2 pos in validDir)
                                {
                                    if (GetDirectionToTile(current, pos) == UtilityFunctions.GetLeftDirection(dirFrom))
                                        nextDir = UtilityFunctions.GetLeftDirection(dirFrom);
                                    else if (GetDirectionToTile(current, pos) == dirFrom && nextDir != UtilityFunctions.GetLeftDirection(dirFrom))
                                        nextDir = dirFrom;
                                    else if (GetDirectionToTile(current, pos) == UtilityFunctions.GetRightDirection(dirFrom) &&
                                             nextDir != UtilityFunctions.GetLeftDirection(dirFrom) && nextDir != dirFrom)
                                        nextDir = UtilityFunctions.GetRightDirection(dirFrom);
                                }

                                found = true;
                                return listOfPaths[i];
                            }
                        }
                    }
                    else if (diffDir.Count > 1)
                    {
                        List<Vector2> originalPath = UtilityFunctions.CopyList(listOfPaths[i]);
                        for (int j = 0; j < diffDir.Count; j++)
                        {
                            if (j == 0)
                            {
                                if (UtilityFunctions.HasPathLooped(listOfPaths[i], diffDir[j]))
                                {
                                    listOfPaths.RemoveAt(i);
                                    i--;
                                    continue;
                                }

                                listOfPaths[i].Add(diffDir[j]);
                                if (MazeHandler.Instance.IsTileJunction(diffDir[j]))
                                {
                                    List<Vector2> validDir = new List<Vector2>();
                                    Vector2 current = diffDir[j];
                                    Vector2 previous = diffDir[j];
                                    if (listOfPaths[i].Count > 1)
                                        previous = listOfPaths[i][listOfPaths[i].Count - 2];

                                    if (!GetValidDirectionsFromMemory(current, previous, out validDir))
                                    {

                                        Direction dirFrom = GetDirectionToTile(previous, current);
                                        nextDir = UtilityFunctions.GetBackDirection(dirFrom);

                                        foreach (Vector2 pos in validDir)
                                        {
                                            if (GetDirectionToTile(current, pos) == UtilityFunctions.GetLeftDirection(dirFrom))
                                                nextDir = UtilityFunctions.GetLeftDirection(dirFrom);
                                            else if (GetDirectionToTile(current, pos) == dirFrom && nextDir != UtilityFunctions.GetLeftDirection(dirFrom))
                                                nextDir = dirFrom;
                                            else if (GetDirectionToTile(current, pos) == UtilityFunctions.GetRightDirection(dirFrom) &&
                                                     nextDir != UtilityFunctions.GetLeftDirection(dirFrom) && nextDir != dirFrom)
                                                nextDir = UtilityFunctions.GetRightDirection(dirFrom);
                                        }

                                        found = true;
                                        return listOfPaths[i];
                                    }
                                }
                            }
                            else
                            {
                                if (UtilityFunctions.HasPathLooped(originalPath, diffDir[j]))
                                    continue;

                                List<Vector2> newPath = UtilityFunctions.CopyList(originalPath);
                                newPath.Add(diffDir[j]);
                                if (MazeHandler.Instance.IsTileJunction(diffDir[j]))
                                {
                                    List<Vector2> validDir = new List<Vector2>();
                                    Vector2 current = diffDir[j];
                                    Vector2 previous = diffDir[j];
                                    if (listOfPaths[i].Count > 1)
                                        previous = listOfPaths[i][listOfPaths[i].Count - 2];

                                    if (!GetValidDirectionsFromMemory(current, previous, out validDir))
                                    {

                                        Direction dirFrom = GetDirectionToTile(previous, current);
                                        nextDir = UtilityFunctions.GetBackDirection(dirFrom);

                                        foreach (Vector2 pos in validDir)
                                        {
                                            if (GetDirectionToTile(current, pos) == UtilityFunctions.GetLeftDirection(dirFrom))
                                                nextDir = UtilityFunctions.GetLeftDirection(dirFrom);
                                            else if (GetDirectionToTile(current, pos) == dirFrom && nextDir != UtilityFunctions.GetLeftDirection(dirFrom))
                                                nextDir = dirFrom;
                                            else if (GetDirectionToTile(current, pos) == UtilityFunctions.GetRightDirection(dirFrom) &&
                                                     nextDir != UtilityFunctions.GetLeftDirection(dirFrom) && nextDir != dirFrom)
                                                nextDir = UtilityFunctions.GetRightDirection(dirFrom);
                                        }

                                        found = true;
                                        return newPath;
                                    }
                                }
                                else
                                    listOfPaths.Add(newPath);
                            }
                        }
                    }
                }
            }
        }

        nextDir = currDir;
        return null;
    }

	/// <summary>
	/// Finds and sets the next destination, using the left hand rule.
	/// </summary>
    protected void FindNextDestination()
    {
        Vector2 tileWorldPos = GetNextDestination(UtilityFunctions.GetLeftDirection(currentDirection));
        TileType tileType = MazeHandler.Instance.GetNextTile(destination, UtilityFunctions.GetLeftDirection(currentDirection));
        if (!UtilityFunctions.IsTileWalkable(tileType) || (tileType == TileType.Door && !MazeHandler.Instance.IsDoorOpen(tileWorldPos)))
        {
            tileWorldPos = GetNextDestination(currentDirection);
            tileType = MazeHandler.Instance.GetNextTile(destination, currentDirection);
            if (!UtilityFunctions.IsTileWalkable(tileType) || (tileType == TileType.Door && !MazeHandler.Instance.IsDoorOpen(tileWorldPos)))
            {
                tileWorldPos = GetNextDestination(UtilityFunctions.GetRightDirection(currentDirection));
                tileType = MazeHandler.Instance.GetNextTile(destination, UtilityFunctions.GetRightDirection(currentDirection));
                if (!UtilityFunctions.IsTileWalkable(tileType) || (tileType == TileType.Door && !MazeHandler.Instance.IsDoorOpen(tileWorldPos)))
                {
                    tileWorldPos = GetNextDestination(UtilityFunctions.GetBackDirection(currentDirection));
                    tileType = MazeHandler.Instance.GetNextTile(destination, UtilityFunctions.GetBackDirection(currentDirection));
                    if (!UtilityFunctions.IsTileWalkable(tileType) || (tileType == TileType.Door && !MazeHandler.Instance.IsDoorOpen(tileWorldPos)))
                        return;
                    else
                        currentDirection = UtilityFunctions.GetBackDirection(currentDirection);

                }
                else
                    currentDirection = UtilityFunctions.GetRightDirection(currentDirection);
            }
        }
        else
            currentDirection = UtilityFunctions.GetLeftDirection(currentDirection);

        destination = GetNextDestination(currentDirection);
        isMoving = true;
    }

	/// <summary>
	/// Finds the next destination from a tile in a given direction.
	/// </summary>
	/// <param name="direction"> The direction to find the next destination in. </param>
	/// <returns> The next position to move to. </returns>
    protected Vector2 GetNextDestination(Direction direction)
    {
        return GetNextDestination(destination, direction);
    }

	/// <summary>
	/// Find the next destination from a tile in a given direction.
	/// </summary>
	/// <param name="tileWorldPos"> The world position of the tile to check from. </param>
	/// <param name="direction"> The direction to find the next destination in. </param>
	/// <returns> The next position to move to. </returns>
    protected Vector2 GetNextDestination(Vector2 tileWorldPos, Direction direction)
    {
        switch (direction)
        {
            case Direction.North:
                return new Vector2(tileWorldPos.x, tileWorldPos.y + 1);
            case Direction.East:
                return new Vector2(tileWorldPos.x + 1, tileWorldPos.y);
            case Direction.South:
                return new Vector2(tileWorldPos.x, tileWorldPos.y - 1);
            case Direction.West:
                return new Vector2(tileWorldPos.x - 1, tileWorldPos.y);
            default:
                return new Vector2(tileWorldPos.x, tileWorldPos.y);
        }
    }

    #endregion

    #region Check Methods

	/// <summary>
	/// Gets the position of the next tile in a specified direction.
	/// </summary>
	/// <param name="tileWorldPos"> The world position of the tile to check from. </param>
	/// <param name="direction"> Direction to check in. </param>
	/// <returns> Position of the next tile. </returns>
    public Vector2 GetNextTileFromMemory(Vector2 tileWorldPos, Direction direction)
    {
        switch (direction)
        {
            case Direction.North:
                if (tileWorldPos.x >= 0.0f && tileWorldPos.y + 1 <= 0.0f && tileWorldPos.x < MazeWidth && -(tileWorldPos.y + 1) < MazeHeight)
                    return new Vector2(tileWorldPos.x, tileWorldPos.y + 1);
                break;
            case Direction.East:
                if (tileWorldPos.x + 1 >= 0.0f && tileWorldPos.y <= 0.0f && tileWorldPos.x + 1 < MazeWidth && -tileWorldPos.y < MazeHeight)
                    return new Vector2(tileWorldPos.x + 1, tileWorldPos.y);
                break;
            case Direction.South:
                if (tileWorldPos.x >= 0.0f && tileWorldPos.y - 1 <= 0.0f && tileWorldPos.x < MazeWidth && -(tileWorldPos.y - 1) < MazeHeight)
                    return new Vector2(tileWorldPos.x, tileWorldPos.y - 1);
                break;
            case Direction.West:
                if (tileWorldPos.x - 1 >= 0.0f && tileWorldPos.y <= 0.0f && tileWorldPos.x - 1 < MazeWidth && -tileWorldPos.y < MazeHeight)
                    return new Vector2(tileWorldPos.x - 1, tileWorldPos.y);
                break;
        }

        return new Vector2(0.5f, 0.5f);
    }

	/// <summary>
	/// Gets the position of the goal from memory, will return a default value if goal hasn't been found.
	/// </summary>
	/// <returns> World position of the goal. </returns>
    protected Vector2 GetGoalWorldPos()
    {
        Vector2 position = new Vector2(0.5f, 0.5f);

        for (int x = 0; x < MazeWidth; x++)
            for (int y = 0; y < MazeHeight; y++)
                if (MazeMemory[x,y] == TileType.End)
                    position = new Vector2(x, -y);

        return position;
    }

	/// <summary>
	/// Checks to see if the goal has been found.
	/// </summary>
	/// <returns> True if goal has been found. </returns>
    protected bool HasFoundGoal()
    {
        for (int x = 0; x < MazeWidth; x++)
            for (int y = 0; y < MazeHeight; y++)
                if (MazeMemory[x, y] == TileType.End)
                    return true;

        return false;
    }

	/// <summary>
	/// Gets the direction that the to tile is from the from tile.
	/// </summary>
	/// <param name="from"> The start tile for the direction. </param>
	/// <param name="to"> The end tile for the direction. </param>
	/// <returns> The direction the to tile is in from the from tile. </returns>
    protected Direction GetDirectionToTile(Vector2 from, Vector2 to)
    {
        Direction direction = Direction.North;

        if (from.y < to.y)
            direction = Direction.North;
        else if (from.x < to.x)
            direction = Direction.East;
        else if (from.y > to.y)
            direction = Direction.South;
        else if (from.x > to.x)
            direction = Direction.West;

        return direction;
    }

    #endregion

    #region Pathing Methods

	
	/// <summary>
	/// Prints the current path to console.
	/// </summary>
	/// <param name="worldPos"> The path. </param>
    protected void PrintPath(List<Vector2> worldPos)
    {
        for (int i = 0; i < worldPos.Count; i++)
            Debug.Log("Tile " + i + ": " + worldPos[i].x + ", " + worldPos[i].y);
    }

	/// <summary>
	/// If the AI has a path to follow, will set the next destination in the path.
	/// </summary>
    protected virtual void FollowPath()
    {
        Vector2 newDest = currPath[0];
        Direction newDir = Direction.North;
        if (newDest.y > destination.y)
            newDir = Direction.North;
        else if (newDest.x > destination.x)
            newDir = Direction.East;
        else if (newDest.y < destination.y)
            newDir = Direction.South;
        else if (newDest.x < destination.x)
            newDir = Direction.West;
        else if (UtilityFunctions.Vector2Compare(newDest, destination))
        {
            currPath.RemoveAt(0);
            FollowPath();
            return;
        }
        else
        {
            Debug.LogError("Path is invalid");
            return;
        }

        destination = newDest;
        currentDirection = newDir;
        isMoving = true;
        currPath.RemoveAt(0);
    }

	/// <summary>
	/// Will find a path to the next left turn in a given direction.
	/// </summary>
	/// <param name="currDirection"> Current direction the AI is traveling. </param>
	/// <param name="endDirection"> Direction the AI will be traveling at the end. </param>
	/// <param name="recursion"> Whether this is a first run or a recursion. </param>
	/// <returns> A path to the next left junction. </returns>
    protected List<Vector2> FindNextLeftTurn(Direction currDirection, out Direction endDirection, bool recursion)
    {
        Vector2 currLoc = destination;
        Direction currDir = currDirection;
        bool found = false;
        int iterations = 0;

        while (!found)
        {
            List<Vector2> validDir = new List<Vector2>();
            GetValidDirectionsFromMemory(currLoc, currLoc, out validDir);

            bool travelledAll = true;
            foreach (Vector2 pos in validDir)
                if (!travelledMemory[(int)pos.x, -(int)pos.y])
                    travelledAll = false;

            if (travelledAll)
            {
                endDirection = currDirection;
                return FindPathToClosestUntravelled(destination, currDirection, out endDirection);
            }

            if (!travelledAll)
            {
                for (int i = 0; i < validDir.Count; i++)
                {
                    if (travelledMemory[(int)validDir[i].x, -(int)validDir[i].y])
                    {
                        validDir.RemoveAt(i);
                        i--;
                    }
                }
            }

            Direction nextDir = UtilityFunctions.GetBackDirection(currDir);

            if (iterations == 0 && UtilityFunctions.IsTileWalkable(GetNextTileFromMemory(currLoc, currDir)))
                nextDir = currDir;
            else
            {
                foreach (Vector2 pos in validDir)
                {
                    if (GetDirectionToTile(currLoc, pos) == UtilityFunctions.GetLeftDirection(currDir))
                        nextDir = UtilityFunctions.GetLeftDirection(currDir);
                    else if (GetDirectionToTile(currLoc, pos) == currDir && nextDir != UtilityFunctions.GetLeftDirection(currDir))
                        nextDir = currDir;
                    else if (GetDirectionToTile(currLoc, pos) == UtilityFunctions.GetRightDirection(currDir) &&
                             nextDir != UtilityFunctions.GetLeftDirection(currDir) && nextDir != currDir)
                        nextDir = UtilityFunctions.GetRightDirection(currDir);
                }
            }

            if (nextDir == UtilityFunctions.GetLeftDirection(currDir) || nextDir == UtilityFunctions.GetRightDirection(currDir))
            {
                if (UtilityFunctions.Vector2Compare(currLoc, destination))
                    return FindNextLeftTurn(nextDir, out endDirection, true);
                else
                {
                    endDirection = nextDir;
                    return UtilityFunctions.FindPath(destination, currLoc, MazeWidth, MazeHeight);
                }
            }
            else if (nextDir == currDir)
            {
                currLoc = GetNextDestination(currLoc, currDir);
                travelledMemory[(int)currLoc.x, -(int)currLoc.y] = true;
            }
            else if (nextDir == UtilityFunctions.GetBackDirection(currDir))
            {
                currLoc = GetNextDestination(currLoc, nextDir);
                currDir = nextDir;
            }

            iterations++;
            if (iterations >= 100)
            {
                Debug.LogError("Unable to find next left direction");
                found = true;
                break;
            }
        }

        endDirection = Direction.North;
        return null;
    }

	
	/// <summary>
	/// Gets all valid travel directions that haven't already been traveled, otherwise all directions if they have all been traveled.
	/// </summary>
	/// <param name="tileWorldPos"> World position of the tile to be checked. </param>
	/// <param name="previousTileWorldPos"> Previous world position the AI came from. </param>
	/// <param name="validDir"> A list of valid travel directions. </param>
	/// <returns> True if all directions have already been travelled. </returns>
    protected bool GetValidDirectionsFromMemory(Vector2 tileWorldPos, Vector2 previousTileWorldPos, out List<Vector2> validDir)
    {
        validDir = UtilityFunctions.GetValidDirections(tileWorldPos, previousTileWorldPos, MazeWidth, MazeHeight);

        bool travelledAll = true;
        foreach (Vector2 pos in validDir)
            if (!travelledMemory[(int)pos.x, -(int)pos.y])
                travelledAll = false;

        if (!travelledAll)
        {
            for (int i = 0; i < validDir.Count; i++)
            {
                if (travelledMemory[(int)validDir[i].x, -(int)validDir[i].y])
                {
                    validDir.RemoveAt(i);
                    i--;
                }
            }
        }
        
        return travelledAll;
    }

    #endregion

    #region Puzzle Solving Methods

	/// <summary>
	/// If a lever is seen the AI will try to path to it if it hasn't already been pulled.
	/// </summary>
	/// <param name="tileWorldPos"> World position of the lever. </param>
	/// <returns> True if it can find a path to the lever. </returns>
    protected virtual bool SeenLever(Vector2 tileWorldPos)
    {
        if (MazeMemory[(int)tileWorldPos.x, -(int)tileWorldPos.y] == TileType.Lever && !isPathingToLever && 
            !MazeHandler.Instance.IsLeverDown(new Vector2(tileWorldPos.x, -tileWorldPos.y)))
        {
            currPath = UtilityFunctions.FindPath(MazeMemory, destination, tileWorldPos, MazeHandler.Instance.MapWidth, MazeHandler.Instance.MapHeight);
            if (currPath != null)
                return true;
            else
                return false;
        }

        return false;
    }

	/// <summary>
	/// If a door is seen the AI will try to path to the connecting lever.
	/// </summary>
	/// <param name="tileWorldPos"> World position of the door. </param>
	/// <returns> True if it has found the connecting lever and can path to it. </returns>
    protected virtual bool SeenDoor(Vector2 tileWorldPos)
    {
        if (MazeMemory[(int)tileWorldPos.x, -(int)tileWorldPos.y] == TileType.Door && !isPathingToLever && 
            !MazeHandler.Instance.IsDoorOpen(new Vector2(tileWorldPos.x, -tileWorldPos.y)))
        {
            Vector2 leverPos = MazeHandler.Instance.FindConnectingPuzzleEle(MazeMemory, tileWorldPos);
            if (leverPos.x != 0.5f)
            {
                currPath = UtilityFunctions.FindPath(MazeMemory, destination, leverPos, MazeHandler.Instance.MapWidth, MazeHandler.Instance.MapHeight);
                if (currPath != null)
                    return true;
                else
                    return false;
            }
        }

        return false;
    }

	/// <summary>
	/// Action of pulling a lever.
	/// </summary>
    protected virtual void PullLever()
    {
        if (MazeMemory[(int)destination.x, -(int)destination.y] == TileType.Lever)
            MazeHandler.Instance.PullLever(destination);
    }

	/// <summary>
	/// Check to see if the AI has found the connecting puzzle element.
	/// </summary>
	/// <param name="tileWorldPos"> World position of the puzzle tile. </param>
	/// <returns> True if the connecting puzzle element has been found. </returns>
    protected bool HasFoundConnectingPuzzle(Vector2 tileWorldPos)
    {
        if (MazeHandler.Instance.FindConnectingPuzzleEle(MazeMemory, tileWorldPos).x == 0.5f)
            return false;
        else
            return true;
    }

    #endregion
}