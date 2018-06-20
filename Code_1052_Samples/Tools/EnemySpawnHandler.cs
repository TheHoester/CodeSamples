using Code.BuildingSystem;
using Code.Reusable;
using System;
using System.Collections.Generic;
using System.IO;
using System.Runtime.Serialization.Formatters.Binary;
using UnityEngine;
using UnityEngine.SceneManagement;

namespace Code.AI
{
    [ExecuteInEditMode]
    public class EnemySpawnHandler : Singleton<EnemySpawnHandler>
    {
        [SerializeField]
        private int tilesPerGroup = 2;
        [SerializeField]
        private List<SpawnGroupHandler> enemySpawnPoints;

        [SerializeField, Header("Percentages: Values Must Add To 100%")]
        int[] difficultyOne = new int[] { 50, 35, 15, 0, 0 };
        [SerializeField]
        int[] difficultyTwo = new int[] { 35, 40, 25, 0, 0 },
              difficultyThree = new int[] { 15, 30, 40, 15, 0 },
              difficultyFour = new int[] { 10, 20, 25, 35, 10 },
              difficultyFive = new int[] { 0, 10, 20, 40, 30 };

        private const string Extension = ".bytes";

        private void Awake()
        {
            enemySpawnPoints = new List<SpawnGroupHandler>();
            SpawnGroupHandler[] groupHandler = GetComponentsInChildren<SpawnGroupHandler>();
            foreach (SpawnGroupHandler handler in groupHandler)
                enemySpawnPoints.Add(handler);

            LoadSpawns();
        }

        /// <summary>
        /// Gets a random list of enemy spawns that will scale depending on the difficulty of the mission.
        /// </summary>
        /// <param name="id"> The ID of the block that the mission is located in. </param>
        /// <param name="difficulty"> The difficulty of the mission. </param>
        /// <param name="positions"> The list of the enemy spawn positions. </param>
        /// <param name="rotations"> The list of the enemy spawn rotations. </param>
        public void GetScaledSpawns(int id, int difficulty, out List<Vector3> positions, out List<float> rotations)
        {
            positions = new List<Vector3>();
            rotations = new List<float>();

            // If block doesn't exist or spawns don't exist then return null values
            List<SpawnGroup> spawnGroups = null;
            foreach (SpawnGroupHandler handler in enemySpawnPoints)
            {
                if (handler.BlockId == id)
                {
                    spawnGroups = handler.SpawnGroups;
                    break;
                }
            }

            if (spawnGroups == null)
            {
#if UNITY_EDITOR
                if (Application.isPlaying) Debug.LogError("No Spawn points found for block " + id);
#endif
                return;
            }

            if (id == 38)
            {
                positions.Add(spawnGroups[0].SpawnPos[0]);
                rotations.Add(spawnGroups[0].SpawnRot[0]);
                return;
            }

            difficulty = Mathf.Clamp(difficulty, 1, 5);

            // Sets spawn chance (may need tweaking)
            int[] chance = new int[5];
            switch (difficulty)
            {
                case 1:
                    chance = difficultyOne;
                    break;
                case 2:
                    chance = difficultyTwo;
                    break;
                case 3:
                    chance = difficultyThree;
                    break;
                case 4:
                    chance = difficultyFour;
                    break;
                case 5:
                    chance = difficultyFive;
                    break;
            }

            if (chance[0] + chance[1] + chance[2] + chance[3] + chance[4] != 100)
                Debug.Log("EnemySpawnHandler: Difficulty [" + difficulty + "] percentages don't add to 100, incorrect spawning possible");

            // Makes sure groups aren't repeated
            bool[] used = new bool[spawnGroups.Count];
            for (int i = 0; i < used.Length; i++)
                used[i] = false;

            // Gets a random group for each spawn and gets the positions and rotations for each group used
            for (int i = 0; i < spawnGroups.Count; i++)
            {
                SpawnGroup group = spawnGroups[i];

                // Chance for spawn group size
                int percentage = UnityEngine.Random.Range(0, 100);

                int spawnNumber = 0;

                // Gets the positions and rotations for the spawn size wanted
                if (percentage < chance[0])
                    spawnNumber = 2;
                else if (percentage < chance[0] + chance[1])
                    spawnNumber = 3;
                else if (percentage < chance[0] + chance[1] + chance[2])
                    spawnNumber = 4;
                else if (percentage < chance[0] + chance[1] + chance[2] + chance[3])
                    spawnNumber = 5;
                else
                    spawnNumber = 6;


                for (int j = 0; j < spawnNumber; j++)
                    positions.Add(group.SpawnPos[j]);
                for (int j = 0; j < spawnNumber; j++)
                    rotations.Add(group.SpawnRot[j]);
            }
        }

