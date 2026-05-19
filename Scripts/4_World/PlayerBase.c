modded class PlayerBase
{
    override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
    {
        super.OnRPC(sender, rpc_type, ctx);

        if (rpc_type == KrydenRewardsConstants.RPC_REDEEM_STATUS && GetGame().IsClient())
        {
            Param2<string, bool> statusData;
            if (ctx.Read(statusData) && g_KrydenRewardsRedeemGUIManager)
            {
                g_KrydenRewardsRedeemGUIManager.SetStatus(statusData.param1, statusData.param2);
            }
            return;
        }

        if (rpc_type != KrydenRewardsConstants.RPC_REDEEM_REQUEST)
        {
            return;
        }
        if (!GetGame().IsServer())
        {
            return;
        }
        if (!sender || !GetIdentity())
        {
            return;
        }
        if (sender.GetPlainId() != GetIdentity().GetPlainId())
        {
            Print("[KrydenRewards] Ignored redeem RPC because sender identity does not match target player.");
            return;
        }
        Param1<string> data;
        if (!ctx.Read(data))
        {
            KrydenRewardsRedeemStatus.Notify(this, "Comando de resgate invalido.", true);
            return;
        }
        KrydenRewardsRedeemService.ProcessRedeem(this, data.param1);
    }
}
