modded class ChatInputMenu
{
    override bool OnChange(Widget w, int x, int y, bool finished)
    {
        if (finished)
        {
            string text = m_edit_box.GetText();
            string redeemCode;
            if (KrydenRewardsChatCommand.TryGetRedeemCode(text, redeemCode))
            {
                PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
                if (player)
                {
                    player.RPCSingleParam(KrydenRewardsConstants.RPC_REDEEM_REQUEST, new Param1<string>(redeemCode), true);
                }
                m_close_timer.Run(0.1, this, "Close");
                GetUApi().GetInputByID(UAPersonView).Supress();
                return true;
            }
        }
        return super.OnChange(w, x, y, finished);
    }
}
