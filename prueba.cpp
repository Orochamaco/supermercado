#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <vector>
#include <cmath>
#include <iomanip>
#include <unordered_set>


struct Producto {
    std::string sku;
    std::string nombre;
    std::string monto;
    std::string descuento;
    int fecha;
    std::string estado;
    int cantidad = 0;
};

std::vector<std::string> obtenerCampos(std::stringstream& ss) {
    std::vector<std::string> valores(6);
    std::string field;
    std::vector<std::string> campos = {"sku", "nombre", "monto", "descuento", "fecha", "estado"};

    for (int i = 0; i < campos.size(); i++) {
        std::getline(ss, field, ';');
        field = field.substr(1, field.length() - 2);
        valores[i] = field;
    }

    return valores;
}

bool cumpleCondicionEstado(const std::string& estado) {
    return estado == "AUTHORIZED" || estado == "FINALIZED";
}

std::vector<Producto>::iterator buscarProducto(std::vector<Producto>& productosMes, const std::string& sku) {
    return std::find_if(productosMes.begin(), productosMes.end(), [&](const Producto& p) {
        return p.sku == sku;
    });
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
        std::vector<std::string> valores = obtenerCampos(ss);

        Producto producto;
        producto.sku = valores[0];
        producto.nombre = valores[1];
        producto.monto = valores[2];
        producto.descuento = valores[3];
        producto.fecha = obtenerMes(valores[4]);
        producto.estado = valores[5];

        if (cumpleCondicionEstado(producto.estado)) {
            auto& productosMes = productosPorMes[producto.fecha - 1];
            auto it = buscarProducto(productosMes, producto.sku);

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

std::vector<std::vector<Producto>> eliminarProductosNoPresentes(std::vector<std::vector<Producto>> productosPorMes) {
    std::unordered_set<std::string> skus; // Conjunto para almacenar los SKUs presentes en enero y diciembre

    // Obtener los SKUs de enero
    for (const Producto& producto : productosPorMes[0]) {
        skus.insert(producto.sku);
    }

    // Obtener los SKUs de diciembre
    for (const Producto& producto : productosPorMes[11]) {
        skus.insert(producto.sku);
    }

    // Eliminar productos que no estén presentes en todos los meses
    for (std::vector<Producto>& productosMes : productosPorMes) {
        productosMes.erase(
            std::remove_if(
                productosMes.begin(),
                productosMes.end(),
                [&](const Producto& producto) {
                    return skus.find(producto.sku) == skus.end();
                }
            ),
            productosMes.end()
        );
    }

    return productosPorMes;
}

double valorPonderado(const std::vector<Producto>& ProdMes) {
    double valorp = 0, mul;
    for (int i = 0; i < ProdMes.size(); i++) {
        mul = (std::stoi(ProdMes[i].monto) - std::stoi(ProdMes[i].descuento)) * ProdMes[i].cantidad;
        valorp += mul;
    }
    return valorp;
}

std::vector<double> calcularIPC(const std::vector<std::vector<Producto>>& productosPorMes) {
    std::vector<double> ipcPorMes;

    if (productosPorMes.size() < 2) {
        std::cout << "No hay suficientes meses para calcular el IPC." << std::endl;
        return ipcPorMes;
    }

    double ipcTotal = 0.0;

    for (int i = 1; i < productosPorMes.size(); i++) {
        double ipcMes = valorPonderado(productosPorMes[i]);
        double ipcBase = valorPonderado(productosPorMes[i-1]);

        double ipcRelativo = (((ipcMes / ipcBase)) * 100.0)-100;
        ipcPorMes.push_back(ipcRelativo);
    }

    return ipcPorMes;
}

void inflacionAcumulada(const std::vector<double>& ipcPorMes) {
    std::vector<double> inflacionAcumulada(ipcPorMes.size());

    inflacionAcumulada[0] = ipcPorMes[0]; // La primera variación acumulada es igual a la primera variación mensual

    for (int i = 1; i < ipcPorMes.size(); i++) {
        inflacionAcumulada[i] = (1 + inflacionAcumulada[i - 1] / 100.0) * (1 + ipcPorMes[i] / 100.0) - 1;
    }

    std::cout << "Inflacion acumulada por mes:" << std::endl;
    for (int i = 0; i < inflacionAcumulada.size(); i++) {
        std::cout << "Mes " << i + 2 << ": " << std::fixed << std::setprecision(4) << inflacionAcumulada[i] << " %" << std::endl;
    }
}

int main() {
    std::ifstream file("C:\\Users\\Jean\\Desktop\\Repositories\\c++\\prueba2.csv");
    std::vector<std::vector<Producto>> productosPorMes = procesarArchivo(file);
    productosPorMes = eliminarProductosNoPresentes(productosPorMes);

    std::vector<double> ipcPorMes = calcularIPC(productosPorMes);

    inflacionAcumulada(ipcPorMes);


    return 0;
}