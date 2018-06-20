using System;
using Code.Agents;
using Code.Mission;
using System.Collections.Generic;
using UI.Mission.InMission;
using UnityEngine;
using UnityEngine.AI;
using UnityEngine.Profiling;

namespace Code.AI
{
    public class Enemy : AbstractCombatPerson
    {
        [SerializeField]
        private float sightRange = 7.0f;
        private Vector3 coverPoint;

        #region Enemy Initailizers

        /// <summary>
        /// Used to pass enemy details from the pool of the current active enemies
        /// </summary>
        /// <param name="details">Details of a given enemy</param>
        public void CreateNewEnemy(EnemyDetails details)
        {
            CreateNewEnemy(details.EnemyClass, details.MaxHealth, details.Health, details.CurrWeapon);
        }

        /// <summary>
        /// Used to reload saved agents
        /// </summary>
        /// <param name="c"> Class of the agent</param>
        /// <param name="mH"> Max Health of agent</param>
        /// <param name="h"> Health of agent</param>
        /// <param name="wea">Weapon that the agent is using</param>
        /// <param name="arm">Armour that the agent is wearing</param>
        public void CreateNewEnemy(ClassType c, float mH, float h, WeaponType weaType)
        {
            Weapon[] weapons = GetComponentsInChildren<Weapon>();
            if (weapons.Length == 0)
                Debug.Log("Enemy " + IdTag + ": No Weapons Found");

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
                Debug.Log("Enemy " + IdTag + ": No Armour Found");
            else
            {
                currArmour = arm;
                currArmour.ResetValues();
            }

            AgentClass = c;
            MaxHealth = mH;
            Health = h;
            SetToIdle(true);
            fireTimer = currWeapon.FireRate;
            deathTimer = 0.0f;
            coverPoint = trans.position;
            IsInCover = false;
            hasBeenTazed = false;
        }

        #endregion

        void FixedUpdate()
        {
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
                    if (!hasBeenTazed)
                        PlayDeathAnimation();
                    break;
            }
        }

        #region State Check Methods

        /// <summary>
        /// Checks to see if any events have happened to cause enemy to change state from idle
        /// Will try to find an target within its sight range and currently in view
        /// </summary>
        private void StateCheckIdle()
        {
            if (IsTargetAlive())
            {
                if (!CheckLineOfSight())
                    CurrTarget = CheckForTargets();

                if (CurrTarget != null)
                    FindNextStateChange();
            }
            else
            {
                CurrTarget = CheckForTargets();
                if (IsTargetAlive())
                    FindNextStateChange();
            }
        }

