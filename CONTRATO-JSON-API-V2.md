# KrydenRewards - Contrato JSON API V2

## 1. Objetivo

Este documento registra o contrato JSON final validado no mod DayZ para a V2 de recompensas compostas.

O objetivo e servir como referencia direta para a implementacao futura da API, sem misturar detalhes de runtime, PBO ou fluxo de testes locais.

Este contrato ja reflete as decisoes operacionais confirmadas no mod:

- `attachments` nao usam `quantity`
- repeticoes de pecas sao feitas por entradas repetidas no array
- magazine cheio e regra fixa do mod
- chamber automatico e regra fixa do mod
- fluidos de veiculo e regra fixa do mod
- distancia de spawn de veiculo e regra fixa do mod

## 2. Estrutura da resposta

Formato completo da resposta:

```json
{
  "orderId": "KR-20260516183000001",
  "requestId": "KRPIX000000001",
  "redeemCode": "KR-SMCCTR",
  "steamId": "76561198181942294",
  "discordUserId": "271828552314388481",
  "status": "ClaimInProgress",
  "items": []
}
```

## 3. Campos do objeto raiz

`orderId`
- identificador do pedido
- obrigatorio

`requestId`
- identificador operacional do resgate
- obrigatorio

`redeemCode`
- codigo usado no `//resgatar CODIGO`
- obrigatorio

`steamId`
- SteamID64 do dono do resgate
- obrigatorio

`discordUserId`
- identificador Discord do comprador
- obrigatorio

`status`
- status atual do pedido durante o fluxo do mod
- obrigatorio
- valor esperado para preview: `ClaimInProgress`

`items`
- lista de entidades raiz da recompensa
- obrigatorio
- pode conter itens simples, containers, armas, roupas, coletes e veiculos

## 4. Estrutura do item

Cada item do array `items` ou de qualquer `cargo` usa o mesmo contrato.

Formato do item:

```json
{
  "className": "BandageDressing",
  "quantity": 4,
  "attachments": [],
  "cargo": []
}
```

## 5. Campos do item

`className`
- nome da classe DayZ a ser criada
- obrigatorio

`quantity`
- quantidade logica do item
- obrigatorio
- para item empilhavel, o mod tenta aplicar stack
- para item nao empilhavel, o mod cria multiplas unidades
- se vier menor que `1`, o mod trata como `1`

`attachments`
- itens anexados ao item atual
- opcional
- nao possuem `quantity` como campo operacional do contrato
- se a mesma peca precisar aparecer varias vezes, repetir a entrada no array

`cargo`
- itens criados dentro do inventario interno do item atual
- opcional
- itens dentro de `cargo` tambem podem ter seus proprios `attachments` e `cargo`

## 6. Campos que NAO devem sair da API

Os campos abaixo foram removidos do contrato e sao responsabilidade fixa do mod:

- `spawnMode`
- `distanceMeters`
- `fillFluids`
- `ammoQuantity`
- `chamberAmmo`

Motivo:

- sao regras de runtime fixas e nao configuracao de pacote
- expor isso na API aumenta chance de erro operacional
- o backend deve informar somente a estrutura da recompensa

## 7. Regras fixas aplicadas pelo mod

O backend nao precisa emitir isso no JSON, mas precisa saber que o mod executa estas regras automaticamente:

- itens, armas, roupas, coletes e containers nascem no chao perto do jogador
- veiculos nascem a frente do jogador
- veiculos nascem com fluidos completos
- magazines criados como attachment nascem cheios
- o mod tenta chamber automatico em armas com magazine valido
- `KrydenRewardsSeaChest` nao pode ser pego, nao pode ir para inventario, so permite retirada e apaga quando fica vazio

## 8. Regra para attachments repetidos

Nao usar `quantity` em `attachments`.

Quando o mesmo attachment precisar existir varias vezes, repetir a entrada no array.

Exemplo correto para 4 rodas:

```json
"attachments": [
  { "className": "HatchbackWheel" },
  { "className": "HatchbackWheel" },
  { "className": "HatchbackWheel" },
  { "className": "HatchbackWheel" }
]
```

## 9. Exemplo 1 - Item simples

