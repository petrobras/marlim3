# Conversão de FLOWedit para Marlim3

Este conversor é capaz de ler a geometria de um sistema de produção de uma planilha FLOWedit, e transpor as informações para um arquivo "*.mr3" de entrada (template) do simulador Marlim3.

# Requisitos

- FLOWedit versão 2026 ou mais recente. Versões antigas, cuja tabela *Rochas* não tem a coluna `id`, são rejeitadas com mensagem explicativa. 
- Aba "Modelo Marlim 3" atualizada na FLOWedit.

## Uso via Python (recomendado para automação)

```python
import json
from marlim3._conversores._conversor_flowedit.conversor._converter import converter_flowedit_para_json

template = json.load(open("template.mr3", encoding="utf-8"))
modelo = converter_flowedit_para_json(
    excel_path="planilha.xlsm",
    json_template_dict=template,
    fluid_model="tab",                  # 'blackoil' | 'tab' | 'compositional' | None
    fluid_model_path="fluido.tab",
    keep_flowedit_xy=False,             # False = origem em (0, 0)
    write_log=True,                     # Log_ConversaoFLOWedit_MARLIM3.txt na pasta da planilha
    vazao_gas_lift=200000.0,            # sm3/d
    temperatura_gas_lift_servico=50.0,  # degC
)
json.dump(modelo, open("modelo.mr3", "w", encoding="utf-8"), indent=2, ensure_ascii=False)
```

> Em `pvtsimArq` é gravado apenas o **nome** do arquivo de fluido. Coloque o `.tab` na pasta do `.mr3` ou informe a pasta ao motor com `-p <pasta>/`.

### Gás-lift

A rota é definida pela presença de dutos de serviço na planilha:

| Planilha | Modelo gerado |
|---|---|
| Com linha de serviço | `linhaGas = true`; `gasInj` ativo com vazão (`vazao_gas_lift`) a `temperatura_gas_lift_servico`; VGLs na posição da planilha com os parâmetros da 1ª VGL do template; fonte de gás `VGL` do template desativada |
| Sem linha de serviço | `linhaGas = false`; VGLs removidas; `gasInj`/`perfilServico` inativos; `acoplamentoTermico = 0`; fonte de gás `VGL` ativa na VGL mais profunda com `vazao_gas_lift` e a menor temperatura externa dos dutos de produção |
| Sem linha e sem VGL | nenhuma injeção é criada (aviso) |

> Essas regras valem para `converter_flowedit_para_json`. A GUI usa as configurações das suas próprias abas.

## Uso via GUI

Um primeiro uso pode ser esclarecedor do escopo de tarefas possíveis de serem realizadas com este código.

### Pré-requisitos 

As dependências da GUI estão declaradas no grupo `flowedit-gui` do `pyproject.toml` do projeto. Para instalá-las, execute na raiz do repositório (`marlim3/`):

```sh
uv sync --group flowedit-gui
```

> **Dependências instaladas:** `PyQt5` e `openpyxl` (compatível com Linux, macOS e Windows).

### Execução

Após baixar ou clonar este repositório e instalar as dependências, execute o script de interface diretamente a partir da pasta `gui/`:

```sh
uv run python marlim3/_conversores/_conversor_flowedit/gui/Interface_Conversor.py
```

Ou, se o ambiente já estiver ativado:

```sh
python marlim3/_conversores/_conversor_flowedit/gui/Interface_Conversor.py
```

Na janela:

- informe a planilha FLOWedit (origem);
- informe o `.mr3` de destino (template que será sobreescrito);
- clique em **Converter** e aguarde a confirmação.

Ao final deste passo-a-passo, a geometria do sistema de produção de interesse terá sido preenchida no arquivo "*.mr3" informado.

Nas demais abas da interface, o programa oferece a possibilidade de o usuário aproveitar o momento da conversão para também configurar o arquivo "*.mr3" de simulação com condições de contorno, fontes de massa, variáveis de saída do Marlim3, etc. Convém reforçar que nestas abas de uso opcional, somente aquelas onde for assinalada a opção "Preencher no json" é que terão suas informações registradas no arquivo de entrada do Marlim3 informado.

### Arquivo executável (Windows)

Também é possível utilizar este conversor na forma de um arquivo executável, acionável por duplo-clique. Para isso, instale também o grupo `desktop`:

```sh
uv sync --group flowedit-gui --group desktop
cd marlim3/_conversores/_conversor_flowedit/gui
uv run pyinstaller --onefile --windowed --name ConversorFLOWedit --paths .. Interface_Conversor.py
```

Ao final da compilação, o arquivo executável será disponibilizado em um novo subdiretório `gui/dist/`.

## Instruções adicionais para o desenvolvedor

Na primeira fase de desenvolvimento deste código, todo o foco estava na transposição das informações da FLOWedit para o Marlim3. O diretório `conversor/` contém o código dedicado a esta etapa, cujo arquivo central é o `FloweditConversion.py`.

O método `Convert`, da classe homônima contida no arquivo, é o "coração" da conversão FLOWedit-Marlim3, sendo o ponto de partida recomendado para o desenvolvedor que precisar se aprofundar mais nos detalhes do procedimento.

Aplicações que necessitem automatizar a conversão de geometrias FLOWedit para Marlim3 somente deverão necessitar do código contido no diretório `conversor/`.

Em um segundo momento, julgou-se importante disponibilizar uma interface gráfica simples para aqueles usuários que necessitassem realizar esta conversão FLOWedit-Marlim3 de forma não-automática. O código-fonte deste segundo desenvolvimento encontra-se no diretório `gui/` do repositório.

O `Interface_Conversor.py` manipula o `sys.path` em tempo de execução para localizar o pacote `conversor/` de forma relativa à sua própria localização no repositório, tornando desnecessária qualquer etapa manual de cópia ou movimentação de arquivos.

Obs: As limitações que os usuários podem encontrar nesta interface simples, como o fato de não contemplar totalmente simulações composicionais, deve-se ao fato de os trabalhos que fizeram uso desta ferramenta até o presente momento ainda não terem necessitado desta funcionalidade.