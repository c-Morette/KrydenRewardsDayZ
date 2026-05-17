class KrydenRewardsDeliveryContext
{
    ref array<EntityAI> createdEntities;

    void KrydenRewardsDeliveryContext()
    {
        createdEntities = new array<EntityAI>();
    }

    void Track(EntityAI entity)
    {
        if (entity)
        {
            createdEntities.Insert(entity);
        }
    }

    void Rollback()
    {
        for (int i = createdEntities.Count() - 1; i >= 0; i--)
        {
            EntityAI entity = createdEntities.Get(i);
            if (entity)
            {
                entity.Delete();
            }
        }

        createdEntities.Clear();
    }
}

class KrydenRewardsRedeemService
{
    private static const int MAX_NESTED_REWARD_DEPTH = 8;
    private static const float VEHICLE_SPAWN_DISTANCE = 10.0;
    private static const float GROUND_SPAWN_DISTANCE = 1.2;
    private static const float VEHICLE_LIFETIME_SECONDS = 3888000.0;

    static void ProcessRedeem(PlayerBase player, string code)
    {
        if (!GetGame().IsServer())
        {
            return;
        }
        if (!player || !player.GetIdentity())
        {
            return;
        }
        code.Trim();
        code.ToUpper();
        if (code == "")
        {
            player.MessageAction("[Kryden] Use: //resgatar CODIGO");
            return;
        }
        string steamId = player.GetIdentity().GetPlainId();
        KrydenRewardsConfig config = KrydenRewardsConfig.Load();
        if (!config.IsConfigured())
        {
            player.MessageAction("[Kryden] Sistema ainda nao configurado. Avise a administracao.");
            return;
        }
        if (config.debugLogs)
        {
            Print("[KrydenRewards] Redeem requested. code=" + code + " steamId=" + steamId);
        }
        player.MessageAction("[Kryden] Consultando recompensa...");
        KrydenRewardsRedeemResponse preview;
        string previewMessage;
        if (!KrydenRewardsApiHelper.RedeemPreview(code, steamId, config, preview, previewMessage))
        {
            player.MessageAction("[Kryden] " + previewMessage);
            return;
        }
        if (!preview.items || preview.items.Count() == 0)
        {
            player.MessageAction("[Kryden] Nenhum item retornado para este resgate.");
            return;
        }

        bool delivered = DeliverItems(player, preview.items);
        if (!delivered)
        {
            KrydenRewardsRedeemResponse failResponse;
            string failMessage;
            if (!KrydenRewardsApiHelper.RedeemFail(code, steamId, config, failResponse, failMessage))
            {
                Print("[KrydenRewards] Failed to notify delivery failure for code=" + code + " steamId=" + steamId + " message=" + failMessage);
            }

            player.MessageAction("[Kryden] Falha ao entregar itens. Tente liberar espaco e avise a administracao.");
            return;
        }
        KrydenRewardsRedeemResponse confirm;
        string confirmMessage;
        if (!KrydenRewardsApiHelper.RedeemConfirm(code, steamId, config, confirm, confirmMessage))
        {
            player.MessageAction("[Kryden] Itens entregues, mas a confirmacao falhou. Avise a administracao.");
            Print("[KrydenRewards] Confirm failed for code=" + code + " steamId=" + steamId + " message=" + confirmMessage);
            return;
        }
        player.MessageAction("[Kryden] Resgate concluido: " + preview.requestId);
    }

    private static bool DeliverItems(PlayerBase player, array<ref KrydenRewardsRedeemItem> items)
    {
        KrydenRewardsDeliveryContext deliveryContext = new KrydenRewardsDeliveryContext();

        foreach (KrydenRewardsRedeemItem rewardItem : items)
        {
            if (!SpawnRootReward(player, rewardItem, deliveryContext, 0))
            {
                deliveryContext.Rollback();
                return false;
            }
        }

        return true;
    }

