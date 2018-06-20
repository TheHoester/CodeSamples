using UI.Mission.InMission;
using UnityEngine;
using UnityEngine.AI;

namespace Code.Agents
{
    public class Agent : AbstractCombatPerson
    {

        #region Variables

        [SerializeField]
        private GameObject marker;
        [SerializeField]
        private Sprite mainMarker, coverMarker;
        private LineRenderer lineRenderer;

        [SerializeField]
        private Color selectedColour, unselectedColour;

        #endregion

        #region Properties

        public Sprite Portrait { get; private set; }
        public GameObject Marker { get { return marker; } protected set { marker = value; } }
        public Color SelectedColor { get { return selectedColour; } protected set { selectedColour = value; } }
        public Color UnselectedColour { get { return unselectedColour; } protected set { unselectedColour = value; } }

        #endregion

        #region Initializers

        /// <summary>
        /// Used to pass agent details from the pool of the players current agents
        /// </summary>
        /// <param name="details">Details of a given agent</param>
        public void CreateNewAgent(AgentDetails details, WeaponType weapon, ArmourType armour)
        {
            CreateNewAgent(details.IdTag, details.PersonName, details.Portrait, details.AgentClass, details.MaxHealth,
                details.Health, weapon, armour);
        }

        /// <summary>
        /// Creates a new Agent
        /// </summary>
        /// <param name="i"> Unique id of agent</param>
        /// <param name="n"> Name of agent</param>
        /// <param name="p"> Portrait sprite of agent</param>
        /// <param name="c"> Class of the agent</param>
        /// <param name="mH"> Max Health of agent</param>
        /// <param name="h"> Health of agent</param>
        /// <param name="eFla"> Exp needed to reach first level</param>
        /// <param name="wea"> Weapon that the agent is using</param>
        /// <param name="hasArm"> Is Agent wearing armour</param>
        public void CreateNewAgent(int i, string n, Sprite p, ClassType c, float mH, float h, WeaponType weaType, ArmourType armour)
        {
            Weapon[] weapons = GetComponentsInChildren<Weapon>();
            if (weapons.Length == 0)
                Debug.Log("Agent " + IdTag + ": No Weapons Found");

            foreach (Weapon weapon in weapons)
            {
                switch (weapon.Type)
                {
                    case WeaponType.pistol:
                        glockObject = weapon;
                        break;
                    case WeaponType.rifle:
                        assualtRifleObject = weapon;
                        break;
                    case WeaponType.shotgun:
                        doubleShotgunObject = weapon;
                        break;
                    case WeaponType.tazer:
                        tazerObject = weapon;
                        break;
                }
            }
            SetWeaponObjectByType(weaType);
            
            Armour arm = GetComponentInChildren<Armour>();
            if (arm == null)
                Debug.Log("Agent " + IdTag + ": No Armour Found");
            else
            {
                currArmour = arm;
                currArmour.ResetValues();
            }

            if (armour == ArmourType.Vest)
                currArmour.gameObject.SetActive(true);
            else
                currArmour.gameObject.SetActive(false);

            IdTag = i;
            PersonName = n;
            Portrait = p;
            AgentClass = c;
            MaxHealth = mH;
            Health = h;
            
            lineRenderer = GetComponent<LineRenderer>();
            SetToIdle(true);
            fireTimer = currWeapon.FireRate;
            deathTimer = 0.0f;
            IsInCover = false;
            UpdateMarker(false);
            hasBeenTazed = false;
        }

        #endregion

        public void Update()
        {
            if (lineRenderer != null)
            {
                if (lineRenderer.enabled && Vector3.Distance(trans.position, navAgent.destination) > navAgent.stoppingDistance)
                {
                    lineRenderer.SetPosition(0, trans.position + (Vector3.up * 0.1f));
                    DrawPath(navAgent.path);
                }
                else
                    lineRenderer.enabled = false;
            }

            UpdateFireTimer();
            UpdateAnimation();

            switch (CurrentState)
            {
                case ActionState.Idle:
                    StateCheckIdle();
                    break;
                case ActionState.MoveToEnemy:
                    StateCheckMoveToTarget();
                    break;
                case ActionState.MoveToCover:
                    StateCheckGoingToCover();
                    break;
                case ActionState.AttackEnemy:
                    StateCheckAttackTarget();
                    break;
                case ActionState.Dying:
                    PlayDeathAnimation();
                    break;
            }
        }

