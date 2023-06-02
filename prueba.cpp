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

    double ipcTotal = 0.0;

    for (int i = 1; i < productosPorMes.size(); i++) {
        double ipcMes = valorPonderado(productosPorMes[i]);
        double ipcBase = valorPonderado(productosPorMes[i-1]);

        double ipcRelativo = ((ipcMes / ipcBase)) * 100.0;
        ipcPorMes.push_back(ipcRelativo);
    }

    std::cout << "IPC por mes:" << std::endl;
    for (int i = 0; i < ipcPorMes.size(); i++) {
        std::cout << "Mes " << i + 2 << ": " << std::fixed << std::setprecision(4) << ipcPorMes[i] << " %" << std::endl;
    }

    return ipcPorMes;
}

void calcularDiferenciaIPC(const std::vector<double>& ipcPorMes) {
    if (ipcPorMes.size() < 11) {
        std::cout << "No se tienen suficientes meses para calcular la diferencia del IPC." << std::endl;
        return;
    }

    double ipcFebrero = std::abs(ipcPorMes[0]);
    double ipcDiciembre = std::abs(ipcPorMes[ipcPorMes.size() - 1]);

    double diferenciaIPC = ipcDiciembre - ipcFebrero;

    std::cout << "Diferencia del IPC de febrero a diciembre: " << std::fixed << std::setprecision(4) << diferenciaIPC << " %" << std::endl;
}

int main() {
    std::ifstream file("C:\\Users\\Jean\\Desktop\\Repositories\\c++\\prueba2.csv");
    std::vector<std::vector<Producto>> productosPorMes = procesarArchivo(file);
    productosPorMes = eliminarProductosNoPresentes(productosPorMes);

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

    std::vector<double> ipcPorMes = calcularIPC(productosPorMes);

    calcularDiferenciaIPC(ipcPorMes);

    return 0;
}