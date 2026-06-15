# SIGLOR: Sistema Inteligente de Gestión Logística y Optimización de Rutas

<p align="center">

  ![Logo](./docs/assets/logo.png)
  
  <img src="https://img.shields.io/badge/C%2B%2B-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white" alt="C++">
  <img src="https://img.shields.io/badge/Raylib-📍_Prov._Buenos_Aires-orange?style=for-the-badge" alt="Interfaz">
  <img src="https://img.shields.io/badge/Estado-En_Desarrollo-green?style=for-the-badge" alt="Estado">
</p>

---

## Sobre el Proyecto

**SIGLOR** es una pieza de software diseñada para resolver uno de los desafíos más críticos de la cadena de suministro: la **eficiencia en la distribución y el transporte terrestre**. 

Tomando como escenario la infraestructura vial de la **Provincia de Buenos Aires**, el sistema modela las rutas y centros logísticos clave mediante **Teoría de Grafos**. A través de algoritmos de optimización matemática, SIGLOR calcula en milisegundos el camino más corto entre múltiples puntos de origen y destino.

> **Proyecto Final** > Asignatura: *Estructuras de Datos y Algoritmos II* > **Universidad Provincial de Ezeiza (UPE)** — Tecnicatura Universitaria en Desarrollo de Software.

---

## Integrantes (Equipo de Desarrollo)

Desarrollado con esfuerzo por el grupo de proyecto:

* **Bravo, Lautaro** — [GitHub](https://github.com/LBravoDev)
* **Sanchez, Lisandro** — [GitHub](https://github.com/LichaSDev)
* **Gonzalez, Joaquin** — [GitHub](https://github.com/JoaquinGDev)
* **Moran, Alex** — [GitHub](https://github.com/GSAniki)

---

## Características Principales

El sistema está diseñado bajo un enfoque modular, ofreciendo herramientas de control logístico en tiempo real:

* **Gestión de Nodos (Ciudades):** Capacidad para trabajar con un núcleo de ciudades base y habilitar/deshabilitar nodos opcionales según demanda.
* **Optimización de Rutas (Dijkstra):** Implementación del algoritmo de Dijkstra sobre matrices de adyacencia para el cálculo de distancias mínimas absolutas.
* **Gestión de Contingencias:** Permite simular bloqueos de rutas o cierres de nodos. El sistema aísla el nodo desactivado mediante banderas lógicas e instantáneamente fuerza un recálculo dinámico de trayectorias alternativas sin alterar la persistencia de la red física.
* **Registro Histórico:** Almacenamiento y auditoría de las consultas de rutas realizadas.
* **Interfaz Gráfica Dinámica:** Renderizado interactivo y mapeo visual en tiempo real potenciado por **Raylib**, permitiendo al operador ver el grafo y las rutas óptimas de forma fluida.

---

## Documentación Completa

A continuación se facilita la documentación del proyecto.

* [Manual de Usuario](./docs/Manual.md): Manual para la instalación y utilización del software.
* [Nodos y Conexiones](./docs/Nodos-y-Conexiones.md): Documentación de la definición de nodos y rutas.
* [Algoritmo de Dijkstra](./docs/Algoritmo-de-Dijkstra.md): Explicación del algoritmo utilizado y su implementación en el programa.
* [Avances](./docs/img_funcionamiento.md): Avances varios del proyecto.