        #region State Check Methods

        /// <summary>
        /// Checks to see if any events have happened to cause agent to change state from idle
        /// Will try to find an target within its sight range and currently in view
        /// </summary>
        private void StateCheckIdle()
        {
            if (IsTargetAlive())
            {
                if (CheckInRange(CurrTarget.Trans.position, CurrWeapon.Range) && CheckLineOfSight())
                    SetToAttack();
                else
                {
                    SetToMoving();
                    UpdateMarker(false);
                }
            }
        }

        /// <summary>
        /// Runs the logic for the move to target state of th AI, moving till within weapon range of the target
        /// </summary>
        private void StateCheckMoveToTarget()
        {
            if (IsTargetAlive())
            {
                AbstractCombatPerson temp = CurrTarget;
                SetDestination(CurrTarget.transform.position);
                CurrTarget = temp;
                if (CheckInRange(CurrTarget.Trans.position, CurrWeapon.Range) && CheckLineOfSight())
                    SetToAttack();
            }
            else
                SetToIdle();
        }
        
        /// <summary>
        /// Checks when the AI has reached its cover point and decided what it should do next
        /// </summary>
        private void StateCheckGoingToCover()
        {
            if (CheckInRange(navAgent.destination, CoverStoppingDistance))
            {
                IsInCover = true;
                UpdateMarker(true);
                if (CurrCoverType == CoverType.Left || CurrCoverType == CoverType.Right)
                    trans.rotation = Quaternion.LookRotation(CoverNormal);

                SetToIdle();
            }
        }
        
        /// <summary>
        /// Logic for when the AI is in the AttackTarget state. Will make sure target is still a valid one at try to deal damage.
        /// If the target is no longer valid it will try to reaquire it or find a new target.
        /// </summary>
        private void StateCheckAttackTarget()
        {
            if (!IsTargetAlive())
            {
                SetToIdle();
                return;
            }

            if (!CheckLineOfSight())
            {
                if (!IsInCover)
                    SetToMoving();
                else
                    SetToIdle();
                return;
            }

            if (CheckInRange(CurrTarget.Trans.position, CurrWeapon.Range))
            {
                if (CurrCoverType == CoverType.No || CurrCoverType == CoverType.Up)
                    trans.LookAt(CurrTarget.Trans);

                if (fireTimer >= CurrWeapon.FireRate && (CurrWeapon.Type != WeaponType.rifle || CurrWeapon.CanFireBurst()))
                {
                    bool isTargetInCover = CurrTarget.IsInCover;

                    // Check if they are flanking
                    if (isTargetInCover)
                    {
                        Vector3 startPos = trans.position + (Vector3.up * GUNSHOTDIST);
                        Vector3 direction = CurrTarget.Trans.position + (Vector3.up * INCOVERDIST) - startPos;
                        direction.Normalize();

                        Ray ray = new Ray(startPos, direction);
                        RaycastHit hit;
                        if (Physics.Raycast(ray, out hit, 1000))
                            if (hit.collider.gameObject.GetInstanceID() == CurrTarget.gameObject.GetInstanceID())
                                isTargetInCover = false;
                    }

                    if (CurrWeapon.GetRandomAccuracy(isTargetInCover, Vector3.Distance(CurrTarget.Trans.position, trans.position)))
                    {
                        float damage = CurrWeapon.GetDamage(critChanceModifier);
                        CurrTarget.TakeDamage(damage, this);
                        PopupTextManager.Instance.NewPopupText(damage.ToString("F2"), CurrTarget.Trans.position, Color.red);
                    }
                    else
                        PopupTextManager.Instance.NewPopupText("Missed", CurrTarget.Trans.position, Color.red);

                    CurrWeapon.PlayGunshot();
                    BulletHandler.Instance.CreateBullet(currWeapon.transform.position, GetSightCheckPosition(CurrTarget.transform, CurrTarget.CoverNormal, CoverType.No));

                    if (CurrWeapon.Type != WeaponType.rifle || CurrWeapon.FinishedBurst())
                        fireTimer = 0.0f;
                }
            }
            else
            {
                if (!IsInCover)
                    SetToMoving();
                else
                    SetToIdle();
            }
        }

