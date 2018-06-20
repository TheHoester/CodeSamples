using System;
using System.Collections.Generic;
using UnityEngine;

public class AIManagedMazeRunner : AIMazerunner
{
    [SerializeField]
    private AIManager manager;
    
    private bool waitingForResponse;
    private bool waitingForResponsePrev;
    
    private int choicesSinceLastRequest;

    private List<Vector2> unpulledLevers;
    private List<Vector2> closedDoors;

    public int Dependency { get; protected set; }
    public int Requests { get; protected set; }

    #region Reset Methods

    public override void Reset(Vector2 position, int width, int height)
    {
        base.Reset(position, width, height);

        RequestNewPathFromManager();
        Requests = 1;
        waitingForResponse = true;
        waitingForResponsePrev = false;
        isPathingToLever = false;
        choicesSinceLastRequest = 0;

        unpulledLevers = new List<Vector2>();
        closedDoors = new List<Vector2>();
    }

    public void Reset(Vector2 position, int width, int height, int depen)
    {
        Reset(position, width, height);
        Dependency = depen;

        if (Dependency > 10)
            Dependency = 10;
        else if (Dependency < 0)
            Dependency = 0;
    }

    #endregion

    public override void Update()
    {
        if (Paused)
            return;

        if (FoundGoal)
            return;

        if (isMoving)
            Move();
        else if (!waitingForResponse)
        {
            CheckAdjacentTiles();
            Vector2 validTile = CheckPathValid();
            if (validTile.x != 0.5f)
            {
                currPath = new List<Vector2>();
                Quaternion rot;
                Color colour;
                MazeHandler.Instance.GetPuzzleInfo(validTile, out rot, out colour);
                SendBlueprintUpdateToManager((int)validTile.x, -(int)validTile.y, MazeMemory[(int)validTile.x, -(int)validTile.y], rot, colour, false);
            }

            if (currPath == null || currPath.Count == 0)
                currentDirection = nextDirection;

            if (MazeMemory[(int)Trans.position.x, -(int)Trans.position.y] == TileType.End)
                FoundGoal = true;
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

            if (!FoundGoal && (currPath == null || currPath.Count == 0))
            {
                if (Dependency > 0 && choicesSinceLastRequest >= CalculateDependencyValue() && !waitingForResponsePrev)
                {
                    RequestNewPathFromManager();
                    Requests++;
                    waitingForResponse = true;
                    isPathingToLever = false;
                    choicesSinceLastRequest = 0;
                }

                if (!waitingForResponse)
                {
                    choicesSinceLastRequest++;

                    bool found = false;
                    int index = 0;
                    while (index < closedDoors.Count)
                    {
                        if (!MazeHandler.Instance.IsDoorOpen(new Vector2(closedDoors[0].x, -closedDoors[0].y)))
                        {
                            Vector2 leverPos = MazeHandler.Instance.FindConnectingPuzzleEle(MazeMemory, closedDoors[0]);
                            if (leverPos.x != 0.5f)
                            {
                                if (Dependency > 0 && choicesSinceLastRequest >= CalculateDependencyValue())
                                {
                                    RequestNewPathFromManager(leverPos);
                                    Requests++;
                                    waitingForResponse = true;
                                    isPathingToLever = true;
                                    choicesSinceLastRequest = 0;
                                    closedDoors.RemoveAt(0);
                                    found = true;
                                    break;
                                }

                                if (!waitingForResponse)
                                {
                                    isPathingToLever = true;
                                    currPath = UtilityFunctions.FindPath(MazeMemory, destination, leverPos, MazeHandler.Instance.MapWidth, MazeHandler.Instance.MapHeight);
                                    closedDoors.RemoveAt(0);
                                    found = true;
                                    break;
                                }
                            }
                            else
                                index++;
                        }
                        else
                            closedDoors.RemoveAt(0);
                    }

                    if (!found)
                    {
                        while (unpulledLevers.Count > 0)
                        {
                            if (!MazeHandler.Instance.IsLeverDown(new Vector2(unpulledLevers[0].x, -unpulledLevers[0].y)))
                            {
                                if (Dependency > 0 && choicesSinceLastRequest >= CalculateDependencyValue())
                                {
                                    RequestNewPathFromManager(unpulledLevers[0]);
                                    Requests++;
                                    waitingForResponse = true;
                                    isPathingToLever = true;
                                    choicesSinceLastRequest = 0;
                                    unpulledLevers.RemoveAt(0);
                                    found = true;
                                    break;
                                }

                                if (!waitingForResponse)
                                {
                                    isPathingToLever = true;
                                    currPath = UtilityFunctions.FindPath(MazeMemory, destination, unpulledLevers[0], MazeHandler.Instance.MapWidth, MazeHandler.Instance.MapHeight);
                                    unpulledLevers.RemoveAt(0);
                                    found = true;
                                    break;
                                }
                            }
                            else
                                unpulledLevers.RemoveAt(0);
                        }
                    }

                    if (!found)
                    {
                        currPath = FindNextLeftTurn(currentDirection, out nextDirection, false);
                        if (currPath == null || currPath.Count == 0)
                            FindNextDestination();
                    }
                }
            }
        }
        waitingForResponsePrev = waitingForResponse;
    }

