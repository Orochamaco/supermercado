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
            productosMes.push_back(producto);
        }

    }

    file.close();

    return productosPorMes;
}

int main() {
    std::ifstream file("C:\\Users\\Jean\\Desktop\\Repositories\\c++\\prueba2.csv");
    std::vector<std::vector<Producto>> productosPorMes = procesarArchivo(file);

    // Mostrar los vectores de SKU y su tamaño por cada mes
    for (int i = 0; i < productosPorMes.size(); i++) {
        if (!productosPorMes[i].empty()) {
            std::cout << "mes " << i + 1 << ": ";

            /* // Mostrar los skus del mes actual
            for (const Producto& producto : productosPorMes[i]) {
                std::cout << producto.cantidad << " ";
            } */

            std::cout << "Tamaño del mes: " << productosPorMes[i].size() << " productos" << std::endl;
        } else {
            std::cout << "No hay productos en el mes " << i + 1 << "." << std::endl;
        }
    }

    std::cout << vectorMenorLargo(productosPorMes).size() << std::endl;

    return 0;
}