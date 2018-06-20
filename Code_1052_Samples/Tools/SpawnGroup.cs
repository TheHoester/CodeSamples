using UnityEngine;

namespace Code.AI
{
    [ExecuteInEditMode]
    public class SpawnGroup : MonoBehaviour
    {
        private Transform trans;
        private Vector3 previousPos;
        [SerializeField]
        private GameObject markerObject;
        private GameObject[] markers;
        private bool isOn = true;

        [SerializeField, Range(2, 6)]
        private int spawnAmount;
        private float radius;

        public Vector3[] SpawnPos { get; private set; }
        public float[] SpawnRot { get; private set; }

        /// <summary>
        /// Initializes the SpawnGroup gameobject.
        /// </summary>
        public void Init()
        {
            trans = transform;
            previousPos = trans.position;
            markers = new GameObject[6];

            spawnAmount = 6;
            radius = 0.5f;

            Transform[] children = GetComponentsInChildren<Transform>();

            int numFound = 0;
            int prevFound = 0;
            for (int i = 0; i < children.Length; i++)
            {
                if (children[i].tag == "Spawnpoint")
                {
                    markers[numFound] = children[i].gameObject;
                    numFound++;

                    if (numFound == markers.Length)
                        break;
                }
            }
            prevFound = numFound;

            while (numFound < markers.Length)
            {
                markers[numFound] = Instantiate(markerObject, trans.position, trans.rotation, trans);
                numFound++;
            }

            Vector3[] spawnPos;
            float[] spawnRot;

            InitSpawn(children, prevFound, out spawnPos, out spawnRot);

            SpawnPos = spawnPos;
            SpawnRot = spawnRot;

            SetSpawn();
        }

#if UNITY_EDITOR
        private void Update()
        {
            if (Application.isPlaying) return;
            if (trans == null) return;

            if (previousPos != trans.position)
            {
                MoveGroup(trans.position - previousPos);
                previousPos = trans.position;
            }

            if (isOn)
            {
                UpdateSpawn();
                SetSpawn();
            }
        }
#endif

        /// <summary>
        /// Initializes the spawns within the group to their initial state.
        /// </summary>
        /// <param name="children"> Array of spawns that already exist in the group. </param>
        /// <param name="prevFound"> Number of spawns that already exist. </param>
        /// <param name="spawnPos"> Array of all the spawn positions. </param>
        /// <param name="spawnRot"> Array of all the spawn rotations. </param>
        private void InitSpawn(Transform[] children, int prevFound, out Vector3[] spawnPos, out float[] spawnRot)
        {
            spawnPos = DefaultSpawnPos();
            spawnRot = new float[6];
            for (int i = 0; i < 6; i++)
            {
                if (i < prevFound)
                {
                    spawnPos[i] = children[i].transform.position;
                    spawnRot[i] = children[i].transform.eulerAngles.y;
                }
                else
                {
                    Vector3 heading = trans.position - spawnPos[i];
                    Vector3 direction = heading / heading.magnitude;
                    spawnRot[i] = Quaternion.LookRotation(direction).eulerAngles.y;
                }
            }
        }

        /// <summary>
        /// Gets the default positions for the spawns.
        /// </summary>
        /// <returns> Array of the default positions. </returns>
        private Vector3[] DefaultSpawnPos()
        {
            Vector3[] spawnpoints;
            Vector3 direction = trans.forward / trans.forward.magnitude;

            spawnpoints = new Vector3[6];
            spawnpoints[0] = trans.position + (direction * radius);
            direction = Quaternion.Euler(0.0f, 60.0f, 0.0f) * direction;
            spawnpoints[1] = trans.position + (direction * radius);
            direction = Quaternion.Euler(0.0f, 60.0f, 0.0f) * direction;
            spawnpoints[2] = trans.position + (direction * radius);
            direction = Quaternion.Euler(0.0f, 60.0f, 0.0f) * direction;
            spawnpoints[3] = trans.position + (direction * radius);
            direction = Quaternion.Euler(0.0f, 60.0f, 0.0f) * direction;
            spawnpoints[4] = trans.position + (direction * radius);
            direction = Quaternion.Euler(0.0f, 60.0f, 0.0f) * direction;
            spawnpoints[5] = trans.position + (direction * radius);

            return spawnpoints;
        }

        /// <summary>
        /// Updates the internal data with the current position and rotation of the spawn markers.
        /// </summary>
        private void UpdateSpawn()
        {
            for (int i = 0; i < spawnAmount; i++)
            {
                SpawnPos[i] = markers[i].transform.position;
                SpawnRot[i] = markers[i].transform.eulerAngles.y;
            }
        }

        /// <summary>
        /// Sets the spawn markers positions and rotations using the internal data.
        /// </summary>
        private void SetSpawn()
        {
            for (int i = 0; i < markers.Length; i++)
            {
                if (i < spawnAmount)
                {
                    markers[i].SetActive(true);
                    markers[i].transform.position = SpawnPos[i];
                    markers[i].transform.rotation = Quaternion.Euler(new Vector3(0, SpawnRot[i], 0));
                }
                else
                    markers[i].SetActive(false);
            }
        }

        /// <summary>
        /// Updates the internal data after the group has been moved.
        /// </summary>
        /// <param name="movement"> Amount the group has been moved. </param>
        private void MoveGroup(Vector3 movement)
        {
            for (int i = 0; i < SpawnPos.Length; i++)
                SpawnPos[i] += movement;
        }

        /// <summary>
        /// Translates the information in the SpawnGroupData class provided.
        /// </summary>
        /// <param name="groupData"> The data to be translated. </param>
        public void LoadData(SpawnGroupData groupData)
        {
            Init();
            SpawnPos = new Vector3[groupData.SpawnPos.Length];
            for (int i = 0; i < groupData.SpawnPos.Length; i++)
                SpawnPos[i] = groupData.SpawnPos[i];

            SpawnRot = groupData.SpawnRot;

            SetSpawn();
        }

        /// <summary>
        /// Resets and reloads all the data in from its child markers.
        /// </summary>
        public void ReloadData()
        {
            trans = transform;
            previousPos = trans.position;
            markers = new GameObject[6];

            spawnAmount = 6;
            radius = 0.5f;

            Transform[] children = GetComponentsInChildren<Transform>();

            int numFound = 0;
            for (int i = 0; i < children.Length; i++)
            {
                if (children[i].CompareTag("Spawnpoint"))
                {
                    markers[numFound] = children[i].gameObject;
                    numFound++;

                    if (numFound == markers.Length)
                        break;
                }
            }

            SpawnPos = new Vector3[6];
            SpawnRot = new float[6];

            for (int i = 0; i < markers.Length; i++)
            {
                SpawnPos[i] = markers[i].transform.position;
                SpawnRot[i] = markers[i].transform.eulerAngles.y;
            }
        }

        /// <summary>
        /// Used to delete this group and removes its data from the spawn data struture.
        /// </summary>
        public void Delete()
        {
            SpawnGroupHandler handler = GetComponentInParent<SpawnGroupHandler>();
            if (handler == null)
            {
                Debug.LogError("Spawn Group Has No Parent Handler");
                return;
            }

            handler.RemoveSpawnGroup(this);
            DestroyImmediate(gameObject);
        }
    }
}