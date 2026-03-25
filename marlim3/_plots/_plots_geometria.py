import math
import plotly.graph_objects as go

def _plotar_geometria(tramo):

    # Configurações iniciais
    x_coords_prod = [0]  # Coordenada X inicial para dutos de produção
    y_coords_prod = [0]  # Coordenada Y inicial para dutos de produção
    tooltips_prod = []  # Tooltip para dutos de produção

    if tramo.dutosServico:
        x_coords_serv = []  # Coordenada X inicial para dutos de serviço
        y_coords_serv = []  # Coordenada Y inicial para dutos de serviço
        tooltips_serv = []  # Tooltip para dutos de serviço

    # Processando dutos de produção
    for duto in tramo.dutosProducao:
        if "xcoor" in duto and "ycoor" in duto:
            # Coordenadas fornecidas diretamente
            x_coords_prod.append(duto["xcoor"])
            y_coords_prod.append(duto["ycoor"])
        elif "angulo" in duto and "discretizacao" in duto:
            # Coordenadas calculadas a partir do ângulo e comprimento
            comprimento = sum(item["nCelulas"] * item["comprimento"] for item in duto["discretizacao"])
            angulo_rad = math.radians(duto["angulo"])  # Converter ângulo para radianos
            if math.isclose(duto["angulo"], math.pi / 2, abs_tol=1e-9):
                dx = 0
                dy = comprimento
            elif math.isclose(duto["angulo"], 3 * math.pi / 2, abs_tol=1e-9):
                dx = 0
                dy = -comprimento
            else:
                dx = comprimento * math.cos(duto["angulo"])
                dy = comprimento * math.sin(duto["angulo"])
            x_coords_prod.append(x_coords_prod[-1] + dx)
            y_coords_prod.append(y_coords_prod[-1] + dy)
        else:
            raise ValueError("Informações insuficientes para determinar as coordenadas do duto.")
        tooltips_prod.append(
            f"ID: {duto['id']}<br>"
            f"Rótulo: {duto.get('rotulo', 'N/A')}<br>"
            f"idCorte: {duto.get('idCorte', 'N/A')}<br>"
            f"Acoplamento Térmico: {duto.get('acoplamentoTermico', 'N/A')}"
        )

    # Coordenadas iniciais da plataforma (último ponto da produção)
    platform_x = x_coords_prod[-1]
    platform_y = y_coords_prod[-1]

    if tramo.dutosServico:
        # Coordenadas iniciais para os dutos de serviço 
        # (começam na plataforma)
        x_coords_serv.append(platform_x)
        y_coords_serv.append(platform_y)

    # Processando dutos de serviço
    if tramo.dutosServico:
        for duto in tramo.dutosServico:
            if "xcoor" in duto and "ycoor" in duto:
                # Coordenadas fornecidas diretamente
                x_coords_serv.append(duto["xcoor"])
                y_coords_serv.append(duto["ycoor"])
            elif "angulo" in duto and "discretizacao" in duto:
                # Coordenadas calculadas a partir do ângulo e comprimento 
                # (invertendo o sentido)
                comprimento = sum(item["nCelulas"] * item["comprimento"] for item in duto["discretizacao"])
                angulo_rad = math.radians(duto["angulo"]) 
                if math.isclose(duto["angulo"], math.pi / 2, abs_tol=1e-9):
                    dx = 0
                    dy = comprimento
                elif math.isclose(duto["angulo"], 3 * math.pi / 2, abs_tol=1e-9):
                    dx = 0
                    dy = -comprimento
                else:
                    dx = comprimento * math.cos(duto["angulo"])
                    dy = comprimento * math.sin(duto["angulo"])

                x_coords_serv.append(x_coords_serv[-1] - dx)
                y_coords_serv.append(y_coords_serv[-1] + dy)
            else:
                raise ValueError("Informações insuficientes para determinar as coordenadas do duto.")
            tooltips_serv.append(
                f"ID: {duto['id']}<br>"
                f"Rótulo: {duto.get('rotulo', 'N/A')}<br>"
                f"idCorte: {duto.get('idCorte', 'N/A')}<br>"
                f"Acoplamento Térmico: {duto.get('acoplamentoTermico', 'N/A')}"
            )

    # Criando o gráfico interativo
    fig = go.Figure()

    # Adicionando os dutos de produção
    fig.add_trace(go.Scatter(
        x=x_coords_prod,
        y=y_coords_prod,
        mode="lines+markers",
        hovertext=tooltips_prod,
        hoverinfo="text",
        line=dict(color="#39C0E0", width=3),
        marker=dict(size=8, color="#39C0E0")
    ))

    # Adicionando os dutos de serviço
    if tramo.dutosServico:
        fig.add_trace(go.Scatter(
            x=x_coords_serv,
            y=y_coords_serv,
            mode="lines+markers",
            hovertext=tooltips_serv,
            hoverinfo="text",
            line=dict(color="#FFA933", width=3),
            marker=dict(size=8, color="#FFA933")
        ))

    # Configurações de layout
    fig.update_layout(
        xaxis_title="x (m)",
        yaxis_title="y (m)",
        showlegend=False,
        xaxis=dict(scaleanchor="y"),  # Same scale for x and y axes
        yaxis=dict(scaleanchor="x")  # Same scale for x and y axes
    )

    # Mostrando o gráfico
    fig.show()