    private static bool SpawnRootReward(PlayerBase player, KrydenRewardsRedeemItem rewardItem, KrydenRewardsDeliveryContext deliveryContext, int depth)
    {
        int quantity;
        int extraIndex;
        int rootIndex;
        bool canUseSingleStack;
        EntityAI firstEntity;
        EntityAI extraEntity;
        EntityAI createdEntity;

        if (!rewardItem || rewardItem.className == "" || depth > MAX_NESTED_REWARD_DEPTH)
        {
            return false;
        }

        quantity = GetNormalizedQuantity(rewardItem);
        canUseSingleStack = !rewardItem.HasNestedDefinitions();

        if (canUseSingleStack)
        {
            if (!CreateRewardEntity(player, null, rewardItem, false, deliveryContext, depth, firstEntity))
            {
                return false;
            }

            if (quantity > 1 && !ApplyStackQuantityIfPossible(firstEntity, quantity))
            {
                for (extraIndex = 1; extraIndex < quantity; extraIndex++)
                {
                    if (!CreateRewardEntity(player, null, rewardItem, false, deliveryContext, depth, extraEntity))
                    {
                        return false;
                    }
                }
            }

            return true;
        }

        for (rootIndex = 0; rootIndex < quantity; rootIndex++)
        {
            if (!CreateRewardEntity(player, null, rewardItem, false, deliveryContext, depth, createdEntity))
            {
                return false;
            }
        }

        return true;
    }

    private static bool CreateRewardEntity(PlayerBase player, EntityAI parent, KrydenRewardsRedeemItem rewardItem, bool createAsAttachment, KrydenRewardsDeliveryContext deliveryContext, int depth, out EntityAI createdEntity)
    {
        KrydenRewardsSeaChest rewardContainer;

        createdEntity = null;

        if (!player || !rewardItem || rewardItem.className == "" || depth > MAX_NESTED_REWARD_DEPTH)
        {
            return false;
        }

        createdEntity = CreateEntityInstance(player, parent, rewardItem.className, createAsAttachment);
        if (!createdEntity)
        {
            return false;
        }

        deliveryContext.Track(createdEntity);

        if (!SpawnChildRewards(player, createdEntity, rewardItem.attachments, true, deliveryContext, depth + 1))
        {
            return false;
        }

        rewardContainer = KrydenRewardsSeaChest.Cast(createdEntity);
        if (rewardContainer)
        {
            rewardContainer.SetAllowSystemCargoInsert(true);
        }

        if (!SpawnChildRewards(player, createdEntity, rewardItem.cargo, false, deliveryContext, depth + 1))
        {
            if (rewardContainer)
            {
                rewardContainer.SetAllowSystemCargoInsert(false);
            }

            return false;
        }

        if (rewardContainer)
        {
            rewardContainer.SetAllowSystemCargoInsert(false);
        }

        FinalizeCreatedEntity(createdEntity);
        return true;
    }

    private static bool SpawnChildRewards(PlayerBase player, EntityAI parent, array<ref KrydenRewardsRedeemItem> childItems, bool createAsAttachment, KrydenRewardsDeliveryContext deliveryContext, int depth)
    {
        if (!childItems)
        {
            return true;
        }

        foreach (KrydenRewardsRedeemItem childItem : childItems)
        {
            if (!SpawnChildReward(player, parent, childItem, createAsAttachment, deliveryContext, depth))
            {
                return false;
            }
        }

        return true;
    }

    private static bool SpawnChildReward(PlayerBase player, EntityAI parent, KrydenRewardsRedeemItem rewardItem, bool createAsAttachment, KrydenRewardsDeliveryContext deliveryContext, int depth)
    {
        int quantity;
        int extraIndex;
        int childIndex;
        bool canUseSingleStack;
        EntityAI firstEntity;
        EntityAI extraEntity;
        EntityAI createdEntity;

        if (!rewardItem || rewardItem.className == "" || !parent || depth > MAX_NESTED_REWARD_DEPTH)
        {
            return false;
        }

        if (createAsAttachment)
        {
            quantity = 1;
        }
        else
        {
            quantity = GetNormalizedQuantity(rewardItem);
        }

        canUseSingleStack = !createAsAttachment && !rewardItem.HasNestedDefinitions();

        if (canUseSingleStack)
        {
            if (!CreateRewardEntity(player, parent, rewardItem, false, deliveryContext, depth, firstEntity))
            {
                return false;
            }

            if (quantity > 1 && !ApplyStackQuantityIfPossible(firstEntity, quantity))
            {
                for (extraIndex = 1; extraIndex < quantity; extraIndex++)
                {
                    if (!CreateRewardEntity(player, parent, rewardItem, false, deliveryContext, depth, extraEntity))
                    {
                        return false;
                    }
                }
            }

            return true;
        }

        for (childIndex = 0; childIndex < quantity; childIndex++)
        {
            if (!CreateRewardEntity(player, parent, rewardItem, createAsAttachment, deliveryContext, depth, createdEntity))
            {
                return false;
            }
        }

        return true;
    }

