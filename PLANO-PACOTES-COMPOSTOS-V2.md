# Kryden Rewards DayZ Mod - Plano De Pacotes Compostos V2

Este documento registra a evolucao planejada para uma versao futura do mod Kryden Rewards.

A primeira versao pode ser liberada com o fluxo atual, baseado em `className + quantity`. A V2 descrita aqui tem como objetivo permitir recompensas mais completas, como armas com attachments, mochilas com itens dentro, coletes montados e veiculos prontos para uso.

## 1. Objetivo

Permitir que um pacote vendido pelo Kryden Rewards entregue objetos compostos no DayZ.

Exemplos desejados:

- Mochila no chao com itens dentro.
- Arma no chao com carregador, mira, supressor e municao.
- Colete no chao com pouches e holster anexados.
- Veiculo nascendo 3 metros a frente do jogador com pecas e fluidos completos.

## 2. Estado Atual

Na versao atual, a API retorna uma lista simples de itens:

```json
{
  "items": [
    {
      "className": "BandageDressing",
      "quantity": 4
    }
  ]
}
```

O mod cria cada item usando o `className` e a `quantity`.

Esse modelo funciona bem para itens simples, mas nao representa kits reais de DayZ.

## 3. Contrato Desejado Para A V2

A V2 deve permitir uma estrutura de item raiz com attachments e cargo.

Exemplo completo:

```json
{
  "items": [
    {
      "className": "MountainBag_Blue",
      "quantity": 1,
      "spawnMode": "GroundNearPlayer",
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
    },
    {
      "className": "AKM",
      "quantity": 1,
      "spawnMode": "GroundNearPlayer",
      "attachments": [
        {
          "className": "Mag_AKM_30Rnd",
          "quantity": 1,
          "ammoQuantity": 30
        },
        {
          "className": "PSO1Optic",
          "quantity": 1
        },
        {
          "className": "AK_Suppressor",
          "quantity": 1
        }
      ],
      "chamberAmmo": true
    },
    {
      "className": "PlateCarrierVest",
      "quantity": 1,
      "spawnMode": "GroundNearPlayer",
      "attachments": [
        {
          "className": "PlateCarrierPouches",
          "quantity": 1
        },
        {
          "className": "PlateCarrierHolster",
          "quantity": 1
        }
      ]
    },
    {
      "className": "OffroadHatchback",
      "quantity": 1,
      "spawnMode": "VehicleInFrontOfPlayer",
      "distanceMeters": 3,
      "fillFluids": true,
      "attachments": [
        {
          "className": "HatchbackWheel",
          "quantity": 4
        },
        {
          "className": "CarBattery",
          "quantity": 1
        },
        {
          "className": "SparkPlug",
          "quantity": 1
        },
        {
          "className": "CarRadiator",
          "quantity": 1
        }
      ]
    }
  ]
}
```

## 4. Regras De Spawn

### 4.1 Itens Normais

Itens simples podem continuar usando o fluxo atual.

Regras:

- Criar no chao perto do jogador ou no inventario, conforme configuracao futura.
- Se o item suportar quantidade, aplicar a quantidade no proprio item.
- Se o item nao suportar quantidade, criar multiplas unidades.

### 4.2 Mochilas, Roupas E Containers

Mochilas, roupas, coletes, caixas e outros containers devem poder receber itens no cargo.

Regras:

- Criar o item raiz.
- Criar os itens definidos em `cargo` dentro do inventario do item raiz.
- Se algum item do cargo falhar, o resgate deve falhar antes da confirmacao na API.

### 4.3 Armas

Armas devem poder receber attachments e municao.

Regras:

- Criar a arma no chao perto do jogador.
- Criar attachments usando o inventario da arma.
- Criar carregador com municao definida em `ammoQuantity`, quando informado.
- Testar se e possivel carregar a municao no chamber de forma confiavel.
- Se chamber automatico nao for estavel, entregar arma com carregador cheio e documentar a limitacao.

### 4.4 Coletes E Roupas Com Attachments

Coletes e roupas devem poder receber attachments como pouches e holster.

Regras:

- Criar o item raiz.
- Criar attachments no item raiz.
- Permitir tambem `cargo` quando o item tiver inventario interno.

### 4.5 Veiculos

Veiculos devem ter fluxo proprio.

Regras:

