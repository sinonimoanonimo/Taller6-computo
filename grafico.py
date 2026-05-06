import matplotlib.pyplot as plt
import csv

nombre_imagen = "GRAFICO_TALLER_FINAL.png"

tamano, t_sec, t_par = [], [], []

print("Procesando datos...")

try:
    with open("resultados.csv", "r") as f:
        reader = csv.reader(f)
        next(reader)  
        for row in reader:
            if len(row) >= 4:

                tamano.append(int(row[1].strip()))
                t_sec.append(float(row[2].strip()))
                t_par.append(float(row[3].strip()))

    if not tamano:
        print("Error: No se encontraron datos válidos en resultados.csv")
        exit()

    plt.figure(figsize=(10, 6))
    
    plt.plot(tamano, t_sec, marker='o', label='Secuencial', color='black', linewidth=2)
    plt.plot(tamano, t_par, marker='s', label='Paralelo (4 Procesos)', color='red', linewidth=2)
    
    plt.xscale('log')
    plt.yscale('log')
    
    plt.xlabel("Tamaño de Matriz (N x N)", fontsize=12)
    plt.ylabel("Tiempo de Ejecución (Segundos)", fontsize=12)
    plt.title("Análisis de Rendimiento MPI: Taller 6", fontsize=14)
    plt.legend()
    
    plt.grid(True, which="both", ls="--", alpha=0.5)
    
    plt.tight_layout()
    plt.savefig(nombre_imagen, dpi=300)
    print(f"¡ÉXITO! El gráfico se guardó como '{nombre_imagen}' en tu escritorio.")

except Exception as e:
    print(f"Error crítico: {e}")