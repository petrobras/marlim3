# Documentação do conversor mr2 -> mr3 (mrt)

## Conceitos básicos

O arquivo MR2 é um XML que possui vários elementos de **ENTRADA**.

### Estrutura do XML de entrada
```xml
<!-- Dados de entrada -->
<!ELEMENT entrada (
  identificacao,
  opcoes_calculo,
  analise_sensibilidade?,
  curva_producao?,
  simulador?,
  simulador_reo?,
  lgpi?,
  fluidos,
  sistema_producao,
  gas_lift,
  conjuntos_bcs,
  equipamentos_especiais?,
  dados_medicoes,
  envelope_hidratos,
  range_validacao?
)>
```
E uma **saída** que é gravada no final desse mesmo arquivo de entrada.
A saída é escrita no mesmo arquivo de entrada (`.MR2`) e não é um elemento no XML, e sim um bloco no formato TXT com alguns identificadores iniciados pelo caracter `#`:
```txt
<!-- ##INICIO_SAIDA##
#RESULT=TODOS
#RGLIT= 0.00000000000000E+0000
#QLSC= 3.06457446289062E+0003
#QGSC=0.0
#QOSC= 3.06457446289062E+0003
#QGIT= 0.00000000000000E+0000
#QGT= 2.65698593750000E+0005
#PWF= 2.69354248046875E+0002
#PTUB= 1.50000000000000E+0001
...
##FIM_SAIDA## -->
```

### Como identificar se o arquivo é um modelo do Marlim Permanente
1. Possuir a linha:

   ```xml
   <!DOCTYPE marlim SYSTEM
   ```

   Não vamos verificar qual a versão do arquivo DTD, logo essa linha será verificada ignorando o final: `marlim-v17.dtd">`.
2. Possuir o elemento:

   ```txt
   #RESULT=TODOS
   ```

   Nesse elemento é que podemos identificar se existe resultado e se os mesmos são válidos.
O Marlim Permanente possui vários tipos de execução e, dependendo do tipo, há uma saída diferente. Vamos apenas importar os arquivos MR2 da rodada considerada **SIMPLES**.


### Como verificar se estamos com um MR2 com resultado da rodada SIMPLES
- Deve possuir o `<!-- ##INICIO_SAIDA##`  
- Deve possuir o `#RESULT=TODOS`  
- **Não** pode possuir linha iniciando com `@PASSO` (indica que é um resultado da análise de sensibilidade)  
- **Não** deve possuir linha iniciando com `@ERRO` ou `#ERRO`  
Exemplo de saída que **não** pode ser importada: (imagens/trechos não textuais)
Exemplo de saída que **pode** ser usada: (imagens/trechos não textuais)


### Informações que podem ser importadas do MR2 para o MRT
Após verificar se o MR2 é de uma rodada simples e possui resultados, estamos prontos para importar os dados do MR2 para o MRT. Será permitido ao usuário importar as seguintes informações:
- Dados da formação do poço  
- Dados do fluido de produção  
- Dados do fluido de gás  
- Sistema de produção  
- Sistema de injeção  
- Válvula de gas lift  
- BCS  
- Posição da Master1  
- Posição da Master2  
- Condição de contorno da linha de Produção: fontes e `separador`  
- Condição de contorno da linha de serviço: `gasInj`

### Elementos obrigatórios
A informação mínima que temos para importar será a **geometria do sistema de produção**. Caso não tenhamos os seguintes elementos nos dados de saída do MR2 (Marlim Permanente), **não** iremos iniciar a importação e uma mensagem de erro será apresentada ao usuário:
Elementos de saída obrigatórios:
- `#NUN`  
- `#SAIDAXORIG`  
- `#NAT`  
- `#NTREC`  
- `#UL`  
- `#ANG`
Mensagem que deverá ser apresentada no caso de erro:
> O MR2 não possui informações da linha de produção.

---
## Correspondência de elementos entre Marlim Permanente e Marlim Transiente

### Tabela de mapeamento (conceitual)
**Marlim Permanente → Marlim Transiente**
- Reservatório → IPR  
  - Se fonte ativa for do tipo `Reservatório`  
  - Obrigatório: **Não**
- Reservatório → `fonteLiquido`  
  - Se fonte ativa for do tipo `FONTE_MASSA`  
  - Obrigatório: **Não**
- Fluido → `fluidosProducao`  
  - Fluido associado à Fonte Ativa  
  - Obrigatório: **Sim**
- Fluido da linha de injeção → `fluidoGas`  
  - Se `<gas_lift><sgig> <> 0`  
  - Obrigatório: **Não**
- SistemaProdução → `dutosProducao`  
  - Cada unidade será um duto na linha de produção  
  - Obrigatório: **Sim**
- SistemaInjeção → `dutoServiço`  
  - Cada unidade será um duto na linha de serviço  
  - Obrigatório: **Não**
- Posição da árvore de Natal → `master1`  
  - Possui sistema de produção  
  - Obrigatório: **Sim**
- Posição da árvore de Natal → `master2`  
  - Possui sistema de injeção (linha de serviço)  
  - Obrigatório: **Não**
- Pressão na plataforma → `separador`  
  - Ponto mais a jusante do sistema de produção  
  - Obrigatório: **Sim**
- `chokeSup`  
  - Usaremos um valor padrão na importação  
  - Obrigatório: **Sim**


### Elementos que não possuem correspondência
Não farão parte do arquivo resultante da importação:
- `fluidoComplementar`  
- `bombaVolumetrica`  
- `deltaPressao`  
- `pig`  
- `tendS`  
- `tendTransP`  
- `tendTransS`  
- `tela`  

### Conversão de pressões
- No Marlim Permanente, as pressões são **manométricas**
- No Marlim Transiente, as pressões são **absolutas**
Regra:
> Pressão absoluta = Pressão manométrica + 1,03323

### Conversões de comprimento – Marlim Permanente vs Transiente

Referências:
- Permanente: referência plataforma  
- Transiente: referência reservatório  

Conversões:
1. **Comprimento Permanente → Transiente**:

   > Comprimento_Transiente = Comprimento_Total_Sistema - Comprimento_Permanente
2. **Profundidade Permanente → Comprimento Permanente**:

   > Comprimento_Permanente = (Profundidade_Medida - Distância_Mesa_Rotativa) + Comprimento_Medido_Master1

Para achar o valor do comprimento medido de um acessório:

- (1) Verificar se foi utilizado Comprimento Medido ou Profundidade Medida no Marlim Permanente.
- (2) Se profundidade medida, então, converter para comprimento medido
- (3) Converter Comprimento Medido do Permanente para Comprimento Medido do Transiente

---

## ConfiguraçãoInicial – `linhaGas` e demais parâmetros

### `sentidoGeometriaSegueEscoamento`
O conversor MR2→MR3 gera o JSON com:
```json
"sentidoGeometriaSegueEscoamento": false
```
Isso significa que **o primeiro duto no JSON (`id=0`) está na plataforma** e o último está no reservatório. O simulador interpreta essa flag e reverte a ordem internamente para que a simulação proceda do reservatório para a plataforma.

Em resumo:
- `false` → geometria ordenada **plataforma → reservatório** (mesma ordem do MR2)
- `true` → geometria ordenada **reservatório → plataforma** (sentido do escoamento)

### `linhaGas`
O único outro elemento da ConfiguraçãoInicial com valor variável na importação é `linhaGas`.
Regra:
- Se tiver válvula de gas lift ativa **por desempenho** (operação = `DESEMPENHO`) e com orifício **não cego** (`orificio ≠ CEGA`), então `linhaGas = true`. Senão, `linhaGas = false`.
- Algoritmo:
    1. Percorrer o vetor `#VV` da saída. Para cada posição com valor ≠ 0, o valor representa `(ID_valvula + 1)` da válvula associada àquele duto.
    2. Para cada válvula identificada, verificar em `<entrada><gas_lift><valvula_instalada[@seq=ID]>`:
        - Se `@operacao = 'DESEMPENHO'` **E** `<vgl_inst><orificio> ≠ 'CEGA'` → válvula ativa para linhaGas.
    3. Se pelo menos uma válvula satisfizer as condições acima → `linhaGas = true`.
    4. Caso contrário → `linhaGas = false`.

---
## `fluidosProducao`

