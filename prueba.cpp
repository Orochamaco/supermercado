#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <vector>


struct Producto {
    double sku;
    std::string nombre;
    double monto;
    double descuento;
    std::string fecha;
    std::string estado;
};

bool cumpleCondicionEstado(const std::string& estado) {
    return estado == "AUTHORIZED" || estado == "FINALIZED";
}

std::vector<Producto> procesarArchivo(std::ifstream& file) {
    std::vector<Producto> productos; // Vector de productos

    int productosLeidos = 0;
    if (!file) {
        std::cerr << "Error al abrir el archivo CSV" << std::endl;
        return productos; // Devolver vector vacío en caso de error
    }

    std::string line;
    std::getline(file, line); // Descartar la primera línea

    while (std::getline(file, line) && productosLeidos < 200) {
        std::stringstream ss(line);
        std::string field;

        Producto producto; // Crear una instancia de la estructura Producto

        std::vector<std::string> campos = {"sku", "nombre", "monto", "descuento", "fecha", "estado"};
        std::vector<std::string> valores(6);

        for (int i = 0; i < campos.size(); i++) {
            std::getline(ss, field, ';');
            // Eliminar las comillas dobles del campo
            field = field.substr(1, field.length() - 2);
            valores[i] = field;
        }

        producto.sku = std::stod(valores[0]);
        producto.nombre = valores[1];
        producto.monto = std::stod(valores[2]);
        producto.descuento = std::stod(valores[3]);
        producto.fecha = valores[4];
        producto.estado = valores[5];

        // Verificar si cumple con la condición del estado
        if (cumpleCondicionEstado(producto.estado)) {
            // Agregar el producto al vector solo si cumple la condición
            productos.push_back(producto);
        }

        productosLeidos++;
    }

    file.close();

    return productos; // Devolver el vector de productos
}

int main() {
    std::ifstream file("C:\\Users\\Jean\\Desktop\\Repositories\\c++\\archivo.csv");
    std::vector<Producto> productos = procesarArchivo(file);

    // Mostrar el contenido del vector de productos
    for (const Producto& producto : productos) {
        std::cout << "sku: " << producto.sku << std::endl;
        std::cout << "nombre: " << producto.nombre << std::endl;
        std::cout << "monto: " << producto.monto << std::endl;
        std::cout << "descuento: " << producto.descuento << std::endl;
        std::cout << "fecha: " << producto.fecha << std::endl;
        std::cout << "estado: " << producto.estado << std::endl;
        std::cout << std::endl;
    }

    return 0;
}