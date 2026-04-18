#ifndef COMPORTAMIENTOTECNICO_H
#define COMPORTAMIENTOTECNICO_H

#include <chrono>
#include <time.h>
#include <thread>
#include <list>

#include "comportamientos/comportamiento.hpp"

class ComportamientoTecnico : public Comportamiento {
public:
  // =========================================================================
  // CONSTRUCTORES
  // =========================================================================

  ComportamientoTecnico(unsigned int size = 0) : Comportamiento(size) {
    zaps = false;
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

  ComportamientoTecnico(std::vector<std::vector<unsigned char>> mapaR,
                       std::vector<std::vector<unsigned char>> mapaC):
                       Comportamiento(mapaR, mapaC) {
    zaps = false;
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

  ComportamientoTecnico(const ComportamientoTecnico &comport): Comportamiento(comport) {}
  ~ComportamientoTecnico() {}

  Action think(Sensores sensores);

  ComportamientoTecnico *clone() {
    return new ComportamientoTecnico(*this);
  }

  // =========================================================================
  // ÁREA DE IMPLEMENTACIÓN DEL ESTUDIANTE
  // =========================================================================

  Action ComportamientoTecnicoNivel_0(Sensores sensores);
  Action ComportamientoTecnicoNivel_1(Sensores sensores);
  Action ComportamientoTecnicoNivel_2(Sensores sensores);
  Action ComportamientoTecnicoNivel_3(Sensores sensores);
  Action ComportamientoTecnicoNivel_4(Sensores sensores);
  Action ComportamientoTecnicoNivel_5(Sensores sensores);
  Action ComportamientoTecnicoNivel_6(Sensores sensores);

protected:
  // =========================================================================
  // FUNCIONES PROPORCIONADAS
  // =========================================================================

  void ActualizarMapa(Sensores sensores);
  bool EsCasillaTransitableLevel0(int f, int c, bool tieneZapatillas);
  bool EsAccesiblePorAltura(const ubicacion &actual);
  ubicacion Delante(const ubicacion &actual) const;
  bool es_camino(unsigned char c) const;

  // Helpers
  bool es_camino1(unsigned char c) const;
  int veoCasillaInteresanteT0(char i, char c, char d, bool zaps);
  int veoCasillaInteresanteT1(char i, char c, char d, bool zaps);
  char viablePorAlturaT(char casilla, int dif);

  void PintaPlan(const list<Action> &plan);
  void PintaPlan(const list<Paso> &plan);
  void VisualizaPlan(const ubicacion &st, const list<Action> &plan);

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
  bool en_bloqueo_U;
  bool giro_defecto;
  vector<vector<int>> visitas;
};

#endif
