class KrydenRewardsSeaChest extends SeaChest
{
    protected bool m_AllowSystemCargoInsert;

    void KrydenRewardsSeaChest()
    {
        m_AllowSystemCargoInsert = false;
    }

    void SetAllowSystemCargoInsert(bool allowInsert)
    {
        m_AllowSystemCargoInsert = allowInsert;
    }

    override bool CanPutIntoHands(EntityAI parent)
    {
        return false;
    }

    override bool CanPutInCargo(EntityAI parent)
    {
        return false;
    }

    override bool CanReceiveItemIntoCargo(EntityAI item)
    {
        if (!m_AllowSystemCargoInsert)
        {
            return false;
        }

        return super.CanReceiveItemIntoCargo(item);
    }

    override bool CanSwapItemInCargo(EntityAI child_entity, EntityAI new_entity)
    {
        if (!m_AllowSystemCargoInsert)
        {
            return false;
        }

        return super.CanSwapItemInCargo(child_entity, new_entity);
    }

    override void OnCEUpdate()
    {
        super.OnCEUpdate();

        #ifdef SERVER
        if (!m_AllowSystemCargoInsert && IsEmpty())
        {
            Delete();
        }
        #endif
    }
}