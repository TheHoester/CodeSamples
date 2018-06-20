using Code.AI.Decorative;
using Code.Mission;
using UnityEngine;

namespace Code.Agents
{
    public class AbstractCombatPerson : AbstractPerson, ICombatPerson
    {
        #region Variables

        protected const float GUNSHOTDIST = 0.37f;
        protected const float INCOVERDIST = 0.16f;
        protected const float TOPHEADINCOVERDIST = 0.21f;
        protected const float ARMINCOVERDIST = 0.08f;
        protected const float COVERPEAKSIDEDIST = 0.24f;

        private int idTag = -1;
        
        protected Weapon currWeapon;
        protected Armour currArmour;
        protected Weapon glockObject, assualtRifleObject, doubleShotgunObject, tazerObject;

        protected float fireTimer;
        protected float deathTimer;
        protected float critChanceModifier = 0.0f;
        private float coverStoppingDistance = 0.01f;
        protected bool hasBeenTazed;
        
        #endregion

        protected override void Awake()
        {
            base.Awake();
            navAgent.stoppingDistance = StandardStoppingDist;
        }

        protected virtual void OnTriggerEnter(Collider other)
        {
            //check if hit by a car
            if (!other.CompareTag("Vehicle")) return;

            RoamingCar car = other.GetComponent<RoamingCar>();
            if (!car || car.CurrentSpeed <= 0.1f) return;

            if (currArmour != null)
                TakeDamage(MaxHealth + currArmour.MaxArmourHealth + 2);
            else
                TakeDamage(MaxHealth + 2);
        }

        #region Properties

        public int IdTag { get { return idTag; } protected set { idTag = value; } }
        public ClassType AgentClass { get; protected set; }
        public Weapon CurrWeapon { get { return currWeapon; } }
        public Armour CurrArmour { get { return currArmour; } }
        public ActionState CurrentState { get; private set; }
        public AbstractCombatPerson CurrTarget { get; protected set; }
        public bool IsInCover { get; protected set; }
        public CoverType CurrCoverType { get; protected set; }
        public float CoverStoppingDistance { get { return coverStoppingDistance; } protected set { coverStoppingDistance = value; } }
        public Vector3 CoverNormal { get; protected set; }
        public bool Dead { get { return Health + GetArmourHealth() <= 0f; } }

        public float GetArmourMax()
        {
            if (currArmour == null || !currArmour.gameObject.activeInHierarchy)
                return 0.0f;
            else
                return currArmour.MaxArmourHealth;
        }

        public float GetArmourHealth()
        {
            if (currArmour == null || !currArmour.gameObject.activeInHierarchy)
                return 0.0f;
            else
                return currArmour.ArmourHealth;
        }

        #endregion

        #region Set State Methods

        /// <summary>
        /// Actives the correct object for the weapon that has been selected.
        /// </summary>
        /// <param name="wType"> The weapon type of the weapon that has been selected. </param>
        public void SetWeaponObjectByType(WeaponType wType)
        {
            switch (wType)
            {
                case WeaponType.pistol:
                    glockObject.gameObject.SetActive(true);
                    assualtRifleObject.gameObject.SetActive(false);
                    doubleShotgunObject.gameObject.SetActive(false);
                    tazerObject.gameObject.SetActive(false);
                    currWeapon = glockObject;
                    break;
                case WeaponType.rifle:
                    glockObject.gameObject.SetActive(false);
                    assualtRifleObject.gameObject.SetActive(true);
                    doubleShotgunObject.gameObject.SetActive(false);
                    tazerObject.gameObject.SetActive(false);
                    currWeapon = assualtRifleObject;
                    break;
                case WeaponType.shotgun:
                    glockObject.gameObject.SetActive(false);
                    assualtRifleObject.gameObject.SetActive(false);
                    doubleShotgunObject.gameObject.SetActive(true);
                    tazerObject.gameObject.SetActive(false);
                    currWeapon = doubleShotgunObject;
                    break;
                case WeaponType.tazer:
                    glockObject.gameObject.SetActive(false);
                    assualtRifleObject.gameObject.SetActive(false);
                    doubleShotgunObject.gameObject.SetActive(false);
                    tazerObject.gameObject.SetActive(true);
                    currWeapon = tazerObject;
                    break;
            }
        }

        /// <summary>
        /// Use for the initial setting to idle when the AI is first created.
        /// </summary>
        /// <param name="init"> Doesn't mater the value. </param>
        protected void SetToIdle(bool init)
        {
            CurrentState = ActionState.Idle;
            navAgent.stoppingDistance = StandardStoppingDist;
            CurrTarget = null;
        }

        /// <summary>
        /// Sets the AI values to its idle state.
        /// </summary>
        protected void SetToIdle()
        {
            SetToIdle(trans.position);
        }

