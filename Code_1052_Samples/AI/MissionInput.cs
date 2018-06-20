using System.Collections.Generic;
using Code.Agents;
using Code.AI;
using Code.Reusable;
using Code.UI;
using Code.UserInterface.Legacy;
using UnityEngine;
using UnityEngine.EventSystems;

namespace Code.Mission
{
    /// <summary>
    /// Control script for giving mission artifacts instructions
    /// </summary>
    public class MissionInput : Singleton<MissionInput>
    {
        [SerializeField]
        private int maxNumAgents;
        private List<Agent> agents;
        private AbstractCombatPerson[] missionEnemies;

        private Agent[] selectedAgents,
            agentCache = new Agent[0];

        // Mission Specific Variables
        private GameObject[] collectables = new GameObject[0];

        // Box Select Variables
        private bool isSelecting = false;
        private Vector3 mousePos;
        private Camera mainCamera;
        private Bounds viewportBounds;

        [SerializeField]
        private Color dragSelectBoxColour = new Color(0f, 0f, 1f);

        [SerializeField]
        private AudioClip missionSuccessClip, missionFailClip;
        private AudioSource missionResultSource;

        public int MaxNumAgents { get { return maxNumAgents; } }

        public bool AreAllAgentsDead()
        {
            foreach (Agent agent in agents)
                if (agent.gameObject.activeInHierarchy)
                    return false;

            return true;
        }

        public int[] GetSelectedIds
        {
            get
            {
                if (selectedAgents == null) return new int[0];

                int[] ids = new int[selectedAgents.Length];
                for (int i = 0; i < selectedAgents.Length; i++)
                    ids[i] = selectedAgents[i].IdTag;

                return ids;
            }
        }

        public void SelectSingleAgent(int id)
        {
            //check if agent is one of the selected agents
            foreach (Agent agent in AllAgents)
            {
                if (agent.IdTag == id)
                {
                    SelectAgents(new[] { id });
                    return;
                }
            }
        }

        public void ToggleAgentSelection(int id)
        {
            bool wasSelected = false;
            List<int> ids = new List<int>(GetSelectedIds);

            for (int i = ids.Count - 1; i >= 0; i--)
            {
                if (ids[i] == id)
                {
                    wasSelected = true;
                    ids.RemoveAt(i);
                }
            }
            if (!wasSelected) ids.Add(id);

            SelectAgents(ids.ToArray());
        }

        private void SelectAgents(int[] agentIds)
        {
            List<Agent> agents = new List<Agent>();

            foreach (Agent agent in AllAgents)
            {
                bool valid = false;
                foreach (int agentId in agentIds)
                {
                    if (agentId == agent.IdTag)
                    {
                        valid = true;
                        break;
                    }
                }

                if (!valid)
                {
                    agent.ToggleMarker(false);
                    continue;
                }

                agent.ToggleMarker(true);
                agents.Add(agent);
            }

            selectedAgents = agents.ToArray();
        }

        void Start()
        {
            agents = new List<Agent>();
            mainCamera = Camera.main;
            missionResultSource = GetComponent<AudioSource>();
        }

        void Update()
        {
            if (GameState.UiControl != UiControl.Combat) return;
            SelectionInput(); // Logic when selection button is pressed (Left Click by default)
            ActionInput(); // Logic when action button is pressed (Right Click by default)
        }

        private void ClearAgentMarkers()
        {
            if (selectedAgents == null) return;

            foreach (Agent a in selectedAgents)
                a.ToggleMarker(false);
        }

        #region Control Methods

        private void SelectionInput()
        {
            if (Input.GetMouseButtonDown((int)MouseButton.LeftMouse))
            {
                if (!isSelecting && EventSystem.current.currentSelectedGameObject == null)
                {
                    Ray ray = mainCamera.ScreenPointToRay(Input.mousePosition);
                    RaycastHit hit;
                    if (Physics.Raycast(ray, out hit))
                    {
                        if (hit.transform.CompareTag("IgnoreCast") && !RaycastThroughObject(hit.point, ray.direction, out hit))
                            return;

                        if (hit.transform.CompareTag("Agent"))
                        {
                            if (selectedAgents != null)
                                foreach (Agent a in selectedAgents)
                                    a.ToggleMarker(false);

                            selectedAgents = new Agent[] { hit.transform.GetComponent<Agent>() };

                            foreach (Agent a in selectedAgents)
                                a.ToggleMarker(true);
                        }
                        else
                        {
                            isSelecting = true;
                            mousePos = Input.mousePosition;
                        }
                    }
                    else
                    {
                        isSelecting = true;
                        mousePos = Input.mousePosition;
                    }
                }
                else mousePos = Input.mousePosition;
            }

            if (isSelecting && Input.GetMouseButtonUp((int)MouseButton.LeftMouse))
            {
                List<Agent> temp = new List<Agent>();
                viewportBounds = GUIUtils.GetViewportBounds(mainCamera, mousePos, Input.mousePosition);
                foreach (Agent a in agents)
                    if (IsWithinSelectionBounds(a.gameObject)) temp.Add(a);

                if (selectedAgents != null) ClearAgentMarkers();

                selectedAgents = new Agent[temp.Count];

                for (int i = 0; i < temp.Count; i++)
                    selectedAgents[i] = temp[i];

                foreach (Agent a in selectedAgents)
                    a.ToggleMarker(true);

                isSelecting = false;
            }
        }

