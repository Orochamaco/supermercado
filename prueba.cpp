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
};

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

    long int limit = 0;

    while (std::getline(file, line) && limit < 4000000) {
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
                productosMes.push_back(producto);
            }
        }

        limit++;

    }

    file.close();

    return productosPorMes;
}

int main() {
    std::ifstream file("C:\\Users\\Jean\\Desktop\\Repositories\\c++\\archivo.csv");
    std::vector<std::vector<Producto>> productosPorMes = procesarArchivo(file);

    // Mostrar los vectores de SKU y su tamaño por cada mes
    for (int i = 0; i < productosPorMes.size(); i++) {
        if (!productosPorMes[i].empty()) {
            std::cout << "mes " << i + 1 << ": ";

            /* // Mostrar los skus del mes actual
            for (const Producto& producto : productosPorMes[i]) {
                std::cout << producto.sku << " ";
            } */

            std::cout << "Tamaño del mes: " << productosPorMes[i].size() << " productos" << std::endl;
        } else {
            std::cout << "No hay productos en el mes " << i + 1 << "." << std::endl;
        }
    }

    return 0;
}