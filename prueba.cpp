#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <vector>


struct Producto {
    std::string sku;
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
    std::vector<std::vector<Producto>> productosPorSKU; // Vector de vectores para los SKU

    if (!file) {
        std::cerr << "Error al abrir el archivo CSV" << std::endl;
        return productosPorSKU;
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
        producto.monto = valores[2];
        producto.descuento = valores[3];
        producto.fecha = obtenerMes(valores[4]);
        producto.estado = valores[5];

        if (cumpleCondicionEstado(producto.estado)) {
            // Buscar el SKU en el vector de vectores
            auto it = std::find_if(productosPorSKU.begin(), productosPorSKU.end(), [&](const std::vector<Producto>& skuVector) {
                return !skuVector.empty() && skuVector[0].sku == producto.sku;
            });

            if (it != productosPorSKU.end()) {
                // Agregar el producto al vector del SKU encontrado
                it->push_back(producto);
            } else {
                // Crear un nuevo vector para el SKU y agregar el producto
                productosPorSKU.push_back({producto});
            }
        }

        limit++;
    }

    file.close();

    return productosPorSKU;
}

int main() {
    std::ifstream file("C:\\Users\\Jean\\Desktop\\Repositories\\c++\\archivo.csv");
    std::vector<std::vector<Producto>> productosPorSKU = procesarArchivo(file);

    // Mostrar los vectores de SKU y su tamaño
    for (const std::vector<Producto>& skuVector : productosPorSKU) {
        if (skuVector.empty()) {
            std::cout << "No hay productos para este SKU." << std::endl;
        } else {
            std::cout << "SKU: " << skuVector[0].sku << " Tamaño del vector: " << skuVector.size() << std::endl;
        }
    }

    return 0;
}

