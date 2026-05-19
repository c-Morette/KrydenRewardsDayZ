# Kryden Rewards DayZ Mod

Kryden Rewards is a DayZ server-side/client-side mod that lets players redeem rewards purchased through the Kryden Rewards platform.

Players redeem an approved order through the Kryden Rewards menu. The default key is `End`.

The legacy chat command is still available as a fallback:

```text
//resgatar CODIGO
```

The mod validates the code with the server API, spawns the configured rewards in game, and confirms the redemption only after the delivery attempt.

## Features

- Redeem rewards through an in-game menu.
- Keeps the legacy chat command as fallback.
- Uses the real SteamID64 from the DayZ player identity.
- Supports simple rewards, containers, weapons, cargo, attachments and vehicles.
- Supports recursive reward payloads from the Kryden Rewards API.
- Supports local JSON test mode for validating item spawning without calling the API.
- Keeps the private server API key outside the PBO, inside the server profile.

## Server Configuration

On first server start, the mod creates:

```text
$profile:KrydenRewards/Config.json
```

Example:

```json
{
  "apiBaseUrl": "https://cliente-a.rewards.kryden.com.br",
  "serverKey": "PASTE_SERVER_KEY_HERE",
  "dropOnGroundIfInventoryFull": true,
  "debugLogs": false,
  "useLocalTestResponse": false,
  "localTestResponsePath": "$profile:KrydenRewards/TestRedeemResponse.json"
}
```

Never put the real `serverKey` inside the public PBO. Keep it only in the server profile.

## Installation

Expected server structure:

```text
@KrydenRewards/
  Addons/
    KrydenRewards.pbo
    KrydenRewards.pbo.bisign
  Keys/
    KrydenRewards.bikey
```

Steps:

1. Copy `@KrydenRewards` to the DayZ server root.
2. Copy `KrydenRewards.bikey` to the server `Keys` folder.
3. Add `@KrydenRewards` to the server mod list.
4. Start the server once.
5. Stop the server.
6. Edit `$profile:KrydenRewards/Config.json`.
7. Set `apiBaseUrl` to the Kryden Rewards instance URL.
8. Set `serverKey` to the instance DayZ server key.
9. Start the server again.
10. Test a paid/approved order by pressing `End` and entering the redeem code.

## Local Test Mode

To test item spawning without the API:

```json
{
  "useLocalTestResponse": true,
  "localTestResponsePath": "$profile:KrydenRewards/TestRedeemResponse.json"
}
```

The mod reads the local JSON payload and skips API preview/confirm calls.

Reference payload:

```text
TestRedeemResponse.example.json
```

## API Flow

Preview:

```text
GET /api/dayz/redeem/{code}?steamId={steamId}&serverKey={serverKey}
```

Confirm successful delivery:

```text
GET /api/dayz/redeem/{code}?steamId={steamId}&serverKey={serverKey}&action=confirm
```

Report failed delivery so the order can be retried:

```text
GET /api/dayz/redeem/{code}?steamId={steamId}&serverKey={serverKey}&action=fail
```

## Reward JSON

Minimal example:

```json
{
  "orderId": "KR-20260501170014743",
  "requestId": "KRPIX000000001",
  "redeemCode": "KR-SMCCTR",
  "steamId": "76561198181942294",
  "discordUserId": "271828552314388481",
  "status": "ClaimInProgress",
  "items": [
    {
      "className": "BandageDressing",
      "quantity": 4,
      "attachments": [],
      "cargo": []
    }
  ]
}
```

Nested example:

```json
{
  "items": [
    {
      "className": "KrydenRewardsSeaChest",
      "quantity": 1,
      "cargo": [
        {
          "className": "AKM",
          "quantity": 1,
          "attachments": [
            { "className": "Mag_AKM_30Rnd" },
            { "className": "PSO1Optic" }
          ]
        }
      ]
    }
  ]
}
```

Attachments do not use `quantity`; repeated attachments should be repeated entries in the `attachments` array.

## Documentation

Additional technical documents are in:

```text
docs/
```

Useful starting points:

- `docs/README-KrydenRewards-DayZMod.md`
- `docs/CONTRATO-JSON-API-V2.md`
- `docs/ESPECIFICACAO-TECNICA-PACOTES-COMPOSTOS-V2.md`

## Source Layout

```text
config.cpp
Config.example.json
Scripts/
  3_Game/
  4_World/
  5_Mission/
TestRedeemResponse.example.json
```

## Security Notes

- Do not commit real server keys.
- Do not ship production `Config.json` inside the PBO.
- Use `debugLogs=false` in production.
- Validate package item class names before selling a package publicly.

## License

License not defined yet.
