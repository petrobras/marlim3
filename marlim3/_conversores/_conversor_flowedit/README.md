# Conversão de FLOWedit para Marlim3

Este conversor é capaz de ler a geometria de um sistema de produção de uma planilha FLOWedit, e transpor as informações para um arquivo "*.mr3" de entrada do simulador Marlim3.

## Uso via GUI

Um primeiro uso pode ser esclarecedor do escopo de tarefas possíveis de serem realizadas com este código.

### Pré-requisitos

As dependências da GUI estão declaradas no grupo `flowedit-gui` do `pyproject.toml` do projeto. Para instalá-las, execute na raiz do repositório (`motor/`):

```sh
uv sync --group flowedit-gui
```

> **Dependências instaladas:** `PyQt5` e `openpyxl` (compatível com Linux, macOS e Windows).

### Execução

Após baixar ou clonar este repositório e instalar as dependências, execute o script de interface diretamente a partir da pasta `gui/`:

```sh
cd marlim3/_conversores/_conversor_flowedit/gui
uv run python Interface_Conversor.py
```

Ou, se o ambiente já estiver ativado:

```sh
python marlim3/_conversores/_conversor_flowedit/gui/Interface_Conversor.py
```

Na janela que se abre, o usuário precisa minimamente:

- informar o caminho completo do arquivo FLOWedit de interesse (origem das informações).
- informar o caminho completo do arquivo "*.mr3" de entrada do Marlim3 para o qual deseja transpor as informações do arquivo FLOWedit.
- apertar o botão "Converter", e aguardar a mensagem confirmando o sucesso da operação.

Ao final deste passo-a-passo, a geometria do sistema de produção de interesse terá sido preenchida no arquivo "*.mr3" informado.

Nas demais abas da interface, o programa oferece a possibilidade de o usuário aproveitar o momento da conversão para também configurar o arquivo "*.mr3" de simulação com condições de contorno, fontes de massa, variáveis de saída do Marlim3, etc. Convém reforçar que nestas abas de uso opcional, somente aquelas onde for assinalada a opção "Preencher no json" é que terão suas informações registradas no arquivo de entrada do Marlim3 informado.

### Arquivo executável

Também é possível utilizar este conversor na forma de um arquivo executável, acionável por duplo-clique. Para isso, instale também o grupo `desktop`:

```sh
uv sync --group flowedit-gui --group desktop
```

Em seguida, execute o seguinte comando a partir da pasta `gui/`:

```sh
cd marlim3/_conversores/_conversor_flowedit/gui
uv run pyinstaller Interface_Conversor.spec
```

Ao final da compilação, o arquivo executável será disponibilizado em um novo subdiretório `dist/`.

## Instruções adicionais para o desenvolvedor

Na primeira fase de desenvolvimento deste código, todo o foco estava na transposição das informações da FLOWedit para o Marlim3. O diretório `conversor/` contém o código dedicado a esta etapa, cujo arquivo central é o `FloweditConversion.py`.

O método `Convert`, da classe homônima contida no arquivo, é o "coração" da conversão FLOWedit-Marlim3, sendo o ponto de partida recomendado para o desenvolvedor que precisar se aprofundar mais nos detalhes do procedimento.

Aplicações que necessitem automatizar a conversão de geometrias FLOWedit para Marlim3 somente deverão necessitar do código contido no diretório `conversor/`.

Em um segundo momento, julgou-se importante disponibilizar uma interface gráfica simples para aqueles usuários que necessitassem realizar esta conversão FLOWedit-Marlim3 de forma não-automática. O código-fonte deste segundo desenvolvimento encontra-se no diretório `gui/` do repositório.

O `Interface_Conversor.py` manipula o `sys.path` em tempo de execução para localizar o pacote `conversor/` de forma relativa à sua própria localização no repositório, tornando desnecessária qualquer etapa manual de cópia ou movimentação de arquivos.

Obs: As limitações que os usuários podem encontrar nesta interface simples, como o fato de não contemplar totalmente simulações composicionais, deve-se ao fato de os trabalhos que fizeram uso desta ferramenta até o presente momento ainda não terem necessitado desta funcionalidade.