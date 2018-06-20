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

	/// <summary>
	/// Resets the values of the AI to its starting state.
	/// </summary>
	/// <param name="position"> Starting position of the AI. </param>
	/// <param name="width"> Tile width of the maze. </param>
	/// <param name="height"> Tile height of the maze. </param>
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

	/// <summary>
	/// Resets the values of the AI to its starting state.
	/// </summary>
	/// <param name="position"> Starting position of the AI. </param>
	/// <param name="width"> Tile width of the maze. </param>
	/// <param name="height"> Tile height of the maze. </param>
	/// <param name="depen"> Dependency of the AI. </param>
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

        if (isMoving) // Moving to the next tile
            Move();
        else if (!waitingForResponse) // Has nothing to do
        {
			// Check surroundings
            CheckAdjacentTiles();
			
			// Will check if current path is a valid one
            Vector2 validTile = CheckPathValid();
            if (validTile.x != 0.5f)
            {
				// Path is blocked and sending information to the manager
                currPath = new List<Vector2>();
                Quaternion rot;
                Color colour;
                MazeHandler.Instance.GetPuzzleInfo(validTile, out rot, out colour);
                SendBlueprintUpdateToManager((int)validTile.x, -(int)validTile.y, MazeMemory[(int)validTile.x, -(int)validTile.y], rot, colour, false);
            }

            if (currPath == null || currPath.Count == 0)
                currentDirection = nextDirection;

            if (MazeMemory[(int)Trans.position.x, -(int)Trans.position.y] == TileType.End) // Has reached the end
                FoundGoal = true;
            else if (HasFoundGoal() && (currPath == null || currPath.Count == 0 || !UtilityFunctions.Vector2Compare(currPath[currPath.Count - 1], GetGoalWorldPos())))
            {
				// Found the goal and trying to get a path to it
                List<Vector2> path = UtilityFunctions.FindPath(destination, GetGoalWorldPos(), MazeWidth, MazeHeight);
                if (path != null)
                    currPath = path;
            }

            if (currPath != null && currPath.Count > 0) // If there is path then get next tile on path
            {
                FollowPath();
                return;
            }
            else if (isPathingToLever && (currPath == null || currPath.Count == 0)) // Currently at lever so pull it
            {
                isPathingToLever = false;
                PullLever();
            }

            if (!FoundGoal && (currPath == null || currPath.Count == 0))
            {
				// Decides whether to ask for information from the manager
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
                    while (index < closedDoors.Count) // Checks whether doors have been seen
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
                        while (unpulledLevers.Count > 0) // Checks whether levers have been seen
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

                    if (!found) // Still got nowhere to go? Find next left instead
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

	/// <summary>
	/// Checks whether a path is able to be traveled.
	/// </summary>
	/// <returns> The position of the tile that can't be traveled, else a default value to signify a clear path. </returns>
    private Vector2 CheckPathValid()
    {
        if (currPath != null)
            foreach (Vector2 tilePos in currPath)
                if (MazeMemory[(int)tilePos.x, -(int)tilePos.y] != TileType.Clear)
                    if (!UtilityFunctions.IsTileWalkable(MazeMemory, tilePos))
                        return tilePos;

        return new Vector2(0.5f, 0.5f);
    }

	/// <summary>
	/// Calculates the dependency action count threshold.
	/// </summary>
	/// <returns> Dependency value. </returns>
    private int CalculateDependencyValue()
    {
        return (10 - Dependency) * 2;
    }

    #region Pathing Methods

	/// <summary>
	/// If the AI has a path to follow, will set the next destination in the path.
	/// </summary>
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

	/// <summary>
	/// Accessor method for the Manager to provide a path.
	/// </summary>
	/// <param name="newPath"> New path to travel. </param>
    public void ProvideNewPath(List<Vector2> newPath)
    {
        currPath = newPath;
        waitingForResponse = false;
    }

	/// <summary>
	/// Accessor method for the Manager tell it there is no path.
	/// </summary>
    public void NoPathFound()
    {
        currPath = new List<Vector2>();
        waitingForResponse = false;
        isPathingToLever = false;
    }

	/// <summary>
	/// Requests for a path from the manager to a specified position.
	/// </summary>
    private void RequestNewPathFromManager()
    {
        manager.MakeRequest(new Request(this, RequestType.FindEnd, destination));
    }

	/// <summary>
	/// Requests for a path from the manager to a specified position.
	/// </summary>
	/// <param name="destWorldPos"> World position of the destination. </param>
    private void RequestNewPathFromManager(Vector2 destWorldPos)
    {
        manager.MakeRequest(new Request(this, RequestType.FindDest, destination, destWorldPos));
    }

	/// <summary>
	/// Sends a message to the manager with an update to its blueprint.
	/// </summary>
	/// <param name="x"> X position of the tile in the map. </param>
	/// <param name="y"> y position of the tile in the map. </param>
	/// <param name="type"> Tile type of the tile at the position. </param>
	/// <param name="rot"> Rotations of the tile. </param>
	/// <param name="colour"> Colour of the tile if it is a puzzle element. </param>
	/// <param name="isActive"> Whether it has been activated if it is a puzzle element. </param>
    private void SendBlueprintUpdateToManager(int x, int y, TileType type, Quaternion rot, Color colour, bool isActive)
    {
        manager.SendBlueprintUpdate(new BlueprintUpdate(x, y, type, rot, colour, isActive));
    }

    #endregion

    #region Puzzle Methods

	/// <summary>
	/// If a lever is seen the AI will store it in memory to return to when it has finished its current tasks.
	/// </summary>
	/// <param name="tileWorldPos"> World position of the lever. </param>
	/// <returns> Doesn't mater in this instance. </returns>
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

	/// <summary>
	/// If a door is seen the AI will store it in memory to return to when it has finished its current tasks.
	/// </summary>
	/// <param name="tileWorldPos"> World position of the door. </param>
	/// <returns> Doesn't mater in this instance. </returns>
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
