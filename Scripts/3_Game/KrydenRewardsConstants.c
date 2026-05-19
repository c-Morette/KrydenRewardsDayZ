class KrydenRewardsConstants
{
    static const int RPC_REDEEM_REQUEST = 914001;
    static const int RPC_REDEEM_STATUS = 914002;
    static const string PROFILE_DIR = "$profile:KrydenRewards";
    static const string CONFIG_PATH = "$profile:KrydenRewards/Config.json";
    static const string LOCAL_TEST_RESPONSE_PATH = "$profile:KrydenRewards/TestRedeemResponse.json";
    static const string REDEEM_COMMAND_PREFIX = "//resgatar";
}
class KrydenRewardsChatCommand
{
    static bool TryGetRedeemCode(string text, out string code)
    {
        code = "";
        if (text == "")
        {
            return false;
        }
        text.Trim();
        string lowered = text;
        lowered.ToLower();
        string prefix = KrydenRewardsConstants.REDEEM_COMMAND_PREFIX;
        prefix.ToLower();
        if (lowered.IndexOf(prefix) != 0)
        {
            return false;
        }
        TStringArray parts = new TStringArray();
        text.Split(" ", parts);
        if (parts.Count() < 2)
        {
            return false;
        }
        code = parts.Get(1);
        code.Trim();
        code.ToUpper();
        return code != "";
    }
}
