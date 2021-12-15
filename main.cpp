#include <iostream>
#include <pthread.h>
#include <unistd.h>

pthread_mutex_t mutex;
int pieces_in_boiler_num;
int hungry_cannibals_ctr;
int cook_wake_up_ctr = 0;
int amount_of_pieces;
pthread_t thread1;

void *func1(void *param) {
    //протокол входа в КС: закрыть двоичный семафор
    pthread_mutex_lock(&mutex);
    ++cook_wake_up_ctr;
    pieces_in_boiler_num = amount_of_pieces;
    std::cout << "Повар приготовил " << amount_of_pieces << " кусков мяса и пошёл спать\n";
    //протокол выхода из КС:
    //открыть двоичный семафор
    pthread_mutex_unlock(&mutex);
    return nullptr;
}

void *func(void *param) {
    int p = *(int *) param;
    while (true) {
        //протокол входа в КС: закрыть двоичный семафор
        pthread_mutex_lock(&mutex);
        //начало критической секции
        if (pieces_in_boiler_num != -1) {
            std::cout << "Каннибал " << p << " проголодался\n";
        }
        if (pieces_in_boiler_num > 0) {
            --pieces_in_boiler_num;
            --hungry_cannibals_ctr;
            std::cout << "Каннибал " << p << " съел кусок мяса\n";
            pthread_mutex_unlock(&mutex);
            return nullptr;
        } else if (pieces_in_boiler_num == 0) {
            std::cout << "Каннибал " << p << " пошёл будить повара\n";
            pthread_create(&thread1, nullptr,
                           func1, NULL);
            --pieces_in_boiler_num;
        }
        //конец критической секции
        //протокол выхода из КС:
        pthread_mutex_unlock(&mutex);
        //открыть двоичный семафор
    }
}

void *func2(void *param) {
    std::cout << "hi";
    return nullptr;
}

int main(int argc, char *argv[]) {
    int m, n;
    if (argc != 3) {
        std::cout << "Incorrect command line\n";
        return 1;
    }
    m = std::stoi(argv[1]);
    amount_of_pieces = m;
    n = std::stoi(argv[2]);
    if (m < 1 || n < 1) {
        std::cout << "Incorrect input, use positive values\n";
        return 1;
    }
    pieces_in_boiler_num = m;
    hungry_cannibals_ctr = n;
    pthread_mutex_init(&mutex, NULL);
    pthread_t *thread = new pthread_t[n];
    int *num = new int[n];
    for (int i = 0; i < n; i++) {
        num[i] = i; //номера каннибалов
    }
    for (int i = 0; i < n; i++) {
        pthread_create(&thread[i], nullptr,
                       func, (void *) (num + i));
    }
    for (int i = 0; i < n; i++) {
        pthread_join(thread[i], NULL);
    }
    std::cout << "Все каннибалы пообедали, повар просыпался " << cook_wake_up_ctr << " раз(а)";
    return 0;
}
