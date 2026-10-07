#!/usr/bin/env python3
"""Modelo educativo simplificado de reduccion de masa durante el proceso.

La tasa de perdida diaria es un SUPUESTO: reemplazala con tus pesajes reales
y compara simulacion vs. prototipo en tu informe.

Uso:
    python simulacion/modelo_proceso/simulacion_basica.py --masa 5 --tasa 0.04 --dias 30
"""
import argparse
from pathlib import Path

import matplotlib
matplotlib.use("Agg")  # funciona sin pantalla
import matplotlib.pyplot as plt


def simular(masa_inicial, tasa, dias):
    masa = [masa_inicial]
    for _ in range(dias):
        masa.append(masa[-1] * (1 - tasa))
    return masa


def main():
    p = argparse.ArgumentParser()
    p.add_argument("--masa", type=float, default=5.0, help="kg iniciales")
    p.add_argument("--tasa", type=float, default=0.04, help="fraccion de masa perdida por dia (supuesto)")
    p.add_argument("--dias", type=int, default=30)
    a = p.parse_args()

    masa = simular(a.masa, a.tasa, a.dias)
    reduccion = (1 - masa[-1] / masa[0]) * 100
    print(f"Masa final estimada: {masa[-1]:.2f} kg ({reduccion:.1f} % de reduccion)")

    plt.figure(figsize=(7, 4))
    plt.plot(range(a.dias + 1), masa, marker="o", markersize=3)
    plt.title("Reduccion de masa estimada durante el proceso (modelo)")
    plt.xlabel("Dias")
    plt.ylabel("Masa (kg)")
    plt.grid(True)
    plt.tight_layout()

    salida = Path(__file__).parent / "reduccion_masa.png"
    plt.savefig(salida, dpi=150)
    print(f"Grafica guardada en {salida}")


if __name__ == "__main__":
    main()