    private static EntityAI CreateEntityInstance(PlayerBase player, EntityAI parent, string className, bool createAsAttachment)
    {
        if (!player || className == "")
        {
            return null;
        }

        if (parent)
        {
            if (createAsAttachment)
            {
                return EntityAI.Cast(parent.GetInventory().CreateAttachment(className));
            }

            return EntityAI.Cast(parent.GetInventory().CreateInInventory(className));
        }

        return CreateRootEntity(player, className);
    }

    private static EntityAI CreateRootEntity(PlayerBase player, string className)
    {
        vector spawnPosition;
        EntityAI createdEntity;

        if (IsVehicleClassName(className))
        {
            spawnPosition = player.GetPosition() + (player.GetDirection() * VEHICLE_SPAWN_DISTANCE);
            createdEntity = EntityAI.Cast(GetGame().CreateObject(className, spawnPosition, false, true, true));
            if (createdEntity)
            {
                createdEntity.SetOrientation(player.GetOrientation());
            }

            return createdEntity;
        }

        spawnPosition = player.GetPosition() + (player.GetDirection() * GROUND_SPAWN_DISTANCE);
        return EntityAI.Cast(GetGame().CreateObject(className, spawnPosition));
    }

    private static int GetNormalizedQuantity(KrydenRewardsRedeemItem rewardItem)
    {
        if (!rewardItem || rewardItem.quantity < 1)
        {
            return 1;
        }

        return rewardItem.quantity;
    }

    private static bool ApplyStackQuantityIfPossible(EntityAI entity, int quantity)
    {
        ItemBase itemBase = ItemBase.Cast(entity);
        if (!itemBase || quantity <= 1 || !itemBase.CanBeSplit())
        {
            return false;
        }

        itemBase.SetQuantity(quantity);
        return true;
    }

    private static void FinalizeCreatedEntity(EntityAI entity)
    {
        Magazine magazine;
        CarScript vehicle;
        Weapon_Base weapon;

        if (!entity)
        {
            return;
        }

        if (Class.CastTo(magazine, entity))
        {
            magazine.ServerSetAmmoCount(magazine.GetAmmoMax());
        }

        if (Class.CastTo(vehicle, entity))
        {
            vehicle.SetLifetimeMax(VEHICLE_LIFETIME_SECONDS);
            vehicle.SetLifetime(VEHICLE_LIFETIME_SECONDS);
            vehicle.Fill(CarFluid.FUEL, vehicle.GetFluidCapacity(CarFluid.FUEL));
            vehicle.Fill(CarFluid.OIL, vehicle.GetFluidCapacity(CarFluid.OIL));
            vehicle.Fill(CarFluid.BRAKE, vehicle.GetFluidCapacity(CarFluid.BRAKE));
            vehicle.Fill(CarFluid.COOLANT, vehicle.GetFluidCapacity(CarFluid.COOLANT));
        }

        if (Class.CastTo(weapon, entity))
        {
            int muzzleIndex = weapon.GetCurrentMuzzle();
            Magazine attachedMagazine = weapon.GetMagazine(muzzleIndex);
            if (attachedMagazine && attachedMagazine.GetAmmoCount() > 0 && weapon.IsChamberEmpty(muzzleIndex))
            {
                weapon.FillChamber(attachedMagazine.GetType(), WeaponWithAmmoFlags.CHAMBER);
            }
        }
    }

    private static bool IsVehicleClassName(string className)
    {
        if (className == "")
        {
            return false;
        }

        return GetGame().IsKindOf(className, "CarScript") || GetGame().IsKindOf(className, "Boat_Base");
    }
}
