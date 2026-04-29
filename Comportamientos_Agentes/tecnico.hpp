#ifndef COMPORTAMIENTOTECNICO_H
#define COMPORTAMIENTOTECNICO_H

#include <chrono>
#include <time.h>
#include <thread>
#include <list>
#include <map>


#include "comportamientos/comportamiento.hpp"

struct EstadoT {
  ubicacion site;
  bool zapatillas;
  bool operator==(const EstadoT &st) const{
    return site == st.site and zapatillas == st.zapatillas;
  }
   bool operator<(const EstadoT &st) const {
    if (site.f < st.site.f) return true;
    else if (site.f == st.site.f and site.c < st.site.c) return true;
    else if (site.f == st.site.f and site.c == st.site.c and site.brujula < st.site.brujula) return true;
    else if (site.f == st.site.f and site.c == st.site.c and site.brujula == st.site.brujula and zapatillas < st.zapatillas) return true;
    else return false;
  }
};
struct NodoT{
 EstadoT estado;
 list<Action> secuencia;
 int g; //coste real acumulado de energia
 int h; //Heurística estimada a la meta
 int f; // = g+h

 // priority_queue saca el MAYOR por defecto, invertir la lógica aquí para que devuelva el de MENOR 'f'
  bool operator<(const NodoT &node) const {
    return f > node.f; 
  }
  
 bool operator==(const NodoT &node) const{
  return estado == node.estado;
 }
/*  
 bool operator<(const NodoT &node) const{
  if (estado.site.f < node.estado.site.f) return true;
  else if (estado.site.f == node.estado.site.f and estado.site.c < node.estado.site.c) return true;
  else if (estado.site.f == node.estado.site.f and estado.site.c == node.estado.site.c and estado.site.brujula <
node.estado.site.brujula) return true;
  else if (estado.site.f == node.estado.site.f and estado.site.c == node.estado.site.c and estado.site.brujula ==
node.estado.site.brujula and estado.zapatillas < node.estado.zapatillas) return true;
 else return false;
 }
 */
};
class ComportamientoTecnico : public Comportamiento {
public:
  // =========================================================================
  // CONSTRUCTORES
  // =========================================================================
  /**
   * @brief Constructor para niveles 0, 1 y 6 (sin mapa completo)
   * @param size Tamaño del mapa (si es 0, se inicializa más tarde)
   */

  ComportamientoTecnico(unsigned int size = 0) : Comportamiento(size) {
    tiene_zapatillas = false;
    last_action = IDLE;
    contador_giros = 0;
    giro_preferido = TURN_SL;
    last_f = -1;
    last_c = -1;
    en_bloqueo = false;
    en_bloqueo_U = false;
    walk_left = true;
    giro_defecto = false;
    cont_walk = 0;
  }

  /**
   * @brief Constructor para niveles 2, 3, 4 y 5 (con mapa completo conocido)
   * @param mapaR Mapa de terreno conocido
   * @param mapaC Mapa de cotas conocido
   */
  ComportamientoTecnico(std::vector<std::vector<unsigned char>> mapaR,
                       std::vector<std::vector<unsigned char>> mapaC):
                       Comportamiento(mapaR, mapaC) {
    // Inicializar Variables de Estado
    hayPlan = false;  
    tiene_zapatillas = false;
  }

  ComportamientoTecnico(const ComportamientoTecnico &comport): Comportamiento(comport) {}
  ~ComportamientoTecnico() {}

  Action think(Sensores sensores);

  ComportamientoTecnico *clone() {
    return new ComportamientoTecnico(*this);
  }

  // =========================================================================
  // ÁREA DE IMPLEMENTACIÓN DEL ESTUDIANTE
  // =========================================================================

  bool es_camino1(unsigned char c) const;
  int veoCasillaInteresanteT0(char i, char c, char d, bool tiene_zapatillas);
  int veoCasillaInteresanteT1(char i, char c, char d, bool tiene_zapatillas);
  list<Action> B_Anchura(const EstadoT &inicio, const EstadoT &final,
                                              const vector<vector<unsigned char>> &terreno,
                                              const vector<vector<unsigned char>> &altura);

  list<Action> B_Anchura_V2(const EstadoT &inicio, const EstadoT &final,
                                                 const vector<vector<unsigned char>> &terreno,
                                                 const vector<vector<unsigned char>> &altura);
  
  char viablePorAlturaT(char casilla, int dif);

/**
 * @brief Comportamiento del técnico para el Nivel 0.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
  Action ComportamientoTecnicoNivel_0(Sensores sensores);

/**
 * @brief Comportamiento del técnico para el Nivel 1.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
  Action ComportamientoTecnicoNivel_1(Sensores sensores);

/**
 * @brief Comportamiento del técnico para el Nivel 2.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
  Action ComportamientoTecnicoNivel_2(Sensores sensores);

/**
 * @brief Comportamiento del técnico para el Nivel 3.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
  Action ComportamientoTecnicoNivel_3(Sensores sensores);

/**
 * @brief Comportamiento del técnico para el Nivel 4.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
  Action ComportamientoTecnicoNivel_4(Sensores sensores);

/**
 * @brief Comportamiento del técnico para el Nivel 5.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
  Action ComportamientoTecnicoNivel_5(Sensores sensores);

/**
 * @brief Comportamiento del técnico para el Nivel 6.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
  Action ComportamientoTecnicoNivel_6(Sensores sensores);

  /**
 * @brief Comportamiento del técnico para el Nivel E (especial tutorial parte 2).
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
  Action ComportamientoTecnicoNivel_E(Sensores sensores);

protected:
  // =========================================================================
  // FUNCIONES PROPORCIONADAS
  // =========================================================================
  /**
   * @brief Actualiza el mapaResultado y mapaCotas con la información de los sensores.
   * IMPORTANTE: Esta función ya está implementada. Actualiza mapaResultado y mapaCotas
   * con la información de los 16 sensores.
   */
  void ActualizarMapa(Sensores sensores);