        /// <summary>
        /// Sets the AI values to its idle state and gives it a place to move to.
        /// </summary>
        /// <param name="destination"> Position the AI should move to. </param>
        protected void SetToIdle(Vector3 destination)
        {
            CurrentState = ActionState.Idle;
            navAgent.stoppingDistance = StandardStoppingDist;
            SetDestination(destination);
            CurrTarget = null;
        }

        /// <summary>
        /// Sets the AI values to its moving state.
        /// </summary>
        protected void SetToMoving()
        {
            CurrentState = ActionState.MoveToEnemy;
            navAgent.stoppingDistance = StandardStoppingDist;
            IsInCover = false;
            CurrCoverType = CoverType.No;
            SetDestination(CurrTarget.Trans.position);
        }

        /// <summary>
        /// Sets the AI values to its moving state and sets its current target.
        /// </summary>
        /// <param name="enemy"> The new current target. </param>
        protected void SetToMoving(AbstractCombatPerson enemy)
        {
            SetToMoving();
            CurrTarget = enemy;
            SetDestination(CurrTarget.Trans.position);
        }

        /// <summary>
        /// Sets the AI values to its take cover state and sets its cover type.
        /// </summary>
        /// <param name="pos"> Position of the cover. </param>
        /// <param name="type"> Type of cover. </param>
        protected void SetToTakeCover(Vector3 pos, CoverType type)
        {
            CurrentState = ActionState.MoveToCover;
            CurrCoverType = type;
            navAgent.stoppingDistance = coverStoppingDistance;
            IsInCover = false;
            SetDestination(pos);
        }

        /// <summary>
        /// Sets the AI values to its attack state.
        /// </summary>
        protected void SetToAttack()
        {
            CurrentState = ActionState.AttackEnemy;
            navAgent.stoppingDistance = StandardStoppingDist;
            SetDestination(trans.position);
        }

        /// <summary>
        /// Sets the AI values to its attack state and sets its current target.
        /// </summary>
        /// <param name="enemy"> The new current target. </param>
        protected void SetToAttack(AbstractCombatPerson enemy)
        {
            SetToAttack();
            CurrTarget = enemy;
        }

        /// <summary>
        /// Sets the AI values to its dying state.
        /// </summary>
        protected void SetToDying()
        {
            CurrentState = ActionState.Dying;
            navAgent.stoppingDistance = StandardStoppingDist;
            IsInCover = false;
            CurrCoverType = CoverType.No;
            SetDestination(trans.position);
        }

        #endregion

        #region Line Of Sight Methods

        /// <summary>
        /// Gets the offset for the sight check depending on the type of cover that the target is using
        /// </summary>
        /// <param name="target"> The transform of the AI that offset is needed from </param>
        /// <param name="normal"> The normal of the cover that is being used </param>
        /// <param name="type"> The cover type that the AI is currently using </param>
        /// <returns> The position that the AI can be seen from. </returns>
        protected Vector3 GetSightCheckPosition(Transform target, Vector3 normal, CoverType type)
        {
            Vector3 point = new Vector3();

            if (type == CoverType.No || type == CoverType.Up)
                point = target.position + (Vector3.up * GUNSHOTDIST);
            else if (type == CoverType.Left)
                point = target.position + (Vector3.up * GUNSHOTDIST) + (Quaternion.Euler(0, -90, 0) * normal * COVERPEAKSIDEDIST);
            else if (type == CoverType.Right)
                point = target.position + (Vector3.up * GUNSHOTDIST) + (Quaternion.Euler(0, 90, 0) * normal * COVERPEAKSIDEDIST);

            return point;
        }

        /// <summary>
        /// Checks whether there is a line of sight between this AI and their target
        /// </summary>
        /// <returns> True if there is line of sight </returns>
        protected bool CheckLineOfSight()
        {
            return CheckLineOfSight(CurrTarget);
        }

        /// <summary>
        /// Checks whether there is a line of sight between this AI and their target
        /// </summary>
        /// <param name="target"> The target of the line of sight check </param>
        /// <returns> True if there is line of sight </returns>
        protected bool CheckLineOfSight(AbstractCombatPerson target)
        {
            return CheckLineOfSight(target, CoverNormal);
        }

