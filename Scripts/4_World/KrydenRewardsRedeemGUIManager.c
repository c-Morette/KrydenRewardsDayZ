class KrydenRewardsRedeemGUIManager
{
    ref KrydenRewardsRedeemMenu m_GUI;

    void OnKeyPress(int key)
    {
        switch (key)
        {
            case KeyCode.KC_ESCAPE:
                CloseGUI();
            break;

            case KeyCode.KC_RETURN:
                SubmitCurrentRedeem();
            break;
        }
    }

    void OpenGUI()
    {
        UIManager uiManager = GetGame().GetUIManager();
        if (!uiManager)
        {
            return;
        }

        if (m_GUI && uiManager.GetMenu() == m_GUI)
        {
            return;
        }

        if (uiManager.GetMenu())
        {
            return;
        }

        if (GetGame().IsInventoryOpen())
        {
            return;
        }

        PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
        if (!player || !player.IsAlive() || player.IsUnconscious())
        {
            return;
        }

        m_GUI = new KrydenRewardsRedeemMenu();
        uiManager.ShowScriptedMenu(m_GUI, NULL);
    }

    void CloseGUI()
    {
        if (m_GUI && GetGame().GetUIManager().GetMenu() == m_GUI)
        {
            GetGame().GetUIManager().Back();
        }

        m_GUI = null;
    }

    void SubmitRedeem(string code)
    {
        string redeemCode;
        if (!KrydenRewardsChatCommand.TryGetRedeemCode(code, redeemCode))
        {
            redeemCode = code;
            redeemCode.Trim();
            redeemCode.ToUpper();
        }

        if (redeemCode == "" || redeemCode == "KR-")
        {
            SetStatus("Informe o codigo de resgate.", true);
            return;
        }

        PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
        if (!player)
        {
            SetStatus("Jogador nao encontrado.", true);
            return;
        }

        SetStatus("Consultando recompensa...", false);
        player.RPCSingleParam(KrydenRewardsConstants.RPC_REDEEM_REQUEST, new Param1<string>(redeemCode), true);
    }

    void SubmitCurrentRedeem()
    {
        if (m_GUI && GetGame().GetUIManager().GetMenu() == m_GUI)
        {
            m_GUI.Submit();
        }
    }

    void SetStatus(string message, bool canSubmit = true)
    {
        if (m_GUI)
        {
            m_GUI.SetStatus(message);
            m_GUI.SetSubmitEnabled(canSubmit);
        }
    }
}

ref KrydenRewardsRedeemGUIManager g_KrydenRewardsRedeemGUIManager;