No Marlim Permanente podemos ter vários fluidos cadastrados, porém neste primeiro momento será buscado **somente o fluido vinculado à fonte ativa**. Logo, teremos apenas **um fluido** no arquivo importado.

### Buscando a fonte ativa
No Marlim Permanente podemos ter várias fontes cadastradas, porém somente uma fonte pode ser **ATIVA**.
- Lista de fontes: `<entrada><sistema_producao><produção><fonte>`
- Fonte ativa: `<ativo> = TRUE`

Com a fonte ativa, buscamos o fluido vinculado a ela via atributo `id_fluido`.
> Atenção: as informações das fontes serão usadas também para o elemento IPR.

### `fluidosProducao` no MRT

```json
"fluidosProducao": [{
  "id",
  "rotulo",
  "ativo",
  "api",
  "rgo",
  "rgl",
  "densidadeGas",
  "bsw",
  "densidadeAgua",
  "temp1",
  "visc1",
  "temp2",
  "visc2",
  "tipoEmul",
  "bswCorte",
  "fracCO2",
  "correlacaoCritica",
  "modeloRsPb",
  "coefAModeloExp",
  "coefBModeloExp",
  "modeloOleoMorto",
  "modeloOleoVivo",
  "modeloOleoSubSaturado",
  "modeloViscBlackOil",
  "modeloAguaBlackOil",
  "fracMolar",
  "arquivoPVT"
}]
```
Com as regras:
- `id` = `0` (integer)  
- `rotulo` = `"fluido_MR2"`  
- `ativo` = `true`  
Demais campos lidos conforme descrito no texto:
- `api` ← `<entrada><fluido><api>`  
- `rgo` ← `<entrada><sistema_producao><produção><fonte><razão_fluido><rgof>`  
- `rgl` ← `<entrada><sistema_producao><produção><fonte><razão_fluido><rglf>`. Se ausente, calcular como `rgo * (1 - bsw)`  
- `densidadeGas` ← `<entrada><fluido><sgfog>`  
- `bsw` ← `<entrada><sistema_producao><produção><fonte><razão_fluido><bsw> / 100`  
- `densidadeAgua` ← `<entrada><fluido><sgw>`  
- `fracaoCO2` ← `<entrada><fluido><yco2>`  
- `correlacaoCritica` = `1` (Brown et al.)  
- `modeloViscBlackOil` = `0` (usar viscosidades da tabela, se aplicável)  
- `modeloAguaBlackOil` = `1` (usar correlações black oil para água)  
- `fracMolar` = `[0.0]` (placeholder para simulações composicionais)

### Propriedades PVT e emulsão
As propriedades PVT podem ser informadas:
- Por correlações convencionais  
- Por arquivo de análise PVT
Verificação:
- `<entrada><fluido><pvt>.tipo = "CORRELACOES"` → buscar em `<correlacoes>`  
- `<entrada><fluido><pvt>.tipo = "ANALISE"` → buscar em `<analise_pvt>`
Mapeamentos:
- `modeloRsPb` (VAZQUEZ_BEGGS, LASATER, STANDING, GLASO → 0 a 3, respectivamente)  
- `modeloOleoMorto` (ASTM, BEGGS_ROBINSON, BEGGS_ROBINSON_MODIFICADO, GLASO, KARTOATMODJO_SCHMIDT, ETROSKY_FARSHAD, BEAL → 0 a 6, respectivamente)  
- `modeloOleoVivo` (BEGGS_ROBINSON, KARTOATMODJO_SCHMIDT, PETROSKY_FARSHAD, CAMPOS) → 0, 1, 2, 0
- `modeloOleoSubSaturado` (VAZQUEZ_BEGGS, KARTOATMODJO_SCHMIDT, PETROSKY_FARSHAD, BEAL, KHAN → 0 a 4, respectivamente)  
- temp1, visc1, temp2, visc2
    - Temos que verificar se a correlação ASTM está sendo usado para o cálculo da viscosidade do óleo morto (modeloOleoMorto). 
    - `<entrada><fluido><pvt>
<correlações><viscosidade><lCode><tipo> = “ASTM”` então,
        - `temp1 = <t1>`
        - `visc1 = <visd1>`
        - `temp2 = <t2>`
        - `visc2 = <visd2>`
    - Caso contrário, atribuir o valor zero.

- `tipoEmul`
    1. Se `<entrada><fluido><iemul> = "EMULSAO"` ou `TRUE` → atribuir `tipoEmul` de acordo com a relação a seguir:
        - `"WOELFLIN_FRACA"` → `tipoEmul = 1`  
        - `"WOELFLIN_MEDIA"` → `tipoEmul = 2`  
        - `"WOELFLIN_FORTE"` → `tipoEmul = 3`  
        - `"EXPONENCIAL"` → `tipoEmul = 4`  
        - `"OLEO_VIVO"` ou `"CAMPOS"` ou `"RONNINGSEN"` → `tipoEmul = 2` 
        2. Caso contrário → `tipoEmul = 0` 
- `bswCorte`:
    - Se  `tipoEmul` for 1,2,3 ou 4: `<entrada><fluido><emulsao><fwre>`. Se o campo `fwre` estiver ausente, **não incluir** `bswCorte` no JSON.
    - Se `tipoEmul` for 0: **não incluir** `bswCorte` no JSON.
- `coefAModeloExp`, `coefBModeloExp`:
    - `coefAModeloExp`  ← `<entrada><fluido><emulsao><coef_a>`
    - `coefBModeloExp`  ← `<entrada><fluido><emulsao><coef_b>`
 
---
## `fluidoGas` – fluido na linha de serviço
É o fluido na linha de serviço. Se no MR2 não tiver linha de serviço, não importamos `fluidoGas`.

- `densidadeGas`:
    - Buscar primeiro em `<gas_lift><sgig>`.
    - Se não encontrado (elemento ausente ou texto vazio), buscar em `<fluidos><fluido><sgig>` (gravidade específica do gás de injeção armazenada no fluido de produção).
    - Se nenhum dos dois tiver valor válido > 0, `densidadeGas` ← `0`.
- `fracCO2` = `0`  
- `correlacaoCritica` = `1`  
- `ativo` = `true`

---
## Fonte de líquido e IPR
No Marlim Permanente temos dois tipos de fonte:
- `Reservatório`  
- `Fonte de Massa`
E cada fonte pode ser **ATIVA** ou **INATIVA**.
Regra:
- Fonte ATIVA e `tipo = "RESERVATORIO"` → cria `IPR` no Transiente  
- Fonte ATIVA e `tipo = "FONTE_MASSA"` → cria `fonteLiquido`  

### `IPR`
Exemplo de template no MRT:
```json
"ipr": [
  {
    "id": 0,
    "comprimentoMedido": 0.0,
    "tipoIPR": 2,
    "pressaoEstatica": ...,
    "tempoPressaoEstatica": [0.0],
    "temperaturas": ...,
    "tempoTemperaturas": [0.0],
    "ip": ...,
    "tempoip": [0.0],
    "ii": ...,
    "tempoii": [0.0],
    "indFluidoPro": 0,
    "ativo": true
  }
]
```
Regras principais:
- `comprimentoMedido` = soma dos valores do vetor `#UL` da saída
- `tipoIPR`:
  - Se `<entrada><fonte><modelo ipr>` = `"LINEAR"` → `tipoIPR = 0`
  - Se `<entrada><fonte><modelo ipr>` = `"VOGEL"` → `tipoIPR = 1` (Vogel combinada – usa `ip` e pressão estática para montar curva linear acima da pressão de bolha e Vogel abaixo)
- `pressaoEstatica` = `<entrada><sistema_producao><produção><fonte><pr> + 1,03323`
- `temperaturas` = `<entrada><sistema_producao><produção><fonte><tmonf>`  
- `ip` = `<entrada><sistema_producao><produção><fonte><dados_ipr><ip>`  
- `ii` = mesmo valor de `ip` (não há índice de injetividade no MR2)  
- `indFluidoPro` = índice do fluido importado do MR2 = 0

