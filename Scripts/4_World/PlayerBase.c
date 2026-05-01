modded class PlayerBase
{
    override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
    {
        super.OnRPC(sender, rpc_type, ctx);
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
            MessageAction("[Kryden] Comando de resgate invalido.");
            return;
        }
        KrydenRewardsRedeemService.ProcessRedeem(this, data.param1);
    }
}
