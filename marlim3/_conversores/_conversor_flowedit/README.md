# Conversão de FLOWedit para Marlim3

O código Python armazenado neste repositório é capaz de ler a geometria de um sistema de produção de uma planilha FLOWedit, e transpor as informações para um arquivo "*.mr3" de entrada do simulador Marlim3.

## Uso via GUI

Um primeiro uso pode ser esclarecedor do escopo de tarefas possíveis de serem realizadas com este código. 

Após baixar ou clonar este repositório, o usuário deve:

- unificar em um único diretório de sua escolha o conteúdo das pastas "Conversao" e "Interface".
- executar o arquivo "Interface_Conversor.py".

Na janela que se abre, o usuário precisa minimamente:

- informar o caminho completo do arquivo FLOWedit de interesse (origem das informações).
- informar o caminho completo do arquivo "*.mr3" de entrada do Marlim3 para o qual deseja transpor as informações do arquivo FLOWedit.
- apertar o botão "Converter", e aguardar a mensagem confirmando o sucesso da operação.

Ao final deste passo-a-passo, a geometria do sistema de produção de interesse terá sido preenchida no arquivo "*.mr3" informado.

Nas demais abas da interface, o programa oferece a possibilidade de o usuário aproveitar o momento da conversão para também configurar o arquivo "*.mr3" de simulação com condições de contorno, fontes de massa, variáveis de saída do Marlim3, etc. Convém reforçar que nestas abas de uso opcional, somente aquelas onde for assinalada a opção "Preencher no json" é que terão suas informações registradas no arquivo de entrada do Marlim3 informado.

### Arquivo executável

Também é possível utilizar este conversor na forma de um arquivo executável, acionável por duplo-clique.

Após unificar os arquivos "*.py" em um único diretório, conforme indicado acima, executa-se no _prompt_ nesse diretório o seguinte comando:

```sh
pyinstaller --onefile --windowed Interface_Conversor.py
```

Ao final da compilação, o arquivo executável será disponibilizado em um novo subdiretório _dist_.

## Instruções adicionais para o desenvolvedor

Na primeira fase de desenvolvimento deste código, todo o foco estava na transposição das informações da FLOWedit para o Marlim3. O diretório "Conversao" contém o código dedicado a esta etapa, cujo arquivo central é o "FLOWeditConversion.py".

O método "Convert", da classe homônima contida no arquivo, é o "coração" da conversão FLOWedit-Marlim3, sendo o ponto de partida recomendado para o desenvolvedor que precisar se aprofundar mais nos detalhes do procedimento.

O arquivo "ConverterFLOWedit.py" contém um brevíssimo script Python que serve de exemplo do que outros códigos teriam de implementar para incorporar a transferência de informações de planilhas FLOWedit para o Marlim3 usando o presente código.

Aplicações que necessitem automatizar a conversão de geometrias FLOWedit para Marlim3 somente deverão necessitar do código contido no diretório "Conversao".

Em um segundo momento, julgou-se importante disponibilizar uma interface gráfica simples para aqueles usuários que necessitassem realizar esta conversão FLOWedit-Marlim3 de forma não-automática. O código-fonte deste segundo desenvolvimento (cujo uso foi descrito acima) encontra-se no diretório "Interface" do repositório.

## Futuro

As limitações que os usuários podem encontrar nesta interface simples, como o fato de não contemplar totalmente simulações composicionais, deve-se ao fato de os trabalhos que fizeram uso desta ferramenta até o presente momento ainda não terem necessitado desta funcionalidade.

Uma interface web mais completa para o simulador Marlim3 está em vias de ser concluída, e existe a previsão de que venha a incorporar este conversor FLOWedit-Marlim3, de modo a permitir seu aproveitamento na montagem de todo tipo de simulação.