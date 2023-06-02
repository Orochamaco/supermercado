#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <vector>


struct Producto {
    std::string sku;
    std::string nombre;
    std::string monto;
    std::string descuento;
    int fecha;
    std::string estado;
    int cantidad = 0;
};

std::vector<Producto> vectorMenorLargo(const std::vector<std::vector<Producto>>& productosPorMes) {
    std::vector<Producto> vectorMenorLargo;

    if (productosPorMes.empty()) {
        std::cout << "El vector de productos por mes está vacío." << std::endl;
        return vectorMenorLargo;
    }

    vectorMenorLargo = productosPorMes[0];

    for (const auto& mes : productosPorMes) {
        std::cout << " largo mes: "<< mes.size() << std::endl;
        if (mes.size() < vectorMenorLargo.size()) {
            vectorMenorLargo = mes;
        }
    }

    return vectorMenorLargo;
}

int obtenerMes(const std::string& fechaUTC) {
    std::istringstream iss(fechaUTC); // Crear un flujo de entrada a partir de la cadena
    std::string token;

    // Leer los componentes de la fecha separados por '-'
    std::getline(iss, token, '-'); // Ignorar el año
    std::getline(iss, token, '-'); // Obtener el mes

    int mes = std::stoi(token);

    return mes;
}

bool cumpleCondicionEstado(const std::string& estado) {
    return estado == "AUTHORIZED" || estado == "FINALIZED";
}

std::vector<std::vector<Producto>> procesarArchivo(std::ifstream& file) {
    std::vector<std::vector<Producto>> productosPorMes(12); // Vector de 12 vectores para los meses

    if (!file) {
        std::cerr << "Error al abrir el archivo CSV" << std::endl;
        return productosPorMes;
    }

    std::string line;
    std::getline(file, line); // Descartar la primera línea

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string field;

        Producto producto;

        std::vector<std::string> campos = {"sku", "nombre", "monto", "descuento", "fecha", "estado"};
        std::vector<std::string> valores(6);

        for (int i = 0; i < campos.size(); i++) {
            std::getline(ss, field, ';');
            field = field.substr(1, field.length() - 2);
            valores[i] = field;
        }

        producto.sku = valores[0];
        producto.nombre = valores[1];
        producto.monto = valores[2];
        producto.descuento = valores[3];
        producto.fecha = obtenerMes(valores[4]);
        producto.estado = valores[5];

        if (cumpleCondicionEstado(producto.estado)) {
            auto& productosMes = productosPorMes[producto.fecha - 1];
            auto it = std::find_if(productosMes.begin(), productosMes.end(), [&](const Producto& p) {
                return p.sku == producto.sku;
            });

            if (it == productosMes.end()) {
                // Producto no encontrado, agregarlo al vector
                producto.cantidad = 1;
                productosMes.push_back(producto);
            } else {
                // Producto encontrado, incrementar la cantidad
                it->cantidad++;
            }
        }

    }

    file.close();

    return productosPorMes;
}

double anualIpc(const std::vector<double>& mesipc) {
    double finalipc = 0;
    for (int i = 0; i < 11; i++) {
        finalipc += mesipc[i];
    }
    return finalipc / 11;
}

double ipcUpDwn(const std::vector<Producto>& ProdMes) {
    double ipc = 0, mul;
    for (int i = 0; i < ProdMes.size(); i++) {
        mul = (std::stoi(ProdMes[i].monto) + std::stoi(ProdMes[i].descuento)) * ProdMes[i].cantidad;
        ipc += mul;
    }
    return ipc;
}

int main() {
    std::ifstream file("C:\\Users\\Jean\\Desktop\\Repositories\\c++\\prueba2.csv");
    std::vector<std::vector<Producto>> productosPorMes = procesarArchivo(file);

    std::vector<double> ipc(11);

    /* for (int i = 0; i < productosPorMes.size(); i++) {
        std::system("pause");
        std::system("cls");
        std::cout << "Mes " << (i + 1) << ":" << std::endl;

        const std::vector<Producto>& productosMes = productosPorMes[i];

        if (productosMes.empty()) {
            std::cout << "No hay productos en este mes." << std::endl;
        } else {
            for (const Producto& producto : productosMes) {
                std::cout << "SKU: " << producto.sku << std::endl;
                std::cout << "Nombre: " << producto.nombre << std::endl;
                std::cout << "Monto: " << producto.monto << std::endl;
                std::cout << "Descuento: " << producto.descuento << std::endl;
                std::cout << "Fecha: " << producto.fecha << std::endl;
                std::cout << "Estado: " << producto.estado << std::endl;
                std::cout << "Cantidad: " << producto.cantidad << std::endl;
                std::cout << std::endl;
            }
        }

        std::cout << std::endl;
    } */

    std::cout << vectorMenorLargo(productosPorMes).size() << std::endl;

    for (int i = 1; i < 11; i++) {
        ipc[i - 1] = ipcUpDwn(productosPorMes[i]) / ipcUpDwn(productosPorMes[i - 1]);
    }

    double ipcFinal = anualIpc(ipc);

     std::cout << "IPC anual: " << ipcFinal << std::endl;

    return 0;
}