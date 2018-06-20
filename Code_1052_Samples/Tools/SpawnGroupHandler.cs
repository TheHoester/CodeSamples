using Code.BuildingSystem;
using System.Collections.Generic;
using UnityEngine;

namespace Code.AI
{
    [ExecuteInEditMode]
    public class SpawnGroupHandler : MonoBehaviour
    {
        private const float RADIUS = 1.0f;

        [SerializeField]
        private GameObject spawnGroupPrefab;
        public int BlockId { get; private set; }
        public List<SpawnGroup> SpawnGroups { get; private set; }

        public void Init(int id)
        {
            BlockId = id;
            SpawnGroups = new List<SpawnGroup>();
            SpawnGroup[] groups = GetComponentsInChildren<SpawnGroup>();
            foreach (SpawnGroup group in groups)
                SpawnGroups.Add(group);
        }

        public void CreateSpawnpoints(BlockManager bManager, int tilesPerGroup)
        {
            BlockId = bManager.IdTag;

            if (SpawnGroups != null && SpawnGroups.Count > 0)
            {
                foreach (SpawnGroup group in SpawnGroups)
                    DestroyImmediate(group.gameObject);
            }

            SpawnGroups = new List<SpawnGroup>();

            int numGroups = bManager.TileCount / tilesPerGroup;

            for (int i = 0; i < numGroups; i++)
            {
                Vector3 spawnpoint = CreateNewSpawnpoint(bManager, 0, bManager.transform.position.y + 0.01f);
                if (spawnpoint.x != 0.0f && spawnpoint.z != 0.0f)
                {
                    GameObject spawn = Instantiate(spawnGroupPrefab, spawnpoint, spawnGroupPrefab.transform.rotation, transform);
                    spawn.GetComponent<SpawnGroup>().Init();
                    SpawnGroups.Add(spawn.GetComponent<SpawnGroup>());
                }
            }
        }

        private Vector3 CreateNewSpawnpoint(BlockManager bManager, int recursionCount, float yPos)
        {
            recursionCount++;
            if (recursionCount >= 100)
            {
                Debug.LogWarning("EnemySpawnCreator: " + bManager.IdTag + ": Unable to find valid enemy spawnpoint");
                return new Vector3(0.0f, 0.0f);
            }

            float[] bounds = bManager.GetBoundingFloats();
            if (bounds.Length == 0)
            {
                Debug.LogError("Could not find block bounds: " + bManager.IdTag);
                return new Vector3();
            }

            float randX = Random.Range(bounds[0], bounds[1]);
            float randZ = Random.Range(bounds[2], bounds[3]);

            List<Collider> obstructiveColliders = CollidersInBoxRange(new Vector3(randX, 0.0f, randZ), RADIUS);

            foreach (Collider obstruction in obstructiveColliders)
                if (!obstruction.CompareTag("Walkable"))
                    return CreateNewSpawnpoint(bManager, recursionCount, yPos);

            Vector3 position = new Vector3(randX, yPos, randZ);

            if (bManager.IsVectorInBlock(position))
                return position;
            else
                return CreateNewSpawnpoint(bManager, recursionCount, yPos);
        }

