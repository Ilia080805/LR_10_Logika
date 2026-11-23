#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <locale.h>
#include <queue>


int** create_matrix(int n) {
    if (n <= 0) return NULL;
    int** matrix = (int**)malloc((size_t)n * sizeof(int*));
    if (!matrix) return NULL;

    for (int i = 0; i < n; i++) {
        matrix[i] = (int*)malloc((size_t)n * sizeof(int));
    }
    return matrix;
}

void free_matrix(int** matrix, int n) {
    if (matrix != NULL) {
        for (int i = 0; i < n; i++) free(matrix[i]);
        free(matrix);
    }
}

void generate_graph(int** matrix, int n, int is_directed, int is_weighted) {
    if (is_directed == 0) {
        for (int i = 0; i < n; i++) {
            matrix[i][i] = 0;
            for (int j = i + 1; j < n; j++) {
                int val = 0;
                if (rand() % 2 == 1) {
                    val = (is_weighted == 1) ? (1 + rand() % 5) : 1;
                }
                matrix[i][j] = val;
                matrix[j][i] = val;
            }
        }
    }
    else {
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                if (i == j) {
                    matrix[i][j] = 0;
                }
                else if (rand() % 2 == 1) {
                    matrix[i][j] = (is_weighted == 1) ? (1 + rand() % 5) : 1;
                }
                else {
                    matrix[i][j] = 0;
                }
            }
        }
    }
}

