namespace Code.Agents
{
    public interface IPerson
    {
        string PersonName { get; }
        float Health { get; }
        float MaxHealth { get; }
    }

    public interface ICombatPerson : IPerson
    {
        int IdTag { get; }
    }
}
