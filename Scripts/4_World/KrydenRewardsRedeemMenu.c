class KrydenRewardsRedeemMenu extends UIScriptedMenu
{
    protected EditBoxWidget m_CodeInput;
    protected ButtonWidget m_RedeemButton;
    protected ButtonWidget m_CloseButton;
    protected MultilineTextWidget m_StatusText;

    override Widget Init()
    {
        layoutRoot = GetGame().GetWorkspace().CreateWidgets("KrydenRewards/layouts/kryden_rewards_redeemv2.layout");

        m_CodeInput = EditBoxWidget.Cast(layoutRoot.FindAnyWidget("kryden_redeem_code_input"));
        m_RedeemButton = ButtonWidget.Cast(layoutRoot.FindAnyWidget("kryden_redeem_button"));
        m_CloseButton = ButtonWidget.Cast(layoutRoot.FindAnyWidget("kryden_close_button"));
        m_StatusText = MultilineTextWidget.Cast(layoutRoot.FindAnyWidget("kryden_status_text"));

        SetStatus("");
        SetSubmitEnabled(true);
        return layoutRoot;
    }

    override void OnShow()
    {
        super.OnShow();

        GetGame().GetMission().PlayerControlDisable(INPUT_EXCLUDE_ALL);
        GetGame().GetUIManager().ShowUICursor(true);
        GetGame().GetMission().GetHud().Show(false);

        if (m_CodeInput)
        {
            SetFocus(m_CodeInput);
        }
    }

    override void OnHide()
    {
        super.OnHide();

        GetGame().GetMission().PlayerControlEnable(true);
        GetGame().GetUIManager().ShowUICursor(false);
        GetGame().GetMission().GetHud().Show(true);
    }

    override bool OnClick(Widget w, int x, int y, int button)
    {
        super.OnClick(w, x, y, button);

        if (w == m_CloseButton)
        {
            if (g_KrydenRewardsRedeemGUIManager)
            {
                g_KrydenRewardsRedeemGUIManager.CloseGUI();
            }
            return true;
        }

        if (w == m_RedeemButton)
        {
            Submit();
            return true;
        }

        return false;
    }

    override void Update(float timeslice)
    {
        super.Update(timeslice);

        if (!g_Game.IsAppActive() && g_KrydenRewardsRedeemGUIManager)
        {
            g_KrydenRewardsRedeemGUIManager.CloseGUI();
        }
    }

    void SetStatus(string message)
    {
        if (m_StatusText)
        {
            m_StatusText.SetText(message);
        }
    }

    void Submit()
    {
        string code = "";
        if (m_CodeInput)
        {
            code = m_CodeInput.GetText();
        }

        if (g_KrydenRewardsRedeemGUIManager)
        {
            g_KrydenRewardsRedeemGUIManager.SubmitRedeem(code);
        }
    }

    void SetSubmitEnabled(bool enabled)
    {
        if (m_RedeemButton)
        {
            m_RedeemButton.Enable(enabled);
        }
    }
}