### `fonteLiquido`
Só será criado se a fonte ATIVA for do tipo `"FONTE_MASSA"`.
Estrutura:
```json
"fonteLiquido": [{
  "comprimentoMedido": 0.0,
  "temperatura": [123.8],
  "beta": [0.0],
  "vazaoLiquido": [1711.0],
  "tempo": [0.0],
  "indiFluidoPro": 0,
  "ativo": true
}]
```
Regras:
- `comprimentoMedido` = soma dos valores do vetor `#UL` da saída
- `temperatura` = `<fonte><tmonf>`  
- `vazaoLiquido`:
  - Se `<fonte>.tipo_fluido = "GAS"` → `<opções_calculo><qgsc>`  
  - Senão → `<opções_calculo><qlsc>`  


---
## Geometria

### `dutosProducao` – geometria e condições iniciais e ambiente

#### Conceitos de unidades no Marlim Permanente
No Marlim Permanente os dutos são chamados de **unidades**, com tipos:
- Linha Marítima (`2`)  
- Linha Enterrada (`4`) – **NÃO SUPORTADA**: se existir qualquer unidade com `#NAT = 4`, a conversão deve ser abortada com a mensagem: `"Modelo de linha enterrada ainda não disponível no Marlim3"`  
- Linha Aérea (`3`)  
- Riser (`1`)  
- Poço com coluna (`5`)  
- Poço sem Coluna (`7`)  
- Choke (`6`)  
- Reservado / BCS / Equipamentos especiais (`9` e `10`) – não viram duto no MRT

Cada unidade é dividida em **trechos**; o número de nós = número de trechos + 1.
#### Referência de numeração
- Em Marlim Permanente, unidades numeradas de jusante para montante (plataforma → reservatório), `SEQ` de `0` até `n`.  
- No JSON gerado pelo conversor (`sentidoGeometriaSegueEscoamento = false`), os dutos mantêm a **mesma ordem do MR2**: plataforma → reservatório. O duto `id=0` é o mais próximo da plataforma e o último é o mais próximo do reservatório.

Unidades com `#NAT = 9` ou `10` (BCS / equipamentos especiais) devem ser **ignoradas** na migração.

#### Leitura de vetores de saída (exemplo)

Não podemos buscar os dutos diretamente dos dados de entrada, pois após a execução da simulação novos dutos podem ter sido criados resultante da divisão de um duto existente, mantendo sempre o comprimento medido total da geometria.

```txt
#NUN=9
#SAIDAXORIG=0;1;2;3;4;4;4;5;5;
#NAT=2;2;2;5;5;5;5;5;5;
#NTREC=3;2;1;2;2;1;1;1;2;
#NANU=0;0;0;1;1;1;1;1;1;
#CORR=4;8;8;4;4;4;4;4;4;
```
Regras:
- `#NUN` = número de unidades/dutos  
- Demais vetores (`#ANG`, `#UL`, `#T`, `#P`, etc.) têm o mesmo número de elementos  
- Para cada duto, a condição nas extremidades é lida do primeiro e último valor dos vetores correspondentes, seguindo a ordem de plataforma para fundo do poço  
Atenção: **não considerar** dutos com `#NAT = 9` ou `10`.

#### Campos importantes para formação de dutos
- `#NUN` – número de unidades (dutos)
- `#SAIDAXORIG` – identificador da unidade em relação aos dados de entrada
    - Vamos utilizar esse identificador para buscar outras informações na entrada de dados do MR2, como por exemplo, o nome que identifica a unidade que será o nome do DUTO no Transiente.
- `#NAT` – natureza da unidade (tipo de unidade)  
    - Códigos de `#NAT`:
    - `1` – Riser  
    - `2` – Linha Marítima  
    - `3` – Linha Aérea  
    - `4` – Linha Enterrada  
    - `5` – Poço com coluna  
    - `6` – Choke  
    - `7` – Poço sem coluna  
    - `8` – Choque (não é duto no Transiente)  
    - `9` – Delta de bomba (não é duto)  
    - `10` – Delta de equipamentos especiais (não é duto)  

No MRT:
- `"id"` – posição da unidade (começa em 0)  
- `"idCorte"` – id da seção transversal, gerado conforme as seções transversais são geRADAS                                  |
- `"angulo"` – vetor `#ANG` convertido de graus para radianos:  

  ```txt
  ANGULO_RADIANOS = ANGULO_GRAUS * 0.017453292519
  ```
