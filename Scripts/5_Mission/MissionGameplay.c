modded class MissionGameplay
{
    override void OnInit()
    {
        super.OnInit();

        if (!g_KrydenRewardsRedeemGUIManager)
        {
            g_KrydenRewardsRedeemGUIManager = new KrydenRewardsRedeemGUIManager();
        }
    }

    override void OnUpdate(float timeslice)
    {
        super.OnUpdate(timeslice);

        if (!GetGame().IsClient())
        {
            return;
        }

        UAInput redeemInput = GetUApi().GetInputByName("UAKrydenRewardsRedeem");
        if (redeemInput && redeemInput.LocalPress() && g_KrydenRewardsRedeemGUIManager)
        {
            g_KrydenRewardsRedeemGUIManager.OpenGUI();
        }
    }

    override void OnKeyPress(int key)
    {
        super.OnKeyPress(key);

        if (g_KrydenRewardsRedeemGUIManager)
        {
            g_KrydenRewardsRedeemGUIManager.OnKeyPress(key);
        }
    }
}