        /// <summary>
        /// Checks whether there is a line of sight between this AI and their target
        /// </summary>
        /// <param name="target"> The target of the line of sight check </param>
        /// <param name="normal"> The normal of the cover of this AI </param>
        /// <returns> True if there is line of sight </returns>
        protected bool CheckLineOfSight(AbstractCombatPerson target, Vector3 normal)
        {
            Vector3 start = GetSightCheckPosition(trans, normal, CurrCoverType);
            Vector3 end = GetSightCheckPosition(target.Trans, target.CoverNormal, target.CurrCoverType);
            Vector3 direction = end - start;
            direction.Normalize();

            float distance = Vector3.Distance(start, end);

            Ray ray = new Ray(start, direction);
            RaycastHit hit;
            if (Physics.Raycast(ray, out hit, distance))
            {
                if (hit.collider.CompareTag(target.tag))
                    return true;
                else if (hit.collider.CompareTag(tag))
                {
                    RaycastHit rehit;
                    Vector3 hitPoint = hit.point;
                    float distanceLeft = distance - Vector3.Distance(hitPoint, start) - 0.05f;
                    int iterations = 0;
                    while (MissionInput.RaycastThroughObject(hitPoint, direction, distanceLeft, out rehit))
                    {
                        if (hit.collider.CompareTag(target.tag))
                            return true;
                        else if (hit.collider.CompareTag(tag))
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

                    return true;
                }
                else
                    return false;
            }
            else
                return true;
        }

        /// <summary>
        /// Checks to see if target is within a specified range
        /// </summary>
        /// <param name="target"> The target position </param>
        /// <param name="range"> The range to be checked</param>
        /// <returns></returns>
        protected bool CheckInRange(Vector3 target, float range)
        {
            return CheckInRange(target, trans.position, range);
        }

        /// <summary>
        /// Checks to see if target is within a specified range
        /// </summary>
        /// <param name="target"> The target position </param>
        /// <param name="from"> The origin of the range check </param>
        /// <param name="range"> The range to be checked</param>
        protected bool CheckInRange(Vector3 target, Vector3 from, float range)
        {
            if (Vector3.Distance(target, from) <= range)
                return true;
            else
                return false;
        }

        /// <summary>
        /// Gets an object that is between from and to
        /// </summary>
        /// <param name="from"> Start point of check </param>
        /// <param name="to"> End point of check </param>
        /// <returns> The gameobject that was hit </returns>
        protected GameObject CheckSightBetweenPoint(Vector3 from, Vector3 to)
        {
            Vector3 direction = to - from;
            direction.Normalize();

            Ray ray = new Ray(from, direction);
            RaycastHit hit;
            if (Physics.Raycast(ray, out hit, 100))
                return hit.transform.gameObject;
            else
                return null;
        }

        #endregion

        #region Animation Methods

		/// <summary>
        /// Plays the death timer and turns the gameobject off when time limit reached.
        /// </summary>
        protected void PlayDeathAnimation()
        {
            deathTimer += Time.deltaTime;
            if (deathTimer >= 2.25f)
                gameObject.SetActive(false);
        }

		/// <summary>
        /// Updates the variable values of the animation controller.
        /// </summary>
        protected override void UpdateAnimation()
        {
            base.UpdateAnimation();
            anim.SetInteger("State", (int)CurrentState);
            anim.SetBool("IsInCover", IsInCover);
            anim.SetInteger("CoverState", (int)CurrCoverType);
            anim.SetInteger("Weapon", (int)CurrWeapon.Type);
        }

        #endregion

        #region Util Methods

        /// <summary>
        /// Updates the fire timer.
        /// </summary>
        protected void UpdateFireTimer()
        {
            if (fireTimer < currWeapon.FireRate)
                fireTimer += Time.deltaTime;
        }

        /// <summary>
        /// Damage is dealt to the AI.
        /// </summary>
        /// <param name="amount">Damage value</param>
        public void TakeDamage(float amount)
        {
            if (currArmour != null)
            {
                if (amount > currArmour.ArmourHealth)
                    Health -= amount - currArmour.ArmourHealth;

                if (currArmour.ArmourHealth > 0)
                    currArmour.Damage(amount);
                else
                    Health -= amount;
            }
            else
                Health -= amount;

            if (Health <= 0)
                SetToDying();
        }

        /// <summary>
        /// Damage is dealt to the AI and if it isn't already engaging another target, will engage the target that damaged them.
        /// </summary>
        /// <param name="amount">Damage value</param>
        /// <param name="from">The target that dealt the damge</param>
        public virtual void TakeDamage(float amount, AbstractCombatPerson from)
        {
            TakeDamage(amount);
            if (CurrentState == ActionState.Dying && from.CurrWeapon.Type == WeaponType.tazer)
                hasBeenTazed = true;

            if (CurrentState != ActionState.Dying && !IsTargetAlive())
                CurrTarget = from;
        }
		
		/// <summary>
        /// Will return with the AI is alive or not.
        /// </summary>
        /// <returns> True if the AI is alive, else false </returns>
        protected bool IsTargetAlive()
        {
            return CurrTarget != null && CurrTarget.gameObject.activeSelf && CurrTarget.CurrentState != ActionState.Dying;
        }

        #endregion
    }

    public enum ClassType
    {
        Pistol, Rifle, Shotgun
    }

    public enum ActionState
    {
        Idle, MoveToEnemy, MoveToCover, AttackEnemy, Dying
    }

    public enum CoverType
    {
        No, Up, Left, Right
    }
}