  /**
   * @brief Determina si una casilla es transitable para el técnico.
   * NOTA: El técnico puede tener reglas de transitabilidad diferentes al ingeniero.
   * @param f Fila de la casilla.
   * @param c Columna de la casilla.
   * @param tieneZapatillas Indica si el agente posee las zapatillas.
   * @return true si la casilla es transitable.
   */
  bool EsCasillaTransitableLevel0(int f, int c, bool tieneZapatillas);

  /**
   * @brief Comprueba si la casilla de delante es accesible por diferencia de altura.
   * REGLA PARA TÉCNICO: Desnivel máximo siempre 1 (independiente de zapatillas).
   * @param actual Estado actual del agente (fila, columna, orientacion).
   * @return true si el desnivel con la casilla de delante es admisible.
   */
  bool EsAccesiblePorAltura(const ubicacion &actual);

  /**
   * @brief Devuelve la posición (fila, columna) de la casilla que hay delante del agente.
   * @param actual Estado actual del agente (fila, columna, orientacion).
   * @return Estado con la fila y columna de la casilla de enfrente.
   */
  ubicacion Delante(const ubicacion &actual) const;


  /**
   * @brief Comprueba si una celda es de tipo transitable por defecto.
   * @param c Carácter que representa el tipo de superficie.
   * @return true si es camino ('C'), zapatillas ('D') o meta ('U').
   */
  bool es_camino(unsigned char c) const;

    /**
 * @brief Imprime por consola la secuencia de acciones de un plan para un agente.
 * @param plan  Lista de acciones del plan.
 */
  void PintaPlan(const list<Action> &plan);


/**
 * @brief Imprime las coordenadas y operaciones de un plan de tubería.
 * @param plan  Lista de pasos (fila, columna, operación).
 */
  void PintaPlan(const list<Paso> &plan);


  /**
 * @brief Convierte un plan de acciones en una lista de casillas para
 *        su visualización en el mapa gráfico.
 * @param st    Estado de partida.
 * @param plan  Lista de acciones del plan.
 */
  void VisualizaPlan(const ubicacion &st, const list<Action> &plan);

private:
  // =========================================================================
  // VARIABLES DE ESTADO
  // =========================================================================

  bool tiene_zapatillas;
  Action last_action;
  int contador_giros;
  int cont_walk;
  bool walk_left;
  Action giro_preferido;
  int last_f;
  int last_c;
  bool en_bloqueo;
  bool en_bloqueo_U;
  bool giro_defecto;
  vector<vector<int>> visitas;

  //Nivel E, 3
  bool hayPlan;            // Indica si hay una plan que ejecutar
  list<Action> plan;       // Almacena el plan a realizar.

    /**
   * @brief Calcula la heurística optimista (Distancia de Chebyshev).
   * @param actual Estado donde se encuentra el Técnico.
   * @param final Estado donde está la filtración de Belkanita.
   * @return El valor de h(n) estimado en coste de energía.
   */
  int Heuristica(const EstadoT &actual, const EstadoT &final) const;
  /**
   * @brief Evalúa si el Técnico puede moverse a la casilla que tiene enfrente.
   * Tiene en cuenta los límites del mapa, los obstáculos (P, M) y las reglas
   * especiales del Bosque ('B') con zapatillas. También el límite de altura (1).
   * @param st Estado previo al movimiento.
   * @param terreno Mapa visual de la superficie.
   * @param altura Mapa de cotas.
   * @return True si es físicamente posible hacer WALK.
   */
  bool CasillaAccesibleTecnico(const EstadoT &st, const std::vector<std::vector<unsigned char>> &terreno, 
  const std::vector<std::vector<unsigned char>> &altura) const;
    /**
   * @brief Calcula la energía consumida por ejecutar una acción específica.
   * @param accion Acción que se quiere simular (WALK, TURN_SL, TURN_SR).
   * @param st Estado desde el cual se inicia la acción.
   * @param terreno Matriz de terreno para saber qué se está pisando.
   * @param altura Matriz de altura para calcular los incrementos/decrementos por desnivel.
   * @return El gasto de energía según las tablas del PDF.
   */
  int CosteAccionTecnico(Action accion, const EstadoT &st, const std::vector<std::vector<unsigned char>> &terreno, 
    const std::vector<std::vector<unsigned char>> &altura) const;

  /**
   * @brief Algoritmo principal de búsqueda A*.
   * Utiliza una priority_queue para expandir siempre el nodo con menor f(n).
   * Gestiona una lista de cerrados (explored) que guarda el menor coste 'g' 
   * encontrado para un estado concreto.
   * @param inicio Estado de partida.
   * @param final Estado de destino (solo importan fila y columna).
   * @param terreno Mapa superficial.
   * @param altura Mapa de desniveles.
   * @return La secuencia de acciones que consume MENOS energía.
   */
  std::list<Action> A_Estrella(const EstadoT &inicio, const EstadoT &final, 
    const std::vector<std::vector<unsigned char>> &terreno, const std::vector<std::vector<unsigned char>> &altura);
};

#endif