- `"rotulo"` – nomenclatura de dutos (tipo de unidade refere-se ao código obtido de #NAT):

  - Produção:  
    `Prod_<tipo_unidade_permanente>_<numero_sequencial_qtde_dutos>`  
  - Serviço:  
    `Serv_<tipo_unidade_permanente>_<numero_sequencial_qtde_duto>`
- `"direcaoConveccao"` = `0`  
- `"acoplamentoTermico"` = `0` (sem acoplamento por padrão). Será alterado para `1` apenas quando existir um duto de serviço acoplado correspondente (ver Etapa 2 da linha de serviço). Quando `acoplamentoTermico = 1`, também setar `direcaoConveccao = 1` (LONGITUDINAL).  
- `"idformacao"` = id da formação no poço (instruções para obtenção dessa informação mais à frente) ou `-1` para unidades que não sejam do tipo poço  

- `"ambienteExterno"`:
    - Significado no Marlim Transiente:
        - `-1` – sem ambiente externo  
        - `0` – usuário informa características externas  
        - `1` – duto de linha marítima  
        - `2` – duto de linha aérea  
    - Regras:
        - Linha marítima ou riser → `ambienteExterno = 1`  
        - Linha aérea → `ambienteExterno = 2`  
        - Poço → **não incluir** `ambienteExterno` no JSON (não faz sentido para poço)  
        - Se `ambienteExterno = 1` → ler temperatura e velocidade do mar.  
        - Se `ambienteExterno = 2` → ler temperatura e velocidade do ar.  
- `"discretizacao"`
    - `nCelulas` = número de trechos (`#NTREC`), mas mínimo 3  
    - `comprimento` = `#UL / nCelulas` 
- `"condicoesIniciais"` 
    - `temp` – lida de `#T`
        - Existem vários vetores `#T`, um por unidade, representando a temperatura em cada nó (nós = trechos + 1). O primeiro vetor `#T` corresponde à unidade mais a jusante (plataforma, `ind=0`) e assim por diante até o reservatório.
        - Cada vetor `#T[ind]` tem os nós ordenados de jusante (plataforma) para montante (reservatório) **dentro daquela unidade**.
        - Para cada duto (na ordem plataforma → reservatório):
            - `temp[0]` = primeiro valor de `#T[ind]` (nó mais a jusante da unidade)
            - `temp[1]` = primeiro valor de `#T[ind+1]` (nó de junção com a unidade seguinte); se não houver unidade seguinte, usa o último valor de `#T[ind]`
        - A mesma lógica aplica-se a `pressao`, `holdup`, `uls`, `ugs`, `tempExterna`.
    - `pressao` – lida de `#P` e convertida para absoluta (+1,03323) 
     
    - `holdup` – lida de `#HOLD` 
    - `bet` –  [0,0]
    - `uls` – lida de `#VELL`  
    - `ugs` – lida de `#VELG`  
    - `tempExterna` – `#TA`  
    - `velExterna` – dos dados de entrada, dependendo do tipo de unidade:
        - Se linha marítima, então: `<entrada><sistema_producao><produção>
<linha_maritima><vmar>`
        - Se linha área, então:
`<entrada><sistema_producao><produção>
<linha_maritima><vento>`
        - Demais unidades, zero.
- `kExterna`: 0.32
- `calorEspecificoExterno`:	4182
- `rhoExterno`: 1000
- `viscExterna`: 0.001

- `"correlacaoMR2"` – índice da correlação multifásica black-box usada pela unidade no MR2. Obtido de `<entrada><sistema_producao><produção><{tipo}[@seq=SAIDAXORIG]><corr @tipo>`. Mapeamento:
    - `POETTMANN_CARPENTER` → `0`
    - `BAXENDELL_THOMAS` → `1`
    - `FANCHER_BROWN` → `2`
    - `HAGEDORN_BROWN` → `3`
    - `DUNS_ROS` → `4`
    - `ORKISZEWSKI` → `5`
    - `BEGGS_BRILL` → `6`
    - `MUKHERJEE_BRILL` → `7`
    - `AZIZ` → `8`
    - `GRAY` → `9`
    - `OLIEMANS` → `10`
    - `DUKLER` → `11`
    - `BEGGS_BRILL_PALMER` → `12`
    - `DUKLER_EATON_FLANIGAN` → `13`
    - `DUKLER_MINAMI_I` → `14`
    - `DUKLER_MINAMI_II` → `15`
    - `SHOHAM` → `6` (placeholder, não implementada ainda)
    
  Se a correlação não for encontrada ou o campo não existir no XML de entrada (comum em unidades de poço), usa-se o valor padrão `6` (BEGGS_BRILL).

#### Fatores de correção (`correcao`) – NÃO IMPORTADOS

O MR2 possui fatores de correção por unidade de produção, definidos em `<entrada><sistema_producao><producao><fatores_correcao>` com campos `<fcorrp>` (correção de pressão) e `<fcorrt>` (correção de temperatura). Estes fatores eram usados pelo Marlim Permanente para ajustar os gradientes calculados pelas correlações black-box.

**Decisão de projeto:** os fatores de correção **não são importados** para o MR3. Motivos:
1. Os fatores foram calibrados para correlações empíricas específicas do MR2 (Orkiszewski, Beggs & Brill, etc.) e não são transferíveis para o modelo mecanístico drift-flux do MR3.
2. Valores sentinela (`-999999`) e extremos encontrados em campo causam divergência no solver permanente do MR3.
3. O MR3 já possui seu próprio modelo de escorregamento (Bhagwat & Ghajar) que não requer calibração por fatores multiplicativos.

Consequentemente:
- Os campos `dPdLFric`, `dPdLHidro` e `dTdL` **não são escritos** nos dutos de produção/serviço.
- O objeto `correcao` **não é incluído** no JSON de saída.
- A função `parse_correcao` existe no código mas **não é invocada** pelo conversor.

#### Formação (#FORMACAO)
O elemento `FORMACAO` na configuração inicial do MRT representa a condição de contorno para transferência de calor dentro do poço.

Só existe se há unidades do tipo poço (com ou sem coluna) no MR2.
Formações default (exemplo):
```json
{"id": ...,"condutividade": 4.00,"massaEspecifica": 2000,"calorEspecifico": 2625},
{"id": ...,"condutividade": 1.60,"massaEspecifica": 2057,"calorEspecifico": 2151},
...

```
Regras:
- Para cada duto do tipo poço (na mesma ordem dos dutos: plataforma → reservatório), ler na saída `#XKF` (condutividade) e `#ALFFOR` (difusividade) pelo índice `ind` da unidade.  
- Se difusividade = 0, usar `1,0000E-6` como default.  
- `calorEspecifico = condutividade / (difusividade * massaEspecifica)`
- Usar 2000 como `massaEspecifica`. 
- Nos dutos do tipo poço, `idFormacao` aponta para o ID da formação correspondente na tabela.
- Fora do poço, `idFormacao = -1`.

### `dutosServico` - linha de serviço e condições

A linha de serviço corresponde ao **sistema de injeção** (por exemplo, linha de gás lift), incluindo:
- Unidades de serviço cadastradas no MR2 (do simulador Marlim II), da plataforma até a cabeça do poço.  
- Unidades internas ao poço, que não são explicitamente cadastradas no MR2, mas são **derivadas** dos dutos de produção.
---
#### Conceito e diferenças em relação à linha de produção
##### Linha de produção (referência comparativa)
- No JSON do conversor (`sentidoGeometriaSegueEscoamento = false`), o primeiro duto é o mais próximo da **plataforma** e o último é o mais próximo do **reservatório**.
- O escoamento real vai do reservatório → plataforma, mas a geometria no JSON está ordenada plataforma → reservatório.
- Unidades de produção no MR2: poços (com ou sem coluna), risers, linhas marítimas, linhas aéreas, linhas enterradas, choke, etc.
##### Linha de serviço (injeção)
- Começa na **plataforma** e segue até um ponto específico no poço.  
- Em geral, termina:
  - Na **válvula de gás lift mais profunda**, quando há válvula de gas lift ativa.
  - No **reservatório** (ou próximo), se há linha de serviço mas não existem válvulas de gas lift.

Em termos de simulação:

- A linha de serviço é responsável pela **injeção de gás** (ou outro fluido) no sistema.
- Sua geometria e condições iniciais impactam:
    - Condição de contorno de injeção (`gasInj`).
    - Acoplamento térmico com a linha de produção dentro do poço.

#### Dados de entrada do MR2 usados para a linha de serviço

- Em `<injecao>`:
    - nome (rótulo);
    - ul (comprimento);
    - ang (ângulo)
    - ntrec (discretização: ncelulas = ul/ntrec)

#### Dados de saída do MR2 usados para a linha de serviço
As **principais variáveis de saída** ligadas à linha de serviço no MR2 são:
```txt
#NUN_INJ      → número de unidades da linha de serviço
#NAT_INJ      → natureza das unidades da linha de serviço
#NTREC_INJ    → número de trechos por unidade
#CORR_INJ     
#Pinj         → pressões das unidades de injeção até a ANM
#TAinj        → temperatura ambiente (externa) das unidades de injeção até a ANM
#Tinj         → temperatura das unidades de injeção até a ANM
#PInjProd     → pressão das unidades de injeção até a ANM + unidades abaixo da ANM (poço)
#TInjProd     → temperatura das unidades de injeção até a ANM + unidades abaixo da ANM (poço)
#QGIT         → vazão de gás total injetada (global)
#QGI1         → vetor de vazão de gás por válvula (quando houver válvulas)
```
##### Interpretação dos campos principais
- `#NUN_INJ`  
  Número total de unidades da linha de serviço (apenas parte cadastrada no MR2 – normalmente da plataforma até a ANM / Master2).
- `#NAT_INJ`  
  Tipo de cada unidade de serviço. Em geral:
  - `2` – Linha marítima  
  - `3` – Linha aérea  
  - `4` – Linha enterrada  
- `#NTREC_INJ`  
  Número de trechos por unidade na linha de serviço. Usado diretamente para definir `nCelulas` dos dutos de serviço.
- `#Pinj`, `#Tinj`, `#TAinj`  
  Condições iniciais nas unidades de injeção:
  - `#Pinj`: pressão interna  
  - `#Tinj`: temperatura interna  
  - `#TAinj`: temperatura ambiente externa  
- `#PInjProd`, `#TInjProd`  
  Condições de pressão e temperatura **na linha de serviço ao longo do poço**, quando a linha de serviço se estende para dentro do poço (dutos de serviço internos, que são replicados dos dutos de produção).

#### Estrutura de `dutosServico` no MRT
No MRT, a linha de serviço é representada por:
```json
"dutosServico": [
  {
    "id": ...,
    "idCorte": ...,
    "angulo": ...,
    "rotulo": "...",
    "acoplamentoTermico": ...,
    "direcaoConveccao": ...,
    "idFormacao": ...,
    "discretizacao": [{
      "nCelulas": ...,
      "comprimento": ...
    }],
    "condicoesIniciais": {
      "compInter": [0.0, 1.0],
      "temp": [...],
      "pressao": [...],
      "vazaoMassicaGas": [...],
      "tempExterna": [...],
      "velExterna": [...],
      "kExterna": 0.32,
      "calorEspecificoExterno": 4182,
      "rhoExterno": 1000,
      "viscExterna": 0.001
    }
  },
  ...
]
```
Os dutos de serviço são construídos em **duas etapas**:
1. Importação das unidades de injeção presentes no MR2 (da plataforma até a cabeça do poço).  
2. Criação dos dutos de serviço dentro do poço (replicando os dutos de produção) até o ponto da última válvula ou até o reservatório, conforme o caso.

#### Etapa 1 – Importando as unidades de injeção do MR2
##### Identificação das unidades de injeção
Nos dados de entrada do MR2:
- `<marlim><entrada><sistema_producao><injeção> ... >`
Para cada unidade de injeção, temos:
- Tipo de unidade de injeção (`<?tipoUnidadeInjecao?>`), por exemplo:
  - `linha_maritima_inj`
  - `linha_aerea_inj`
  - `linha_enterrada_inj`
- Medidas de injeção:

  ```xml
  <medidas_inj>
    <ang>...</ang>    <!-- ângulo em graus -->
    <ul>...</ul>      <!-- comprimento total (unidade de serviço) -->
    <ntrec>...</ntrec><!-- número de trechos -->
  </medidas_inj>
  ```
##### Campos de geometria
Para cada unidade de injeção → um duto de serviço no MRT:
- `id`:  
  - inteiro sequencial começando em `0`.
- `idCorte`:  
  - obtido da tabela de cortes (sessão transversal) conforme o tipo da unidade (`#NAT_INJ`).
- `angulo`:  
  - lido de `<injeção><?tipoUnidadeInjecao?><medidas_inj><ang>` (em graus)  
  - convertido para radianos:

    ```txt
    angulo_radianos = angulo_graus * 0.017453292519
    ```
- `rotulo` (nome do duto de serviço):  

  ```txt
  "Serv_" + <tipo de unidade do Marlim Permanente> + "_" + <número sequencial>
  ```

  Onde o tipo de unidade é derivado de `#NAT_INJ`:

  - `1` → Riser  
  - `2` → LinhaM  
  - `3` → LinhaA  
  - `4` → LinhaE  
  - `5` → PocoComColuna  
  - `7` → PocoSemColuna  

  Exemplo:

  - `Serv_PocoComColuna_0`  
  - `Serv_LinhaM_1`  
- `acoplamentoTermico`:

  - Para estas unidades externas (antes de entrar no poço) → tipicamente `0` (sem acoplamento com formação).
- `direcaoConveccao`:

  - Em geral `0` (direção padrão, convecção "plataforma → poço").
- `idFormacao`:

  - Para unidades fora do poço → `-1`.
  - Para unidades dentro do poço → id da formação correspondente (mesma lógica que dutos de produção).
##### Discretização
Para cada unidade:
```json
"discretizacao": [{
  "nCelulas": ...,
  "comprimento": ...
}]
```
Regras:
- `nCelulas`:

  ```txt
  nCelulas = <injeção><?tipoUnidadeInjecao?><medidas_inj><ntrec>
  ```
- `comprimento`:

  ```txt
  comprimento_total = <injeção><?tipoUnidadeInjecao?><medidas_inj><ul>
  comprimento = comprimento_total / nCelulas
  ```

#### Condições iniciais dos dutos de serviço (parte externa)
As condições iniciais são representadas por:
```json
"condicoesIniciais": {
  "compInter": [0.0, 1.0],
  "temp": [...],
  "pressao": [...],
  "vazaoMassicaGas": [...],
  "tempExterna": [...],
  "velExterna": [...],
  "kExterna": [0.32,0.32],
  "calorEspecificoExterno": [4182,4182],
  "rhoExterno": [1000,1000],
  "viscExterna": [0.001,0.001]
}
```
##### `compInter`
- Sempre `[0.0, 1.0]`  
  Representa frações adimensionais de posição ao longo do duto (início e fim).
##### Temperatura interna – `temp`
- Dada por `#Tinj` ou `#TInjProd`, dependendo de se estamos na parte externa (da plataforma até a ANM) ou nas unidades internas ao poço.  
- Para as **unidades externas** (etapa 1), usamos:

  ```txt
  temp = [T_inicial, T_final]
  ```

  onde os valores são obtidos diretamente da série `#Tinj` correspondente à unidade.
##### Pressão interna – `pressao`
- Para as unidades externas (até a ANM):

  ```txt
  pressao = [Pinj_inicial, Pinj_final] + 1,03323
  ```
- A conversão é necessária porque:
  - No MR2, pressões são **manométricas**.
  - No MRT, pressões são **absolutas**.
##### Vazão mássica de gás – `vazaoMassicaGas`
A vazão mássica de gás injetado depende de:
- Existência de válvulas de gas lift.  
- `#QGIT` (vazão total de gás injetado).  
- Densidade do gás (`densidadeGas`) do **fluido de produção** já importado.  
- Eventuais vazões por válvula (`#QGI1`) para ajustar após cada válvula.
###### Regra básica (unidades externas, antes de válvulas)
- Se **não existe válvula de gas lift instalada**:

  ```txt
  vazaoMassicaGas = 0
  ```
- Se **existe válvula de gas lift**:

  ```txt
  vazaoMassicaGas = (#QGIT / 86400) * densidadeGas * 1.225
  ```

  Onde:

  - `#QGIT` é a vazão volumétrica de gás (tipicamente em m³/dia)  
  - `densidadeGas` é lida do `FLUIDOPRODUCAO` (`densidadeGas`)  
  - `1.225` é um fator de conversão adicional (ajuste de unidade / referência)
###### Ajuste após válvulas (na parte interna, etapa 2)
Nos dutos de serviço **dentro do poço**, o valor de `vazaoMassicaGas` é reavaliado considerando a vazão realmente injetada nas válvulas (`#QGI1`). Isso é detalhado no ponto pertinente do texto.
###### Temperatura externa – `tempExterna`
- Utiliza `#TAinj` para unidades externas:

  ```txt
  tempExterna = [TA_inicial, TA_final]
  ```
- Para unidades internas ao poço, pode ser `#TA` ou valores fixos, dependendo da regra de acoplamento com a formação.
##### Velocidade externa – `velExterna`
Depende do tipo de unidade de serviço:
- Se **linha marítima**:

  ```xml
  <injeção><linha_maritima_inj><vmar>
  ```
- Se **linha aérea**:

  ```xml
  <injeção><linha_aerea_inj><vento>
  ```
- Se **linha enterrada**:

  ```txt
  velExterna = 0
  ```
##### Propriedades térmicas externas fixas
Para todas as unidades:
```json
"kExterna": [0.32,0.32],
"calorEspecificoExterno": [4182,4182],
"rhoExterno": [1000,1000],
"viscExterna": [0.001,0.001]
```

#### Etapa 2 – Dutos de serviço dentro do poço
Após importar as unidades de injeção até a ANM (ou cabeça do poço), é necessário **prolongar a linha de serviço para dentro do poço** no MRT, até:
- A válvula de gás lift mais próxima do reservatório (caso haja válvulas ativas), ou  
- O reservatório, se tivermos linha de serviço mas não tivermos válvula de gas lift.
##### Identificação da válvula mais profunda (se houver)
Utiliza-se o vetor `#VV`:
```txt
#VV = 0;0;0;1;2;3;0;0;
```
- Cada posição corresponde a um duto de produção.
- Valores diferentes de `0` indicam que há válvula naquele duto.
- A válvula com ID maior, na posição mais a jusante (reservatório), é a **mais profunda**.
Exemplo:
- `#VV` acima → válvula mais profunda tem ID = 3.  
- Com isso, identifica-se o duto de produção correspondente (por exemplo, `id = 2` no MRT).
##### Intervalo de dutos de serviço internos
###### Caso 1 – Existe válvula de gas lift ativa
- As unidades de serviço internas vão da unidade que possui a válvula mais próxima do fundo do poço
  até a Master1.

###### Caso 2 – Não existe válvula de gas lift
- `#VV` tem todos os valores iguais a 0.
- As unidades internas vão da primeira unidade da linha de produção (fundo do poço) até a Master1.

##### Criação dos dutos de serviço internos ao poço
Para cada duto de produção no intervalo definido acima:
- Criar um duto de serviço correspondente, com:

  - `id` = identificador da **última unidade de injeção externa** + 1, 2, 3... (sequencial).  
  - `idCorte` = id de corte conforme tabela (normalmente igual ou compatível com o duto de produção correspondente).  
  - `angulo` = ângulo da unidade de produção, **invertido o sinal**:

    ```txt
    angulo_servico = -angulo_producao
    ```

  - `rotulo` = por exemplo `"dutoInjecao<id>"` ou `Serv_PocoComColuna_k`, conforme padrão adotado.  
  - `acoplamentoTermico` = `1` (há acoplamento térmico com a linha de produção e/ou formação).  
  - `direcaoConveccao` = `0` (padrão).  
  - `idFormacao` = id da formação correspondente (dentro do poço, em contato com formação ou com o duto de produção acoplado).
- `discretizacao`:
  - `nCelulas` = mesmo número de células do duto de produção correspondente.  
  - `comprimento` = mesmo comprimento da unidade de produção.
##### Condições iniciais nas unidades internas
Para cada duto de serviço interno:
```json
"condicoesIniciais": {
  "compInter": [0.0, 1.0],
  "temp": [...],
  "pressao": [...],
  "vazaoMassicaGas": [...],
  "tempExterna": [...],
  "velExterna": [...],
  "kExterna": 0.0 ou 0.32,
  "calorEspecificoExterno": 0.0 ou 4182,
  "rhoExterno": 0.0 ou 1000,
  "viscExterna": 0.0 ou 0.001
}
```
Regras (principalmente para o exemplo do documento):
- `temp`:

  ```txt
  temp = valores de #TInjProd (para cada unidade do poço)
  ```
- `pressao`:

  ```txt
  pressao = valores de #PInjProd + 1,03323
  ```
- `vazaoMassicaGas`:

  - Unidades **sem válvula**: mesmo valor da unidade anterior (serviço).  
  - Unidade que **contém válvula**:
    - Primeiro ponto: igual ao valor anterior.
    - Segundo ponto: recalculado subtraindo a vazão injetada na válvula: `NovaMassica = (#QGIT - vazaoInjecaoValvula) / 86400 * densidadeGas * 1.225`
        - Onde:
            - `vazaoInjecaoValvula = #QGI1[i]`, sendo `i` o índice da válvula (primeira, segunda, etc).  
- `tempExterna`, `velExterna`, `kExterna`, `calorEspecificoExterno`, `rhoExterno`, `viscExterna`:
    - No caso em questão (dutos internos ao poço), esses valores são todos `[0,0]`.


#### Relação entre dutos de serviço, válvulas e `fonteGasLift`
Depois de importada toda a linha de serviço (externa + interna), é preciso **ajustar a posição e parâmetros das válvulas de gas lift** (`fonteGasLift`):
- `comprimentoMedidoProducao` (da válvula) já é conhecido da importação dos dutos de produção.  
- `comprimentoMedidoServico` (da válvula) é calculado como:

  ```txt
  comprimentoMedidoServico = Comprimento_acumulado_linha_producao - comprimentoMedidoProducao
  ```
Exemplo:
- Comprimento acumulado da linha de produção: `6350 m`
- Posição da válvula na linha de produção: `2000 m`
- Então:

  ```txt
  comprimentoMedidoServico = 6350 - 2000 = 4350 m
  ```
Além disso, precisamos ajustar também algumas propriedades dos dutos de produção dentro do poço:
    
- Se no duto de serviço a formação = true, então o seu correspondente na linha de produção precisa ser false (não tem como as duas estarem em contato com a formação).
- Se no duto de serviço a formação = true, então o seu acoplamento térmico = 1.
- Se no duto de serviço o acoplamento térmico = 1, então o seu duto respectivo na linha de produção também será 1.
- Os dutos de produção que ficam a montante da última válvula de gás lift, tem formação = true, pois nesse caso não temos mais linha de serviço.


#### Resumo das principais regras de `dutosServico`
1. **Existência de linha de serviço**:
   - Se não há linha de serviço no MR2 → não há `dutosServico` no MRT.
2. **Importação em duas etapas**:
   - Etapa 1: Unidades de injeção do MR2 (plataforma → ANM).
   - Etapa 2: Dutos internos ao poço, replicados da produção, até Master1 ou até a válvula mais profunda.
3. **Conversões de unidade obrigatórias**:
   - Ângulo: graus → radianos.
   - Pressão: manométrica → absoluta (+1,03323).
4. **Vazão mássica de gás**:
   - Baseia-se em `#QGIT`, densidade do gás (`densidadeGas`) e possíveis vazões individuais por válvula (`#QGI1`).
5. **Acoplamento térmico e formação**:
   - Unidades externas: sem formação (`idFormacao = -1`), sem acoplamento térmico com dutos de produção.
   - Unidades internas: `idFormacao = <id formação>`, `acoplamentoTermico = 1`, com ajuste correspondente na linha de produção.
6. **Relação com `fonteGasLift`**:
   - Posição da válvula na linha de serviço depende tanto da geometria da produção quanto do comprimento da linha de serviço criada.


---

### Seção Transversal (`secaoTransversal`)
A seção transversal representa, radialmente, as camadas de materiais que compõem o duto (tubo, isolamento, revestimento, cimento, fluido no anular etc.). No arquivo MRT, isso é modelado pelo array `secaoTransversal`, e cada duto (produção ou serviço) referencia uma única seção transversal via `idCorte`.
#### Estrutura geral (`secaoTransversal`)
```jsonc
"secaoTransversal": [
  {
    "ativo": true,
    "id": 0,
    "anular": false,
    "diametroInterno": 0.124256,
    "rugosidade": 1.83e-4,
    "camadas": [
      {
        "tipoMedicaoCamada": "ESPESSURA",
        "espessura": 0.006,          // m
        "discretizacao": 1,
        "idMaterial": 0
      }
      // ...
    ]
  }
]
```
- `ativo`: indica se a seção pode ser usada.
- `id`: identificador inteiro da seção transversal (referenciado por `idCorte` nos dutos).
- `anular`: se `true`, a região de escoamento é anular.
- `diametroInterno`: diâmetro interno da região de escoamento da seção (em metros).
- `diametroExterno`: presente **somente** quando `anular = true`. Representa o maior diâmetro da área de escoamento do anular (em metros).
- `rugosidade`: rugosidade absoluta (em metros), típica de `#EDA` (produção) ou `#EDA_INJ` (injeção) convertidos de mm → m.
- `camadas`: lista de camadas radiais de **dentro para fora**.
Cada camada em `camadas`:
```jsonc
{
  "tipoMedicaoCamada": "DIAMETRO", // ou "ESPESSURA"
  "diametro": 0.130,               // m, se tipoMedicaoCamada = "DIAMETRO"
  "espessura": 0.006,              // m, se tipoMedicaoCamada = "ESPESSURA"
  "discretizacao": 1,
  "idMaterial": 0
}
```

---
#### Dados de entrada relevantes do MR2
Para cada unidade (duto) no MR2, os dados de seção transversal vêm principalmente dos vetores de saída (produção) e seus equivalentes de injeção, por exemplo:
- Produção:
  - `#DTI`  – diâmetro interno do tubo/linha (mm)  
  - `#DRI`  – diâmetro interno do revestimento/riser (mm)  
  - `#ESPT` – espessura do tubo/linha (mm)  
  - `#ESPI` – espessura do isolamento do tubo (mm)  
  - `#ESPR` – espessura do revestimento (mm)  
  - `#ESPC` – espessura do cimento (mm)  
  - `#EDA`  – rugosidade absoluta (mm)  
  - `#XKT`  – condutividade térmica do tubing  
  - `#XKI`  – condutividade térmica do isolamento  
  - `#XKR`  – condutividade térmica do revestimento  
  - `#XKC`  – condutividade térmica do cimento  
  - `#NANU` – tipo de fluido no anular (0: sem fluido; 1: gás; 2: líquido)
- Injeção (linha de serviço):
  - `#DTI_INJ`, `#ESPT_INJ`, `#ESPI_INJ`, `#EDA_INJ`, … (mesma lógica, para a linha de serviço).
Todos esses vetores são organizados por unidade, na ordem plataforma → reservatório. Para cada unidade que se tornará um duto no MRT, deve-se construir uma seção transversal correspondente ou reutilizar uma já existente com a mesma geometria.
---
#### Conversão de unidades
As grandezas geométricas do MR2 estão em **mm** e devem ser convertidas para **m**:
```text
DTI_m  = DTI_mm  / 1000.0
DRI_m  = DRI_mm  / 1000.0
ESPT_m = ESPT_mm / 1000.0
ESPI_m = ESPI_mm / 1000.0
ESPR_m = ESPR_mm / 1000.0
ESPC_m = ESPC_mm / 1000.0
EDA_m  = EDA_mm  / 1000.0
```
---
#### Padrão de construção das camadas
Camadas são montadas **de dentro para fora**. Adota-se:
```text
tipoMedicaoCamada = "ESPESSURA"
```
com:
- Para cada camada, informa-se diretamente a espessura radial em metros.

**Exceção**: a camada do anular (gap entre tubing/isolamento e revestimento) dentro do poço usa `tipoMedicaoCamada = "DIAMETRO"` com o diâmetro interno do revestimento (`DRI_m`), pois o gap não é informado como espessura fixa.

onde:
- `D_interno` é o `diametroInterno` da seção (em m), normalmente:
  - `DTI_m` para linha/tubing (linhas marítimas, aéreas, enterradas, risers, poço com coluna).  
  - `DRI_m` para poço sem coluna (escoamento no espaço do revestimento).
- `espessura_k` é a espessura radial da camada `k` (tubing, isolamento, revestimento, cimento etc.), em metros.
Em todas as camadas:
- `discretizacao` = `1` (pode ser ajustado se precisar de maior refinamento).  
- `idMaterial` aponta para um material definido em outra parte do JSON (materiais de tubulação, isolamento, revestimento, cimento, fluido anular etc.).

---
#### Casos típicos de seção transversal
A seguir, estão os padrões mínimos necessários para representar linhas e poços, cobrindo os casos essenciais.

##### Linhas marítimas, aéreas e enterradas (produção e serviço)

**Sem isolamento (`ESPI <= 0`)**  
Camada única: tubing.
- Entradas:

  - `DTI_m`, `ESPT_m`, `EDA_m`.
- Seção transversal:

  ```jsonc
  {
    "ativo": true,
    "id": <id>,
    "anular": false,
    "diametroInterno": DTI_m,
    "rugosidade": EDA_m,
    "camadas": [
      {
        "tipoMedicaoCamada": "ESPESSURA",
        "espessura": ESPT_m,
        "discretizacao": 1,
        "idMaterial": idMaterialTubing
      }
    ]
  }
  ```
**Com isolamento (`ESPI > 0`)**  
Duas camadas: tubing + isolamento.
- Entradas:

  - `DTI_m`, `ESPT_m`, `ESPI_m`, `EDA_m`.
- Seção transversal:

  ```jsonc
  {
    "ativo": true,
    "id": <id>,
    "anular": false,
    "diametroInterno": DTI_m,
    "rugosidade": EDA_m,
    "camadas": [
      {
        "tipoMedicaoCamada": "ESPESSURA",
        "espessura": ESPT_m,
        "discretizacao": 1,
        "idMaterial": idMaterialTubing
      },
      {
        "tipoMedicaoCamada": "ESPESSURA",
        "espessura": ESPI_m,
        "discretizacao": 1,
        "idMaterial": idMaterialIsolamento
      }
    ]
  }
  ```
##### Poço com coluna — caso desacoplado (a montante da última VGL)
Quando o duto de produção está entre o reservatório e a última válvula de gas lift (não há linha de serviço acoplada nesse trecho), a seção transversal inclui todas as camadas desde o tubing até o cimento, incluindo o fluido no anular como material explícito.

Camadas:
1. Tubing (coluna de produção).  
2. Isolamento do tubing (se `ESPI > 0`).  
3. Anular (gap entre tubing/isolamento e revestimento) — usa `tipoMedicaoCamada = "DIAMETRO"` com `DRI_m` e material fluido.  
4. Revestimento.  
5. Cimento (se `ESPC > 0`).
- Entradas:

  - `DTI_m`, `ESPT_m`, `ESPI_m`, `DRI_m`, `ESPR_m`, `ESPC_m`, `EDA_m`, `#NANU`.

Se houver isolamento:
```jsonc
{
  "ativo": true,
  "id": <id>,
  "anular": false,
  "diametroInterno": DTI_m,
  "rugosidade": EDA_m,
  "camadas": [
    {
      "tipoMedicaoCamada": "ESPESSURA",
      "espessura": ESPT_m,
      "discretizacao": 1,
      "idMaterial": idMatTubing
    },
    {
      "tipoMedicaoCamada": "ESPESSURA",
      "espessura": ESPI_m,
      "discretizacao": 1,
      "idMaterial": idMatIsolTubing
    },
    {
      "tipoMedicaoCamada": "DIAMETRO",
      "diametro": DRI_m,
      "discretizacao": 1,
      "idMaterial": idMatAnular
    },
    {
      "tipoMedicaoCamada": "ESPESSURA",
      "espessura": ESPR_m,
      "discretizacao": 1,
      "idMaterial": idMatRevestimento
    },
    {
      "tipoMedicaoCamada": "ESPESSURA",
      "espessura": ESPC_m,
      "discretizacao": 1,
      "idMaterial": idMatCimento
    }
  ]
}
```
Se não houver isolamento (`ESPI_m = 0`), omitir a camada de isolamento.
Se não houver cimento (`ESPC_m = 0`), omitir a camada de cimento.

A camada do anular sempre está presente e referencia um material de acordo com `#NANU`:
- `0` → material sólido  
- `1` → AR (tipo=3)  
- `2` → AGUANULAR (tipo=2)

##### Poço com coluna — caso acoplado (entre ANM e última VGL)
Quando existe linha de serviço acoplada, a seção transversal do poço é **dividida em duas**:

**Seção de produção** (`anular = false`): contém apenas tubing + isolamento (sem anular, sem revestimento, sem cimento).
```jsonc
{
  "ativo": true,
  "id": <id>,
  "anular": false,
  "diametroInterno": DTI_m,
  "rugosidade": EDA_m,
  "camadas": [
    {
      "tipoMedicaoCamada": "ESPESSURA",
      "espessura": ESPT_m,
      "discretizacao": 1,
      "idMaterial": idMatTubing
    },
    {
      "tipoMedicaoCamada": "ESPESSURA",
      "espessura": ESPI_m,
      "discretizacao": 1,
      "idMaterial": idMatIsolTubing
    }
  ]
}
```
Se não houver isolamento, a segunda camada é omitida.

**Seção de serviço** (`anular = true`): o escoamento ocorre no espaço anular entre o tubing/isolamento e o revestimento. Não é definido material fluido no anular porque o próprio escoamento já ocorre nesse espaço. As camadas representam apenas o revestimento + cimento (de dentro para fora):
```jsonc
{
  "ativo": true,
  "id": <id>,
  "anular": true,
  "diametroInterno": DTI_m + 2*ESPT_m + 2*ESPI_m,
  "diametroExterno": DRI_m,
  "rugosidade": EDA_m,
  "camadas": [
    {
      "tipoMedicaoCamada": "ESPESSURA",
      "espessura": ESPR_m,
      "discretizacao": 1,
      "idMaterial": idMatRevestimento
    },
    {
      "tipoMedicaoCamada": "ESPESSURA",
      "espessura": ESPC_m,
      "discretizacao": 1,
      "idMaterial": idMatCimento
    }
  ]
}
```
Onde:
- `diametroInterno` = diâmetro externo do tubing+isolamento (limite interno do anular).
- `diametroExterno` = `DRI_m` (diâmetro interno do revestimento, limite externo do anular).
- Camada de cimento omitida se `ESPC_m = 0`.

##### Poço sem coluna
No poço sem coluna, o escoamento é modelado dentro do revestimento; o cimento envolve o revestimento.
- Entradas:

  - `DRI_m`, `ESPR_m`, `ESPC_m`, `EDA_m`.
- Seção transversal:

  ```jsonc
  {
    "ativo": true,
    "id": <id>,
    "anular": false,
    "diametroInterno": DRI_m,
    "rugosidade": EDA_m,
    "camadas": [
      {
        "tipoMedicaoCamada": "ESPESSURA",
        "espessura": ESPR_m,
        "discretizacao": 1,
        "idMaterial": idMatRevestimento
      },
      {
        "tipoMedicaoCamada": "ESPESSURA",
        "espessura": ESPC_m,
        "discretizacao": 1,
        "idMaterial": idMatCimento
      }
    ]
  }
  ```
---
#### Materiais e `idMaterial`
Cada camada referencia um material por `idMaterial`. Os materiais típicos incluem:
- Tubulação (aço de produção).  
- Isolamento (térmico).  
- Revestimento.  
- Cimento.  
- Fluido em anular (ar, água).
Exemplos de materiais fluídos usados no anular:
- AR:
  - `condutividade ≈ 0.022`
  - `rho ≈ 1.29`
  - `tipo = 3` (gás)
  - `visc ≈ 1.708e-5`
  - `calorEspecifico ≈ 1004.0`
- AGUANULAR:
  - `condutividade ≈ 0.603`
  - `rho ≈ 1060.0`
  - `tipo = 2` (água)
  - `visc ≈ 0.001`
  - `calorEspecifico ≈ 4216.0`
É importante **não duplicar** materiais genéricos: AR, AGUANULAR, cimento padrão, aço padrão etc. Se um material já existe na lista, o mesmo `idMaterial` deve ser reutilizado em outras seções/camadas.

---

## Acessórios

### ChokeSup – válvula choke ao final do sistema de produção
Elemento default:
```json
"chokeSup": {
  "coeficienteDescarga": 0.84,
  "modelo": 0,
  "abertura": [1],
  "tempo": [0]
}
```

### Separador – pressão na plataforma
```json
"separador": {
  "pressao": <opções_calculo>.<ptub> + 1,03323,
  "tempo": [0]
}
```

Se `<ptub>` estiver vazio, usar `0`.

### `fonteGas` – fonte de gás na linha de produção
Para unidades de tipo linha marítima, aérea, enterrada ou riser:
- Ler `<entrada><sistema_producao><produção><linha_maritima><qgi>`  
- Se valor ≠ 0 → criar elemento `fonteGas`.
Estrutura:
```json
"fonteGas": [{
  "ativo": true,
  "id": <contador começando em 0>,
  "comprimentoMedido": <comprimento acumulado do reservatório até a unidade anterior>,
  "temperatura": [40],
  "vazaoGas": <entrada><sistema_producao><produção><linha_maritima><qgi>,
  "tempo": [0]
}]
```

Outra fonte de gás: válvulas ativas que **não** são por desempenho (tratadas como `fonteGas`). As válvulas por desempenho são tratadas como `fonteGasLift` no MRT, um outro objeto que detalha a válvula de gas lift quando há linha de serviço (`configuracaoInicial: linhaGas = true`), detalhado mais abaixo.

**Regras de descarte de `fonteGas`:**
- Descartar válvulas do tipo **CEGA** (`<vgl_inst><orificio> = 'CEGA'`): não representam injeção real.
- Descartar fontes com **vazão zero** (`vazaoGas = 0`): não faz sentido injetar gás com vazão nula.
- Se após os filtros não restar nenhuma fonte válida, o campo `fonteGas` não é incluído no JSON.

Para válvulas tratadas como `fonteGas`, a temperatura é determinada assim:
- Se existe linha de serviço com perfil de temperatura (`#TINJ`): usar a temperatura de entrada da linha de serviço no duto correspondente.
- Caso contrário: usar a temperatura do revestimento (`#TTI`) no duto de produção correspondente.
- Se nenhuma temperatura for encontrada: usar 0.

### Master1
Comprimento medido da `master1`:
> Soma do comprimento (`#UL`) das unidades da plataforma até a última unidade que antecede uma unidade do tipo poço com coluna ou poço sem coluna

A `master1` sempre começa aberta:
```json
"master1": {
  "comprimentoMedido": 3000,
  "abertura": [1],
  "tempo": [0]
}
```

### Válvula de gás lift – `fonteGasLift`

Válvula sempre instalada a montante do duto.
Posição da válvula:
- Vetor `#VV` da saída guarda o (ID+1) da válvula associado a cada duto.  
- Exemplo:

  ```txt
  #VV=0;0;0;0;1;2;3;0;0;
  ```

  Com `#NUN=9`, dutos 2, 3 e 4 possuem válvulas (ID's 2,1,0, respectivamente). Lembrando que a contagem é de trás para frente!


- Cálculo do comprimento medido de produção da válvula:
    - A partir de `#UL` e regras descritas, somando comprimentos.

- Outras informações:
    - `tipoValvula`: `<entrada><gas_lift><válvula_instalada>< <vgl_inst tipo=>`
    - `diametroOrificio`: `<entrada><gas_lift><válvula_instalada>< <vgl_inst><diam_orificio>`
        - Atenção: o diâmetro do orifício no Marlim Permanente está na unidade polegadas (in) e no Transiente está em metros, logo teremos que fazer a conversão, sendo:
            - Metros = (in * 2.54) / 100
    - `cdVGL`: 1.0
    - `pressaoCalibracao`
        - `(<entrada><gas_lift><válvula_instalada><pcal> + 1,03323) * 14,223`
        - Conversão: kgf/cm² manométrica → absoluta (+1,03323) → psi (×14,223)
    - `temperaturaCalibracao`
        - `<entrada><gas_lift><válvula_instalada><tcal> * 9/5 + 32`
        - Conversão: °C → °F


---

## Master2
Criada se existe linha de serviço (unidades de injeção no Permanente):

- Comprimento medido da `master2` = soma dos comprimentos das unidades/dutos da linha de serviço **antes de entrar no poço**.
---

## BCS – bombas centrífugas

Importadas se existe `entrada->conjuntos_bcs->conjunto_bcs`.  

Incluem:

- Comprimento medido/ profundidade medida convertido conforme:
  - Para dentro do poço (`<conjuntos_bcs><conjunto_BCS> <bcs_local_instalacao tipo>` = DENTRO)
    - comprimentoMedido = (profundidade medida – `distancia_mesa_rotativa`) + comprimentoMedido da `master1` 
        - obs: `distancia_mesa_rotativa` deve ser lido de `<entrada><sistema_producao><dmrcp>`
  - Para fora do poço (`<conjuntos_bcs><conjunto_BCS> <bcs_local_instalacao tipo>` = FORA):  
    comprimentoMedido = profundidade medida (valor direto do MR2, sem transformação)
Campos principais:
- `ativo` ← `<conjunto_BCS><ativo>`  
- `comprimentoMedido` (conforme regra acima)  
- `tempo = 0`  
- `frequencia` ← `<conjuntos_bcs><conjunto_BCS><freq_motor>`  
- Vetores `vazao`, `head`, `eficiencia`, `potencia` – da `<conjuntos_bcs><conjunto_BCS><bomba><tabela_desempenho>
<linha_desempenho><valor>`
    - Cada elemento `linha_desempenho´ tem quatro valores, que são respectivamente `vazao`, `head`, `eficiencia`, `potencia`.
- `freqref` ← `<conjuntos_bcs><conjunto_BCS><bomba><freq_motor>`  
- `nestag` ← `<conjuntos_bcs><conjunto_BCS><estagios>`
- `nestagFab` ← `<conjuntos_bcs><conjunto_BCS><bomba><estagios>`

---

## Condições de Contorno

Elementos no MR3:
- `separador` (define pressão a jusante da linha de produção)  
- `condicaoPressao` (define pressão a montante da linha de produção)  
- `gasInj` (define condições de contorno na linha de serviço)

### Condição de contorno da linha de produção

Elementos no MR2 (`opcoes_calculo->contorno`):

- `1` – `VAZAO_EQUILIBRIO` (conhecidos PJUS e IPR, calcula vazão)
- `2` – `PMON`  (conhecidos PJUS e vazão, calcula PMON)
- `3` – `VAZAO`  (conhecidos PMON e PJUS, calcula vazão)
- `4` – `PJUS` (conhecidos PMON e vazão, calcula PJUS)

### Condição de contorno da linha de serviço
- Se **não** temos linha de serviço → montante e jusante = `-1`  
- Se temos linha de serviço → montante = pressão (`0`), jusante = fechado (`2`)  
### `condicaoPressao`
Elemento criado sempre:
```json
"condicaoPressao": {
  "ativo": false,
  "pressao": #PWH,
  "temperatura": #TWF,
  "titulo": fórmula abaixo,
  "razaoBeta": [0],
  "tempo": [0]
}
```
`titulo = (Vs_gas * dens_gas) / ((Vs_gas * dens_gas) + (Vs_liq * dens_liq))`  
com dados de `#DENL` (dens_liq), `#DENG` (dens_gas), `#VELL` (Vs_liq), `#VELG` (Vs_gas).

### `GasInj` (linha de serviço)
Criado apenas quando houver válvula de gas lift ativa e por desempenho:
- `tipoCC = 1`  (condição do tipo vazão de injeção – escolhido para facilitar a convergência no Marlim3)
- `temperatura` – `#TINJ` do primeiro nó do primeiro trecho da linha de serviço (se houver linha de serviço) ou `#TTI`, temperatura do revestimento do primeiro trecho do primeiro nó da coluna de produção, se não houver linha de serviço  
- `pressaoInjecao` – `GasLift->PREV + 1,03323`  
- `vazaoGas` – `#QGIT`  
- `chuteVazaoInjecao` – `true` se `vazaoGas > 0`, senão `false`. Usado como chute inicial de vazão para facilitar a convergência quando tipoCC = 0 (pressão).
- `tempo = [0]`  
- `ativo = true`
