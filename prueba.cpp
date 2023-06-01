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

    while (std::getline(file, line) && limit < 1000) {
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
            // Insertar el producto ordenadamente en el vector correspondiente al mes
            auto& productosMes = productosPorMes[producto.fecha - 1];
            auto insertPos = std::lower_bound(productosMes.begin(),
                                              productosMes.end(),
                                              producto,
                                              [](const Producto& a, const Producto& b) {
                                                  return a.sku < b.sku;
                                              });
            productosMes.insert(insertPos, producto);
        }

        limit++;

    }

    file.close();

    return productosPorMes;
}

int main() {
    std::ifstream file("C:\\Users\\Jean\\Desktop\\Repositories\\c++\\archivo.csv");
    std::vector<std::vector<Producto>> productosPorMes = procesarArchivo(file);

    // Verificar si hay productos en el primer mes
    if (!productosPorMes.empty() && !productosPorMes[0].empty()) {
        std::cout << "Productos del primer mes:" << std::endl;

        // Mostrar el contenido del primer mes
        for (const Producto& producto : productosPorMes[0]) {
            std::cout << "sku: " << producto.sku << std::endl;
        }

        std::cout << "Tamano del primer mes: " << productosPorMes[0].size() << " productos" << std::endl;
    } else {
        std::cout << "No hay productos en el primer mes." << std::endl;
    }

    return 0;
}