class KrydenRewardsRedeemService
{
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
        bool delivered = DeliverItems(player, preview.items, config.dropOnGroundIfInventoryFull);
        if (!delivered)
        {
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
    private static bool DeliverItems(PlayerBase player, array<ref KrydenRewardsRedeemItem> items, bool dropOnGroundIfInventoryFull)
    {
        bool allDelivered = true;
        foreach (KrydenRewardsRedeemItem rewardItem : items)
        {
            if (!rewardItem || rewardItem.className == "")
            {
                allDelivered = false;
                continue;
            }
            int quantity = rewardItem.quantity;
            if (quantity < 1)
            {
                quantity = 1;
            }
            EntityAI firstItem = CreateSingleItem(player, rewardItem.className, dropOnGroundIfInventoryFull);
            if (!firstItem)
            {
                allDelivered = false;
                continue;
            }
            ItemBase itemBase = ItemBase.Cast(firstItem);
            if (itemBase && quantity > 1 && itemBase.CanBeSplit())
            {
                itemBase.SetQuantity(quantity);
                continue;
            }
            for (int i = 1; i < quantity; i++)
            {
                EntityAI extraItem = CreateSingleItem(player, rewardItem.className, dropOnGroundIfInventoryFull);
                if (!extraItem)
                {
                    allDelivered = false;
                }
            }
        }
        return allDelivered;
    }
    private static EntityAI CreateSingleItem(PlayerBase player, string className, bool dropOnGroundIfInventoryFull)
    {
        EntityAI createdItem = EntityAI.Cast(player.GetInventory().CreateInInventory(className));
        if (createdItem)
        {
            return createdItem;
        }
        if (!dropOnGroundIfInventoryFull)
        {
            return null;
        }
        vector pos = player.GetPosition();
        createdItem = EntityAI.Cast(GetGame().CreateObject(className, pos));
        return createdItem;
    }
}