        /// <summary>
        /// Turns on and off the spawn markers for debugging.
        /// </summary>
        public void SwitchSpawns()
        {
            MeshRenderer[] renderers = GetComponentsInChildren<MeshRenderer>();
            foreach (MeshRenderer renderer in renderers)
            {
                if (renderer.enabled)
                    renderer.enabled = false;
                else
                    renderer.enabled = true;
            }
        }

        #region Spawn Creation Methods

        /// <summary>
        /// Used to generate enemy spawnpoints for the entire level
        /// </summary>
        public void CreateNewSpawnsAll()
        {
            UnityEngine.Random.InitState((int)(DateTime.UtcNow - new DateTime(1970, 1, 1, 0, 0, 0, DateTimeKind.Utc)).TotalSeconds);

            if (enemySpawnPoints == null || enemySpawnPoints.Count == 0)
                enemySpawnPoints = new List<SpawnGroupHandler>();

            List<BlockManager> bManagers = CityManager.Instance.GetAllBlockMangers();
            if (bManagers.Count > 0)
            {
                for (int i = 0; i < bManagers.Count; i++)
                {
                    if (bManagers[i].AllowMissions)
                    {
                        bool found = false;
                        for (int j = 0; j < enemySpawnPoints.Count; j++)
                        {
                            if (enemySpawnPoints[j].BlockId == bManagers[i].IdTag)
                            {
                                enemySpawnPoints[j].CreateSpawnpoints(bManagers[i], tilesPerGroup);
                                found = true;
                                break;
                            }
                        }

                        if (!found)
                        {
                            GameObject folder = new GameObject();
                            folder.transform.parent = transform;
                            folder.name = bManagers[i].IdTag.ToString();
                            folder.AddComponent<SpawnGroupHandler>();
                            SpawnGroupHandler folderHandler = folder.GetComponent<SpawnGroupHandler>();
                            folderHandler.CreateSpawnpoints(bManagers[i], tilesPerGroup);
                            enemySpawnPoints.Add(folderHandler);
                        }
                    }
                }
            }
        }

        /// <summary>
        /// Used to generate enemy spawnpoints for a certain block
        /// </summary>
        /// <param name="bManager">The BlockManager of the block to create spawns for</param>
        public void CreateNewSpawnpointsForBlock(BlockManager bManager)
        {
            bool found = false;
            foreach (SpawnGroupHandler handler in enemySpawnPoints)
            {
                if (handler.BlockId == bManager.IdTag)
                {
                    handler.CreateSpawnpoints(bManager, tilesPerGroup);
                    found = true;
                    break;
                }
            }

            if (!found)
            {
                if (bManager.AllowMissions)
                {
                    GameObject folder = new GameObject();
                    folder.transform.parent = transform;
                    folder.name = bManager.IdTag.ToString();
                    folder.AddComponent<SpawnGroupHandler>();
                    SpawnGroupHandler folderHandler = folder.GetComponent<SpawnGroupHandler>();
                    folderHandler.CreateSpawnpoints(bManager, tilesPerGroup);
                    enemySpawnPoints.Add(folderHandler);
                }
                else
                    Debug.LogError("EnemySpawnCreator: Invalid Block ID");
            }
        }

        #endregion

        #region Serialization Methods

        /// <summary>
        /// Saves the spawn data to file.
        /// </summary>
        public void SaveSpawns()
        {
            ReloadData();

            BinaryFormatter bFormatter = new BinaryFormatter();
            FileStream fStream = File.Create(Application.dataPath + "/Resources/Generated/Spawns/Enemy Spawns " + SceneManager.GetActiveScene().name + Extension);

            EnemyAllSpawnData spawnData = new EnemyAllSpawnData(enemySpawnPoints);
            bFormatter.Serialize(fStream, spawnData);
            fStream.Close();

            Debug.Log("Spawns Saved");
        }

