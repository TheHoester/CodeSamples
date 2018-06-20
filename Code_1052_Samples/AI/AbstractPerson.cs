using UnityEngine;
using UnityEngine.AI;
using UnityEngine.Profiling;

namespace Code.Agents
{
    [RequireComponent(typeof(Animator)), RequireComponent(typeof(NavMeshAgent))]
    public abstract class AbstractPerson : MonoBehaviour, IPerson
    {
        [SerializeField]
        private string personName;
        
        [SerializeField, Header("Health")]
        private float health, maxHealth;

        protected Animator anim;
        protected NavMeshAgent navAgent;
        protected Transform trans;

        [SerializeField, Header("Other")]
        private float standardStoppingDist = 0.5f;

        protected virtual void Awake()
        {
            anim = GetComponent<Animator>();
            navAgent = GetComponent<NavMeshAgent>();
            trans = GetComponent<Transform>();
            navAgent.stoppingDistance = standardStoppingDist;
        }

        public Transform Trans {
            get {
                if (trans == null)
                    trans = GetComponent<Transform>();
                return trans;
            } }

        public float StandardStoppingDist
        {
            get { return standardStoppingDist; }
            protected set { standardStoppingDist = value; }
        }

        public string PersonName
        {
            get { return personName; }
            protected set { personName = value; }
        }

        public float Health
        {
            get { return health; }
            protected set { health = value; }
        }

        public float MaxHealth
        {
            get { return maxHealth; }
            protected set { maxHealth = value; }
        }

        public bool isMoving { get { return navAgent.velocity.magnitude > 0.01f; } }

		/// <summary>
        /// Used to set the current position of the AI. (A work around for a Unity glitch)
        /// </summary>
        /// <param name="location"> The position to be set </param>
        public virtual void SetLocalLocation(Vector3 location)
        {
            navAgent.enabled = false;
            trans.localPosition = location;
            navAgent.enabled = true;
        }

		/// <summary>
        /// Used to set the current position and rotation of the AI. (A work around for a Unity glitch)
        /// </summary>
        /// <param name="location"> The position to be set </param>
        /// <param name="yRotation"> The rotation to be set </param>
        public virtual void SetPositioning(Vector3 location, float yRotation)
        {
            navAgent.enabled = false;
            trans.position = location;
            trans.rotation = Quaternion.Euler(trans.rotation.x, yRotation, trans.rotation.z);
            navAgent.enabled = true;
        }

		/// <summary>
        /// Sets the destination of the AI to the NavMeshAgent.
        /// </summary>
        /// <param name="destination"> The position to be set to </param>
        protected virtual void SetDestination(Vector3 destination)
        {
            Profiler.BeginSample("NavMesh Pathing");
            if(!navAgent.isOnNavMesh)
            {
                Profiler.EndSample();
                return;
            }

            NavMeshPath path = new NavMeshPath();
            if (navAgent.CalculatePath(destination, path))
                navAgent.SetPath(path);
            else
                Debug.LogError("Unable to calculate path");

            Profiler.EndSample();
        }

		/// <summary>
        /// Updates the variable values of the animation controller.
        /// </summary>
        protected virtual void UpdateAnimation()
        {
            Profiler.BeginSample("Update Animation");

            anim.SetFloat("Speed", navAgent.velocity.magnitude);

            Profiler.EndSample();
        }
    }
}