        /// <summary>
        /// Will check if a given destination will result in the agent moving into cover.
        /// </summary>
        /// <param name="destination"> The destination the agent wil move to </param>
        /// <returns></returns>
        public bool CheckMoveToCover(Vector3 destination)
        {
            NavMeshHit hit;
            NavMesh.FindClosestEdge(destination, out hit, NavMesh.AllAreas);
            if (CheckInRange(destination, hit.position, 0.15f))
            {
                CoverNormal = hit.normal;
                if (Physics.Raycast(destination + (Vector3.up * GUNSHOTDIST), -CoverNormal, 0.2f))
                {
                    if (Physics.Raycast(destination + (Vector3.up * GUNSHOTDIST) + (Quaternion.Euler(0, -90, 0) * CoverNormal * COVERPEAKSIDEDIST), -CoverNormal, 0.2f))
                        SetToTakeCover(new Vector3(hit.position.x, transform.position.y, hit.position.z), CoverType.Left);
                    else
                        SetToTakeCover(new Vector3(hit.position.x, transform.position.y, hit.position.z), CoverType.Right);
                }
                else
                    SetToTakeCover(new Vector3(hit.position.x, transform.position.y, hit.position.z), CoverType.Up);
                
                UpdateMarker(false);
                return true;
            }
            else
                return false;
        }

        #endregion

		#region Util Methods
		
        /// <summary>
        /// Turns on and off the marker under the AIs feet.
        /// </summary>
        /// <param name="toggle"> True: turns marker on. False: turns marker off. </param>
        public void ToggleMarker(bool toggle)
        {
            marker.SetActive(toggle);
        }
		
        /// <summary>
        /// Damage is dealt to the AI and if it isn't already engaging another target, will engage the target that damaged them.
        /// </summary>
        /// <param name="amount">Damage value</param>
        /// <param name="from">The target that dealt the damge</param>
        public override void TakeDamage(float amount, AbstractCombatPerson from)
        {
            TakeDamage(amount);
            if (CurrentState == ActionState.Dying && from.CurrWeapon.Type == WeaponType.tazer)
                hasBeenTazed = true;

            if (CurrentState != ActionState.Dying && !IsTargetAlive() && !IsMoving())
                if (CheckInRange(from.Trans.position, CurrWeapon.Range))
                    CurrTarget = from;
        }

        /// <summary>
        /// Used by input to set current target.
        /// </summary>
        /// <param name="target"> The new current target. </param>
        public void SetTarget(AbstractCombatPerson target)
        {
            CurrTarget = target;
        }

        /// <summary>
        /// Used by input to set new destination.
        /// </summary>
        /// <param name="destination"> The new position to move to. </param>
        public new void SetDestination(Vector3 destination)
        {
            if (lineRenderer != null)
            {
                if (lineRenderer.enabled == false)
                    lineRenderer.enabled = true;

                lineRenderer.SetPosition(0, trans.position + (Vector3.up * 0.1f));
            }

            SetToIdle(destination);
            IsInCover = false;
            UpdateMarker(false);
            CurrCoverType = CoverType.No;

            if (lineRenderer != null)
                DrawPath(navAgent.path);
        }

        /// <summary>
        /// Resets the AIs health back to full.
        /// </summary>
        public void ReplenHealth()
        {
            Health = MaxHealth;
        }

        /// <summary>
        /// Changes the marker image to show when the AI is in cover or not.
        /// </summary>
        /// <param name="inCover"> Is in cover or not. </param>
        private void UpdateMarker(bool inCover)
        {
            if (inCover)
                marker.GetComponent<SpriteRenderer>().sprite = coverMarker;
            else
                marker.GetComponent<SpriteRenderer>().sprite = mainMarker;
        }

        /// <summary>
        /// Checks if the AI is still moving to its destination.
        /// </summary>
        /// <returns> True if the AI is still moving, else false </returns>
        private bool IsMoving()
        {
            if (trans.position.x > navAgent.destination.x + navAgent.stoppingDistance || trans.position.x < navAgent.destination.x - navAgent.stoppingDistance)
                return true;
            else if (trans.position.z > navAgent.destination.z + navAgent.stoppingDistance || trans.position.z < navAgent.destination.z - navAgent.stoppingDistance)
                return true;
            else
                return false;
        }
		
		#endregion
    }
}