        private List<Collider> CollidersInBoxRange(Vector3 centre, float radius)
        {
            Collider[] northBox = Physics.OverlapBox(new Vector3(centre.x, centre.y, centre.z + radius), new Vector3(radius, 1.0f, 0.08f));
            Collider[] southBox = Physics.OverlapBox(new Vector3(centre.x, centre.y, centre.z - radius), new Vector3(radius, 1.0f, 0.08f));
            Collider[] eastBox = Physics.OverlapBox(new Vector3(centre.x + radius, centre.y, centre.z), new Vector3(0.08f, 1.0f, radius));
            Collider[] westBox = Physics.OverlapBox(new Vector3(centre.x - radius, centre.y, centre.z), new Vector3(0.08f, 1.0f, radius));
            Collider[] northEastBox = Physics.OverlapBox(new Vector3(centre.x + (radius / 2), centre.y, centre.z + (radius / 2)), new Vector3(radius, 1.0f, 0.08f), Quaternion.Euler(0.0f, 45.0f, 0.0f));
            Collider[] northWestBox = Physics.OverlapBox(new Vector3(centre.x - (radius / 2), centre.y, centre.z + (radius / 2)), new Vector3(radius, 1.0f, 0.08f), Quaternion.Euler(0.0f, -45.0f, 0.0f));
            Collider[] southEastBox = Physics.OverlapBox(new Vector3(centre.x + (radius / 2), centre.y, centre.z - (radius / 2)), new Vector3(0.08f, 1.0f, radius), Quaternion.Euler(0.0f, 45.0f, 0.0f));
            Collider[] southWestBox = Physics.OverlapBox(new Vector3(centre.x - (radius / 2), centre.y, centre.z - (radius / 2)), new Vector3(0.08f, 1.0f, radius), Quaternion.Euler(0.0f, -45.0f, 0.0f));

            List<Collider> uniqueColliders = new List<Collider>();

            foreach (Collider north in northBox)
                uniqueColliders.Add(north);

            foreach (Collider south in southBox)
            {
                bool match = false;
                foreach (Collider unique in uniqueColliders)
                    if (south.GetInstanceID() == unique.GetInstanceID())
                    {
                        match = true;
                        break;
                    }

                if (!match)
                    uniqueColliders.Add(south);
            }

            foreach (Collider east in eastBox)
            {
                bool match = false;
                foreach (Collider unique in uniqueColliders)
                    if (east.GetInstanceID() == unique.GetInstanceID())
                    {
                        match = true;
                        break;
                    }

                if (!match)
                    uniqueColliders.Add(east);
            }

            foreach (Collider west in westBox)
            {
                bool match = false;
                foreach (Collider unique in uniqueColliders)
                    if (west.GetInstanceID() == unique.GetInstanceID())
                    {
                        match = true;
                        break;
                    }

                if (!match)
                    uniqueColliders.Add(west);
            }

            foreach (Collider northEast in northEastBox)
            {
                bool match = false;
                foreach (Collider unique in uniqueColliders)
                    if (northEast.GetInstanceID() == unique.GetInstanceID())
                    {
                        match = true;
                        break;
                    }

                if (!match)
                    uniqueColliders.Add(northEast);
            }

            foreach (Collider northWest in northWestBox)
            {
                bool match = false;
                foreach (Collider unique in uniqueColliders)
                    if (northWest.GetInstanceID() == unique.GetInstanceID())
                    {
                        match = true;
                        break;
                    }

                if (!match)
                    uniqueColliders.Add(northWest);
            }

            foreach (Collider southEast in southEastBox)
            {
                bool match = false;
                foreach (Collider unique in uniqueColliders)
                    if (southEast.GetInstanceID() == unique.GetInstanceID())
                    {
                        match = true;
                        break;
                    }

                if (!match)
                    uniqueColliders.Add(southEast);
            }

            foreach (Collider southWest in southWestBox)
            {
                bool match = false;
                foreach (Collider unique in uniqueColliders)
                    if (southWest.GetInstanceID() == unique.GetInstanceID())
                    {
                        match = true;
                        break;
                    }

                if (!match)
                    uniqueColliders.Add(southWest);
            }

            return uniqueColliders;
        }

        public void LoadData(int blockId, SpawnGroupData[] spawnData)
        {
            if (spawnData == null)
                return;

            Init(blockId);

            for (int i = 0; i < SpawnGroups.Count; i++)
            {
                if (i >= spawnData.Length)
                    break;

                SpawnGroups[i].LoadData(spawnData[i]);
            }
        }

        public void ReloadData()
        {
            BlockId = int.Parse(gameObject.name);
            SpawnGroups = new List<SpawnGroup>();
            SpawnGroup[] groups = GetComponentsInChildren<SpawnGroup>();
            foreach (SpawnGroup group in groups)
            {
                SpawnGroups.Add(group);
                group.ReloadData();
            }
        }

        public void AddSpawnGroup()
        {
            BlockManager bManager = CityManager.Instance.GetBlockManager(BlockId);
            float[] bounds = bManager.GetBoundingFloats();
            Vector3 spawnpoint = new Vector3(bounds[0], bManager.transform.position.y + 0.01f, bounds[2]);

            if (spawnpoint.x != 0.0f && spawnpoint.z != 0.0f)
            {
                GameObject spawn = Instantiate(spawnGroupPrefab, spawnpoint, spawnGroupPrefab.transform.rotation, transform);
                spawn.GetComponent<SpawnGroup>().Init();
                SpawnGroups.Add(spawn.GetComponent<SpawnGroup>());
            }
        }

        public void RemoveSpawnGroup(SpawnGroup group)
        {
            for (int i = 0; i < SpawnGroups.Count; i++)
            {
                if (group.GetInstanceID() == SpawnGroups[i].GetInstanceID())
                {
                    SpawnGroups.RemoveAt(i);
                    return;
                }
            }

            Debug.LogError("Spawn Group Doesn't Exist");
        }
    }
}
