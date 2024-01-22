#include <QThread>
#include <QMutex>
#include <QDebug>
#include <array>
#include <memory>
#include <cstdlib>
#include <ctime>
#include <QElapsedTimer>

const int NumFilosofos = 5;

class Palillo {
public:
    bool intentarCoger() {
        return mutex.tryLock();
    }

    void soltar() {
        mutex.unlock();
    }

private:
    QMutex mutex;
};

class Filosofo : public QThread {
public:
    Filosofo(int num, Palillo *izquierdo, Palillo *derecho)
        : num(num), izquierdo(izquierdo), derecho(derecho) {}

protected:
    void run() override {
        for (int i = 0; i < 100; ++i) {
            pensar();
            intentarComer();
        }
    }

private:
    void pensar() {
        qDebug() << "Filósofo" << num << "está pensando.";
        QThread::msleep(rand() % 100 + 50);  // Espera aleatoria entre 50 y 150 milisegundos
    }

    void intentarComer() {
        QElapsedTimer timer;
        while (true) {
            timer.start();
            while (!izquierdo->intentarCoger()) {
                if (timer.elapsed() > 1000) { // Límite de 1 segundo para intentar
                    return; // Abandonar intento después de 1 segundo
                }
                QThread::msleep(rand() % 5 + 1);
            }

            if (!derecho->intentarCoger()) {
                izquierdo->soltar();
                if (timer.elapsed() > 1000) {
                    return;
                }
                QThread::msleep(rand() % 5 + 1);
                continue;
            }

            qDebug() << "Filósofo" << num << "está comiendo.";
            QThread::msleep(rand() % 100 + 50);  // Simula la comida
            derecho->soltar();
            izquierdo->soltar();
            break;
        }
    }

    int num;
    Palillo *izquierdo;
    Palillo *derecho;
};

int main() {
    srand(time(nullptr)); // Inicialización del generador de números aleatorios
    std::array<Palillo, NumFilosofos> palillos;
    std::array<std::unique_ptr<Filosofo>, NumFilosofos> filosofos;

    for (int i = 0; i < NumFilosofos; ++i) {
        filosofos[i] = std::make_unique<Filosofo>(i, &palillos[i], &palillos[(i + 1) % NumFilosofos]);
    }

    for (auto &filosofo : filosofos) {
        filosofo->start();
    }

    for (auto &filosofo : filosofos) {
        filosofo->wait();
    }

    qDebug() << "Finalizado";
    return 0;
}