        /// <summary>
        /// Loads the spawn data from file.
        /// </summary>
        /// <returns> True loading was successful </returns>
        public bool LoadSpawns()
        {
            string sceneName = SceneManager.GetActiveScene().name;
            string fileName = "Generated/Spawns/Enemy Spawns " + sceneName;

            TextAsset file = UnityEngine.Resources.Load(fileName) as TextAsset;
            if (!file)
            {
                Debug.LogError("No Spawn Data");
                return false;
            }

            BinaryFormatter bFormatter = new BinaryFormatter();
            using (Stream stream = new MemoryStream(file.bytes))
            {
                EnemyAllSpawnData spawnData = (EnemyAllSpawnData)bFormatter.Deserialize(stream);
                foreach (EnemySpawnData data in spawnData.allSpawns)
                {
                    for (int i = 0; i < enemySpawnPoints.Count; i++)
                    {
                        if (int.Parse(enemySpawnPoints[i].gameObject.name) == data.blockId)
                        {
                            enemySpawnPoints[i].LoadData(data.blockId, data.spawnGroupData);
                            break;
                        }
                    }
                }
            }

            Debug.Log("Spawns Loaded");
            return true;
        }

        /// <summary>
        /// Reloads the data of the spawns. (Used if data becomes out of synch or is lost.)
        /// </summary>
        public void ReloadData()
        {
            enemySpawnPoints = new List<SpawnGroupHandler>();
            SpawnGroupHandler[] groupHandler = GetComponentsInChildren<SpawnGroupHandler>();
            foreach (SpawnGroupHandler handler in groupHandler)
            {
                enemySpawnPoints.Add(handler);
                handler.ReloadData();
            }
            Debug.Log("Data Reloaded Saved");
        }

        #endregion
    }

    #region Data Class and Stucts

    [Serializable]
    public class EnemyAllSpawnData
    {
        public EnemySpawnData[] allSpawns;

        public EnemyAllSpawnData(List<SpawnGroupHandler> allSpawnGroups)
        {
            allSpawns = new EnemySpawnData[allSpawnGroups.Count];
            for (int i = 0; i < allSpawnGroups.Count; i++)
                if (allSpawnGroups[i].SpawnGroups.Count != 0)
                    allSpawns[i] = new EnemySpawnData(allSpawnGroups[i].BlockId, allSpawnGroups[i].SpawnGroups);
        }
    }

    [Serializable]
    public struct EnemySpawnData
    {
        public int blockId;
        public SpawnGroupData[] spawnGroupData;

        public EnemySpawnData(int id, List<SpawnGroup> spawnGroups)
        {
            blockId = id;
            spawnGroupData = new SpawnGroupData[spawnGroups.Count];

            for (int i = 0; i < spawnGroups.Count; i++)
                spawnGroupData[i] = new SpawnGroupData(spawnGroups[i].SpawnPos, spawnGroups[i].SpawnRot);
        }
    }

    [Serializable]
    public struct SpawnGroupData
    {
        public SerializableVector3[] SpawnPos;
        public float[] SpawnRot;

        public SpawnGroupData(SpawnGroup group)
        {
            SpawnPos = new SerializableVector3[group.SpawnPos.Length];
            for (int i = 0; i < group.SpawnPos.Length; i++)
                SpawnPos[i] = group.SpawnPos[i];

            SpawnRot = group.SpawnRot;
        }

        public SpawnGroupData(Vector3[] sP, float[] sR)
        {
            SpawnPos = new SerializableVector3[sP.Length];
            for (int i = 0; i < sP.Length; i++)
                SpawnPos[i] = sP[i];

            SpawnRot = sR;
        }
    }

    [Serializable]
    public struct SerializableVector3
    {
        public float x;
        public float y;
        public float z;

        public SerializableVector3(float pX, float pY, float pZ)
        {
            x = pX;
            y = pY;
            z = pZ;
        }

        public override string ToString()
        {
            return String.Format("[{0}, {1}, {2}]", x, y, z);
        }

        public static implicit operator Vector3(SerializableVector3 value)
        {
            return new Vector3(value.x, value.y, value.z);
        }

        public static implicit operator SerializableVector3(Vector3 value)
        {
            return new SerializableVector3(value.x, value.y, value.z);
        }
    }

    #endregion
}