- Nunca criar o veiculo dentro do jogador.
- Criar o veiculo a frente do jogador.
- Distancia padrao: 3 metros.
- Aplicar rotacao coerente com a direcao do jogador.
- Adicionar pecas por attachment.
- Completar fluidos quando `fillFluids = true`.
- Validar combustivel, oleo, coolant e brake.
- Se o local de spawn estiver inseguro, o resgate deve falhar com mensagem clara.

## 5. Confirmacao Na API

O mod so deve chamar a confirmacao do resgate depois que todos os objetos forem criados com sucesso.

Fluxo desejado:

1. Jogador usa `//resgatar CODIGO`.
2. Mod consulta a API e recebe o blueprint da recompensa.
3. Mod tenta criar todos os objetos.
4. Se tudo funcionar, mod confirma o resgate na API.
5. API marca o pedido como `Claimed`.

Se qualquer item falhar, o mod nao deve confirmar o resgate.

## 6. Risco Importante: Entrega Parcial

DayZ nao possui uma transacao real para spawn de objetos.

Se o mod criar alguns itens e falhar em outro, pode existir risco de entrega parcial.

Para reduzir esse risco:

- Validar todos os `className` antes de criar objetos.
- Criar objetos em ordem controlada.
- Guardar referencias dos objetos criados durante a tentativa.
- Se algo falhar, tentar remover os objetos ja criados antes de retornar erro.
- Confirmar na API somente depois do sucesso total.

## 7. Testes Necessarios Antes Da Implementacao Final

Antes de mudar API, banco e DayZ Admin App, validar tecnicamente no mod:

### Teste 1 - Item Simples

Criar:

- `BandageDressing` com quantidade maior que 1.
- Item nao empilhavel com quantidade maior que 1.

Resultado esperado:

- Stack correta quando suportado.
- Multiplos itens quando nao suportado.

### Teste 2 - Mochila Com Cargo

Criar:

- Uma mochila.
- Bandagens e comida dentro dela.

Resultado esperado:

- Mochila nasce no chao.
- Itens aparecem dentro da mochila.

### Teste 3 - Arma Montada

Criar:

- Uma arma.
- Um carregador cheio.
- Uma mira.
- Um supressor.

Resultado esperado:

- Arma nasce no chao.
- Attachments entram corretamente.
- Carregador vem com municao.
- Validar possibilidade de chamber carregado.

### Teste 4 - Colete Montado

Criar:

- `PlateCarrierVest`.
- `PlateCarrierPouches`.
- `PlateCarrierHolster`.

Resultado esperado:

- Colete nasce no chao.
- Attachments aparecem anexados.

### Teste 5 - Veiculo Completo

Criar:

- Um veiculo 3 metros a frente do jogador.
- Rodas, bateria, vela e radiador.
- Fluidos completos.

Resultado esperado:

- Veiculo nasce em local seguro.
- Jogador nao fica preso dentro do veiculo.
- Pecas e fluidos aplicados corretamente.

## 8. Impacto Na API

A API precisara evoluir de `className + quantity` para um contrato de blueprint.

Recomendacao:

- Manter compatibilidade com o modelo antigo.
- Adicionar campos opcionais na resposta de resgate.
- Salvar o blueprint completo no snapshot do pedido.
- Garantir que pedidos antigos continuem funcionando.

## 9. Impacto No DayZ Admin App

O DayZ Admin App deve facilitar a configuracao dos pacotes.

Tipos de blocos recomendados:

- Item simples.
- Arma montada.
- Mochila ou container.
- Colete ou roupa.
- Veiculo.
- Objeto de base.

Campos esperados:

- ClassName principal.
- Quantidade.
- Modo de spawn.
- Attachments.
- Itens dentro.
- Municao do carregador.
- Completar fluidos para veiculo.
- Distancia de spawn para veiculo.

## 10. Escopo Recomendado Para V2

Para nao atrasar a primeira versao, esta melhoria deve ficar para uma etapa posterior.

Escopo minimo recomendado:

1. Suporte a cargo dentro de mochila/container.
2. Suporte a attachments em armas e roupas.
3. Suporte a carregador com municao.
4. Suporte a veiculo na frente do jogador com pecas e fluidos.
5. Manter compatibilidade com pacotes simples existentes.

## 11. Decisao Atual

A primeira versao do Kryden Rewards pode ser liberada com o fluxo atual.

Este plano fica registrado para a evolucao V2, quando o objetivo for transformar o sistema de recompensas em um editor completo de kits DayZ.