        /// <summary>
        /// Runs the logic for the move to target state of th AI, moving till within weapon range of the target
        /// </summary>
        private void StateCheckMoveToTarget()
        {
            // Check if current target is still alive and active
            if (!IsTargetAlive())
            {
                CurrTarget = CheckForTargets();
                if (IsTargetAlive())
                    FindNextStateChange();
                else
                    SetToIdle();

                return;
            }

            bool sightCheck = CheckLineOfSight();
            if (sightCheck && CheckInRange(CurrTarget.Trans.position, sightRange))
            {
                FindNextStateChange();
                return;
            }
            else if (!sightCheck)
            {
                Agent agent = CheckForTargets();
                if (agent != null)
                {
                    CurrTarget = agent;
                    if (IsTargetAlive())
                    {
                        FindNextStateChange();
                        return;
                    }
                }
            }

            if (!navAgent.pathPending && !navAgent.hasPath && Vector3.Distance(navAgent.destination, trans.position) <= StandardStoppingDist)
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

                if (CurrCoverType == CoverType.Left || CurrCoverType == CoverType.Right)
                    trans.rotation = Quaternion.LookRotation(CoverNormal);

                if (IsTargetAlive() && CheckInRange(CurrTarget.Trans.position, CurrWeapon.Range) && CheckLineOfSight())
                    SetToAttack();
                else
                {
                    CurrTarget = CheckForTargets();
                    if (IsTargetAlive())
                        FindNextStateChange();
                    else
                        SetToIdle();
                }
            }
        }

        /// <summary>
        /// Logic for when the AI is in the AttackTarget state. Will make sure target is still a valid one at try to deal damage.
        /// If the target is no longer valid it will try to reacquire it or find a new target.
        /// </summary>
        private void StateCheckAttackTarget()
        {
            if (!IsTargetAlive() || !CheckLineOfSight())
            {
                CurrTarget = CheckForTargets();
                if (IsTargetAlive())
                    FindNextStateChange();
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
                        PopupTextManager.Instance.NewPopupText(damage.ToString("F2"), CurrTarget.Trans.position, Color.blue);
                    }
                    else
                        PopupTextManager.Instance.NewPopupText("Missed", CurrTarget.Trans.position, Color.blue);

                    CurrWeapon.PlayGunshot();
                    BulletHandler.Instance.CreateBullet(currWeapon.transform.position, GetSightCheckPosition(CurrTarget.transform, CurrTarget.CoverNormal, CoverType.No));

                    if (CurrWeapon.Type != WeaponType.rifle || CurrWeapon.FinishedBurst())
                        fireTimer = 0.0f;
                }
            }
            else
                SetToMoving();
        }

        /// <summary>
        /// Used to check where the AI has met requirements to be able to attack, take cover or will have to move
        /// closer to the target
        /// </summary>
        private void FindNextStateChange()
        {
            bool weaponRangeCheck = CheckInRange(CurrTarget.Trans.position, CurrWeapon.Range);
            if (IsInCover && weaponRangeCheck)
                SetToAttack();
            else if (!TakeCover())
            {
                if (weaponRangeCheck)
                    SetToAttack();
                else
                    SetToMoving();
            }
        }

        #endregion

        #region Cover Methods

        /// <summary>
        /// Finds the closest cover point to the AI
        /// </summary>
        /// <returns> True if cover point found </returns>
        private bool TakeCover()
        {
            Profiler.BeginSample("Take Cover");
            if (!IsTargetAlive())
            {
                Profiler.EndSample();
                return false;
            }

            CoverType coverType = CoverType.No;

            // Finds all colliders within sight range that are scenery objects
            Collider[] colliders = Physics.OverlapSphere(transform.position, sightRange);
            List<Collider> sceneryColliders = new List<Collider>();
            foreach (Collider coll in colliders)
                if (LayerMask.LayerToName(coll.gameObject.layer) == "Scenery" && CheckSightOfCoverObject(coll.transform) && CheckPointNotBehindPlayer(coll.transform.position))
                    sceneryColliders.Add(coll);

            sceneryColliders = SortColliderByDistance(sceneryColliders);

            for (int i = 0; i < sceneryColliders.Count; i++)
            {
                Vector3[] boundsDirections;
                Vector3[] bounds = GetObjectBounds(sceneryColliders[i].transform, out boundsDirections);
                Vector3 centre = new Vector3(sceneryColliders[i].transform.position.x, trans.position.y, sceneryColliders[i].transform.position.z);
                float heightDist = bounds[0].y - trans.position.y;
                bounds[0] = new Vector3(bounds[0].x, trans.position.y, bounds[0].z);
                bounds[1] = new Vector3(bounds[1].x, trans.position.y, bounds[1].z);
                bounds[2] = new Vector3(bounds[2].x, trans.position.y, bounds[2].z);
                bounds[3] = new Vector3(bounds[3].x, trans.position.y, bounds[3].z);

                if (heightDist > GUNSHOTDIST)
                {
                    // Standing Cover
                    for (int j = 0; j < bounds.Length; j++)
                    {
                        Vector3 point = new Vector3();
                        // Corner to the left
                        if (IsStandingCoverValid(sceneryColliders[i].gameObject, bounds[j], boundsDirections[j], -1, out point, out coverType))
                        {
                            SetToTakeCover(point, coverType);
                            Profiler.EndSample();
                            return true;
                        }

                        //Corner to the right
                        if (IsStandingCoverValid(sceneryColliders[i].gameObject, bounds[j], boundsDirections[j], 1, out point, out coverType))
                        {
                            SetToTakeCover(point, coverType);
                            Profiler.EndSample();
                            return true;
                        }
                    }
                }
                else if (heightDist < GUNSHOTDIST && heightDist > TOPHEADINCOVERDIST)
                {
                    // Crouching Cover
                    Vector3 playerDir = CurrTarget.Trans.position - centre;
                    playerDir.Normalize();

                    float playerRot = Quaternion.LookRotation(playerDir, Vector3.up).eulerAngles.y;
                    float neRot = Quaternion.LookRotation(boundsDirections[0], Vector3.up).eulerAngles.y;
                    float nwRot = Quaternion.LookRotation(boundsDirections[1], Vector3.up).eulerAngles.y;
                    float seRot = Quaternion.LookRotation(boundsDirections[2], Vector3.up).eulerAngles.y;
                    float swRot = Quaternion.LookRotation(boundsDirections[3], Vector3.up).eulerAngles.y;

                    Vector3 point = new Vector3(0, 0, 0);
                    try
                    {
                        float sceneryEularY = sceneryColliders[i].transform.eulerAngles.y;
                        Vector3 sharedMesh = sceneryColliders[i].GetComponent<MeshFilter>().sharedMesh.bounds.extents;
                        Vector3 sceneryScale = sceneryColliders[i].transform.localScale;

                        if (IsPlayerSideOfCover(nwRot, neRot, playerRot)) // Forward Direction (Between NW - NE)
                            point = centre +
                                (Quaternion.Euler(0, sceneryEularY, 0) * -Vector3.forward * sharedMesh.z * sceneryScale.z) +
                                (Quaternion.Euler(0, sceneryEularY, 0) * -Vector3.forward * 0.15f);
                        else if (IsPlayerSideOfCover(neRot, seRot, playerRot)) // Right Direction (Between NE - SE)
                            point = centre +
                                (Quaternion.Euler(0, sceneryEularY + 90, 0) * -Vector3.forward * sharedMesh.x * sceneryScale.x) +
                                (Quaternion.Euler(0, sceneryEularY + 90, 0) * -Vector3.forward * 0.15f);
                        else if (IsPlayerSideOfCover(seRot, swRot, playerRot)) // Backwards Direction (Between SE - SW)
                            point = centre +
                                (Quaternion.Euler(0, sceneryEularY + 180, 0) * -Vector3.forward * sharedMesh.z * sceneryScale.z) +
                                (Quaternion.Euler(0, sceneryEularY + 180, 0) * -Vector3.forward * 0.15f);
                        else if (IsPlayerSideOfCover(swRot, nwRot, playerRot)) // Left Direction (Between SW - NW)
                            point = centre +
                                (Quaternion.Euler(0, sceneryEularY + 270, 0) * -Vector3.forward * sharedMesh.x * sceneryScale.x) +
                                (Quaternion.Euler(0, sceneryEularY + 270, 0) * -Vector3.forward * 0.15f);
                    }
                    catch (Exception e)
                    {
#if UNITY_EDITOR
                        Debug.LogError(e);
#endif
                    }

                    NavMeshHit hit;
                    NavMesh.FindClosestEdge(point, out hit, NavMesh.AllAreas);
                    point = hit.position;

                    if (!CheckPointNotBehindPlayer(point) || !CheckInRange(point, sightRange))
                    {
                        Profiler.EndSample();
                        return false;
                    }

                    // Checks if point is already being used and will move the point along the object till it finds a place that hasn't been used
                    if (IsCoverpointInUse(point))
                    {
                        bool usedFlag = true;
                        int movementNum = 1;
                        Vector3 movedPoint = new Vector3();
                        while (usedFlag)
                        {
                            usedFlag = false;
                            movedPoint = point + Quaternion.Euler(0, -90, 0) * hit.normal * (0.35f * movementNum);
                            if (IsCoverpointInUse(movedPoint))
                                usedFlag = true;

                            if (usedFlag)
                            {
                                usedFlag = false;
                                movedPoint = point + Quaternion.Euler(0, 90, 0) * hit.normal * (0.35f * movementNum);
                                if (IsCoverpointInUse(movedPoint))
                                    usedFlag = true;
                            }

                            movementNum++;
                            if (movementNum >= 100)
                                break;
                        }
                        point = movedPoint;
                    }

                    // Raycasts to check if cover is valid, cover wide enough
                    // Players left side (if back to wall)
                    GameObject hitTarget = CheckSightBetweenPoint(point + (Vector3.up * INCOVERDIST) + (Quaternion.Euler(0, -90, 0) * hit.normal * ARMINCOVERDIST),
                                                                  GetSightCheckPosition(CurrTarget.Trans, CurrTarget.CoverNormal, CurrTarget.CurrCoverType));
                    if (hitTarget == null || hitTarget.layer != LayerMask.NameToLayer("Scenery"))
                        return false;

                    // Players right side (if back to wall)
                    hitTarget = CheckSightBetweenPoint(point + (Vector3.up * INCOVERDIST) + (Quaternion.Euler(0, 90, 0) * hit.normal * ARMINCOVERDIST),
                                                       GetSightCheckPosition(CurrTarget.Trans, CurrTarget.CoverNormal, CurrTarget.CurrCoverType));
                    if (hitTarget == null || hitTarget.layer != LayerMask.NameToLayer("Scenery"))
                        return false;

                    coverPoint = point;
                    SetToTakeCover(point, CoverType.Up);
                    return true;
                }
            }

            return false;
        }

        /// <summary>
        /// Checks if a standing corner is a valid place to use a cover.
        /// </summary>
        /// <param name="coverObject"> The object being used as cover </param>
        /// <param name="corner"> Position vector of the corner </param>
        /// <param name="cornerDirection"> Directional vector pointing out from the corner</param>
        /// <param name="checkDirection"> -1 = Left direction, 1 = Right direction</param>
        /// <param name="coverType"> An output of the cover type it will be, left or right </param>
        /// <returns> True if a corner position can be used as cover</returns>
        private bool IsStandingCoverValid(GameObject coverObject, Vector3 corner, Vector3 cornerDirection, int checkDirection, out Vector3 point, out CoverType coverType)
        {
            coverType = CoverType.No;
            point = new Vector3();

            if (checkDirection >= 0)
                checkDirection = 1;
            else
                checkDirection = -1;

            Vector3 cornerPos = corner + (Quaternion.Euler(0, 45 * checkDirection, 0) * cornerDirection * 0.08f) +
                                         (Quaternion.Euler(0, 135 * checkDirection, 0) * cornerDirection * 0.20f);
            Vector3 cornerOffset = corner + (Quaternion.Euler(0, 45 * checkDirection, 0) * cornerDirection * 0.08f);

            NavMeshHit hit;
            NavMesh.FindClosestEdge(cornerPos, out hit, NavMesh.AllAreas);
            cornerPos = hit.position;

            float distanceToCorner = Vector3.Distance(cornerOffset, cornerPos);
            if (distanceToCorner > 0.20f)
                cornerPos += Quaternion.Euler(0, 90 * checkDirection, 0) * hit.normal * (distanceToCorner - 0.20f);
            else if (distanceToCorner < 0.20f)
                cornerPos += Quaternion.Euler(0, 90 * checkDirection * -1, 0) * hit.normal * (0.20f - distanceToCorner);

            NavMesh.FindClosestEdge(cornerPos, out hit, NavMesh.AllAreas);
            cornerPos = hit.position;

            if (!CheckPointNotBehindPlayer(cornerPos) || !CheckInRange(cornerPos, sightRange))
                return false;

            // Makes sure another AI isn't already moving towards or using the point
            if (IsCoverpointInUse(cornerPos))
                return false;

            // AIs left side (if back to wall)
            GameObject hitTarget = CheckSightBetweenPoint(cornerPos + (Vector3.up * INCOVERDIST) + (Quaternion.Euler(0, -90, 0) * hit.normal * ARMINCOVERDIST),
                                                          GetSightCheckPosition(CurrTarget.Trans, CurrTarget.CoverNormal, CurrTarget.CurrCoverType));
            if (hitTarget == null || hitTarget.layer != LayerMask.NameToLayer("Scenery"))
                return false;

            // AIs right side (if back to wall)
            hitTarget = CheckSightBetweenPoint(cornerPos + (Vector3.up * INCOVERDIST) + (Quaternion.Euler(0, 90, 0) * hit.normal * ARMINCOVERDIST),
                                               GetSightCheckPosition(CurrTarget.Trans, CurrTarget.CoverNormal, CurrTarget.CurrCoverType));
            if (hitTarget == null || hitTarget.layer != LayerMask.NameToLayer("Scenery"))
                return false;

            // AI will look left
            hitTarget = CheckSightBetweenPoint(cornerPos + (Vector3.up * INCOVERDIST) + (Quaternion.Euler(0, -90, 0) * hit.normal * COVERPEAKSIDEDIST),
                                               GetSightCheckPosition(CurrTarget.Trans, CurrTarget.CoverNormal, CurrTarget.CurrCoverType));
            if (hitTarget != null && hitTarget.layer != LayerMask.NameToLayer("Scenery"))
                coverType = CoverType.Left;

            // AI will look right
            hitTarget = CheckSightBetweenPoint(cornerPos + (Vector3.up * INCOVERDIST) + (Quaternion.Euler(0, 90, 0) * hit.normal * COVERPEAKSIDEDIST),
                                               GetSightCheckPosition(CurrTarget.Trans, CurrTarget.CoverNormal, CurrTarget.CurrCoverType));
            if (hitTarget != null && hitTarget.layer != LayerMask.NameToLayer("Scenery"))
                coverType = CoverType.Right;

            if (coverType != CoverType.No)
            {
                coverPoint = cornerPos;
                point = cornerPos;
                return true;
            }
            else
                return false;
        }

        /// <summary>
        /// Checks to see if player is on object side
        /// </summary>
        /// <param name="minRot"> Minimum, most ACW rotation of the area to be checked </param>
        /// <param name="maxRot"> Maximum, most CW rotation of the area to be checked </param>
        /// <param name="playerRot"> Players rotation with respect to the object </param>
        /// <returns> Is the player on this side of the object </returns>
        private bool IsPlayerSideOfCover(float minRot, float maxRot, float playerRot)
        {
            if (maxRot > minRot && playerRot > minRot && playerRot < maxRot)
                return true;
            else if (maxRot < minRot && (playerRot > minRot || playerRot < maxRot))
                return true;

            return false;
        }

        /// <summary>
        /// True if another AI is already moving towards or using the point
        /// </summary>
        /// <param name="point"> Point to be checked for cover </param>
        /// <returns> Is the point currently being used </returns>
        private bool IsCoverpointInUse(Vector3 point)
        {
            List<Enemy> enemies = MissionInput.Instance.ActiveEnemies;
            foreach (Enemy e in enemies)
                if (e.GetInstanceID() != GetInstanceID() && Vector3.Distance(point, e.navAgent.destination) <= 0.3f)
                    return true;

            return false;
        }

        /// <summary>
        /// Checks to see if the AI has direct line of sight of an object it want s to use as cover
        /// </summary>
        /// <param name="coverObject"> Object to be checked for cover </param>
        /// <returns> True if the AI has direct line of sight </returns>
        private bool CheckSightOfCoverObject(Transform coverObject)
        {
            Vector3 start = GetSightCheckPosition(trans, CoverNormal, CurrCoverType);
            Vector3 direction = coverObject.position - start;
            direction.Normalize();

            float distance = Vector3.Distance(start, coverObject.position);

            Ray ray = new Ray(start, direction);
            RaycastHit hit;
            if (Physics.Raycast(ray, out hit, distance))
            {
                if (hit.collider.transform.GetInstanceID() == coverObject.GetInstanceID())
                    return true;
                else if (hit.collider.gameObject.layer != LayerMask.NameToLayer("Scenery"))
                {
                    RaycastHit rehit;
                    Vector3 hitPoint = hit.point;
                    float distanceLeft = distance - Vector3.Distance(hitPoint, start) - 0.05f;
                    int iterations = 0;
                    while (MissionInput.RaycastThroughObject(hitPoint, direction, distanceLeft, out rehit))
                    {
                        if (hit.collider.transform.GetInstanceID() == coverObject.GetInstanceID())
                            return true;
                        else if (hit.collider.gameObject.layer != LayerMask.NameToLayer("Scenery"))
                        {
                            hitPoint = rehit.point;
                            distanceLeft = distance - Vector3.Distance(hitPoint, start) - 0.05f;

                            iterations++;

                            if (iterations > 50)
                                break;

                            continue;
                        }
                        else
                            return false;
                    }
                    return false;
                }
                else
                    return false;
            }

            return false;
        }

        /// <summary>
        /// Checks to see if the AI would be running past the player to get to a cover point
        /// </summary>
        /// <param name="point"> The point to be checked </param>
        /// <returns> True if the AI won't run past the player </returns>
        private bool CheckPointNotBehindPlayer(Vector3 point)
        {
            Vector3 playerDir = CurrTarget.Trans.position - trans.position;
            playerDir.Normalize();
            Vector3 leftDir = Quaternion.Euler(0, -45, 0) * playerDir;
            Vector3 rightDir = Quaternion.Euler(0, 45, 0) * playerDir;
            Vector3 objectDir = point - trans.position;
            objectDir.Normalize();

            float objectRot = Quaternion.LookRotation(objectDir, Vector3.up).eulerAngles.y;
            float leftRot = Quaternion.LookRotation(leftDir, Vector3.up).eulerAngles.y;
            float rightRot = Quaternion.LookRotation(rightDir, Vector3.up).eulerAngles.y;

            float playerDist = Vector3.Distance(CurrTarget.Trans.position, trans.position);
            float objectDist = Vector3.Distance(point, trans.position);

            if (rightRot > leftRot && objectRot > leftRot && objectRot < rightRot && objectDist >= playerDist)
                return false;
            else if (rightRot < leftRot && (objectRot > leftRot || objectRot < rightRot) && objectDist >= playerDist)
                return false;
            else
                return true;
        }

        #endregion

        #region Sight and Target Methods

        /// <summary>
        /// Checks whether there is any new targets within sight range and can be seen
        /// </summary>
        /// <returns> Returns the closest target in sight, null if none are found </returns>
        private Agent CheckForTargets()
        {
            Agent[] agents = MissionInput.Instance.AllAgents;
            float dist = 1000.0f;
            Agent curr = null;
            if (agents.Length != 0)
            {
                foreach (Agent a in agents)
                {
                    if (CheckInRange(a.Trans.position, sightRange) && CheckLineOfSight(a))
                    {
                        float temp = Vector3.Distance(a.Trans.position, trans.position);
                        if (temp < dist)
                        {
                            dist = temp;
                            curr = a;
                        }
                    }
                }
            }
            return curr;
        }

        #endregion

        #region Util Methods


        /// <summary>
        /// Sorts a list of colliders by distance from the AI, from closest to furthest.
        /// </summary>
        /// <param name="colliderList"> The list to be sorted. </param>
        /// <returns> The list provided but sorted by distance. </returns>
        private List<Collider> SortColliderByDistance(List<Collider> colliderList)
        {
            trans = transform;

            List<Collider> sortedList = new List<Collider>();
            int listSize = colliderList.Count;
            for (int i = 0; i < listSize; i++)
            {
                Collider closest = null;
                float currDistance = 1000;
                int index = 0;
                for (int j = 0; j < colliderList.Count; j++)
                {
                    if (closest == null)
                    {
                        closest = colliderList[j];
                        currDistance = Vector3.Distance(trans.position, colliderList[j].transform.position);
                        index = j;
                    }
                    else
                    {
                        float newDistance = Vector3.Distance(trans.position, colliderList[j].transform.position);
                        if (newDistance < currDistance)
                        {
                            closest = colliderList[j];
                            currDistance = newDistance;
                            index = j;
                        }
                    }
                }
                sortedList.Add(closest);
                colliderList.RemoveAt(index);
            }
            return sortedList;
        }


        /// <summary>
        /// Finds the bounding corners of a provided object transform and outputs the directions of the corners.
        /// </summary>
        /// <param name="objTrans"> The transform of the object to be checked. </param>
        /// <param name="cornerDirections"> Outputs the directional vector3 of the corners. </param>
        /// <returns> An array or the position of the corners. </returns>
        private Vector3[] GetObjectBounds(Transform objTrans, out Vector3[] cornerDirections)
        {
            Vector3[] bounds = new Vector3[4];
            cornerDirections = new Vector3[4];
            Bounds objectBounds = objTrans.GetComponent<MeshFilter>().sharedMesh.bounds;
            bounds[0] = objTrans.transform.position +
                        (Quaternion.Euler(0, 90 + objTrans.transform.eulerAngles.y, 0) * Vector3.forward * objectBounds.extents.x * objTrans.transform.localScale.x) +
                        (Quaternion.Euler(0, objTrans.transform.eulerAngles.y, 0) * Vector3.forward * objectBounds.extents.z * objTrans.transform.localScale.z) +
                        new Vector3(0, objectBounds.size.y * objTrans.transform.localScale.y, 0); // NE
            bounds[1] = objTrans.transform.position +
                        (Quaternion.Euler(0, 90 + objTrans.transform.eulerAngles.y, 0) * Vector3.forward * -objectBounds.extents.x * objTrans.transform.localScale.x) +
                        (Quaternion.Euler(0, objTrans.transform.eulerAngles.y, 0) * Vector3.forward * objectBounds.extents.z * objTrans.transform.localScale.z) +
                        new Vector3(0, objectBounds.size.y * objTrans.transform.localScale.y, 0); // NW
            bounds[2] = objTrans.transform.position +
                        (Quaternion.Euler(0, 90 + objTrans.transform.eulerAngles.y, 0) * Vector3.forward * objectBounds.extents.x * objTrans.transform.localScale.x) +
                        (Quaternion.Euler(0, objTrans.transform.eulerAngles.y, 0) * Vector3.forward * -objectBounds.extents.z * objTrans.transform.localScale.z) +
                        new Vector3(0, objectBounds.size.y * objTrans.transform.localScale.y, 0); // SE
            bounds[3] = objTrans.transform.position +
                        (Quaternion.Euler(0, 90 + objTrans.transform.eulerAngles.y, 0) * Vector3.forward * -objectBounds.extents.x * objTrans.transform.localScale.x) +
                        (Quaternion.Euler(0, objTrans.transform.eulerAngles.y, 0) * Vector3.forward * -objectBounds.extents.z * objTrans.transform.localScale.z) +
                        new Vector3(0, objectBounds.size.y * objTrans.transform.localScale.y, 0); // SW

            cornerDirections[0] = Quaternion.Euler(0, 45 + objTrans.transform.eulerAngles.y, 0) * Vector3.forward;
            cornerDirections[1] = Quaternion.Euler(0, -45 + objTrans.transform.eulerAngles.y, 0) * Vector3.forward;
            cornerDirections[2] = Quaternion.Euler(0, 135 + objTrans.transform.eulerAngles.y, 0) * Vector3.forward;
            cornerDirections[3] = Quaternion.Euler(0, -135 + objTrans.transform.eulerAngles.y, 0) * Vector3.forward;


            return bounds;
        }

        #endregion
    }
}
