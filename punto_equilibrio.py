#!/usr/bin/env python3
"""Calcula ahorro mensual neto y punto de equilibrio de Fertilízate.

Ejemplo (valores ilustrativos: usa los tuyos):
    python scripts/punto_equilibrio.py --inversion 300000 --kg-mes 20 --precio-kg 4000 --costo-mes 5000
"""
import argparse


def main():
    p = argparse.ArgumentParser(description="Punto de equilibrio Fertilízate")
    p.add_argument("--inversion", type=float, required=True, help="Costo total de construccion")
    p.add_argument("--kg-mes", type=float, required=True, help="kg de abono producidos por mes")
    p.add_argument("--precio-kg", type=float, required=True, help="Precio del abono comercial equivalente por kg")
    p.add_argument("--costo-mes", type=float, required=True, help="Costo operativo mensual (energia + mantenimiento)")
    a = p.parse_args()

    ahorro_bruto = a.kg_mes * a.precio_kg
    ahorro_neto = ahorro_bruto - a.costo_mes

    print(f"Ahorro mensual bruto : $ {ahorro_bruto:,.0f}")
    print(f"Costo operativo      : $ {a.costo_mes:,.0f}")
    print(f"Ahorro mensual neto  : $ {ahorro_neto:,.0f}")

    if ahorro_neto > 0:
        meses = a.inversion / ahorro_neto
        print(f"Punto de equilibrio  : {meses:.1f} meses (~{meses / 12:.1f} anios)")
    else:
        print("Con estos valores no hay ahorro neto. Revisa los datos.")


if __name__ == "__main__":
    main()