    private Vector2 CheckPathValid()
    {
        if (currPath != null)
            foreach (Vector2 tilePos in currPath)
                if (MazeMemory[(int)tilePos.x, -(int)tilePos.y] != TileType.Clear)
                    if (!UtilityFunctions.IsTileWalkable(MazeMemory, tilePos))
                        return tilePos;

        return new Vector2(0.5f, 0.5f);
    }

    private int CalculateDependencyValue()
    {
        return (10 - Dependency) * 2;
    }

    #region Pathing Methods

    protected override void FollowPath()
    {
        Vector2 newDest = currPath[0];
        if (!UtilityFunctions.IsTileWalkable(MazeMemory, newDest))
        {
            TileType type = MazeMemory[(int)newDest.x, -(int)newDest.y];
            if (type != TileType.Door && type != TileType.Lever)
                SendBlueprintUpdateToManager((int)newDest.x, -(int)newDest.y, MazeMemory[(int)newDest.x, -(int)newDest.y], new Quaternion(), new Color(), false);

            currPath = new List<Vector2>();
            return;
        }

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

    #endregion

    #region Info Transfer Methods

    public void ProvideNewPath(List<Vector2> newPath)
    {
        currPath = newPath;
        waitingForResponse = false;
    }

    public void NoPathFound()
    {
        currPath = new List<Vector2>();
        waitingForResponse = false;
        isPathingToLever = false;
    }

    private void RequestNewPathFromManager()
    {
        manager.MakeRequest(new Request(this, RequestType.FindEnd, destination));
    }

    private void RequestNewPathFromManager(Vector2 destWorldPos)
    {
        manager.MakeRequest(new Request(this, RequestType.FindDest, destination, destWorldPos));
    }

    private void SendBlueprintUpdateToManager(int x, int y, TileType type, Quaternion rot, Color colour, bool isActive)
    {
        manager.SendBlueprintUpdate(new BlueprintUpdate(x, y, type, rot, colour, isActive));
    }

    #endregion

    #region Puzzle Methods

    protected override bool SeenLever(Vector2 tileWorldPos)
    {
        if (MazeMemory[(int)tileWorldPos.x, -(int)tileWorldPos.y] == TileType.Lever)
        {
            Quaternion rot;
            Color colour;
            MazeHandler.Instance.GetPuzzleInfo(tileWorldPos, out rot, out colour);
            SendBlueprintUpdateToManager((int)tileWorldPos.x, -(int)tileWorldPos.y, TileType.Lever, rot, colour, false);
            
            if (!MazeHandler.Instance.IsLeverDown(new Vector2(tileWorldPos.x, -tileWorldPos.y)))
            {
                if (unpulledLevers == null)
                    unpulledLevers = new List<Vector2>();
                unpulledLevers.Add(tileWorldPos);
            }
        }
        return false;
    }

    protected override bool SeenDoor(Vector2 tileWorldPos)
    {
        if (MazeMemory[(int)tileWorldPos.x, -(int)tileWorldPos.y] == TileType.Door)
        {
            Quaternion rot;
            Color colour;
            MazeHandler.Instance.GetPuzzleInfo(tileWorldPos, out rot, out colour);
            SendBlueprintUpdateToManager((int)tileWorldPos.x, -(int)tileWorldPos.y, TileType.Door, rot, colour, 
                                         MazeHandler.Instance.IsDoorOpen(new Vector2(tileWorldPos.x, -tileWorldPos.y)));

            if (currPath.Count == 0 && !isPathingToLever && !MazeHandler.Instance.IsDoorOpen(new Vector2(tileWorldPos.x, -tileWorldPos.y)))
            {
                Vector2 leverPos = MazeHandler.Instance.FindConnectingPuzzleEle(MazeMemory, tileWorldPos);
                if (leverPos.x != 0.5f)
                {
                    if (Dependency > 0)
                    {
                        if (choicesSinceLastRequest >= CalculateDependencyValue())
                        {
                            if (!waitingForResponsePrev)
                            {
                                RequestNewPathFromManager(leverPos);
                                Requests++;
                                waitingForResponse = true;
                                isPathingToLever = true;
                                choicesSinceLastRequest = 0;
                            }
                        }
                    }

                    if (!waitingForResponse && !isPathingToLever)
                    {
                        isPathingToLever = true;
                        currPath = UtilityFunctions.FindPath(MazeMemory, destination, leverPos, MazeHandler.Instance.MapWidth, MazeHandler.Instance.MapHeight);
                    }
                }
            }
            else if (!MazeHandler.Instance.IsDoorOpen(new Vector2(tileWorldPos.x, -tileWorldPos.y)))
            {
                if (closedDoors == null)
                    closedDoors = new List<Vector2>();
                closedDoors.Add(tileWorldPos);
            }
        }

        return false;
    }

    protected override void PullLever()
    {
        if (MazeMemory[(int)destination.x, -(int)destination.y] == TileType.Lever)
        {
            MazeHandler.Instance.PullLever(destination);
            choicesSinceLastRequest = 0;
        }
    }

    #endregion
}