        private void ActionInput()
        {
            if (Input.GetMouseButtonDown((int)MouseButton.RightMouse) && selectedAgents != null)
            {
                Ray ray = mainCamera.ScreenPointToRay(Input.mousePosition);
                RaycastHit hit;

                if (Physics.Raycast(ray, out hit))
                {
                    foreach (Agent a in selectedAgents)
                    {
                        if (a.CurrentState == ActionState.Dying) continue;
                        
                        if (hit.transform.CompareTag("IgnoreCast") && !RaycastThroughObject(hit.point, ray.direction, out hit))
                            return;

                        if (hit.transform.CompareTag("Enemy"))
                            a.SetTarget(hit.transform.GetComponent<Enemy>());
                        else if (hit.transform.CompareTag("MissionObject"))
                            a.SetDestination(new Vector3(hit.transform.position.x, a.transform.position.y, hit.transform.position.z));
                        else if (!a.CheckMoveToCover(hit.point) && hit.transform.CompareTag("Walkable"))
                            a.SetDestination(new Vector3(hit.point.x, a.transform.position.y, hit.point.z));
                    }
                }
            }
        }

        #endregion

        #region Active Agent Methods

        public void AddAgent(Agent agent)
        {
            if (agents.Count == maxNumAgents)
            {
                Debug.LogError("Too many agents on mission");
                return;
            }

            agents.Add(agent);
            agent.ToggleMarker(false);
        }

        public void AddAgents(AbstractCombatPerson[] agents)
        {
            foreach (AbstractCombatPerson a in agents)
                if (a.gameObject.CompareTag("Agent"))
                    AddAgent((Agent)a);
        }

        public void ReleaseAgents()
        {
            int i = agents.Count - 1;
            while (i >= 0)
            {
                agents[i].gameObject.SetActive(false);
                agents.RemoveAt(i);
                i--;
            }

            AgentFactory.Instance.DeSpawnVan();
        }

        public void SetMissionEnemies(AbstractCombatPerson[] enemies)
        {
            missionEnemies = enemies;
        }

        public Agent[] AllAgents
        {
            get
            {
                if (agentCache.Length != agents.Count)
                    agentCache = agents.ToArray();

                return agentCache;
            }
        }

        #endregion

        public List<Enemy> ActiveEnemies
        {
            get
            {
                List<Enemy> list = new List<Enemy>();
                if (missionEnemies != null)
                {
                    foreach (AbstractCombatPerson e in missionEnemies)
                        if (e != null && !e.Dead) list.Add((Enemy)e);
                }
                return list;
            }
        }

        public List<Enemy> AllEnemies
        {
            get
            {
                List<Enemy> list = new List<Enemy>();
                if (missionEnemies != null)
                {
                    foreach (AbstractCombatPerson e in missionEnemies)
                        if (e != null) list.Add((Enemy)e);
                }
                return list;
            }
        }

        public GameObject[] AllCollectables
        {
            get { return collectables; }
            set { collectables = value; }
        }

        #region Audio Methods

        public void PlaySuccessClip()
        {
            missionResultSource.PlayOneShot(missionSuccessClip);
        }

        public void PlayFailClip()
        {
            missionResultSource.PlayOneShot(missionFailClip);
        }

        #endregion

        #region Box Select Methods

        private void OnGUI()
        {
            if (isSelecting)
            {
                Rect rect = GUIUtils.GetScreenRect(mousePos, Input.mousePosition);
                GUIUtils.DrawScreenRect(rect, new Color(dragSelectBoxColour.r, dragSelectBoxColour.g, dragSelectBoxColour.b, 0.25f));
                GUIUtils.DrawScreenRectBorder(rect, 2.0f, dragSelectBoxColour);
            }
        }

        private bool IsWithinSelectionBounds(GameObject gameObj)
        {
            if (!isSelecting)
                return false;

            return viewportBounds.Contains(mainCamera.WorldToViewportPoint(gameObj.transform.position));
        }

        #endregion

        public static bool RaycastThroughObject(Vector3 startPos, Vector3 direction, out RaycastHit hit)
        {
            return Physics.Raycast(new Ray(startPos + (direction * 0.05f), direction), out hit, 100);
        }
         public static bool RaycastThroughObject(Vector3 startPos, Vector3 direction, float distance, out RaycastHit hit)
        {
            return Physics.Raycast(new Ray(startPos + (direction * 0.05f), direction), out hit, distance);
        }
    }
}
