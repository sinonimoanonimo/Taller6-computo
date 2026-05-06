#include <iostream>
#include <vector>
#include <mpi.h>
#include <cmath>

using namespace std;

void inicializar_datos(float* A, float* B, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            A[i * n + j] = (float)(1.5 * (1 + (5 * (i + j)) % 3) / (1 + (i + j) % 5));
            B[i * n + j] = (float)(2.0 * (1 + (3 * (i + j)) % 2) / (1 + (i + j) % 4));
        }
    }
}

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int n = (argc > 1) ? atoi(argv[1]) : 10;

    if (n % size != 0) {
        if (rank == 0) cout << "Error: N (" << n << ") debe ser divisible por P (" << size << ")" << endl;
        MPI_Finalize();
        return 1;
    }

    int filas_por_proceso = n / size;

    float* A = nullptr;
    float* B = new float[n * n]; 
    float* C_final = nullptr;
    
    float* sub_A = new float[filas_por_proceso * n];
    float* sub_C = new float[filas_por_proceso * n];

    if (rank == 0) {
        A = new float[n * n];
        C_final = new float[n * n];
        inicializar_datos(A, B, n);
    }

    MPI_Bcast(B, n * n, MPI_FLOAT, 0, MPI_COMM_WORLD);

    MPI_Scatter(A, filas_por_proceso * n, MPI_FLOAT, sub_A, filas_por_proceso * n, MPI_FLOAT, 0, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD); 
    double start_time = MPI_Wtime();

    for (int i = 0; i < filas_por_proceso; i++) {
        for (int j = 0; j < n; j++) {
            sub_C[i * n + j] = 0;
            for (int k = 0; k < n; k++) {
                sub_C[i * n + j] += sub_A[i * n + k] * B[k * n + j];
            }
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);
    double end_time = MPI_Wtime();
    // --- FIN DE MEDICIÓN CRÍTICA ---

    MPI_Gather(sub_C, filas_por_proceso * n, MPI_FLOAT, C_final, filas_por_proceso * n, MPI_FLOAT, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        double tiempo_paralelo = end_time - start_time;
        
        double start_sec = MPI_Wtime();
        float* comprueba = new float[n * n];
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                comprueba[i * n + j] = 0;
                for (int k = 0; k < n; k++) {
                    comprueba[i * n + j] += A[i * n + k] * B[k * n + j];
                }
            }
        }
        double tiempo_secuencial = MPI_Wtime() - start_sec;

        // Verificación de integridad
        bool ok = true;
        for (int i = 0; i < n * n; i++) {
            if (abs(C_final[i] - comprueba[i]) > 0.1) {
                ok = false;
                break;
            }
        }

        cout << size << "," << n << "," << tiempo_secuencial << "," << tiempo_paralelo << "," << (tiempo_secuencial / tiempo_paralelo) << endl;
        
        if (!ok) cerr << "ADVERTENCIA: Error en la validacion de datos." << endl;

        delete[] A;
        delete[] C_final;
        delete[] comprueba;
    }

    delete[] B;
    delete[] sub_A;
    delete[] sub_C;

    MPI_Finalize();
    return 0;
}
