#!/usr/bin/env python3
"""Calcula las metricas de rendimiento de EcoAbono a partir de un CSV.

Uso:
    python scripts/calcular_metricas.py
    python scripts/calcular_metricas.py --archivo data/pruebas_rendimiento.csv
"""
import argparse
import sys

import pandas as pd

COLUMNAS = ["masa_entrada_kg", "masa_no_procesada_kg", "tiempo_ciclo_min", "energia_Wh"]


def main():
    parser = argparse.ArgumentParser(description="Metricas de rendimiento EcoAbono")
    parser.add_argument("--archivo", default="data/pruebas_rendimiento.csv")
    args = parser.parse_args()

    try:
        df = pd.read_csv(args.archivo)
    except FileNotFoundError:
        sys.exit(f"No se encontro el archivo: {args.archivo}")

    faltantes = [c for c in COLUMNAS if c not in df.columns]
    if faltantes:
        sys.exit(f"Faltan columnas en el CSV: {faltantes}")

    # Ignora filas de plantilla sin datos reales
    df = df[(df["masa_entrada_kg"] > 0) & (df["tiempo_ciclo_min"] > 0)].copy()
    if df.empty:
        sys.exit("No hay pruebas con datos validos (masa y tiempo > 0). Completa el CSV.")

    df["masa_procesada_kg"] = df["masa_entrada_kg"] - df["masa_no_procesada_kg"]
    df["aprovechamiento_pct"] = df["masa_procesada_kg"] / df["masa_entrada_kg"] * 100
    df["no_procesado_pct"] = df["masa_no_procesada_kg"] / df["masa_entrada_kg"] * 100
    df["rendimiento_kg_h"] = df["masa_procesada_kg"] / (df["tiempo_ciclo_min"] / 60)

    cols = ["aprovechamiento_pct", "no_procesado_pct", "rendimiento_kg_h",
            "tiempo_ciclo_min", "energia_Wh"]
    resumen = df[cols].agg(["mean", "std", "min", "max"]).round(2)

    print(f"\nRESUMEN DE {len(df)} PRUEBA(S)")
    print(resumen.to_string())
    print(f"\nTotal procesado: {df['masa_procesada_kg'].sum():.2f} kg")

    salida = "data/resumen_rendimiento.csv"
    resumen.to_csv(salida)
    print(f"Resumen guardado en {salida}")


if __name__ == "__main__":
    main()
