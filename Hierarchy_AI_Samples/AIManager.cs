using System.Collections.Generic;
using UnityEngine;

public class AIManager : MonoBehaviour
{
    private TileType[,] blueprint;
    private int mazeWidth;
    private int mazeHeight;

    private List<Request> requestList;
    private List<BlueprintUpdate> blueprintUpdateList;

	/// <summary>
	/// Resets the AIs values to its starting state.
	/// </summary>
    public void Reset()
    {
        blueprint = MazeHandler.Instance.GetBlueprint();
        mazeWidth = MazeHandler.Instance.MapWidth;
        mazeHeight = MazeHandler.Instance.MapHeight;

        blueprintUpdateList = new List<BlueprintUpdate>();
        requestList = new List<Request>();
    }

    public void Update()
    {
        int iterations = 0;
        while (blueprintUpdateList.Count > 0 && iterations < 5)
        {
            if (blueprintUpdateList[0].xPos >= 0 && blueprintUpdateList[0].xPos < mazeWidth &&
                blueprintUpdateList[0].yPos >= 0 && blueprintUpdateList[0].yPos < mazeHeight)
            {
                UpdateBlueprint(blueprintUpdateList[0].xPos, blueprintUpdateList[0].yPos, blueprintUpdateList[0].type);
                if (blueprintUpdateList[0].type == TileType.Door || blueprintUpdateList[0].type == TileType.Lever)
                    ManagedCamera.Instance.UpdateBlueprint(new Vector2(blueprintUpdateList[0].xPos, blueprintUpdateList[0].yPos),
                                                           blueprintUpdateList[0].type,
                                                           blueprintUpdateList[0].rot,
                                                           blueprintUpdateList[0].colour);
                else
                    ManagedCamera.Instance.UpdateBlueprint(new Vector2(blueprintUpdateList[0].xPos, blueprintUpdateList[0].yPos), 
                                                           blueprintUpdateList[0].type);
            }

            blueprintUpdateList.RemoveAt(0);
        }

        while (requestList.Count > 0 && iterations < 5)
        {
            List<Vector2> path = null;
            switch (requestList[0].RequestType)
            {
                case RequestType.FindEnd:
                    path = UtilityFunctions.FindPath(blueprint, requestList[0].StartPos, GetGoalTileWorldPos(), mazeWidth, mazeHeight);
                    if (path == null)
                        requestList[0].Client.NoPathFound();
                    else
                        requestList[0].Client.ProvideNewPath(path);
                    break;
                case RequestType.FindDest:
                    path = UtilityFunctions.FindPath(blueprint, requestList[0].StartPos, requestList[0].Destination, mazeWidth, mazeHeight);
                    if (path == null)
                        requestList[0].Client.NoPathFound();
                    else
                        requestList[0].Client.ProvideNewPath(path);
                    break;
            }

            requestList.RemoveAt(0);

            iterations++;
        }
	}

	/// <summary>
	/// Will try to get the position of the goal.
	/// </summary>
	/// <returns> Position of the goal, otherwise a default value if no goal is found. </returns>
    public Vector2 GetGoalTileWorldPos()
    {
        for (int x = 0; x < mazeWidth; x++)
            for (int y = 0; y < mazeHeight; y++)
                if (blueprint[x, y] == TileType.End)
                    return new Vector2(x, -y);

        return new Vector2(0.5f, 0.5f);
    }

	/// <summary>
	/// Accessor for the Maze Runner to request information.
	/// </summary>
	/// <param name="request"> Request information from the Maze Runner. </param>
    public void MakeRequest(Request request)
    {
        requestList.Add(request);
    }

	/// <summary>
	/// Accessor for the Maze Runner to provide information about the maze.
	/// </summary>
	/// <param name="update"> Update information from the Maze Runner. </param>
    public void SendBlueprintUpdate(BlueprintUpdate update)
    {
        blueprintUpdateList.Add(update);
    }

	/// <summary>
	/// Updates information within the blueprint.
	/// </summary>
	/// <param name="xPos"> X position of the tile in the blueprint. </param>
	/// <param name="yPos"> Y position of the tile in the blueprint. </param>
	/// <param name="type"> New tile type of the tile. </param>
    private void UpdateBlueprint(int xPos, int yPos, TileType type)
    {
        blueprint[xPos, yPos] = type;
    }
}

public struct Request
{
    public AIManagedMazeRunner Client;
    public RequestType RequestType;
    public Vector2 StartPos;
    public Vector2 Destination;

    public Request(AIManagedMazeRunner client, RequestType type, Vector2 start, Vector2 dest)
    {
        Client = client;
        RequestType = type;
        StartPos = start;
        Destination = dest;
    }

    public Request(AIManagedMazeRunner client, RequestType type, Vector2 start)
    {
        Client = client;
        RequestType = type;
        StartPos = start;
        Destination = new Vector2();
    }
}

public struct BlueprintUpdate
{
    public int xPos;
    public int yPos;
    public TileType type;
    public Quaternion rot;
    public Color colour;
    public bool isActive;

    public BlueprintUpdate(int x, int y, TileType t, Quaternion r, Color c, bool a)
    {
        xPos = x;
        yPos = y;
        type = t;
        rot = r;
        colour = c;
        isActive = a;
    }
}

public enum RequestType
{
    FindEnd, FindDest
}