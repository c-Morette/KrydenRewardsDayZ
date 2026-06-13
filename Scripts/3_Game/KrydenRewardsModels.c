class KrydenRewardsRedeemItem
{
    string className;
    int quantity;

    ref array<ref KrydenRewardsRedeemItem> attachments;
    ref array<ref KrydenRewardsRedeemItem> cargo;

    void KrydenRewardsRedeemItem()
    {
        quantity = 1;
        attachments = new array<ref KrydenRewardsRedeemItem>();
        cargo = new array<ref KrydenRewardsRedeemItem>();
    }

    bool HasNestedDefinitions()
    {
        return (attachments && attachments.Count() > 0) || (cargo && cargo.Count() > 0);
    }
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
    bool success;
    string error;
    string message;
}

// Espelha KVMPlayerDataFile (KrydenVirtualMarket) para preservar todos os campos no round-trip do JSON.
// O saldo de coins fica no campo "Coin"; Name/SteamID64/FirstLogin precisam existir aqui para nao serem
// apagados ao regravar o arquivo.
class KrydenRewardsPlayerCoinBalance
{
    string Name;
    string SteamID64;
    string FirstLogin;
    int Coin;
}