void print_matrix(int** matrix, int n, int is_directed, int is_weighted) {
    printf("\nМатрица смежности G (%s, %s):\n    ",
        is_directed ? "ориентированный" : "неориентированный",
        is_weighted ? "взвешенный" : "невзвешенный");

    for (int j = 0; j < n; j++) printf("%3d", j + 1);
    printf("\n----");
    for (int j = 0; j < n; j++) printf("---");
    printf("\n");

    for (int i = 0; i < n; i++) {
        printf("%3d|", i + 1);
        for (int j = 0; j < n; j++) {
            printf("%3d", matrix[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

void reset_dist(int* DIST, int n, int start_v) {
    for (int i = 0; i < n; i++) {
        DIST[i] = -1;
    }
    DIST[start_v] = 0;
}

void print_dist(int* DIST, int n, int start_v) {
    printf("\nРасстояния от вершины %d:\n", start_v + 1);
    for (int i = 0; i < n; i++) {
        if (DIST[i] == -1) {
            printf("  До вершины %2d: нет пути (-1)\n", i + 1);
        }
        else {
            printf("  До вершины %2d: %d\n", i + 1, DIST[i]);
        }
    }
    printf("\n");
}

void along_the_width(int start_v, int n, int** matrix, int* DIST) {
    std::queue<int> q;
    q.push(start_v);
    DIST[start_v] = 0;

    while (!q.empty()) {
        int v = q.front();
        q.pop();

        for (int i = 0; i < n; i++) {
            if (matrix[v][i] > 0 && (DIST[i] == -1 || DIST[i] > DIST[v] + matrix[v][i])) {
                DIST[i] = DIST[v] + matrix[v][i];
                q.push(i);
            }
        }
    }
}

void all_pairs_distances(int n, int** matrix, int** dist_matrix) {
    for (int i = 0; i < n; i++) {
        reset_dist(dist_matrix[i], n, i);
        along_the_width(i, n, matrix, dist_matrix[i]);
    }
}

void print_dist_matrix(int** dist_matrix, int n) {
    printf("\nМатрица расстояний:\n    ");
    for (int j = 0; j < n; j++) printf("%3d", j + 1);
    printf("\n----");
    for (int j = 0; j < n; j++) printf("---");
    printf("\n");

    for (int i = 0; i < n; i++) {
        printf("%3d|", i + 1);
        for (int j = 0; j < n; j++) {
            if (dist_matrix[i][j] == -1) {
                printf("  -");
            }
            else {
                printf("%3d", dist_matrix[i][j]);
            }
        }
        printf("\n");
    }
    printf("\n");
}

void analyze_graph(int** dist_matrix, int n) {
    int* ecc = (int*)malloc((size_t)n * sizeof(int));
    if (!ecc) return;

    for (int i = 0; i < n; i++) {
        ecc[i] = 0;
        for (int j = 0; j < n; j++) {
            if (dist_matrix[i][j] > ecc[i]) {
                ecc[i] = dist_matrix[i][j];
            }
        }
    }

    printf("Эксцентриситеты вершин:\n");
    for (int i = 0; i < n; i++) {
        printf("  Вершина %2d: %d\n", i + 1, ecc[i]);
    }

    int diameter = 0;
    int radius = -1;
    for (int i = 0; i < n; i++) {
        if (ecc[i] > diameter) {
            diameter = ecc[i];
        }
        if (ecc[i] > 0 && (radius == -1 || ecc[i] < radius)) {
            radius = ecc[i];
        }
    }

    if (radius == -1) {
        printf("\nГраф полностью пустой (нет ни одного ребра).\n\n");
        free(ecc);
        return;
    }

    printf("\nРадиус графа r(G)  : %d\n", radius);
    printf("Диаметр графа D(G) : %d\n", diameter);

    printf("Центральные вершины  : ");
    for (int i = 0; i < n; i++) {
        if (ecc[i] == radius) printf("%d ", i + 1);
    }
    printf("\n");

    printf("Периферийные вершины : ");
    for (int i = 0; i < n; i++) {
        if (ecc[i] == diameter) printf("%d ", i + 1);
    }
    printf("\n\n");

    free(ecc);
}

void along_the_width_manual(int start_v, int n, int** matrix, int* DIST) {
    std::queue<int> q;
    q.push(start_v);
    DIST[start_v] = 0;

    while (!q.empty()) {
        int v = q.front();
        q.pop();

        for (int i = 0; i < n; i++) {
            if (matrix[v][i] > 0 && DIST[i] == -1) {
                q.push(i);
                DIST[i] = DIST[v] + matrix[v][i];
            }
        }
    }
}


int main(int argc, char* argv[]) {
    setlocale(LC_ALL, "Russian");
    srand((unsigned int)time(NULL));

    int is_weighted = 1;
    int is_directed = 0; 

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-w") == 0 && i + 1 < argc) {
            is_weighted = (atoi(argv[i + 1]) != 0) ? 1 : 0;
        }
        if (strcmp(argv[i], "-d") == 0 && i + 1 < argc) {
            is_directed = (atoi(argv[i + 1]) != 0) ? 1 : 0;
        }
    }

    int n = 6;
    int start_node = 0;

    printf("============================================================\n");
    printf("   Лабораторная работа №10: Поиск расстояний во взвешенном графе\n");
    printf("   Параметры CLI: -w (0/1 взвешенный), -d (0/1 ориентированный)\n");
    printf("============================================================\n");
    printf("Введите количество вершин графа (N): ");
    if (scanf("%d", &n) != 1 || n < 1) n = 6;

    int** matrix = create_matrix(n);
    int* DIST = (int*)malloc((size_t)n * sizeof(int));
    generate_graph(matrix, n, is_directed, is_weighted);
    print_matrix(matrix, n, is_directed, is_weighted);

    int choice = -1;
    while (choice != 0) {
        printf("----------------- МЕНЮ -----------------\n");
        printf("Режим: %s | %s | Старт: %d\n",
            is_directed ? "Ориент." : "Неориент.",
            is_weighted ? "Взвеш." : "Невзвеш.",
            start_node + 1);
        printf("1. Найти расстояния от стартовой вершины (BFSD)\n");
        printf("2. Матрица всех кратчайших расстояний + Радиус, Диаметр, Центр, Периферия\n");
        printf("3. Выбрать стартовую вершину\n");
        printf("4. Переключить: Ориентированный / Неориентированный\n");
        printf("5. Переключить: Взвешенный / Невзвешенный\n");
        printf("6. Сгенерировать новый граф\n");
        printf("7. Показать матрицу смежности\n");
        printf("8. Матрица расстояний по методичке (первый проход BFS)\n");
        printf("0. Выход\n");
        printf("Выберите пункт: ");

        if (scanf("%d", &choice) != 1) break;

        switch (choice) {
        case 1:
            reset_dist(DIST, n, start_node);
            along_the_width(start_node, n, matrix, DIST);
            print_dist(DIST, n, start_node);
            break;

        case 2: {
            int** dist_matrix = create_matrix(n);
            if (dist_matrix != NULL) {


                all_pairs_distances(n, matrix, dist_matrix);
                print_dist_matrix(dist_matrix, n);
                analyze_graph(dist_matrix, n);
                free_matrix(dist_matrix, n);
            }
            break;
        }

        case 3: {
            int input_v;
            printf("Введите номер стартовой вершины (от 1 до %d): ", n);
            if (scanf("%d", &input_v) == 1 && input_v >= 1 && input_v <= n) {
                start_node = input_v - 1;
                printf("Стартовая вершина установлена: %d\n\n", start_node + 1);
            }
            else {
                printf("Некорректный номер!\n\n");
            }
            break;
        }

        case 4:
            is_directed = (is_directed == 0) ? 1 : 0;
            generate_graph(matrix, n, is_directed, is_weighted);
            print_matrix(matrix, n, is_directed, is_weighted);
            break;

        case 5:
            is_weighted = (is_weighted == 0) ? 1 : 0;
            generate_graph(matrix, n, is_directed, is_weighted);
            print_matrix(matrix, n, is_directed, is_weighted);
            break;

        case 6:
            generate_graph(matrix, n, is_directed, is_weighted);
            print_matrix(matrix, n, is_directed, is_weighted);
            break;

        case 7:
            print_matrix(matrix, n, is_directed, is_weighted);
            break;

        case 8: {
            int** dist_matrix = create_matrix(n);
            if (dist_matrix != NULL) {
                for (int i = 0; i < n; i++) {
                    reset_dist(dist_matrix[i], n, i);
                    along_the_width_manual(i, n, matrix, dist_matrix[i]);
                }
                printf("\n[По методичке: первый проход BFS без сокращения путей]");
                print_dist_matrix(dist_matrix, n);
                free_matrix(dist_matrix, n);
            }
            break;
        }

        case 0:
            printf("\nВыход из программы.\n");
            break;

        default:
            printf("\nНеверный пункт меню!\n\n");
            break;
        }
    }

    free_matrix(matrix, n);
    free(DIST);
    return 0;
}