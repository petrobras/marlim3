import networkx as nx 
import numpy as np
import matplotlib.pyplot as plt

def _plotar_rede(rede_obj):
    # Criar o grafo da rede
    G = nx.DiGraph()

    # Adicionar tramos como arestas no grafo
    edges = []  # Para armazenar as arestas representando os tramos
    for idx, tramo in enumerate(rede_obj.Conexao):
        coletores = tramo.get("coletores", [])
        afluentes = tramo.get("afluentes", [])

        # Criar arestas entre nós conectados
        for afluente in afluentes:
            edges.append((afluente, idx))
        for coletor in coletores:
            edges.append((idx, coletor))

    G.add_edges_from(edges)

    # Ajustar as posições para o layout
    pos = nx.spring_layout(G, k=0.5, iterations=100)

    # Plotar a rede de escoamento
    plt.figure(figsize=(12, 8))
    plt.gca().set_facecolor('#f0f0f0')

    # Primeiro, desenhe todas as arestas
    for idx, tramo in enumerate(rede_obj.Conexao):
        coletores = tramo.get("coletores", [])
        afluentes = tramo.get("afluentes", [])

        for afluente in afluentes:
            draw_edge(pos, afluente, idx)

        for coletor in coletores:
            draw_edge(pos, idx, coletor)

    # Em seguida, desenhe todos os nós
    for idx in pos:
        draw_node(pos, idx)

    # Configurações finais do gráfico
    plt.axis('off')
    plt.tight_layout()
    plt.show()

def draw_edge(pos, source, target):
    x1, y1 = pos[source]
    x2, y2 = pos[target]

    # Calcular o vetor direção
    dx = x2 - x1
    dy = y2 - y1

    # Normalizar o vetor
    length = np.sqrt(dx**2 + dy**2)
    dx, dy = dx / length, dy / length

    # Raio do círculo
    radius = 0.05

    # Ajustar os pontos finais para que a seta termine na borda do círculo
    x2 = x2 - dx * radius
    y2 = y2 - dy * radius

    plt.plot([x1, x2], [y1, y2], color="#FFA933",
                linewidth=2, zorder=1, alpha=0.7)
    plt.arrow(
        x1, y1, x2 - x1, y2 - y1,
        head_width=0.03, head_length=0.03, fc='#FFA933', ec='#FFA933',
        length_includes_head=True, zorder=2, alpha=0.8
    )

def draw_node(pos, idx):
    x, y = pos[idx]
    circle = plt.Circle((x, y), 0.05, facecolor='#39C0E0',
                        edgecolor='#2980b9', zorder=1)
    plt.gca().add_patch(circle)
    plt.text(x, y, str(idx), fontsize=11, ha='center', va='center',
                color='white', fontweight='bold', zorder=2)     