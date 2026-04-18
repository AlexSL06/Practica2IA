#ifndef COMPORTAMIENTOINGENIERO_H
#define COMPORTAMIENTOINGENIERO_H

#include <chrono>
#include <list>
#include <map>
#include <set>
#include <thread>
#include <time.h>

#include "comportamientos/comportamiento.hpp"

class ComportamientoIngeniero : public Comportamiento {
public:
  // =========================================================================
  // CONSTRUCTORES
  // =========================================================================

  /**
   * @brief Constructor para niveles 0, 1 y 6 (sin mapa completo)
   * @param size Tamaño del mapa (si es 0, se inicializa más tarde)
   */
  ComportamientoIngeniero(unsigned int size = 0) : Comportamiento(size) {
    zaps = false;
    last_action = IDLE;
    contador_giros = 0;
    giro_preferido = TURN_SL;
    last_f = -1;
    last_c = -1;
    en_bloqueo = false;
    en_bloqueo_J = false;
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
  ComportamientoIngeniero(std::vector<std::vector<unsigned char>> mapaR,
                         std::vector<std::vector<unsigned char>> mapaC):
                         Comportamiento(mapaR, mapaC) {
    zaps = false;
    last_action = IDLE;
    contador_giros = 0;
    giro_preferido = TURN_SL;
    last_f = -1;
    last_c = -1;
    en_bloqueo = false;
    en_bloqueo_J = false;
    en_bloqueo_U = false;
    walk_left = true;
    giro_defecto = false;
    cont_walk = 0;
  }

  ComportamientoIngeniero(const ComportamientoIngeniero &comport)
      : Comportamiento(comport) {}
  ~ComportamientoIngeniero() {}

  Action think(Sensores sensores);

  ComportamientoIngeniero *clone() {
    return new ComportamientoIngeniero(*this);
  }

  // =========================================================================
  // ÁREA DE IMPLEMENTACIÓN DEL ESTUDIANTE
  // =========================================================================

  Action ComportamientoIngenieroNivel_0(Sensores sensores);
  Action ComportamientoIngenieroNivel_1(Sensores sensores);
  Action ComportamientoIngenieroNivel_2(Sensores sensores);
  Action ComportamientoIngenieroNivel_3(Sensores sensores);
  Action ComportamientoIngenieroNivel_4(Sensores sensores);
  Action ComportamientoIngenieroNivel_5(Sensores sensores);
  Action ComportamientoIngenieroNivel_6(Sensores sensores);

protected:
  // =========================================================================
  // FUNCIONES PROPORCIONADAS
  // =========================================================================

  void ActualizarMapa(Sensores sensores);
  bool EsCasillaTransitableLevel0(int f, int c, bool tieneZapatillas);
  bool EsAccesiblePorAltura(const ubicacion &actual, bool zap);
  ubicacion Delante(const ubicacion &actual) const;
  bool es_camino(unsigned char c) const;

  // Helpers (nivel 1)
  bool es_camino1(unsigned char c) const;
  int veoCasillaInteresanteI0(char i, char c, char d, bool zaps);
  int veoCasillaInteresanteI1(char i, char c, char d, bool zaps);
  char viablePorAlturaI(char casilla, int dif, bool zap);
  bool puedeSaltarI(const Sensores &sensores, bool tiene_zaps);

  void PintaPlan(const list<Action> &plan);
  void PintaPlan(const list<Paso> &plan);
  void VisualizaPlan(const ubicacion &st, const list<Action> &plan);
  void VisualizaRedTuberias(const list<Paso> &plan);

private:
  // =========================================================================
  // VARIABLES DE ESTADO
  // =========================================================================

  bool zaps;
  Action last_action;
  int contador_giros;
  int cont_walk;
  bool walk_left;
  Action giro_preferido;
  int last_f;
  int last_c;
  bool en_bloqueo;
  bool en_bloqueo_J;
  bool en_bloqueo_U;
  bool giro_defecto;
  vector<vector<int>> visitas;
};

#endif
