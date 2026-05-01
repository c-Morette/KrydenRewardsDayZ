class KrydenRewardsRedeemItem
{
    string className;
    int quantity;
}
class KrydenRewardsRedeemResponse
{
    string orderId;
    string requestId;
    string redeemCode;
    string steamId;
    string discordUserId;
    string status;
    ref array<ref KrydenRewardsRedeemItem> items;
    void KrydenRewardsRedeemResponse()
    {
        items = new array<ref KrydenRewardsRedeemItem>();
    }
}
class KrydenRewardsErrorResponse
{
    string error;
    string message;
}