```json
{
  "orderId": "KR-ITEM-001",
  "requestId": "KRLOCALITEM001",
  "redeemCode": "ITEM001",
  "steamId": "76561198181942294",
  "discordUserId": "271828552314388481",
  "status": "ClaimInProgress",
  "items": [
    {
      "className": "BandageDressing",
      "quantity": 4
    }
  ]
}
```

## 10. Exemplo 2 - Caixa com itens dentro

```json
{
  "orderId": "KR-CHEST-001",
  "requestId": "KRLOCALCHEST001",
  "redeemCode": "CHEST001",
  "steamId": "76561198181942294",
  "discordUserId": "271828552314388481",
  "status": "ClaimInProgress",
  "items": [
    {
      "className": "KrydenRewardsSeaChest",
      "quantity": 1,
      "cargo": [
        {
          "className": "BandageDressing",
          "quantity": 4
        },
        {
          "className": "TacticalBaconCan",
          "quantity": 2
        }
      ]
    }
  ]
}
```

## 11. Exemplo 3 - Caixa com arma montada dentro

```json
{
  "orderId": "KR-CHEST-GUN-001",
  "requestId": "KRLOCALCHESTGUN001",
  "redeemCode": "CHESTGUN001",
  "steamId": "76561198181942294",
  "discordUserId": "271828552314388481",
  "status": "ClaimInProgress",
  "items": [
    {
      "className": "KrydenRewardsSeaChest",
      "quantity": 1,
      "cargo": [
        {
          "className": "BandageDressing",
          "quantity": 4
        },
        {
          "className": "AKM",
          "quantity": 1,
          "attachments": [
            {
              "className": "Mag_AKM_30Rnd"
            },
            {
              "className": "PSO1Optic"
            },
            {
              "className": "AK_Suppressor"
            }
          ]
        }
      ]
    }
  ]
}
```

## 12. Exemplo 4 - Colete montado

```json
{
  "orderId": "KR-VEST-001",
  "requestId": "KRLOCALVEST001",
  "redeemCode": "VEST001",
  "steamId": "76561198181942294",
  "discordUserId": "271828552314388481",
  "status": "ClaimInProgress",
  "items": [
    {
      "className": "PlateCarrierVest",
      "quantity": 1,
      "attachments": [
        {
          "className": "PlateCarrierPouches"
        },
        {
          "className": "PlateCarrierHolster"
        }
      ]
    }
  ]
}
```

## 13. Exemplo 5 - Veiculo completo

```json
{
  "orderId": "KR-CAR-001",
  "requestId": "KRLOCALCAR001",
  "redeemCode": "CAR001",
  "steamId": "76561198181942294",
  "discordUserId": "271828552314388481",
  "status": "ClaimInProgress",
  "items": [
    {
      "className": "OffroadHatchback",
      "quantity": 1,
      "attachments": [
        {
          "className": "HatchbackWheel"
        },
        {
          "className": "HatchbackWheel"
        },
        {
          "className": "HatchbackWheel"
        },
        {
          "className": "HatchbackWheel"
        },
        {
          "className": "CarBattery"
        },
        {
          "className": "SparkPlug"
        },
        {
          "className": "CarRadiator"
        }
      ]
    }
  ]
}
```

## 14. Regras de validacao para a API

Antes de devolver a resposta ao mod, a API deve garantir pelo menos:

- `orderId`, `requestId`, `redeemCode`, `steamId`, `discordUserId` e `status` preenchidos
- `items` presente, mesmo que vazio em cenarios de erro controlado
- todo item com `className` preenchido
- todo item com `quantity >= 1`
- `attachments` como array de objetos contendo apenas `className`
- `cargo` como array de itens no mesmo formato recursivo

## 15. Regras de serializacao recomendadas para backend

- sempre serializar `attachments` como `[]` quando nao houver anexos
- sempre serializar `cargo` como `[]` quando nao houver itens internos
- evitar `null` nesses arrays
- preservar exatamente os nomes de campos usados pelo mod

## 16. Resumo final para implementacao da API

O backend deve montar uma resposta com estes principios:

- um array `items` contendo as entidades raiz da recompensa
- cada entidade pode ter `attachments` e `cargo`
- `cargo` e recursivo
- `attachments` nao usam `quantity`
- pecas repetidas sao representadas por repeticao explicita de entradas
- o mod cuida sozinho de magazine cheio, chamber, fluidos e spawn